#include "webserv.hpp"

int main(int argc, char **argv)
{
#if TEST_MODE == 1
  return health_check();
#endif
  Config  config;
  char*   path = (char *)"conf/default.conf";

  if (argc > 2)
  {
    std::cerr << "Usage: ./webserv <config file path>" << std::endl;
    return (0);
  }
  else if (argc == 2)
    path = argv[1];
  try
  {
    set_signal();
    config = Config(path);
#if DEBUG_MODE == 1
    config.show_servers();
#endif
    Log("Start Server...");

    // Cluster Cluster(config);
    // Cluster.main_loop();
  }
  catch(std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
  return (0);
}
