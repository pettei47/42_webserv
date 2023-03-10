#include "webserv.hpp"

size_t Connection::_total_connections = 0;

Connection::Connection(int fd, std::vector< Server >& servers)
  : _cfd(fd)
  , _phase(RECV)
  , _response(NULL)
  , _servers(servers)
  , _request(_servers)
{
  _total_connections += 1;
  _id = _total_connections;
  Log("Connection Established", _id);
}

Connection::~Connection(){}

Connection::Connection(const Connection& other)
  : _servers(other._servers)
  , _request(_servers)
{
  *this = other;
}

/**
 * deep copyする
 */
Connection& Connection::operator=(Connection const& other)
{
  if(this != &other)
  {
    _cfd = other._cfd;
    _phase = other._phase;
    _info = other._info;
    _response = other._response;
    _request = other._request;
    _servers = other._servers;
    _id = other._id;
  }
  return *this;
}
/**
 * @brief リクエストの中身を解析し、レスポンスを作る
 */
void Connection::recv_request()
{
  ssize_t buf_size = _recv();

  if(buf_size <= 0)
  {
    _phase = CLOSE;
    return;
  }

  try
  {
    _request.append_raw_data(_buf, buf_size);
    _request.handle_request();
  }
  catch(http::StatusException& e)
  {
    Log(std::string("Request ") + e.what());

    _request.setup_default_http_info(_info, e.get_status());
    _make_response(e.get_status());
    return;
  }
  if(_request.get_suspended())
  {
    Log("Request Suspeneded", _id);
    return;
  }
  _request.setup_http_info(_info);
  _make_response(200);
  Log("Request Completed", _id);
}

/**
 * @brief Responseクラスを作成する
 * @param status_code: ステータスコード
 */
void Connection::_make_response(int status_code)
{
  if(_response != NULL)
  {
    delete _response;
    _response = NULL;
  }
  try
  {
    _response = new Response(_info, _cfd, status_code);
    _phase = _response->handle_response();
  }
  catch(const std::exception& e)
  {
    Log("failed to make new Response.", _id);
    _phase = CLOSE;
  }
}

/**
 * @brief recvして読み込んだbuf末尾にヌル文字を入れる。エラー処理は外でやる。
 */
ssize_t Connection::_recv()
{
  ssize_t buf_size = ::recv(_cfd, _buf, _max_buffer_size, 0);
  if(buf_size > 0)
    _buf[buf_size] = '\0';
  // recvの戻り値が0以下の場合のエラー処理はこの関数の外でやってる
  return buf_size;
}

/**
 * @brief コネクションをcloseする。
 */
void Connection::close() const
{
  if(::close(_cfd) == -1)
    throw std::exception();
}

/**
 * @brief selectで必要なパラメータの設定をする
 * @param read_set: read用のset
 * @param write_set: write用のset
 * @param max_fd: fdのmax値
 */
void Connection::set_select_fd(fd_set& read_set, fd_set& write_set, int& max_fd) const
{
  switch(_phase)
  {
  case RECV:
    ft::set_fd(_cfd, read_set, max_fd);
    break;
  case SEND:
    ft::set_fd(_cfd, write_set, max_fd);
    break;
  default:
    if(_response) // この地点で_responseがNULLのことはないが念のためチェック
      _response->set_select_fd(read_set, write_set, max_fd);
    break;
  }
}

/**
 * @brief fdが有効になっているかのチェックとそれに合わせた処理
 * @param read_set: read用のset
 * @param write_set: write用のset
 * @return このConnectionをcloseする場合はtrue、それ以外はfalse
 */
bool Connection::check_and_handle(fd_set& read_set, fd_set& write_set)
{
  if(_phase == RECV)
  {
    if(ft::clear_fd(_cfd, read_set))
    {
      Log("Request Recieved", _id);
      recv_request();
    }
  }
  else if(_response == NULL) //ここに入る場合はないはず
  {
    Log("_response == NULL", _id);
    _phase = CLOSE;
  }
  else if(_phase == SEND)
  {
    if(ft::clear_fd(_cfd, write_set))
    {
      Log("Response Send", _id);
      _phase = _response->handle_response();
    }
  }
  else
    _phase = _response->check_and_handle(read_set, write_set);
  if(_phase == CLOSE)
  {
    Log("Response Deleted", _id);
    delete _response;
    _response = NULL;
    // ここでcloseしないタイプの場合falseを返す
    return true;
  }
  return false;
}

/**
 * @brief Connection用のfdのgetter
 */
int Connection::get_cfd() const
{
  return _cfd;
}

/**
 * @brief phaseのgetter
 */
enum phase Connection::get_phase() const
{
  return _phase;
}

/**
 * @brief Responseのgetter
 */
const Response* Connection::get_response() const
{
  return _response;
}
