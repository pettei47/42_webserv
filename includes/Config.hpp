#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "parse.hpp"
#include "Server.hpp"
#include "ft_util.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class Config
{
  private:
    void      _parse_config(std::string filepath);
    void      _parse_server(size_t line_start, size_t line_end);
    void      _parse_server_property(size_t line, Server& server);
    Location  _parse_location(size_t line_start, size_t line_end);
    void      _parse_location_property(size_t line, Location& location);
    void      _validate_config();
    void      _complete_config();
    Server    _default_server();
    Location  _default_location();
    bool      _check_redirect_status(std::string status);

    std::vector<std::string>  _file_contents;

  public:
    Config();
    Config(std::string filepath);
    ~Config();
    Config&             operator=(const Config& other);
    std::vector<Server> get_servers();
    void                show_servers();
    std::vector<Server> _servers;
};

#endif
