#include "webserv.hpp"

namespace http
{
  /**
   * @brief httpステータスコードを返す
   */
  int StatusException::get_status() const
  {
    return _status;
  }

  /**
   * @brief ヘッダーのKEY部分が適切な文字でできているかを調べる
   */
  bool  is_token(std::string& str)
  {
    for(size_t i = 0; str[i]; ++i)
    {
      if(std::isalpha(str[i]))
        continue;
      if(std::isdigit(str[i]))
        continue;
      if(str[i] == '!' || str[i] == '#' || str[i] == '$' || str[i] == '%' || str[i] == '&' ||
          str[i] == '\'' || str[i] == '*' || str[i] == '+' || str[i] == '-' || str[i] == '.' ||
          str[i] == '^' || str[i] == '_' || str[i] == '`' || str[i] == '|' || str[i] == '~')
        continue;
      return false;
    }
    return true;
  }

  /**
   * @brief 渡されたstringがカンマ区切りになっているかを見る
   */
  bool is_comma_separated_list(std::string const& value)
  {
    int in_quote = -1;
    int in_comment = -1;

    for(size_t i = 0; i < value.length(); ++i)
    {
      if(value[i] == '"')
        in_quote *= -1;
      else if(value[i] == '(')
        in_comment = 1;
      else if(value[i] == ')')
        in_comment = -1;
      else if(in_quote < 0 && in_comment < 0 && value[i] == ',')
        return true;
    }
    return false;
  }

  /**
   * @brief 現在時刻を取得
   */
  std::string get_current_time()
  {
    time_t  rawtime;
    char    now[80];

    time(&rawtime);
    std::strftime(now, 80, "%a, %d %b %Y %T GMT", std::gmtime(&rawtime));
    return std::string(now);
  }

  /**
   * @brief URLのポートまでの部分を作る
   */
  std::string generate_uri_head(const Server& s)
  {
    return "http://" + s.host + ":" + ft::to_string(s.port);
  }
} // namespace http
