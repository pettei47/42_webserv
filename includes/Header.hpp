#ifndef HEADER_HPP
#define HEADER_HPP

#include "ft_util.hpp"
#include <iostream>
#include <map>
#include <string>

/**
 * @brief Httpのレスポンスメッセージに使用するヘッダーのクラス
 * @note TODO: 追加すべきヘッダーの項目については調べられていない
 */

class Header
{
  typedef std::map<std::string, std::string> header;
  typedef std::map<std::string, std::string>::iterator headerItr;
  typedef std::map<std::string, std::string>::const_iterator headerItrConst;

  private:
    /**
     * @brief コンテントのタイプとヘッダーでのコンテントタイプを対応付けるための構造体
     */
    struct ContentType
    {
      header  _types;
      ContentType();
    };

    static struct ContentType _content_type;

    header  _headers;

    const std::string&  _ext_to_content_type(const std::string& ext) const;
    void                _set_default_header();

  public:
    Header();
    ~Header();

    Header(const Header& other);
    Header& operator=(const Header& other);
    std::string&  operator[](std::string key);
    const std::string&  at(const std::string& key) const;

    void              set_content_type(const std::string& file);
    void              set_content_length(size_t length);
    ssize_t           get_content_length();
    const std::string to_string() const;

    void  show_headers() const;
    bool  contains(std::string key) const;
    void  append(std::string key, std::string value);
    void  append(std::pair<std::string, std::string> key_value);

    std::string content_type_to_ext() const;
};

#endif
