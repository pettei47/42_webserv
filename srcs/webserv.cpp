#include "webserv.hpp"
#include "ft_signal.hpp"
#include "Config.hpp"

int main(int argc, char **argv)
{
  Config  config;
  char*   path;

	if (argc > 2)
	{
		std::cerr << "Usage: ./webserv <config file path>" << std::endl;
		return (0);
	}
  else if (argc != 2)
    path = "conf/default.conf";
  else
    path = argv[1];
  try
  {
    set_signal();
    config = Config(path);
#if DEBUG == 1
		config.show_server_info();
#endif
    Log("Start Server(s)...");

    Cluster Cluster(config);
    Cluster.loop();
  }
  catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return (0);
}
