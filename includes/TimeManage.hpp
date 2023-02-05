#ifndef TIMEMANAGE_HPP
#define TIMEMANAGE_HPP

#include <time.h>

class TimeManage
{
private:
  // 秒単位
  time_t _time;

public:
  static const int CGI_TIME_LIMIT = 10;
  TimeManage();
  ~TimeManage();

  void start();
  bool check() const;

private:
  TimeManage(TimeManage const& other);
  TimeManage& operator=(TimeManage const& other);
};

#endif
