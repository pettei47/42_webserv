#ifndef FT_UTIL_HPP
#define FT_UTIL_HPP

#include <climits>
#include <sstream>
#include <string>
#include <sys/socket.h>

namespace ft_util
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
  template <typename T> std::string ft_to_string(const T& src)
  {
    std::ostringstream ostream;
    ostream << src;
    return ostream.str();
  }

  bool        ft_is_upper(std::string src);
  bool        ft_is_space(char c);
  std::string ft_to_titlecase(std::string const& str);
  std::string ft_trim_space(const std::string& str);
  std::string ft_get_file_ext(const std::string& filepath);
  void        ft_set_fd(int fd, fd_set& set, int& max_fd);
  bool        ft_isset_clear_fd(int fd, fd_set& set);
  size_t      ft_unsigned_decimal_str_to_ssize_t(std::string str);
  size_t      ft_hex_str_to_ssize_t(std::string str);
  bool        ft_end_with(const std::string& str, char c);
  std::string ft_clean_uri(std::string const& uri);
} // namespace ft_util

#endif
