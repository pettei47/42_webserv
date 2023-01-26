#include "ft_util.hpp"

namespace ft
{
  /**
   * @brief 文字列が全て大文字ならtrue, そうでなければfalseを返す
   * @param str: 文字列
   */
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

  /**
   * @brief 文字列をTitleCaseに変換する
   * @param str: 文字列
   */
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

  /**
   * @brief スペースとタブ文字を判定する
   * @param c: 文字
   */
  bool  is_space(char c)
  {
    return (c == ' ' || c == '\t');
  }

  /**
   * @brief 文字列の前後の空白スペースをトリムする
   * @param str: 文字列
   */
  std::string trim_space(const std::string& str)
  {
    size_t  first = str.find_first_not_of(" \t");

    if (first == std::string::npos)
      return "";
    size_t  last = str.find_last_not_of(" \t");

    return str.substr(first, last - first + 1);
  }

  /**
   * @brief ファイルの拡張子を取る
   * @param filepath: ファイルパス
   */
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
  bool clear_fd(int fd, fd_set& set)
  {
    if(!FD_ISSET(fd, &set)) // fdがsetに含まれていない場合0が返る
      return false;
    FD_CLR(fd, &set); // fdをsetから削除する
    return true;
  }

  /**
   * @brief 符号なしstringをssize_tに変換する
   * @param str: 変換するstring
   * @return 数字以外の文字があった場合、SSIZE_MAXを超える場合-1
   * TODO: これHeaderでしか使ってないのでHeaderの中に実装する
   */
  ssize_t unsigned_decimal_str_to_ssize_t(std::string str)
  {
    if(str.compare("0") == 0)
      return 0;

    size_t ret = 0;
    int i = 0;
    for(; str[i] && std::isdigit(str[i]); ++i)
    {
      ret *= 10;
      ret += str[i] - '0';
      if(ret > SSIZE_MAX)
        return -1;
    }
    if(str[i] || ret == 0)
      return -1;

    return ret;
  }

  /**
   * @brief hex-stringをssize_tに変換する
   * @param str: 変換するstring
   * @return 数字以外の文字があった場合、SSIZE_MAXを超える場合-1
   * TODO: これRequestでしか使ってないのでRequestの中に実装する
   */
  ssize_t hex_str_to_ssize_t(std::string str)
  {
    if(str.compare("0") == 0)
      return 0;

    size_t ret = 0;
    int i = 0;
    for(; str[i] && std::isxdigit(str[i]); ++i)
    {
      ret *= 16;
      if(std::isdigit(str[i]))
        ret += str[i] - '0';
      else
        ret += std::tolower(str[i]) - 'a' + 10;
      if(ret > SSIZE_MAX)
        return -1;
    }
    if(str[i] || ret == 0)
      return -1;

    return ret;
  }

  /**
   * @brief 説明書く
   * @param s: string
   * @param c: char
   * @return あとで書く
   * TODO: これRequestとResponseでしか使ってないのでhttp_utilの中に実装する
   */
  bool end_with(const std::string& s, char c)
  {
    if(s.size() == 0)
      return false;
    return (s[s.size() - 1] == c);
  }

  /**
   * @brief 説明書く
   * @param uri: string
   * @return あとで書く
   * TODO: これResponseでしか使ってないのでResponseの中に実装する
   */
  std::string clean_uri(std::string const& uri)
  {
    std::string new_uri;
    size_t size = uri.size();
    if(size == 0)
      return std::string("");
    for(size_t i = 0; i + 1 < size; ++i)
    {
      if(uri[i] == '/' && uri[i + 1] == '/')
        continue;
      new_uri += uri[i];
    }
    new_uri += uri[size - 1];
    return new_uri;
  }

}
