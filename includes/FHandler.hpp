#pragma once

# include <iostream>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <time.h>
# include <string.h>
# include <errno.h>
# include <algorithm>
# include <vector>

# include "MIMEs.hpp"
# include "SRVstd.hpp"
# include "Utils.hpp"

extern SRVstd srvSTD;
extern MIMEs g_mimes;

struct ls {
	bool dir;
	std::string name;
	std::string date;
	size_t len;
	ls() : dir(false) {};
};

class FHandler {
	public:
		FHandler();
		FHandler(std::string path);
		~FHandler();

		void fcreate(std::string & cnt);
		bool fopen(bool cfl = false);
		void fappend(std::string & cnt);
		void fclose();
		void funlink();

		bool isDir();
		bool isFile();
		bool isFile(std::string &path);
		std::string isMod();
		std::string autoindex(std::string & target);
		int & getFd();
		std::string getIdx(std::vector<std::string> & idx);
		std::vector<std::string> & getDirs();
		std::string & getPath();
		void setPath(std::string path, bool non = false);
		std::string getCnt();
		std::string & getMime();
		void setDirs();

		void setMime();
		void setNon();

	private:
		int _fd;
		std::string _name;
		std::string _path;
		std::string _rpath;
		std::string _mime;
		std::vector<std::string> _dirs;
};
