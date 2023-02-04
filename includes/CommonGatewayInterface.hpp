#ifndef CGIPYTHON_HPP
#define CGIPYTHON_HPP

#include <cstdlib>
#include <cstring>
#include <string>
#include "http_util.hpp"
#include "Body.hpp"
#include "Header.hpp"
#include "HttpInfo.hpp"
#include "Status.hpp"
#include "EnvVar.hpp"
#include "FileData.hpp"
#include "HttpString.hpp"
#include "TimeManage.hpp"
#include "ft_util.hpp"

class CommonGatewayInterface
{
private:
  enum cgiphase
  {
    ENV,
    ARG,
    PIPE,
    PROCESS
  };

  static const int BUF_SIZE = 8192;

  int           _write_fd;
  int           _read_fd;
  pid_t         _ch_pid;
  HttpString    _data;
  Status&       _status;
  Header&       _header;
  Body&         _body;
  HttpInfo&     _info;
  std::string&  _filepath;
  TimeManage    _time;
  size_t        _sent_size;

  void  _check_cgi_path();
  void  _check_execute_file();
  void  _set_env(EnvVar& env);
  void  _make_arg(char*** arg);
  void  _set_pipe(int pipe_c2p[2], int pipe_p2c[2]);
  void  _make_process();
  void  _delete_resources(char** arg, int* pipe_c2p, int* pipe_p2c, int flag);
  void  _child_process(int pipe_c2p[2], int pipe_p2c[2], char** arg, char** env);
  void  _set_fd(int pipe_c2p[2], int pipe_p2c[2]);
  void  _first_preparation_delete(char** env, char** arg);
  void  _read();
  void  _write();
  void  _read_or_write_error(int status);
  void  _kill_process();

public:
  CommonGatewayInterface(HttpInfo& info,
        Status& httpstatus,
        Header& httpheader,
        Body& body,
        std::string& filepath);
  ~CommonGatewayInterface();

  enum phase  first_preparation();
  void        set_select_fd(fd_set& read_set, fd_set& write_set, int& max_fd) const;
  enum phase  check_and_handle(fd_set& read_set, fd_set& write_set);

  HttpString  get_data() const;

private:
  CommonGatewayInterface();
  CommonGatewayInterface(CommonGatewayInterface const& other);
  CommonGatewayInterface& operator=(CommonGatewayInterface const& other);
};

#endif
