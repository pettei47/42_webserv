#ifndef HTTP_INFO_HPP
#define HTTP_INFO_HPP

#include "Header.hpp"
#include "Request.hpp"
#include "Http_string.hpp"
#include "Server.hpp"
#include <map>
#include <string>
#include <vector>

struct Http_info
{
	std::string method;
	std::string uri;
	std::string script_name;
	std::string query_string;
	std::string protocol_version;
	Header      headers;
	Http_string body;
	Server*     server;
	Location*   location;
};

#endif /* HTTP_INFO_CPP */
