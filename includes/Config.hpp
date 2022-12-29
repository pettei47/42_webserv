#ifndef CONFIG_HPP
#define CONFIG_HPP

// #include "Parser.hpp"
// #include "Server.hpp"
// #include "Utility.hpp"
#include "WebServ.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class Config
{
  private:
    void      _parse_config(std::string filepath);
    void      _parse_server(std::string src, size_t line_start, size_t line_end);
    void      _parse_server_property(std::string src, size_t line, Server& server);
    void      _parse_location_property(std::string src, size_t line, Location& location);
    void      _validate_config(void);
    void      _complete_config(void);
    Server    _default_server(void);
    Location  _default_Location(void);
    Location  _parse_location(std::string src, size_t line_start, size_t line_end);
    bool      _check_redirect_status(std::string stat);
  public:
    std::vector<Server> _servers;
    Config(void);
    Config(std::string file);
    Config& oparator=(const Config& other);
    ~Config(void);
    std::vector<Server> get_servers(void);
    void  show_server_info(void);
};


#endif
