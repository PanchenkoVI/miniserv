#pragma once

#include <iostream>
#include <string>

#include <stack>
#include <vector>
#include <set>
#include <list>
#include <fcntl.h>

#include "CNFHandler.hpp"
#include "Server.hpp"
#include "Utils.hpp"

class CNFHandler;

class GNLHandler {

	public:
		GNLHandler(std::string & file);
		~GNLHandler();

		size_t getWNum();
		std::string & getGNLline();

		void clear();
		void gnlHandler();
		void cnfParser();

		std::vector<CNFHandler> & getSRVConf();
		std::string & getFile();

	private:

		int _fd;
		size_t _wNum;
		std::string _file;
		std::string _gnlLine;
		std::vector<std::string> _gnlBuff;
		std::vector<CNFHandler> _srvConf;


};

