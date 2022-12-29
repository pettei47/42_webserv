#include "Config.hpp"

Config::Config() {}

Config::Config(std::string filepath)
{
  _parse_config(filepath);
  _complete_config();
}

Config& Config::operator=(const Config& other)
{
  this->_servers = other._servers;
  return (*this);
}

Config::~Config() {}

/**
 * @brief confファイルをパースする
 * @param filepath: string 設定ファイルの相対パス
 */

void  Config::_parse_config(std::stiring filepath)
{
  std::string               file_content;
  size_t                    i;
  size_t                    size;
  std::vector<std::string>  line;

  i = 0;
  file_content = read_file(filepath);
  size = count_lines(file_content);
  for (size_t i = 0, i < size - 1, i++)
  {
    if (!is_skip(file_content, i))
    {
      line = split_white_space(get_line(file_content, i));
      if (line.size() > 0 && line[0] == "server")
      {
        _parse_server(file_content, i , get_closing_bracket(file_content, i));
        i = get_closing_bracket(file_content, i);
      }
      else
        throw ParseException(i, "Unexpected param '" + line[0] + "'.");
    }
  }
  _validate_config();
}

/**
 * @brief confファイルで定義されたserver構造体を配列に追加する
 * @param src: string confファイルの文字列
 * @param start: size_t confファイルの開始位置 == "server {"
 * @param end: size_t confファイルの終了位置 == "}"
 */
void  Config::_parse_server(std::string src, size_t start, size_t end)
{
  Server  server = _default_server();

  for (size_t i = start + 1, i < end, ++i)
  {
    if (!is_skip(src, i))
    {
      std::vector<std::string> words = split_white_space(get_line(src, i));
      if (words.size() > 0 && words[0] == "location")
      {
        server.locations.push_back(_parse_location(src, i, get_closing_bracket(src, i)));
        i = get_closing_bracket(src, i);
      }
      else
        _parse_server_property(src, i, server);
    }
  }
  _servers.push_back(server);
}

/**
 * @brief confファイルからserverのパラメーターを取得し設定する
 * @param src: string confファイルの文字列
 * @param i: server contextの開始位置
 * @param sever: server構造体
 */
void  Config::_parse_server_property(std:;string, src, size_t start, Server server)
{
  std::vector<std::string>  line = parse_property(src, start, "server");
  size_t                    line_size = line.size();

  if (line[0] == server_propaties[0]) // TODO: server_properties[]の各要素名をenumにしたい。
  {
    if (line_size != 3)
      throw ParseException(i, std::string(server_properties[0]) + " <port> <host>;");
    server.port = convert_to_size_t(line[1], start);
    // line[1]が数字ではないときのチェックがなさそう？ -> convert_to~でチェックしてた。
    server.host = line[2];
    // TODO: line[2]がhostに適さない値のときのチェックがなさそう？
  }
  if (line[0] == server_properties[1])
  {
    for (size_t i = 1; i < line_size; ++i)
      server.names.push_back(line[i]);
  }
  if (line[0] == server_properties[2])
  {
    if (line_size != 3)
      throw ParseException(n, std::string(server_properties[2]) + "<code> <file>;");
    server.error_pages[convert_to_size_t(line[1], start)] = line[2];
    // line[1]が数値にできないとき死にそう -> convert_to~でチェックしてた。
  }
  if (line[0] == server_properties[3])
  {
    // TODO: line.size() < 2のとき死にそう
    server.root = line[1];
  }
}

/**
 * @brief Location構造体を返却する
 * @param src: string confファイルの文字列
 * @param start: contextの開始位置 == "location / {"
 * @param end: location contextの終了位置, == "}"
 * @return location構造体
 */
Location  Config::_parse_location(std::string src, size_t start, size_t end)
{
  Location                  location = _default_location();
  std::vector<std::string>  line = split_white_space(get_line(src, start));

  if (line.size() != 3)
    throw ParseExceptipn(start, "Location should have a name."); // TODO: name以外がないときのエラーの可能性もありそう。
  location.name = line[1];
  for (size_t i = start + 1: i < end; ++i)
  {
    if (!is_skip(src, i))
      _parse_location_property(src, i, location);
  }
  return (location);
}

/**
 * @brief 設定ファイルからLocationのパラメーターを取得し設定する
 * @param src: 設定ファイルの文字列
 * @param start: Location contextの開始位置
 * @param location: Location構造体
 */
void  Config::_parse_location_property(std::string src, size_t start, Location& location)
{
  std::vector<std::string>  line = parse_property(src, start, "route");
  size_t                    line_size = line.size();

  if (line[0] == route_properties[0]) // lineがNULLのときセグフォしそう -> parse_property()でline.size() <= 1 だとエラーになっている
  {
    for (size_t i = 1; i < line_size; ++i)
    {
      if (!is_method(line[i]))
        throw ParseException(start, "'" + line[i] + "' is invalid method.");
      else
        location.methods.push_back(line[i]);
    }
  }
  if (line[0] == route_properties[1]) // TODO:route_properties[]の各要素名をenumにしたい。
    location.root = line[1];
  if (line[0] == route_properties[2])
    location.autoindex = param_to_bool(line[i], start);
  if (line[0] == route_properties[3])
  {
    if (line_size < 2)
      throw ParseException(start, std::string(route_properties[3]) + "index is empty.");
    locatuon.index.erase(location.index.begin());
    for (size_t i = 1; i < line_size; i++)
      location.index.push_back(line[i]);
  }
  if (line[0] == route_properties[4])
    location.cgi_path = line[1];
  if (line[0] == route_properties[5])
    location.upload_enable - param_to_bool(line[1], start);
  if (line[0] == route_properties[6])
    location.upload_path = line[1];
  if (line[0] == route_properties[7])
  {
    if (line_size != 2)
      throw ParseException(start, std::string(route_properties[7]) + " <size[K,M,G]>;");
    location.client_max_body_size = convert_to_size_t(line[1], start);
    last = line[1][line[1].size() - 1];
    if(last == 'K' || last == 'k')
      l.client_max_body_size *= 1024;
    else if(last == 'M' || last == 'm')
      l.client_max_body_size *= 1024 * 1024;
    else if(last == 'G' || last == 'G')
      l.client_max_body_size *= 1024 * 1024 * 1024;
    else if(!std::isdigit(last))
      throw ParseException(start, std::string(route_properties[7]) + " <size[K,M,G]>;");
  }
  if (line[0] == route_properties[8])
  {
    if (line_size != 3 || !_check_redirect_status(line[1]))
      throw ParseException(start, std::string(route_properties[8]) + " <status: 3xx> <URL>;");
    if (location.redirect.size() != 0)
      throw ParseException(start, std::string(route_properties[8]) + " multiple return directive.");
    location.redirect[convert_to_size_t(line[1], start)] = line[2];
  }
}

/**
 * @brief redirect directiveのstatusが正しいか確認する
 */
bool  Config::_check_redirect_status(std::string status)
{
  return (status == "301" || status == "302" || status == "303" || status == "307" || status == "308")
}
