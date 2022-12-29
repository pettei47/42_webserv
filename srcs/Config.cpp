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
  std::vector< std::string > line = parse_property(src, start, "server");

  if (line[0] == server_propaties[0]) // 配列の箇所で扱う値が決まっているなら、ハードコードでもよさそう？
  {
    if (line.size() != 3)
      throw ParseException(i, std::string(server_properties[0]) + " <port> <host>;");
    server.port = convert_to_size_t(line[1], start);
    // line[1]が数字ではないときのチェックがなさそう？ -> convert_to~でチェックしてた。
    server.host = line[2];
    // line[2]がhostに適さない値のときのチェックがなさそう？
  }
  if (line[0] == server_properties[1])
  {
    for (size_t i = 1; i < line.size(); ++i)
      server.names.push_back(line[i]);
  }
  if (line[0] == server_properties[2])
  {
    if (line.size() != 3)
      throw ParseException(n, std::string(server_properties[2]) + "<code> <file>;");
    server.error_pages[convert_to_size_t(line[1], start)] = line[2];
    // line[1]が数値にできないとき死にそう -> convert_to~でチェックしてた。
  }
  if (line[0] == server_properties[3])
  {
    // line.size() < 2のとき死にそう
    server.root = line[1];
  }
}
