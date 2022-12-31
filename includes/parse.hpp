#ifndef PARSE_HPP
#define PARSE_HPP

#include "ft_util.hpp"
#include "webserv.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFFER_SIZE 256
extern const char* server_properties[];
extern const char* location_properties[];
extern const char* methods[];

enum  server_properties
{
  PROP_LISTEN,
  PROP_SERVER_NAME,
  PROP_ERROR_PAGE,
  PROP_SERVER_ROOT,
};

enum  location_properties
{
  PROP_METHOD,
  PROP_LOCATION_ROOT,
  PROP_AUTOINDEX,
  PROP_INDEX,
  PROP_CGI_PATH,
  PROP_UPLOAD_ENABLE,
  PROP_UPLOAD_PATH,
  PROP_CLIENT_MAX_BODY_SIZE,
  PROP_RETURN,
};

enum  http_methods
{
  HTTP_GET,
  HTTP_HEAD,
  HTTP_POST,
  HTTP_PUT,
  HTTP_DELETE,
  HTTP_CONNECT,
  HTTP_OPTIONS,
  HTTP_TRACE,
};

std::vector<std::string>  read_file(std::string file);
std::vector<std::string>  split(std::string src, std::string sep);
bool                      is_skip(std::string line);
size_t                    get_close_bracket_line(std::vector<std::string> contents, size_t start);
bool                      is_end_with_open_bracket(std::string line);
std::vector<std::string>  parse_property(std::string src, size_t line, std::string object);
bool                      is_property_name(std::string name, const char** valid_names);
bool                      is_method(std::string method);
size_t                    convert_to_size_t(std::string param, size_t line);
bool                      autoindex_to_bool(std::string param, size_t line);
std::string               replace(std::string src, std::string to_replace, std::string new_value);
bool                      check_path(std::string path);

class ParseException: public std::exception
{
  private:
    std::string _msg;

  public:
    ParseException(int line = 0, std::string msg = "Failed to parse the config file.")
      : _msg("Line: " + ft::to_string(line + 1) + ": " + msg){};
    ~ParseException() throw(){};
    const char* what() const throw()
    {
      return (_msg.c_str());
    };
};

#endif
