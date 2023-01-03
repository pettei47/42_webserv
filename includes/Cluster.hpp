#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Config.hpp"
#include "server.hpp"
#include "Socket.hpp"
#include "webserv.hpp"
#include <list>
#include <sys/socket.h>
#include <unistd.h>

class Cluster
{
public:
	typedef std::list<Socket>::iterator socket_itr;

private:
	std::list<Socket> _sockets;
	int               _max_fd;
	fd_set            _read_set;
	fd_set            _write_set;

	void        _set_max_fd();
	socket_itr  _get_server_socket(Server s);
	void        _set_select_fds();
	void        _register_socket(Server s);

	static const useconds_t _usleep_time = 500;

public:
	Cluster(Config config);
	~Cluster();
	void main_loop();
};

#endif
