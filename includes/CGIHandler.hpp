#pragma once

#include <iostream>
#include <map>

#include <sys/types.h>
#include <sys/wait.h>

#include "RQTHandler.hpp"
#include "FHandler.hpp"
#include "Utils.hpp"

class RQTHandler;

class CGIHandler {
	public:
		CGIHandler(FHandler & file, RQTHandler & cnf, std::map<std::string, std::string, ft::cmp> & req);
		CGIHandler(FHandler & file, RQTHandler & cnf, std::map<std::string, std::string, ft::cmp> & req, std::string & body);
		~CGIHandler();

		void initCGI(int idw);
		int exeCGI();
		bool setENV();
		void getHDR(std::map<std::string, std::string> & hdr);
		std::string &getBody();

	private:
		FHandler & _file;
		RQTHandler & _cnf;
		std::map<std::string, std::string, ft::cmp> & _req;
		std::string _cpath;
		std::string _exec;
		std::string _mime;
		std::string _pwd;
		std::string _fpath;
		std::string _body;
		std::string _rbody;
		FHandler _tmp;
		std::map<std::string, std::string> _cenv;
		char **_env;
		char *_av[3];
};