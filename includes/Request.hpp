#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Header.hpp"
#include "Message.hpp"
#include "HttpString.hpp"
#include "Server.hpp"
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

struct HttpInfo;

class Request : public Message
{
private:
  std::vector< Server >& _servers;
  Server* _server;
  Location* _location;

  std::string _method;
  std::string _uri;
  std::string _protocol_version;
  Header      _headers;
  ssize_t     _content_length;
  HttpString  _body;
  bool        _suspended;
  int         _invalid;

  enum body_type
  {
    NONE,
    CONTENT_LENGTH,
    CHUNKED,
  };
  enum body_type _body_type;

  static const size_t _max_method_len = 6;
  static const size_t _max_request_uri_len = 1 << 13;
  static const size_t _max_header_fileds_size = 1 << 13;

  void _select_location();

  void _ignore_empty_lines();

  void _retrieve_header();
  void _retrieve_body();

  void _validate_startline();
  void _validate_headerfield(std::pair< std::string, std::string >& headerfield);
  void _validate_header();
  void _validate_request();

  void _parse_startline();
  void _parse_header();
  void _parse_body();
  void _set_body_type();

  static void _set_script_name(HttpInfo& info);
  static bool _location_matches_uri(std::string uri, std::string location_name);

public:
  Request(std::vector< Server >& servers);
  ~Request();

  void handle_request();
  void append_raw_data(char* buf, ssize_t len);
  void setup_default_http_info(HttpInfo& info, int status) const;
  void setup_http_info(HttpInfo& info) const;
  bool get_suspended();

  void show_request() const;
  void show_server() const;
  Request(Request const& other);
  Request& operator=(Request const& other);

private:
  Request();
};

#endif /* HTTPREQUEST_HPP */
