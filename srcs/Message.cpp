#include "Message.hpp"

Message::Message()
  : _parse_phase(START)
{
  _raw_data.index = 0;
}

Message::Message(std::string delim)
  : _parse_phase(START)
  , _delim(delim)
{
  _raw_data.index = 0;
}

Message::~Message() { }

Message::Message(Message const& other)
{
  *this = other;
}

Message& Message::operator=(Message const& /*other*/)
{
  return *this;
}

Message::parse_phase Message::get_parse_phase() const
{
  return _parse_phase;
}

std::string Message::_get_next_line(size_t max, int status_code)
{
  return _get_next_word(true, max, status_code);
}

std::string Message::_get_next_word(bool eol, size_t max, int status_code)
{
  std::string word;
  size_t separator_size = eol ? _delim.size() : 1;
  size_t index;
  if(eol)
    index = _raw_data.buf.find(_delim, _raw_data.index);
  else
    index = _raw_data.buf.find_first_of(" \t", _raw_data.index);
  if(index == HttpString::npos)
    return ""; //for now

  if(index - _raw_data.index > max)
    throw http::StatusException(status_code);

  word = _raw_data.buf.substr(_raw_data.index, index - _raw_data.index).std_str();

  _raw_data.index = _raw_data.buf.find_first_not_of(" \t", index + separator_size);
  if(_raw_data.index == HttpString::npos)
    _raw_data.index = index + separator_size;

  return word;
}

void Message::_retrieve_startline()
{
  if(_parse_phase != FIRST_LINE)
    return;

  _parse_startline();
  _parse_phase = HEADER;
}

void Message::_retrieve_header() { }

void Message::_retrieve_body()
{
  if(_parse_phase != BODY)
    return;

  _parse_body();

  _parse_phase = DONE;
}

void Message::_parse_startline() { }
void Message::_parse_header() { }
void Message::_parse_body() { }

std::pair< std::string, std::string > Message::_parse_headerfield(std::string line)
{
  size_t index, value_len;
  std::string key, value;
  index = line.find(":");
  value_len = (line.length() <= index + 1) ? 0 : line.length() - (index + 1);

  key = ft::to_titlecase(line.substr(0, index));
  value = ft::trim_space(line.substr(index + 1, value_len));
  return std::make_pair(key, value);
}

void Message::_validate_startline() { }
void Message::_validate_header() { }
