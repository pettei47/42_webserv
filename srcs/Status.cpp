#include "webserv.hpp"

Status::StatusDescription::StatusDescription()
{
  description[200] = "OK";
  description[201] = "Created";
  description[204] = "No Content";
  description[301] = "Moved Permanently";
  description[302] = "Found";
  description[303] = "See Other";
  description[307] = "Permanent Redirect";
  description[308] = "Moved Permanently";
  description[400] = "Bad request";
  description[403] = "Forbidden";
  description[404] = "Not Found";
  description[405] = "Method Not allowed";
  description[413] = "Payload Too Large";
  description[414] = "URI Too Long";
  description[500] = "Internal Server Error";
  description[501] = "Not implemented";
  description[502] = "Bad Gateway";
  description[504] = "Gateway Timeout";
  description[505] = "HTTP Version Not Supported";
}

struct Status::StatusDescription Status::_status_description;

/**
 * @brief ステータスコードをテキストに変換
 * @param code: ステータスコード
 * @return ステータスのテキスト。リストにない場合は500のテキストを返す
 */
const std::string& Status::status_code_to_string(int code)
{
  // 一覧にないステータスコードが来た場合は、500で返す
  if(_status_description.description.find(code) == _status_description.description.end())
  {
    return _status_description.description[500];
  }
  return _status_description.description[code];
}

Status::Status()
  : _status_code(0)
{ }

Status::Status(const std::string& protocol_version, int status_code)
  : _protocol_version(protocol_version)
  , _status_code(status_code)
{ }

Status::~Status() { }

void Status::set_status_code(int code)
{
  _status_code = code;
}

int Status::get_status_code() const
{
  return _status_code;
}

/**
 * @brief ステータスコードが200番台のときtrueを返す
 * @return ステータスコードが200番台のときtrue
 */
bool Status::is_success() const
{
  return 200 <= _status_code && _status_code < 300;
}

/**
 * @brief ステータスコードが300番台のときtrueを返す
 * @return ステータスコードが300番台のときtrue
 */
bool Status::is_redirect() const
{
  return 300 <= _status_code && _status_code < 400;
}

/**
 * @brief ステータス行をstringとして返す
 * @return ステータス行のstring。終端はCRLF
 */
const std::string Status::to_string() const
{
  std::string status_line = _protocol_version + " " + ft::to_string(_status_code) + " " +
                status_code_to_string(_status_code) + " " + ft::CRLF;
  return status_line;
}
