#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "http_util.hpp"
#include "ft_util.hpp"
#include "HttpInfo.hpp"
#include "CommonGatewayInterface.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "webserv.hpp"
#include <vector>
#include <sys/socket.h>
#include <unistd.h>

class Connection
{
private:
  size_t                _id;
  int                   _cfd;  //connection用のfd
  enum phase            _phase;
  HttpInfo             _info;
  Response*             _response;
  std::vector<Server>&  _servers;
  Request               _request;


  void    _make_response(int status_code);
  ssize_t _recv();

  static const size_t _max_buffer_size = (1 << 14);
  static size_t       _total_connections;

  char _buf[_max_buffer_size + 1];

public:
  Connection(int fd, std::vector< Server >& servers);
  ~Connection();
  Connection(Connection const& other);
  Connection& operator=(Connection const& other);

  void  recv_request();
  void  close() const;
  void  set_select_fd(fd_set& read_set, fd_set& write_set, int& max_fd) const;
  bool  check_and_handle(fd_set& read_set, fd_set& write_set);

  int             get_cfd() const;
  enum phase      get_phase() const;
  const Response* get_response() const;
};

#endif
