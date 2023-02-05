#include "webserv.hpp"

Request::Request(std::vector< Server >& servers)
  : Message(ft::CRLF)
  , _servers(servers)
  , _content_length(0)
  , _suspended(false)
  , _body_type(NONE)
{ }

Request::~Request() { }

Request::Request(Request const& other)
  : Message(other)
  , _servers(other._servers)
{
  *this = other;
}

Request& Request::operator=(Request const& other)
{
  if(this != &other)
  {
    _raw_data.buf = other._raw_data.buf;
    _raw_data.index = other._raw_data.index;
    _servers = other._servers;
    _server = other._server;
    _location = other._location;
    _parse_phase = other._parse_phase;
    _method = other._method;
    _uri = other._uri;
    _protocol_version = other._protocol_version;
    _headers = other._headers;
  }
  return *this;
}

/**
 * @brief リクエストをパーツごとに読み取る
 *        FIRST_LINE: GET HTTP/1.0 uri
 */
void Request::handle_request()
{
  switch(_parse_phase)
  {
  case START:
    _parse_phase = FIRST_LINE;
    /* Falls through. */
  case FIRST_LINE:
    _retrieve_startline();
    if (_suspended)
      break;
    /* Falls through. */
  case HEADER:
    _retrieve_header();
    if (_suspended)
      break;
    /* Falls through. */
  case BODY:
    _retrieve_body();
    if (_suspended)
      break;
    _validate_request();
    /* Falls through. */
  default:
    break;
  }
#if DEBUG_MODE == 1
  show_request();
#endif
}

void Request::_parse_startline()
{
  size_t eol = _raw_data.buf.find(_delim);
  if(eol == HttpString::npos)
  {
    _suspended = true;
    return ;
  }

  _method = _get_next_word(false, _max_method_len, 501);
  _uri = _get_next_word(false, _max_request_uri_len, 414);
  _protocol_version = _get_next_word(true);
}

void Request::_validate_startline()
{
  if(_method.empty() || _uri.empty() || _protocol_version.empty())
    throw http::StatusException(400);
  if(!ft::is_upper(_method) || _uri.at(0) != '/' || _protocol_version.compare(0, 5, "HTTP/"))
    throw http::StatusException(400);
  if(_method != "GET" && _method != "POST" && _method != "DELETE")
    throw http::StatusException(405);
  if(ft::trim_space(_protocol_version).compare("HTTP/1.1"))
    throw http::StatusException(505);
}

/**
 * @brief エラー時に、HttpInfoにデフォルト値を設定する
 */
void Request::setup_default_http_info(HttpInfo& info, int status) const
{
  std::map<int, std::string>& error_pages = _server->error_pages;

  info.method = "GET";
  info.uri = "";
  info.protocol_version = "HTTP/1.1";
  info.headers = Header();
  info.body = HttpString();
  info.server = NULL;
  info.location = NULL;
  info.error_filepath = error_pages[status];
  info.script_name = "";
  info.query_string = "";
}

/**
 * @brief HttpInfoに必要な値を設定する
 */
void Request::setup_http_info(HttpInfo& info) const
{
  info.method = _method;
  info.uri = _uri;
  info.protocol_version = _protocol_version;
  info.headers = _headers;
  info.body = _body;
  info.server = _server;
  info.location = _location;
  info.error_filepath = "";

  size_t index = _uri.find('?');
  if(index != std::string::npos)
  {
    info.script_name = _uri.substr(0, index);
    info.query_string = _uri.substr(index + 1);
  }
  else
    info.script_name = _uri;

  _set_script_name(info);
}

/**
 * @brief uriのlocationのパスをrootに置き換えてscript_nameにセットする
 */
void Request::_set_script_name(HttpInfo& info)
{
  size_t size = info.location->name.size();
  if(ft::end_with(info.location->name, '/'))
    --size;
  info.script_name = info.script_name.substr(size);
  if(info.script_name.empty())
    info.script_name = "/";
}

/**
 * @brief ヘッダーフィールドのパース
 * @note 空行までがヘッダーフィールドのため空行がない場合は接続を継続して再受信する
 */
void Request::_parse_header()
{
  std::string line;
  std::pair< std::string, std::string > headerfield;
  size_t size = _raw_data.buf.size();

  while(_raw_data.index < size)
  {
    line = _get_next_line(_max_header_fileds_size, 400);
    if(line.empty())
      return;
    if(line.find(":") == std::string::npos)
      continue;
    headerfield = _parse_headerfield(line);
    _validate_headerfield(headerfield);
    _headers.append(headerfield);
  }
  _suspended = true;
}

void Request::_set_body_type()
{
  if(_headers.contains("Content-Length"))
  {
    _content_length = _headers.get_content_length();
    _body_type = CONTENT_LENGTH;
  }
  if(_headers.contains("Transfer-Encoding"))
    _body_type = CHUNKED;
}

void Request::_validate_headerfield(std::pair< std::string, std::string >& headerfield)
{
  std::string key = headerfield.first;
  std::string value = headerfield.second;
  if(!http::is_token(key))
    throw http::StatusException(400);
  if(_headers.contains(key))
  {
    if(key == "Host" || key == "Content-Length")
      throw http::StatusException(400);
  }
  if(key == "Host")
  {
    if(value.empty() || http::is_comma_separated_list(value))
      throw http::StatusException(400);
  }
}

void Request::_validate_header()
{
  if(!_headers.contains("Host"))
    throw http::StatusException(400);
  if(_headers.contains("Content-Length"))
  {
    if(http::is_comma_separated_list(_headers["Content-Length"]))
      throw http::StatusException(400);
    _content_length = _headers.get_content_length();
    if(_content_length == -1)
      throw http::StatusException(400);
    if(_content_length > _location->client_max_body_size)
      throw http::StatusException(413);
  }
  if(_headers.contains("Transfer-Encoding"))
  {
    if(_headers["Transfer-Encoding"] != "chunked")
      throw http::StatusException(501);
    if(_headers.contains("Content-Length"))
      throw http::StatusException(400);
  }
}

/**
 * @brief uriがlocationにマッチするかどうか判定する
 */
bool Request::_location_matches_uri(std::string uri, std::string location_name)
{
  size_t size = location_name.size();
  if(ft::end_with(location_name, '/'))
  {
    if(uri.compare(0, size, location_name) == 0)
      return true;
  }
  else
  {
    if(uri.compare(0, size + 1, location_name) == 0 ||
       uri.compare(0, size + 1, location_name + '/') == 0)
      return true;
  }
  return false;
}

/**
 * @brief uriを使ってlocationを選択する
 */
void Request::_select_location()
{
  _server = &_servers[0];
  _server->name = _servers[0].names[0];

  for(size_t i = 0; i < _servers.size(); ++i)
  {
    for(size_t j = 0; j < _servers[i].names.size(); ++j)
    {
      if(_servers[i].names[j].compare(_headers["Host"]) == 0 ||
        (_servers[i].names[j] + ":" + ft::to_string(_servers[i].port)).compare(_headers["Host"]))
      {
        _server = &_servers[i];
        _server->name = _servers[i].names[j];
      }
    }
  }
  _location = &(_server->locations[0]);
  for(size_t i = 0; i < _server->locations.size(); ++i)
  {
    if(_location_matches_uri(_uri, _server->locations[i].name))
      _location = &(_server->locations[i]);
  }
}

/**
 * @brief ヘッダーをパースし値をセットする
 */
void Request::_retrieve_header()
{
  if(_parse_phase != HEADER)
    return;

  _parse_header();
  _set_body_type();
  _select_location();
  // _validate_header();

  if(_method == "POST")
    _parse_phase = BODY;
  else
    _parse_phase = DONE;
}

void Request::_retrieve_body()
{
  if(_parse_phase != BODY)
    return;

  _parse_body();

  if (!_suspended)
    _parse_phase = DONE;
}


void Request::_parse_body()
{
  HttpString new_body_chunk;

  switch(_body_type)
  {
  case CHUNKED:
    if(_content_length == 0)
    {
      if(_raw_data.buf.find(_delim, _raw_data.index) == HttpString::npos)
      {
        _suspended = true;
        return ;
      }
      _content_length = ft::hex_str_to_ssize_t(_get_next_line());
    }
    if(_content_length == -1)
      throw http::StatusException(400);
    /* Falls through. */
  case CONTENT_LENGTH:
    new_body_chunk = _raw_data.buf.substr(_raw_data.index, _content_length);
    _content_length -= new_body_chunk.size();
    _raw_data.index += new_body_chunk.size();
    _body.append(new_body_chunk);
  default:
    if(_content_length > 0)
    {
      _suspended = true;
      return ;
    }
    break;
  }
  _suspended = false;
}

void Request::_validate_request()
{
  _validate_startline();
  _validate_header();
  // for (Header _headers)
  // {
  //   _validate_headerfield(headerfield);
  // }
  size_t size = _location->methods.size();
  size_t i = 0;
  for(; i < size; ++i)
  {
    if(_location->methods[i] == _method)
      break;
  }
  if(i >= size)
    throw http::StatusException(405);
}

void Request::append_raw_data(char* buf, ssize_t len)
{
  _raw_data.buf.append(buf, len);
}

/**
 * @brief HttpInfo構造体の情報を出力する、デバッグ用
 */
void Request::show_request() const
{
  std::cout << " Request Info" << std::endl;
  std::cout << "  - method: " << _method << std::endl;
  std::cout << "  - uri: '" << _uri << "'" << std::endl;
  _headers.show_headers();
  std::cout << "  - body: '" << _body << "'" << std::endl;
}

bool Request::get_suspended() {
  return _suspended;
}

void  Request::show_server() const
{
  std::cout << "Server Info" << std::endl;
  std::cout << "  - name: " << _server->name << std::endl;
  std::cout << "  - root: '" << _server->root << "'" << std::endl;
  _headers.show_headers();
}
