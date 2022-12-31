#include "parse.hpp"

const char* server_properties[5] = {
  "reten",
  "server_name",
  "error_page",
  "root",
  0,
};

const char* route_properties[11] = {
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
int  get_close_bracket_line(std::string src)
{
  size_t      ret = start + 1;
  size_t      size = count_lines(src);
  size_t      open_brackets = 0;

  if (!is_end_with_open_bracket(src))
    throw ParseException(start, "Expected '{'.");
  while (ret < size)
  {
    if (!is_skip(src, ret))
    {
      if (is_end_with_open_bracket(src, ret))
        open_brackets++;
      if (get_line(src, ret) == "}")
      {
        if (open_brackets == 0)
          return (ret);
        open_brackets--;
      }
    }
    ret++;
  }
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
  char        buffer[BUFFER_SIZE + 1]{};
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
    ret.push_back(string);
  } else {
    size_t offset = std::string::size_type(0);
    while (1) {
      size_t pos = string.find(separator, offset);
      if (pos == std::string::npos) {
        ret.push_back(ft::trim_space(string.substr(offset)));
        break;
      }
      ret.push_back(ft::trim_space(string.substr(offset, pos - offset)));
      offset = pos + separator_length;
    }
  }
  return (ret);
}

/**
 * @brief 該当行がスキップ可能かチェックする（空白行 or #コメント行）
 * @param line: confファイルの該当行の内容
 * @return スキップ可能か否か
 */
bool  is_skip(std::string line)
{
  return (
    line[0] == '#' ||
    split(line, " ").size() == 0
  );
}
