#include "FHandler.hpp"

FHandler::FHandler(): _fd(0) {}

FHandler::FHandler(std::string path): _fd(0) { setPath(path); }

FHandler::~FHandler() { fclose(); }


void FHandler::fclose() {
	if (_fd <= 0) { return ; }
	close(_fd);
	_fd = 0;
}

bool FHandler::fopen(bool cfl) {
	fclose();
	if (cfl) { _fd = open(_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 00755); }
	else { _fd = open(_path.c_str(), O_RDONLY); }
	return _fd > 0;
}

void FHandler::fcreate(std::string & cnt) {
	if (!fopen(1)) { throw std::runtime_error(strerror(errno)); }
	if (cnt.length() && write(_fd, cnt.c_str(), cnt.length()) <= 0) { throw std::runtime_error(strerror(errno)); }
	}

void FHandler::fappend(std::string & cnt) {
	fclose();
	_fd = open(_path.c_str(), O_RDWR | O_APPEND);
	if (_fd < 0) { return ; }
	if (cnt.length() && write(_fd, cnt.c_str(), cnt.length()) <= 0) { throw std::runtime_error(strerror(errno)); }
}

void FHandler::funlink() {
  if (!isFile()) { return ; }
  if (unlink(_path.c_str()) == -1) { throw std::runtime_error(strerror(errno)); }
}

int & FHandler::getFd() { return _fd; }



static std::string setWid(size_t wid, std::string s) {
	std::string wd;
	size_t len = s.length();
	if (len > wid) { wid = 0; }
	for (size_t _ = 0; _ < wid - len; _++) { wd += " "; }
	wd += s;
	return wd;
}

static bool sorted(ls i, ls j) {
	if (i.dir && j.dir) { return (i.name < j.name); }
	else if (!i.dir && !j.dir) { return (i.name < j.name); }
	return (i.dir > j.dir);
}

std::string FHandler::autoindex(std::string & target) {
	struct tm	*tm;
	char buf[32];
	DIR *dirp;
	struct stat sbuf;
	struct dirent *dent;
	std::string body;

	dirp = opendir(_path.c_str());
	body += "<html>\r\n";
	body += "<head><title>WebServ!</title></head>\r\n";
	body += "<body>\r\n";
	body += "<h1>Hello from autoindex!</h1><hr><pre>";
	std::vector<ls> lbuf;

	readdir(dirp);
	while ((dent = readdir(dirp))) {
		ls la;
		la.name = dent->d_name;
		if (la.name.length() > 50) { la.name.erase(47); la.name += "..>"; }
		std::string path(_path + "/" + dent->d_name);
		stat(path.c_str(), &sbuf);
		if (S_ISDIR(sbuf.st_mode)) { la.dir = 1; la.name += "/"; }
		tm = gmtime(&sbuf.st_mtime);
		int _ = strftime(buf, 32, "%b %d %Y %H:%M", tm);
		la.date = std::string(buf, _);
		la.len = sbuf.st_size;
		lbuf.push_back(la);
	}
	std::sort(lbuf.begin(), lbuf.end(), sorted);
	std::vector<ls>::iterator it = lbuf.begin();
	for (; it != lbuf.end(); it++) {
		body = body + "<a href=\"" + ft::uchar(target + + "/" + it->name) + "\">" + it->name + "</a>";
		if (it != lbuf.begin()) {
			body += setWid(68 - it->name.length(), it->date);
			if (it->dir) { body += setWid(20, "-"); }
			else { body += setWid(20, ft::ntos(it->len)); }
		}
		body += "\r\n";
	}
	body += "</pre><hr></body>\r\n";
	body += "</html>\r\n";
	closedir(dirp);
	return body;
}

bool FHandler::isDir() { struct stat sbuf; stat(_path.c_str(), &sbuf); return S_ISDIR(sbuf.st_mode); }

bool FHandler::isFile() { struct stat sbuf; return stat(_path.c_str(), &sbuf) == 0; }

bool FHandler::isFile(std::string &path) { struct stat sbuf; return stat(path.c_str(), &sbuf) == 0; }

std::string FHandler::isMod() {
	struct tm	*tm;
	struct stat sbuf;
	char buf[32];
	stat(_path.c_str(), &sbuf);
	tm = gmtime(&sbuf.st_mtime);
	int ret = strftime(buf, 32, "%a, %b %d %Y %T GMT", tm);
	return std::string(buf, ret);
}

std::string FHandler::getIdx(std::vector<std::string> & idx) {
	DIR *dirp;
	struct dirent *dent;

	if ((dirp = opendir(_path.c_str()))) {
		while ((dent = readdir(dirp))) {
			std::vector<std::string>::iterator it = idx.begin();
			for (; it != idx.end(); it++) {
				if (*it == dent->d_name) {
					std::string _ = "/" + std::string(dent->d_name);
					closedir(dirp);
					return _;
				}
			}
		}
		closedir(dirp);
	} else { srvSTD.srvstd(std::string(strerror(errno)), RED); return ""; }
	return "";
}
std::vector<std::string> &FHandler::getDirs() { return _dirs; }
std::string &FHandler::getPath() { return _path; }

std::string FHandler::getCnt() {
	int _;
	char buf[4096 + 1];
	std::string str;
	lseek(_fd, 0, SEEK_SET);
	while ((_ = read(_fd, buf, 4096)) != 0) {
		if (_ == -1) { srvSTD.srvstd(std::string(strerror(errno)), RED); return ""; }
		buf[_] = '\0';
		str.insert(str.length(), buf, _);
	}
	return str;
}

std::string & FHandler::getMime() { return _mime; }

void FHandler::setPath(std::string path, bool non) {
	_path = ft::uchar(path);
	if (non) { setNon(); }
	else { setMime(); }
}

void FHandler::setDirs() {
	DIR *dirp;
	struct dirent *dent;
	std::string path = _path.substr(0, _path.find_last_of("/"));
	if (!_dirs.empty()) { _dirs.clear(); }
	if ((dirp = opendir(path.c_str()))) {
		while ((dent = readdir(dirp))) {
			std::string name(dent->d_name);
			if (_rpath != name && !name.find(_name) && name.find(_mime) != size_t(-1)) { _dirs.push_back(dent->d_name); }
		}
		closedir(dirp);
	} else { throw std::runtime_error(strerror(errno)); }
}

void FHandler::setMime() {
	std::string fl = _path.substr(_path.find_last_of("/") + 1);
	if (fl.empty()) { return; }
	_rpath = fl;
	_name = fl.substr(0, fl.find("."));
	fl.erase(0, fl.find("."));
	if (fl.find_last_of(".") != size_t(-1)) { int end = fl.find_last_of("."); _mime = fl.substr(end); }
}

void FHandler::setNon() {
	std::string fl = _path.substr(_path.find_last_of("/") + 1);
	if (fl.empty()) { return; }
	_rpath = fl;
	_name = fl.substr(0, fl.find("."));
	fl.erase(0, fl.find("."));
	if (fl.find_last_of(".") != size_t(-1)) {
		int end = fl.find_last_of(".");
		_mime = fl.substr(end);
		while (!g_mimes.getMime(_mime).compare("application/octet-stream")) {
			int _ = end;
			if ((fl.find_last_of(".", end - 1) != size_t(-1))) { end = fl.find_last_of(".", end - 1); _mime = fl.substr(end, _ - end) ; }
		else { break ; }
		if (end <= 0) { break ; }
		}
	}
}



