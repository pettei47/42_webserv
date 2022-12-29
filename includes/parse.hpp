#ifndef PARSE_HPP
#define PARSE_HPP

#include "webserv.hpp"

#define BUFFER_SIZE 256
extern const char* server_properties[];
extern const char* route_properties[];
extern const char* methods[];

enum  server_properties
{
  PROP_LISTEN,
  PROP_SERVER_NAME,
  PROP_ERROR_PAGE,
  PROP_SERVER_ROOT,
};

enum  route_properties
{
  PROP_METHOD,
  PROP_ROUTE_ROOT,
  PROP_AUTOINDEX,
  PROP_INDEX,
  PROP_CGI_PATH,
  PROP_UPLOAD_ENABLE,
  PROP_UPLOAD_PATH,
  PROP_CLIENT_MAX_BODY_SIZE,
  PROP_RETURN,
  PROP_ZERO,
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
  HTTP_ZERO,
};

size_t                    count_lines(std::string src);
size_t                    get_closing_bracket(std::string src, size_t line);
std::vector<std::string>  parse_property(std::string src, size_t line, std::string object);
std::vector<std::string>  split_white_space(std::string src);
bool                      is_property_name(std::string name, const char** valid_names);
std::string               read_file(std::string file);
std::string               get_line(std::string, size_t line);
bool                      is_skip(std::string src, size_t line);
bool                      end_with_open_bracket(std::string src, size_t line);
size_t                    convert_to_size_t(std::string param, size_t line);
bool                      param_to_bool(std::string param, size_t line);
bool                      is_method(std::string method);
std::string               replace(std::string src, std::string to_replace, std::string new_value);
bool                      check_path(std::string path);

class ParseException: public std::exception
{
  private:
    std::string _msg;

  public:
    ParseException(int line = 0, std::string msg = "Failed to parse the config file.");
    //  : _msg("Line: " + ft::to_string(line + 1) + ": " + msg){}; // この書き方よくわからん
    ~ParseException() throw(){};
    const char* what() const throw()
    {
      return (_msg.c_str());
    };
};

#endif
