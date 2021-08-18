# include "SRVcds.hpp"

// RFC 7231
SRVcds::SRVcds() {
	_errcds[100] = "Continue";
	_errcds[101] = "Switching Protocols";
	_errcds[200] = "OK";
	_errcds[201] = "Created";
	_errcds[202] = "Accepted";
	_errcds[203] = "Non-Authoritative Information";
	_errcds[204] = "No Content";
	_errcds[205] = "Reset Content";
	_errcds[206] = "Partial Content";
	_errcds[300] = "Multiple Choices";
	_errcds[301] = "Moved Permanently";
	_errcds[302] = "Found";
	_errcds[303] = "See Other";
	_errcds[304] = "Not Modified";
	_errcds[305] = "Use Proxy";
	_errcds[307] = "Temporary Redirect";
	_errcds[400] = "Bad Request";
	_errcds[401] = "Unauthorized";
	_errcds[402] = "Payment Required";
	_errcds[403] = "Forbidden";
	_errcds[404] = "Not Found";
	_errcds[405] = "Method Not Allowed";
	_errcds[406] = "Not Acceptable";
	_errcds[407] = "Proxy Authentication Required";
	_errcds[408] = "Request Timeout";
	_errcds[409] = "Conflict";
	_errcds[410] = "Gone";
	_errcds[411] = "Length Required";
	_errcds[412] = "Precondition Failed";
	_errcds[413] = "Payload Too Large";
	_errcds[414] = "URI Too Long";
	_errcds[415] = "Unsupported Media Type";
	_errcds[416] = "Range Not Satisfiable";
	_errcds[417] = "Expectation Failed";
	_errcds[426] = "Upgrade Required";
	_errcds[500] = "Internal Server Error";
	_errcds[501] = "Not Implemented";
	_errcds[502] = "Bad Gateway";
	_errcds[503] = "Service Unavailable";
	_errcds[504] = "Gateway Timeout";
	_errcds[505] = "HTTP Version Not Supported";
}

std::string &SRVcds::operator[](int cds) { return _errcds[cds]; }