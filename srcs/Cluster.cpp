#include "Cluster.hpp"

Cluster::Cluster(Config config): _max_fd(0)
{
  std::vector<Server>&  servers = config._servers;

  for(size_t i = 0; i < servers.size(); i++)
  {
    socket_itr it = _check_virtual(servers[i]);
    if(it != _sockets.end())
      it->add_server(servers[i]);
    else
    {
      Socket sc(servers[i]);
      sc.open_listenfd();
      sc.add_server(servers[i]);
      _sockets.push_back(sc);
    }
  }
}

Cluster::~Cluster(){}

/**
 * @brief TODO: この関数の説明を書く。
 *  いつ使われているか。何が目的か。（場合によっては関数名を変える）
 */
Cluster::socket_itr Cluster::_check_virtual(Server s)
{
  socket_itr s_it_end = _sockets.end();
  for(socket_itr s_it = _sockets.begin(); s_it != s_it_end; ++s_it)
  {
    if((s_it->get_port() == s.port) && (s_it->get_host() == s.host))
      return s_it;
  }
  return s_it_end;
}

/**
 * @brief _max_fdをセットする。
 */
void  Cluster::_set_max_fd()
{
  _max_fd = 2;
  for(socket_itr s_it = _sockets.begin(); s_it != _sockets.end(); ++s_it)
  {
    int fd = s_it->get_lfd();
    _max_fd = _max_fd > fd ? _max_fd : fd;
    for(Socket::connection_citr c_it = s_it->get_connections().begin();
      c_it != s_it->get_connections().end(); ++c_it)
    {
      enum phase phase = c_it->get_phase();
      const HttpResponse* response = c_it->get_response();

      if(response == NULL || phase == SEND)
        fd = c_it->get_sfd();
      else
        fd = response->get_fd();
      _max_fd = _max_fd > fd ? _max_fd : fd;
    }
  }
}

/**
 * @brief TODO: この関数の説明を書く。
 */
void  Cluster::_set_select_fd()
{
  _max_fd = 2;
  FD_ZERO(&_write_set);
  FD_ZERO(&_read_set);
  for(socket_itr s_it = _sockets.begin(); s_it != _sockets.end(); ++s_it)
    s_it->set_select_fd(_read_set, _write_set, _max_fd);
}

/**
 * @brief TODO: この関数の説明を書く。
 */
void Cluster::loop()
{
  int ret = 0;
  while(1)
  {
    struct timeval timeout;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    _set_select_fd();
    Log(ft::to_string(_max_fd));
    ret = select(_max_fd + 1, &_read_set, &_write_set, NULL, &timeout);
    if(ret < 0)
    {
      Log("Select Error");
      break; //main loop end
    }
    for(socket_itr s_it = _sockets.begin(); s_it != _sockets.end(); ++s_it)
    {
      s_it->check_and_handle(_read_set, _write_set);
      usleep(_usleep_time);
    }
    ret = 0;
  }
}
