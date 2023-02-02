#include "webserv.hpp"

static void _exit_webserv(int sig)
{
  (void)sig;
  std::exit(0);
}

/**
 * @brief signalをセットする
 */
void set_signal()
{
  signal(SIGABRT, _exit_webserv);
  signal(SIGINT, _exit_webserv);
  signal(SIGTERM, _exit_webserv);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
}


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

    Cluster Cluster(config); // Socketをopen(listen)する。
    Cluster.main_loop();
  }
  catch(std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
  return (0);
}
