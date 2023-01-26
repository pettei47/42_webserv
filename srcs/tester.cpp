#include "webserv.hpp"

template <typename T, typename S, typename U>
void perr(T t, S s, U u) {
  std::cerr << "[" << t << "]" << s << "/" << u << std::endl;
}

bool health_check(){
  bool ret = false;

  try {
    size_t test = convert_to_size_t("012", 0);
    if (test != 12){
      ret = true;
      perr("convert_to_size_t(012, 0)", 12, ret);
    }
  } catch (const std::exception& e) {
      ret = true;
      std::cerr << "std::exception: " << e.what() << std::endl;
  }

  try {
    size_t test = convert_to_size_t("abc", 0);
    if (test != 0){
      ret = true;
      perr("convert_to_size_t(abc, 0)", 0, ret);
    }
  } catch (const std::exception& e) {
      std::cerr << "std::exception: " << e.what() << std::endl;
  }

  try {
    size_t test = convert_to_size_t("-1", 0);
    if (test != 0){
      ret = true;
      perr("convert_to_size_t(-1, 0)", -1, ret);
    }
  } catch (const std::exception& e) {
      std::cerr << "std::exception: " << e.what() << std::endl;
  }

  try {
    size_t test = convert_to_size_t("", 0);
    if (test != 0){
      ret = true;
      perr("convert_to_size_t(, 0)", 0, ret);
    }
  } catch (const std::exception& e) {
      std::cerr << "std::exception: " << e.what() << std::endl;
  }

  return ret;
}
