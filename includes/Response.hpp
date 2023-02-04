#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <sys/socket.h>
#include <dirent.h>
#include <sstream>
#include "ft_util.hpp"
#include "http_util.hpp"
#include "HttpInfo.hpp"
#include "Body.hpp"
#include "Header.hpp"
#include "Message.hpp"
#include "Status.hpp"
#include "HttpString.hpp"
#include "CommonGatewayInterface.hpp"
#include "FileData.hpp"

class Response : public Message
{
private:
  int _fd;
  int _connection_fd;
  enum phase _phase;
  std::string _filepath;
  Status  _status;
  Header  _header;
  HttpInfo& _info;
  Body _body;
  CommonGatewayInterface _cgi;
  bool _first_response;
  size_t _send_size;
  bool _autoindex;
  FileData _filedata;

  std::string _status_line;

  bool _check_cgi();
  void _check_filepath();
  bool _check_return_redirect();
  void _set_error_filepath();
  void _set_filepath();
  std::string _set_upload_filename();

  bool _check_index_file();
  void _prepare_get_with_slash();
  void _prepare_get_no_slash();
  void _prepare_get();

  void _handle_error(int status_code);
  void _open_and_set_fd(bool read);
  void _prepare_read();
  void _check_write();
  void _prepare_write();
  void _handle_read_error(int status_code);
  void _handle_write_error(int status_code);
  void _read();
  void _write();
  void _handle_send_error(int status_code);
  void _set_header();
  void _first_send();
  void _send_content(char const* content, size_t size);
  void _send();
  void _prepare_CGI();
  void _handle_cgi(fd_set& read_set, fd_set& write_set);
  void _prepare_redirect(bool to_directory = false);
  void _handle_delete();
  void _first_preparation();

  void _retrieve_header();

  void _parse_message();
  void _parse_header();
  void _parse_body();

public:
  Response(HttpInfo& info, int connection_fd, int status_code);
  ~Response();

  enum phase handle_response();
  void set_select_fd(fd_set& read_set, fd_set& write_set, int& max_fd) const;
  enum phase check_and_handle(fd_set& read_set, fd_set& write_set);
  void show_response() const;

  int get_fd() const;

private:
  Response();
  Response(Response const& other);
  Response& operator=(Response const& other);
};

#endif
