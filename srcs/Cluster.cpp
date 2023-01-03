#include "Cluster.hpp"


/**
 * @brief Configで渡されたサーバーが新規か既存か
 *        (Socketでlisten_fdを開放済みかどうか)をチェックして、
 *        未開放の場合は開けてから(新しいSocketを作ってから)_socketsに追加する。
 * @param config: configクラス
 * TODO: この関数の説明を山下くんに共有する
 */
Cluster::Cluster(Config config): _max_fd(0)
{
  std::vector<Server>&  servers = config._servers;

  for(size_t i = 0; i < servers.size(); i++)
  {
    socket_itr it = _get_server_socket(servers[i]);

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
 * @brief 引数で渡されたサーバーが新規か既存か
 *        (Socketでlisten_fdを開放済みかどうか)をチェックして、
 * @param server: server
 * @return socket_itr:  開放済みのSocketを見つけたらそのイテレータを返す。
 *                      見つからなければend()のイテレータを返す。
 * TODO: この関数の説明を山下くんに共有する
 */
Cluster::socket_itr Cluster::_get_server_socket(Server server)
{
  for(socket_itr s_it = _sockets.begin(); s_it != _sockets.end(); ++s_it)
  {
    if((s_it->get_port() == server.port) && (s_it->get_host() == server.host))
      return s_it;
  }
  return _sockets.end();
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
      const Response* response = c_it->get_response();

      if(response == NULL || phase == SEND)
        fd = c_it->get_sfd();
      else
        fd = response->get_fd();
      _max_fd = _max_fd > fd ? _max_fd : fd;
    }
  }
}

/**
 * @brief 全てのsocketに、selectするためのfdをセットする
 */
void  Cluster::_set_select_fds()
{
  _max_fd = 2;
  FD_ZERO(&_write_set);
  FD_ZERO(&_read_set);
  for(socket_itr s_it = _sockets.begin(); s_it != _sockets.end(); ++s_it)
    s_it->set_select_fd(_read_set, _write_set, _max_fd);
}

/**
 * @brief メインループ。
 *        select -> TODO: ここに処理の流れを書いておきたい。
 */
void Cluster::main_loop()
{
  int ret = 0;
  while(1)
  {
    struct timeval timeout;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    _set_select_fds();
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
