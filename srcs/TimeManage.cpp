#include "webserv.hpp"

TimeManage::TimeManage() { }

TimeManage::~TimeManage() { }

/**
 * @brief 計測の開始
 */
void TimeManage::start()
{
  _time = time(NULL);
}

/**
 * @brief 規定時間経過したか確認
 * @return 経過していた場合はtrue
 */
bool TimeManage::check() const
{
  time_t now = time(NULL);
  // _time < now しかありえないが、一応書いてる
  return _time < now && now - _time >= CGI_TIME_LIMIT;
}
