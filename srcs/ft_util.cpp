#include "ft_util.hpp"

namespace ft
{
  bool is_upper(std::string str)
  {
    size_t len = str.length();
    for (size_t i = 0; i < len; i++)
    {
      if (!std::isupper(str[i]))
        return false;
    }
    return true;
  }

  std::string to_titlecase(std::string const& str)
  {
    std::string ret;
    size_t      size = str.size();
    bool        isupper = true;

    for (size_t i = 0; i < size; ++i)
    {
      if (isupper)
      {
        ret += std::toupper(str[i]);
        isupper = false;
        continue;
      }
      if (str[i] == '-')
        isupper = true;
      ret += std::tolower(str[i]);
    }
    return ret;
  }

  bool  is_space(char c)
  {
    return (c == ' ' || c == '\t');
  }

  std::string trim_space(const std::string& str)
  {
    size_t  first = str.find_first_not_of(" \t");

    if (first == std::string::npos)
      return "";
    size_t  last = str.find_last_not_of(" \t");

    return str.substr(first, last - first + 1);
  }

  std::string get_file_ext(const std::string& filepath)
  {
    size_t  dot_index = filepath.find_last_of(".");

    if (dot_index == std::string::npos)
      return "";
    return filepath.substr(dot_index + 1);
  }

  /**
   * @brief fd_setにfdをセットしてmax_fdを更新する
   * @param fd: ファイルディスクリプタ
   * @param set:  selectで使用するファイルディスクリプタのセット
   *              FD_ZERO済みの状態で渡される。
   * @param max_fd: ファイルディスクリプタの最大値
   */
  void set_fd(int fd, fd_set& set, int& max_fd)
  {
    max_fd = std::max(max_fd, fd);
    FD_SET(fd, &set);
  }

  /**
   * @brief FD_ISSETでtrueだった場合FD_CLRを行う
   * @param fd: ファイルディスクリプタ
   * @param set: selectで使用するファイルディスクリプタのセット
   */
  bool clr_fd(int fd, fd_set& set)
  {
    if(!FD_ISSET(fd, &set)) // fdがsetに含まれていない場合0が返る
      return false;
    FD_CLR(fd, &set); // fdをsetから削除する
    return true;
  }

}
