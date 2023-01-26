#ifndef STATUS_HPP
#define STATUS_HPP

#include "ft_util.hpp"
#include <map>
#include <string>

/**
 * @brief Httpのレスポンスメッセージに使用するステータス行のクラス
 * @note 追加すべきステータスコードについては調べられていない
 */
class Status
{
private:
  /**
   * @brief ステータスコードとそのテキストを対応付けるための構造体
   */
  struct StatusDescription
  {
    std::map< int, std::string > description;
    StatusDescription();
  };

  static struct StatusDescription _status_description;

  const std::string _protocol_version;
  int _status_code;

public:
  Status();
  Status(const std::string& protocol_version, int status_code);
  ~Status();

  static const std::string& status_code_to_string(int code);
  void set_status_code(int code);
  int get_status_code() const;

  bool is_success() const;
  bool is_redirect() const;

  const std::string to_string() const;

private: /* for now: preventing bugs */
  Status(const Status& other);
  Status& operator=(const Status& other);
};

#endif
