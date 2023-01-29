#include "webserv.hpp"

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

void  Config::_parse_config(std::string filepath)
{
  _file_contents = read_file(filepath);
  size_t size = _file_contents.size();
  std::vector<std::string>  line_items;

  for (size_t i = 0; i < size - 1; i++)
  {
    if (!is_skip(_file_contents[i]))
    {
      line_items = split(_file_contents[i], " ");
      if (line_items.size() > 0 && line_items[0] == "server")
      {
        _parse_server(i , get_close_bracket_line(_file_contents, i));
        i = get_close_bracket_line(_file_contents, i);
      }
      else
        throw ParseException(i, "Unexpected param '" + line_items[0] + "'.");
    }
  }
  _validate_config();
}

/**
 * @brief confファイルで定義されたserver構造体を配列に追加する
 * @param start: server directiveの開始位置 == "server {"
 * @param end: server directiveの終了位置 == "}"
 */
void  Config::_parse_server(size_t start, size_t end)
{
  Server  server = _default_server();

  for (size_t i = start + 1; i < end; ++i)
  {
    if (!is_skip(_file_contents[i]))
    {
      std::vector<std::string> line_items = split(_file_contents[i], " ");
      if (line_items.size() > 0 && line_items[0] == "location")
      {
        server.locations.push_back(_parse_location(i, get_close_bracket_line(_file_contents, i)));
        i = get_close_bracket_line(_file_contents, i);
      }
      else
        _parse_server_property(i, server);
    }
  }
  _servers.push_back(server);
}

/**
 * @brief confファイルからserverのパラメーターを取得し設定する
 * @param n: server directiveのチェック位置
 * @param server: server構造体
 */
void  Config::_parse_server_property(size_t n, Server& server)
{
  std::vector<std::string>  line_items = parse_property(_file_contents[n], n, "server");

  // TODO: line_items = null のとき死にそう
  //       -> parse_propertyの方で、result.size < 2はthrowされてるのでOK
  if (line_items[0] == server_properties[PROP_LISTEN])
  {
    if (line_items.size() != 3)
      throw ParseException(n, std::string(server_properties[PROP_LISTEN]) + " <port> <host>;");
    server.port = convert_to_size_t(line_items[1], n);
    server.host = line_items[2];
    // TODO: line_items[2]がhostに適さない値のときのチェックがなさそう？
  }
  if (line_items[0] == server_properties[PROP_SERVER_NAME])
  {
    for (size_t i = 1; i < line_items.size(); ++i)
      server.names.push_back(line_items[i]);
  }
  if (line_items[0] == server_properties[PROP_ERROR_PAGE])
  {
    if (line_items.size() != 3)
      throw ParseException(n, std::string(server_properties[PROP_ERROR_PAGE]) + "<code> <file>;");
    server.error_pages[convert_to_size_t(line_items[1], n)]
      = webserv::ERROR_PAGES + line_items[2];
  }
  if (line_items[0] == server_properties[PROP_SERVER_ROOT])
  {
    // TODO: line_items.size() < 2のとき死にそう
    //       -> parse_propertyの方で、result.size < 2はthrowされてるのでOK
    server.root = line_items[1];
  }
}

/**
 * @brief Location構造体を返却する
 * @param src: string confファイルの文字列
 * @param start: location directiveの開始位置 == "location / {"
 * @param end: location directiveの終了位置, == "}"
 * @return location構造体
 */
Location  Config::_parse_location(size_t start, size_t end)
{
  Location                  location = _default_location();
  std::vector<std::string>  line_items = split(_file_contents[start], " ");

  if (line_items.size() != 3)
    throw ParseException(start, "Location should have `location /path_name/ {`.");
  location.name = line_items[1];
  for (size_t i = start + 1; i < end; ++i)
  {
    if (!is_skip(_file_contents[i]))
      _parse_location_property(i, location);
  }
  return (location);
}

/**
 * @brief 設定ファイルからLocationのパラメーターを取得し設定する
 * @param n: Location directiveのチェック位置
 * @param location: Location構造体
 */
void  Config::_parse_location_property(size_t n, Location& location)
{
  std::vector<std::string>  line_items = parse_property(_file_contents[n], n, "location");

  if (line_items[0] == location_properties[PROP_METHOD])
  {
    for (size_t i = 1; i < line_items.size(); ++i)
    {
      if (!is_method(line_items[i]))
        throw ParseException(n, "'" + line_items[i] + "' is invalid method.");
      else
        location.methods.push_back(line_items[i]);
    }
  }
  if (line_items[0] == location_properties[PROP_LOCATION_ROOT])
    location.root = line_items[1];
  if (line_items[0] == location_properties[PROP_AUTOINDEX])
    location.autoindex = autoindex_to_bool(line_items[1], n);
  if (line_items[0] == location_properties[PROP_INDEX])
  {
    if (line_items.size() < 2)
      throw ParseException(n, std::string(location_properties[PROP_INDEX]) + "index is empty.");
    location.index.erase(location.index.begin());
    for (size_t i = 1; i < line_items.size(); i++)
      location.index.push_back(line_items[i]);
  }
  if (line_items[0] == location_properties[PROP_CGI_PATH])
    location.cgi_path = line_items[1];
  if (line_items[0] == location_properties[PROP_UPLOAD_ENABLE])
    location.upload_enable = autoindex_to_bool(line_items[1], n);
  if (line_items[0] == location_properties[PROP_UPLOAD_PATH])
    location.upload_path = line_items[1];
  if (line_items[0] == location_properties[PROP_CLIENT_MAX_BODY_SIZE])
  {
    if (line_items.size() != 2)
      throw ParseException(n, std::string(location_properties[PROP_CLIENT_MAX_BODY_SIZE]) + " <size[K,M,G]>;");
    location.client_max_body_size = convert_to_size_t(line_items[1], n);
    char  last = line_items[1][line_items[1].size() - 1];
    if(last == 'K' || last == 'k')
      location.client_max_body_size *= 1024;
    else if(last == 'M' || last == 'm')
      location.client_max_body_size *= 1024 * 1024;
    else if(last == 'G' || last == 'G')
      location.client_max_body_size *= 1024 * 1024 * 1024;
    else if(!std::isdigit(last))
      throw ParseException(n, std::string(location_properties[PROP_CLIENT_MAX_BODY_SIZE]) + " <size[K,M,G]>;");
  }
  if (line_items[0] == location_properties[PROP_RETURN])
  {
    if (line_items.size() != 3 || !_check_redirect_status(line_items[1]))
      throw ParseException(n, std::string(location_properties[8]) + " <status: 3xx> <URL>;");
    if (location.redirect.size() != 0)
      throw ParseException(n, std::string(location_properties[8]) + " multiple return directive.");
    location.redirect[convert_to_size_t(line_items[1], n)] = line_items[2];
  }
}

/**
 * @brief redirect directiveのstatusが正しいか確認する
 */
bool  Config::_check_redirect_status(std::string status)
{
  return (status == "301" || status == "302" || status == "303" || status == "307" || status == "308");
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
 * TODO: この辺ハードコードになってるけど変数か環境変数で渡すようにしたい
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
  location.upload_path = "./pages/upload/";
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
 * @brief confファイルが正しいものかチェックする
 */
void Config::_validate_config()
{
  if(_servers.size() == 0)
    throw ParseException(0, "Your configuration file must provide at least one server.");
  for(size_t i = 0; i < _servers.size(); ++i)
  {
    for(size_t j = 0; j < _servers[i].locations.size(); ++j)
    {
      if(_servers[i].locations[j].cgi_path.size() > 0 &&
        !check_path(_servers[i].locations[j].cgi_path))
        throw ParseException(0,
                    "The cgi path '" + _servers[i].locations[j].cgi_path +
                    "' is not a valid file.");
    }
  }
  for(size_t i = 0; i < _servers.size(); ++i)
  {
    for(size_t j = 0; j < _servers.size(); ++j)
    {
      if(i != j)
      {
        if(_servers[i].host == "127.0.0.1")
          _servers[i].host = "localhost";
        if(_servers[j].host == "127.0.0.1")
          _servers[j].host = "localhost";
        // TODO: このコメントアウトが何なのか確認する↓
        //if(_servers[i].host == _servers[j].host && _servers[i].port == _servers[j].port)
        //throw ParseException(0, "Two servers have the same host and port.");
      }
    }
  }
}

/**
 * @brief server構造体の情報を出力する、デバッグ用
 * TODO:これはserver構造体をclass化してそこにメソッド持たせた方が綺麗な気がする…？
 *      -> debug用なので余力があれば対応する
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
    while (it_location != _servers[i].locations.end())
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
