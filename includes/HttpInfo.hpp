#ifndef HTTP_INFO_HPP
#define HTTP_INFO_HPP

#include "Header.hpp"
#include "HttpString.hpp"
#include "Server.hpp"
#include <map>
#include <string>
#include <vector>

struct HttpInfo
{
	std::string method;
	std::string uri;
	std::string script_name;
	std::string query_string;
	std::string protocol_version;
	Header      headers;
	HttpString	body;
	Server*     server;
	Location*   location;
};

#endif /* HTTP_INFO_CPP */
