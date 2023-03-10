#include "webserv.hpp"

/**
 * @brief Configで渡されたサーバーが新規か既存か
 *        (Socketでlisten_fdを開放済みかどうか)をチェックして、
 *        未開放の場合は開けてから(新しいSocketを作ってから)_socketsに追加する。
 * @param config: configクラス
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
 *        Socketは開いた状態（＝listenは開始済み）
 *        -> selectでfdが読み書き可能かどうかチェック
 *        -> ソケットごとにコネクションを作って
 */
void Cluster::main_loop()
{
  int ret = 0;
  int loopcnt = 0;
  const int TIMEOUT = TimeManage::CGI_TIME_LIMIT + 10; // CGIは10secで切り上げ
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
    if (ret == 0){ // すべてのconnectionについて規定回数変化がない場合、すべてのconnectionを閉じる
      ++loopcnt;
      if (loopcnt > TIMEOUT){
        for(socket_itr s_it = _sockets.begin(); s_it != _sockets.end(); ++s_it){
          s_it->close_connection();
        }
        loopcnt = 0;
      }
    }
    else{
      loopcnt = 0;
    }
    for(socket_itr s_it = _sockets.begin(); s_it != _sockets.end(); ++s_it)
    {
      s_it->check_and_handle(_read_set, _write_set);
      usleep(_usleep_time);
    }
    ret = 0;
  }
}
