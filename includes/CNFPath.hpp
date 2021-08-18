#pragma once

#include <string>

#include "Server.hpp"

class CNFPath {
	public:
		CNFPath(int ac, char **av);
		~CNFPath();

		void setPath();
		std::string & getPath();

	private:
		int _ac;
		char **_av;
		std::string _path;

};