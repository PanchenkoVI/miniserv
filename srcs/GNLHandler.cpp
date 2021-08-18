#include "GNLHandler.hpp"

GNLHandler::GNLHandler(std::string & file):
	_file(file) {
	_fd = open(_file.c_str(), O_RDONLY);
	if (_fd < 0) { throw std::invalid_argument(strerror(errno)); }
	_wNum = 0;
}

GNLHandler::~GNLHandler() { clear(); }

size_t GNLHandler::getWNum() {  return _wNum; }

std::string & GNLHandler::getGNLline() { return _gnlLine; }

void GNLHandler::clear() {
	if (_fd > 0) { close(_fd); _fd = 0; }
	_gnlBuff.clear();
	_gnlLine.clear();
}

void GNLHandler::gnlHandler() {

	char	*str;

	std::string::size_type start, end;
	std::string line, subline;
	std::stack<bool> sep;

	std::string _[] = {
		"server_name",
		"root",
		"auth",
		"listen",
		"index",
		"autoindex",
		"upload",
		"error_page",
		"cgi",
		"cgi_bin"
	};
	std::set<std::string> cmd(_, _ + sizeof(_) / sizeof(_[0]));


	while (ft::gnl(_fd, & str)) {
		line = str;
		_gnlLine += line + "\n";
		end = 0;
		while ((start = line.find_first_not_of(" \t", end)) != size_t(-1)) {
			if (line[start] == '#') { break; }
		end = line.find_first_of(" \t", start);
		subline = line.substr(start, end - start);
		if (subline == "{") { sep.push(true); }
		else if (subline == "}") {
			if (sep.empty()) { throw std::invalid_argument(strerror(errno)); }
			sep.pop();
		}
		if (cmd.count(subline) && line[line.find_last_not_of(" \t", line.length())] != ';') { throw std::invalid_argument(strerror(errno)); }
		if (subline.find(';', subline.length() - 1) != size_t(-1)) {
			subline.erase(subline.length() - 1, 1);
			_gnlBuff.push_back(subline);
			_gnlBuff.push_back(";");
		}
		else { _gnlBuff.push_back(subline); }
	}
	free(str);
	}
	free(str);
}

void GNLHandler::cnfParser() {
	gnlHandler();
	std::vector<std::string>::iterator it = _gnlBuff.begin();
	std::vector<std::string>::iterator ite = _gnlBuff.end();
	int i = 1;

	for ( ; it != ite; ++it) {
		if (*it == "server") {
			CNFHandler serv;
			serv.idx = i++;
			serv.sepChecker(++it);
			_srvConf.push_back(serv);
		}
		else if (*it == "workers") {
			_wNum = ft::stoi(*(++it));
			if (_wNum < 0 or _wNum > 21) { throw std::invalid_argument(strerror(errno)); }
			if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
		}
		else { throw std::invalid_argument(strerror(errno)); }
	}
	if (_srvConf.empty()) { throw std::invalid_argument(strerror(errno)); }
}

std::vector<CNFHandler> & GNLHandler::getSRVConf() { return _srvConf; }

std::string & GNLHandler::getFile() { return _file; }
