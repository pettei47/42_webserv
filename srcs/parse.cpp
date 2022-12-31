#include "parse.hpp"

const char* server_properties[5] = {
  "listen",
  "server_name",
  "error_page",
  "root",
  0,
};

const char* location_properties[11] = {
  "method",
  "root",
  "autoindex",
  "index",
  "cgi_path",
  "upload_enable",
  "upload_path",
  "client_max_body_size",
  "return",
  0,
};

const char* methods[9] = {
  "GET",
  "HEAD",
  "POST",
  "PUT",
  "DELETE",
  "CONNECT",
  "OPTIONS",
  "TRACE",
  0,
};

/**
 * @brief contextの終了位置を返す
 * @param src: 設定ファイルの文字列
 * @param start: contextの開始位置
 * @return contextの終了位置
 * @example "server {\na\nb\nc\n}" with line 0 will return line 4
 */
size_t  get_close_bracket_line(std::vector<std::string> file_contents, size_t start)
{
  size_t      ret = start + 1;
  size_t      open_brackets = 0;

  if (!is_end_with_open_bracket(file_contents[start]))
    throw ParseException(start, "Expected '{'.");
  while (ret < file_contents.size())
  {
    if (!is_skip(file_contents[ret]))
    {
      if (is_end_with_open_bracket(file_contents[ret]))
        open_brackets++;
      if (file_contents[ret] == "}")
      {
        if (open_brackets == 0)
          return (ret);
        open_brackets--;
      }
    }
    ret++;
  }
  return (0);
}

/**
 * @brief 該当の行が"{" で終わっているかチェックする
 * @param src: 設定ファイルの文字列
 * @param n: チェックする位置
 * @return True/False
 */
bool  is_end_with_open_bracket(std::string line)
{
  std::vector<std::string>  line_items = split(line, " ");

  if (line_items.empty())
    return (false);
  return line_items[line_items.size() - 1] == "{";
}

/**
 * @brief ファイルの中身を改行でsplitしてstd::stringのvectorにして返す
 * @param filepath: fileパス
 * @return std::vector<std::string>
 */
std::vector<std::string>  read_file(std::string filepath)
{
  char        buffer[BUFFER_SIZE + 1] = {};
  int         fd = open(filepath.c_str(), O_RDONLY);
  int         rc;
  std::string file_content;

  if (fd < 0)
    throw ParseException(0, "failed tp open " + filepath);
  while ((rc = read(fd, buffer, BUFFER_SIZE)) > 0)
  {
    buffer[rc] = '\0';
    file_content += buffer;
  }
  if (rc < 0)
    throw ParseException(0, "failed to read " + filepath);
  close(fd);
  return (split(file_content, "\n"));
}

/**
 * @brief contentをseparatorでsplitしてstd::stringのvectorにして返す
 * @param content: 分割したい文字列
 * @param separator: 区切り文字
 * @return std::vector<std::string>
 */
std::vector<std::string>  split(std::string content, std::string separator)
{
  size_t  separator_length = separator.length();
  std::vector<std::string> ret;

  if (separator_length == 0) {
    ret.push_back(content);
  } else {
    size_t offset = std::string::size_type(0);
    while (1) {
      size_t pos = content.find(separator, offset);
      if (pos == std::string::npos) {
        ret.push_back(ft::trim_space(content.substr(offset)));
        break;
      }
      ret.push_back(ft::trim_space(content.substr(offset, pos - offset)));
      offset = pos + separator_length;
    }
  }
  return (ret);
}

/**
 * @brief parameterが適切かのチェックを行い、paramsを分割してvectorで返す
 * @example "listen 80 localhost;"  -> "listen", "80" "localhost" のvector
 */
std::vector<std::string>  parse_property(std::string line, size_t n, std::string directive)
{
  std::vector<std::string>  result;

  if (line[line.size() - 1] != ';')
    throw ParseException(n, "Expected ';'.");
  line.erase(line.size() - 1, 1);
  result = split(line, " ");
  if (result.size() < 2)
    throw ParseException(n, "Properties should have at least one value.");
  if (!is_property_name(result[0], directive == "server" ? server_properties : location_properties))
    throw ParseException(n, "`" + result[0] +"` is invalid property for " + directive + ".");
  return (result);
}

/**
 * @brief 読み込んだparameterが適切かチェックする
 */
bool  is_property_name(std::string property_name, const char** valid_name_list)
{
  for (size_t i = 0; valid_name_list[i]; i++)
  {
    if (property_name == valid_name_list[i])
      return (true);
  }
  return (false);
}

/**
 * @brief 該当行がスキップ可能かチェックする（空白行 or #コメント行）
 * @param line: confファイルの該当行の内容
 * @return スキップ可能か否か
 */
bool  is_skip(std::string line)
{
  return (
    line.size() == 0 ||
    line[0] == '#' ||
    split(line, " ").size() == 0
  );
}

/**
 * @brief std::stringをsize_tに変換
 */
// TODO: 負の数渡したときどうなるのか確認。
size_t  convert_to_size_t(std::string item, size_t line_num)
{
  size_t              value;
  std::istringstream  convert(item);

  if (!(convert >> value) || item[0] == '-')
    throw ParseException(line_num, "`" + item + "` is not a positive integer");
  return (value);
}

/**
 * @brief methodに書かれている内容がHTTPメソッドかチェックする
 */
bool  is_method(std::string method)
{
  for (size_t i = 0; methods[i]; i++)
  {
    if (method == methods[i])
      return (true);
  }
  return (false);
}

/**
 * @brief autoindexの on/offをboolに変換する
 */
bool  autoindex_to_bool(std::string autoindex, size_t line_num)
{
  if (autoindex == "on")
    return (true);
  else if (autoindex == "off")
    return (false);
  else
    throw ParseException(line_num, "autoindex parameter should be \"on\" or \"off\".");
}

/**
 * @brief sed -i -e 's/to_replace/new_value/g' source
 */
std::string replace(std::string src, std::string to_replace, std::string new_value)
{
	size_t start = 0;
	while((start = src.find(to_replace, start)) != std::string::npos)
	{
		src.replace(start, to_replace.length(), new_value);
		start += new_value.length();
	}
	return (src);
}

/**
 * @brief ファイルの存在を確認する
 * @ref https://www.c-lang.net/stat/index.html
 */
bool check_path(std::string path)
{
	struct stat buffer;

	int exist = stat(path.c_str(), &buffer);
	if(exist != 0)
    return (false);
	//通常のファイルの場合
	if(S_ISREG(buffer.st_mode))
		return (true);
	//ディレクトリの場合
	else
		return (false);
}
