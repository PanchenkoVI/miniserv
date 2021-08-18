#pragma once

#include <iostream>
#include <exception>
#include <string>
#include <cstring>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/time.h>

#include <stdlib.h>
#include <cstring>
#include <limits>
#include <unistd.h>
#include <string.h>



namespace ft {


	int		gnl(int fd, char **line);
	std::string stol(std::string s);
	std::string stoup(std::string s);
	std::string sltrim(std::string str, char c);
	std::string srtrim(std::string str, char c);
	size_t stoh(std::string &str);
	char *strdup(char const *str);
	std::string uchar(std::string str);
	int stoi(std::string const & str, std::size_t *pos = 0, int base = 10);


	struct cmp {
		bool operator() (std::string const & lhs, std::string const & rhs) const {
			return ft::stol(lhs) < ft::stol(rhs);
		};
	};

	template<typename T>
	std::string ntos(T n) { std::stringstream s; s << n; return s.str(); }

};