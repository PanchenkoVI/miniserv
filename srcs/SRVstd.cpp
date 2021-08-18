#include "SRVstd.hpp"

SRVstd::SRVstd() {};

void SRVstd::srvstd(std::string str, std::string color) {

	char buf[32];
	time_t ctm;
	struct tm	*tm;
	time(&ctm);
	tm = localtime (&ctm);
	int _ = strftime(buf, 32, "%T", tm);
	buf[_] = '\0';
	pthread_mutex_lock(&g_write);
	std::cerr << LBLUE << "[" << buf << "] " << RESET;
	std::cerr << color << str << RESET;
	std::cerr << "\n";
	pthread_mutex_unlock(&g_write);

};