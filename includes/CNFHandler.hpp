#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stdint.h>

#include "Utils.hpp"
#include "Server.hpp"



struct Sock {
	std::string _ip;
	size_t _p;
	Sock() : _ip(""), _p(0) {};
	Sock(std::string ip, size_t p): _ip(ip), _p(p) {};
};

inline bool operator==(Sock const & lhs, Sock const & rhs) {
	return lhs._ip == rhs._ip && lhs._p == rhs._p;
}

class CNFHandler {
	friend class RQTHandler;
	public:
		int idx;

		CNFHandler();
		~CNFHandler();

		CNFHandler	& operator=(CNFHandler const & rhs);

		typedef void (CNFHandler::*type)(std::vector<std::string>::iterator &);
		static void initCNF();

		void sepChecker(std::vector<std::string>::iterator &it);
		void location(std::vector<std::string>::iterator &it);
		void locHandler(std::vector<std::string>::iterator &it, std::vector<CNFHandler> &loc);
		void listen(std::vector<std::string>::iterator &it);
		void server_name(std::vector<std::string>::iterator &it);
		void allow_only(std::vector<std::string>::iterator &it);
		void auth(std::vector<std::string>::iterator &it);

		void client_max_body_size(std::vector<std::string>::iterator &it);
		void error_page(std::vector<std::string>::iterator &it);
		void root(std::vector<std::string>::iterator &it);
		void index(std::vector<std::string>::iterator &it);
		void cgi(std::vector<std::string>::iterator &it);
		void cgi_bin(std::vector<std::string>::iterator &it);
		void autoindex(std::vector<std::string>::iterator &it);
		void upload(std::vector<std::string>::iterator &it);

		std::string &getUri();
		std::vector<Sock> & getCon();
		std::vector<std::string> & getSRVs();
		std::vector<CNFHandler> & getLoc();



	private:
		enum LocMod {
			NONE,
			EXACT,
			CASE_SENSITIVE_REG,
			CASE_INSENSITIVE_REG,
			LONGEST,
		};
		static std::map<std::string, type> _cmd;
		std::vector<Sock> _con;
		std::vector<std::string> _srvName;
		std::vector<CNFHandler> _loc;
		std::string _uri;
		LocMod _mod;
		std::string _creds;
		bool _autoindex;
		size_t _mbLen;
		std::string _root;
		std::string _upload;
		std::vector<std::string> _meth;
		std::map<int, std::string> _err;
		std::vector<std::string> _idx;
		std::map<std::string, std::string> _cgi;
		std::string _cgi_bin;
};
