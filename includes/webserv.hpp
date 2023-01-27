#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#if DEBUG_ACTIVE == 1
#	define DEBUG(x) std::cout << x << std::endl;
#else
#	define DEBUG(x)
#endif

#include <errno.h>
#include <exception>
#include <iostream>
#include <map>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <sys/time.h>
#include <vector>
#include <string>
#include <climits>
#include <sys/socket.h>
#include <cstdlib>

#include "ft_util.hpp"
#include "parse.hpp"
#include "Server.hpp"
#include "ft_signal.hpp"
#include "Config.hpp"
#include "tester.hpp"
#include "Cluster.hpp"

namespace webserv
{
  const static std::string DEFAULT_ROOT = "./path/";
};

class Log
{
public:
  Log(std::string msg, size_t id = 0)
  {
// #if DEBUG_MODE == 1
    struct timeval current_time;
    char time_buffer[100];
    bzero(time_buffer, 100);
    gettimeofday(&current_time, NULL);
    strftime(time_buffer, 100, "%F %R:%S", localtime(&current_time.tv_sec));
    if (id)
      std::cout << "[" << time_buffer << "] : [" << id << "] " << msg << std::endl;
    else
      std::cout << "[" << time_buffer << "] : " << msg << std::endl;
// #else
//     (void)msg;
//     (void)id;
// #endif
  }
};

#endif
