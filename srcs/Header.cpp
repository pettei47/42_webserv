#include "Header.hpp"

Header::Header()
{
  _set_default_header();
}

Header::Header(const Header& other)
{
  *this = other;
}

Header& Header::operator=(const Header& other)
{
  if(this != &other)
    _headers = other._headers;
  return *this;
}

Header::~Header(){}

std::string&  Header::operator[](std::string key)
{
  if(_headers.find(key) == _headers.end())
    _headers[key] = "";
  return _headers[key];
}

const std::string&  Header::at(const std::string& key) const
{
  return _headers.at(key);
}

Header::ContentType::ContentType()
{
  _types["txt"] = "text/plain";
  _types["csv"] = "text/csv";
  _types["html"] = "text/html";
  _types["css"] = "text/css";
  _types["jpg"] = "image/jpeg";
  _types["png"] = "image/png";
  _types["gif"] = "image/gif";
  _types["error"] = "";
}

struct Header::ContentType Header::_content_type;

/**
 * @brief 拡張子をヘッダーでのコンテントタイプに変換
 * @param ext: 拡張子
 * @return ヘッダーでのコンテントタイプ。リストにない場合は""を返す
 * @note 一覧にないtypeが来た場合は、""で返す
 */
const std::string&  Header::_ext_to_content_type(const std::string& ext) const
{
  if(_content_type._types.find(ext) == _content_type._types.end())
    return _content_type._types["error"];
  return _content_type._types[ext];
}

/**
 * @brief コンテントタイプを拡張子に変換
 * @return 対応する拡張子
 */
std::string Header::content_type_to_ext() const
{
  if(!contains("Content-Type") || _headers.at("Content-Type").empty())
    return "txt";
  for(headerItr it = _content_type._types.begin();
      it != _content_type._types.end(); ++it)
  {
    if(_headers.at("Content-Type").compare(it->second) == 0)
      return it->first;
  }
  return "";
}

/**
 * @brief headerでデフォルトで設定されているものを格納する
 * @note TODO: デフォルトについてはまだ詳しく調べられていない
 */
void Header::_set_default_header()
{
  _headers["Server"] = "webserv";
}

/**
 * @brief ファイルのパス、ファイル名から、Content-Typeを設定する
 * @param file: ファイルのパス、ファイル名
 */
void  Header::set_content_type(const std::string& file)
{
  const std::string ext = ft::get_file_ext(file);
  _headers["Content-Type"] = _ext_to_content_type(ext);
  if(_headers["Content-Type"].empty())
    _headers["Content-Type"] = "text/html";
}

/**
 * @brief contentのサイズからContent-Lengthを設定する
 * @param length: contentのサイズ
 */
void Header::set_content_length(size_t length)
{
  _headers["Content-Length"] = ft::to_string(length);
  // TODO: to_stringって必要か？調べる。
}

/**
 * @brief contentのサイズからcontent-lengthを設定する
 * @param length: contentのサイズ
 */
ssize_t Header::get_content_length()
{
  std::string str_value = _headers["Content-Length"];
  return ft::unsigned_decimal_str_to_ssize_t(str_value);
}

/**
 * @brief ヘッダーをstringとして返す
 * @return ヘッダーのstring。終端はCRLFCRLF
 */
const std::string Header::to_string() const
{
  std::string header_str;

  for(headerItrConst it = _headers.begin();
      it != _headers.end(); ++it)
  {
    header_str += it->first + ": " + it->second;
    header_str += "\r\n";
  }
  header_str += "\r\n";
  return header_str;
}

/**
 * @brief TODO: この関数の説明を書く
 */
void  Header::show_headers() const
{
  for(headerItrConst it = _headers.begin();
      it != _headers.end(); ++it)
    std::cout << "  - " << it->first << ": " << it->second << std::endl;
}
/**
 * @brief TODO: この関数の説明を書く
 */
bool  Header::contains(std::string key) const
{
  return (_headers.find(key) != _headers.end());
}

/**
 * @brief TODO: この関数の説明を書く
 */
void Header::append(std::string key, std::string value)
{
  if(!contains(key))
    _headers[key] = value;
  else
  {
    std::string old_value = _headers[key];

    if(!old_value.empty() && old_value.at(old_value.size() - 1) != ',')
      value = ',' + value;
    _headers[key] += value;
  }
}

/**
 * @brief TODO: この関数の説明を書く
 * TODO: これ必要か？確認する
 */
void Header::append(std::pair<std::string, std::string> key_value)
{
  append(key_value.first, key_value.second);
}
