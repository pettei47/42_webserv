#ifndef HTTP_UTIL_HPP
#define HTTP_UTIL_HPP

#include "ft_util.hpp"
#include "Server.hpp"
#include <cctype>
#include <ctime>
#include <string>

struct Server;

namespace http
{
  class StatusException : public std::exception
  {
  public:
    static const int CLOSE = 600;

  private:
    int         _status;
    std::string _msg;

  public:
    StatusException(int status)
      : _status(status)
      , _msg("HttpError: " + ft::to_string(status)){};
    ~StatusException() throw(){};
    const char* what() const throw()
    {
      return (_msg.c_str());
    };
    int get_status() const;
  };

  bool        is_token(std::string& str);
  bool        is_comma_separated_list(std::string const& value);
  std::string get_current_time();
  std::string generate_uri_head(const Server& s);

} // namespace http

#endif /* HTTP_UTIL_HPP */
