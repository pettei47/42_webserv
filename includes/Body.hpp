#ifndef BODY_HPP
#define BODY_HPP

#include "http_util.hpp"
#include "Status.hpp"
#include "HttpString.hpp"
#include "Autoindex.hpp"
#include <cerrno>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @brief Httpのレスポンスメッセージに使用するボディ部分を作成するクラス
 */
class Body
{
private:
  static const int _buf_size = 8192;

  int         _fd;
  bool        _ongoing; // fdが有効で、read中の場合true
  HttpString  _body;
  size_t      _read_size;
  size_t      _body_size;

  bool _read_buf();

public:
  Body();
  ~Body();

  int               get_fd() const;
  bool              get_ongoing() const;
  const HttpString& get_body() const;
  char const*       get_body_data() const;
  size_t            get_all_body_size() const;
  size_t            get_body_size() const;

  void read_body();
  void _set_autoindex_body(std::string script_name, std::string dirpath);
  bool set_error_default_body(int status_code = 500);
  void close_fd();

  void set_fd(int fd);
  void set_body(const HttpString& data);
  void set_body(const std::string& data);
  void set_body_size(size_t file_size);

private:
  static std::string _generate_html(std::string title, std::string body);

  Body(const Body& other);
  Body& operator=(const Body& other);
};

#endif
