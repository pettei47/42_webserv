#include "http_util.hpp"

namespace http
{
  /**
   * @brief httpステータスコードを返す
   */
  int StatusException::get_http_status() const
  {
    return _http_status;
  }

  /**
   * @brief TODO: この関数の説明を書く。
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
   * @brief TODO: この関数の説明を書く。
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
   * @brief TODO: この関数の説明を書く。
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
   * @brief TODO: この関数の説明を書く。
   */
  std::string generate_uri_head(const Server& s)
  {
    return "http://" + s.host + ":" + utility::to_string(s.port);
  }
} // namespace http
