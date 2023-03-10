#include "webserv.hpp"

Body::Body()
  : _fd(0)
  , _ongoing(false)
  , _read_size(0)
  , _body_size(0)
{ }

Body::~Body() { }

int Body::get_fd() const
{
  return _fd;
}

bool Body::get_ongoing() const
{
  return _ongoing;
}

const HttpString& Body::get_body() const
{
  return _body;
}

char const* Body::get_body_data() const
{
  return _body.data();
}

/**
 * @brief 読み込みがすべて完了したときのbody全体のサイズを返す
 * @return body全体のサイズ
 */
size_t Body::get_all_body_size() const
{
  return _body_size;
}

/**
 * @brief 現在読み込んでいるbodyのサイズを返す
 * @return 現在読み込んでいるbodyのサイズ
 */
size_t Body::get_body_size() const
{
  return _body.size();
}

/**
 * @brief _buf_size分読み込んで_bodyに追加
 * @return まだ読み込み分がある場合はtrue
 * @note エラー時500送出
 */
bool Body::_read_buf()
{
  // ファイルの読み込み
  char buf[_buf_size] = {0};
  int n = read(_fd, buf, _buf_size);
  if(n < 0)
    throw http::StatusException(500);

  _body.append(buf, n);
  _read_size += _body.size();
  if(_read_size > _body_size - 2 || (_read_size < _body_size - 2 && n == 0))
    throw http::StatusException(500);
  return (_read_size != _body_size - 2);
}

/**
 * @brief ファイルを開いてそのfdを返す
 * @note エラー時500送出
 */
void Body::read_body()
{
  _body.clear();
  if(!_ongoing)
    throw http::StatusException(500);
  _ongoing = _read_buf();
  if(_ongoing)
    return;

  close_fd();
  // 最後の改行の追加
  char buf[2];
  buf[0] = '\r';
  buf[1] = '\n';
  _body.append(buf, 2);
}

void Body::_set_autoindex_body(std::string script_name, std::string dirpath)
{
  std::string content = Autoindex::generate_body(script_name, dirpath);
  std::string html = _generate_html(script_name, content);
  set_body(html);
}

/**
 * @brief エラー用のページを作成する
 * @return error時falseを返す
 */
bool Body::set_error_default_body(int status_code)
{
  _body.clear();
  try
  {
    std::string status_str =
      ft::to_string(status_code) + ' ' + Status::status_code_to_string(status_code);
    std::string content = "<center><h1>" + status_str + "</h1></center>" + ft::CRLF;
    content += "<hr><center>webserv</center>" + ft::CRLF;

    std::string html = _generate_html(status_str, content);
    set_body(html);
  }
  catch(const std::exception& e)
  {
    return false;
  }
  return true;
}

/**
 * @brief fdを閉じて_ongoing=falseとする
 */
void Body::close_fd()
{
  close(_fd);
  _ongoing = false;
  _read_size = 0;
}

void Body::set_fd(int fd)
{
  _fd = fd;
  if(_fd >= 0)
    _ongoing = true;
}

/**
 * @brief bodyをセットする
 * @param data: setするbody
 * @note _body_sizeを今のbodyのサイズにする
 */
void Body::set_body(const HttpString& data)
{
  _body = data;
  _body_size = _body.size();
}

void Body::set_body(const std::string& data)
{
  _body.append(data);
  _body_size = _body.size();
}

void Body::set_body_size(size_t file_size)
{
  _body_size = file_size;
}

std::string Body::_generate_html(std::string title, std::string body)
{
  std::string header = "<html>" + ft::CRLF;
  std::string tail = "</body>" + ft::CRLF + "</html>" + ft::CRLF;

  title = "<head><title>" + title + "</title></head>" + ft::CRLF;
  body = "<body>" + ft::CRLF + body;

  std::string html = header + title + body + tail;
  return html;
}
