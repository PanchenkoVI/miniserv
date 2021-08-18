#pragma once

#include <map>
#include <string>
#include "Utils.hpp"

class MIMEs {
	public:
		MIMEs();
		std::string getMime(std::string mime);

	private:
		std::map<std::string, std::string, ft::cmp> _mime;
};
