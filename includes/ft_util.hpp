#ifndef FT_UTIL_HPP
#define FT_UTIL_HPP

#include <climits>
#include <sstream>
#include <string>
#include <sys/socket.h>

namespace ft
{
  static const std::string CRLF = "\r\n";

  enum  filetype
  {
    DNE,
    FILE,
    DIRECTORY,
  };

  /**
 * @brief streamに流せるものをstringに変換する
 * @param n: streamに流せるもの(intなど)
 * @return 変換したstring
  */
  template <typename T> std::string to_string(const T& src)
  {
    std::ostringstream ostream;
    ostream << src;
    return ostream.str();
  }

  bool        is_upper(std::string src);
  bool        is_space(char c);
  std::string to_titlecase(std::string const& str);
  std::string trim_space(const std::string& str);
  std::string get_file_ext(const std::string& filepath);
  void        set_fd(int fd, fd_set& set, int& max_fd);
  bool        clear_fd(int fd, fd_set& set);
  ssize_t     unsigned_decimal_str_to_ssize_t(std::string str);
  ssize_t     hex_str_to_ssize_t(std::string str);
  bool        end_with(const std::string& str, char c);
  std::string clean_uri(std::string const& uri);
} // namespace ft

enum phase
{
  RECV,
  WRITE,
  READ,
  CGI,
  SEND,
  CLOSE
};

#endif
