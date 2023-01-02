#include "Cluster.hpp"

Cluster::Cluster(Config config)
  : _max_fd(0)
{
  std::vector<Server>& servers = config._servers;
  for(size_t i = 0; i < servers.size(); i++)
  {
    socket_itr it = _check_virtual(servers[i]);
    if(it != _sockets.end())
    {
      it->add_server(servers[i]);
    }
    else
    {
      Socket sc(servers[i]);
      sc.open_listenfd();
      sc.add_server(servers[i]);
      _sockets.push_back(sc);
    }
  }
}
