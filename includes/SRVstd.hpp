#pragma once

#include <iostream>
#include <map>
#include <string>

#define RESET	"\e[0m"
#define RED		"\e[31m"
#define GREEN	"\e[32m"
#define LYELLOW	"\e[93m"
#define LBLUE	"\e[94m"
#define LCYAN	"\e[96m"

extern pthread_mutex_t g_write;

class SRVstd {
	public:
		SRVstd();
		void srvstd(std::string str, std::string color = LCYAN);
};