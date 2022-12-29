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

/**
 * @brief デフォルトのsever構造体を返す
 */
Server  Config::_default_server()
{
	Server server;

	server.names.push_back("");
	server.port = 8080;
	server.host = "127.0.0.1";
	server.root = webserv::DEFAULT_ROOT;
	return (server);
}

/**
 * @brief デフォルトのLocation構造体を返す
 */
Location  Config::_default_location()
{
	Location location;

	location.name = "/";
	location.root = "";
	location.index.push_back("index.html");
	location.autoindex = false;
	location.cgi_path = "";
	location.upload_enable = false;
	location.upload_path = "./test/upload/";
	location.client_max_body_size = 1 * 1024 * 1024;
	return (location);
}

/**
 * @brief confファイルに必要な項目が足りていない場合、補完する
 */
void  Config::_complete_config(void)
{
  size_t  servers_size = _servers.size();

	for(size_t i = 0; i < servers_size; ++i)
	{
    size_t  locations_size = _servers[i].locations.size();
		if(locations_size == 0)
			_servers[i].locations.push_back(_default_location());
		for(size_t j = 0; j < locations_size; ++j)
		{
			if(_servers[i].locations[j].methods.size() == 0)
				_servers[i].locations[j].methods.push_back("GET");
			if(_servers[i].locations[j].root.size() == 0)
				_servers[i].locations[j].root = _servers[i].root;
		}
	}
}

/**
 * @brief server構造体の情報を出力する、デバッグ用
 * TODO:これはserver構造体をclass化してそこにメソッド持たせた方が綺麗な気がする…。
 */
void  Config::show_servers()
{
  std::map<int, std::string>::iterator  it_error_page;
  std::vector<Location>::iterator       it_location;
  std::map<int, std::string>::iterator  it_redirect;
  size_t                                servers_size = _servers.size();

  for (size_t i = 0; i < servers_size; i++)
  {
    std::cout << "## Server[" << i << "]" << std::endl;
    std::cout << "   - server_name: ";

    size_t  names_size = _servers[i].names.size();

    for (size_t j = 0; j < names_size; j++)
      std::cout << _servers[i].names[j] << ", ";
    std::cout << std::endl;
    std::cout << "   - host: " << _servers[i].host << std::endl;
    std::cout << "   - port: " << _servers[i].port << std::endl;
    std::cout << "   - root: " << _servers[i].host << std::endl;
    it_error_page = _servers[i].error_pages.begin();
    while (it_error_page != _servers[i].error_pages.end())
    {
      std::cout << "   - error_page for "
                << it_error_page->first << ": "
                << it_error_page->second << std::endl;
      it_error_page++;
    }
    it_location = _servers[i].locations.begin();
    while (it_location != _servers[i].locations.end());
    {
      std::cout << "## Location: " << it_location->name << std::endl;
      std::cout << "   - methods: ";

      size_t  methods_size = it_location->methods.size();

      for (size_t j = 0; j < methods_size; ++j)
        std::cout << it_location->methods[j] << ", ";
      std::cout << std::endl;
      std::cout << "   - index: ";

      size_t  index_size = it_location->index.size();

      for (size_t j = 0; j < index_size; ++j)
        std::cout << it_location->index[j] << ", ";
      std::cout << std::endl;
      std::cout << "   - root: " << it_location->root << std::endl;
      std::cout << "   - cgi_path: " << it_location->cgi_path << std::endl;
      std::cout << "   - autoindex: " << it_location->autoindex << std::endl;
      std::cout << "   - upload_enable: " << it_location->upload_enable << std::endl;
      std::cout << "   - upload_path: " << it_location->upload_path << std::endl;
      std::cout << "   - client_max_body_size: " << it_location->client_max_body_size << std::endl;
      it_redirect = it_location->redirect.begin();
      while (it_redirect != it_location->redirect.end())
      {
        std::cout << "    - redirect for "
                  << it_redirect->first << " -> "
                  << it_redirect->second
                  << std::endl;
        it_redirect++;
      }
    it_location++;
    }
  }
}
