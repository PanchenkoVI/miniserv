#pragma once

#include <string>
#include <map>

class SRVcds {
	public:
		SRVcds();
		std::string &operator[](int cds);

	private:
		std::map<int, std::string> _errcds;
};