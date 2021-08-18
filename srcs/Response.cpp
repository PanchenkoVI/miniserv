#include "Response.hpp"

Response::Response(RQTHandler &cnf, int idw, int errcds):
_cnf(cnf), _idw(idw), _errcds(errcds), _cds(0), _sent(0),
_hlen(0), _blen(0), _rcds(0), _rdirect(0), _encode("") {
	initMeth();
}

Response::~Response() {}

void Response::clear() {

	_errcds = 0;
	_cds = 0;
	_sent = 0;
	_hlen = 0;
	_blen = 0;
	_rdirect = 0;
	_res.clear();
	_body.clear();
	_hdr.clear();
}

void Response::initMeth() {
	_meth["GET"] = &Response::GET;
	_meth["HEAD"] = &Response::GET;
	_meth["POST"] = &Response::POST;
	_meth["DELETE"] = &Response::DELETE;
}

bool Response::checkCGI(std::string mime) {
	std::map<std::string, std::string> & cgi = _cnf.getCGI();
	std::map<std::string, std::string>::iterator it = cgi.begin();
	for (; it != cgi.end(); it++) {
		if (it->first == mime) { return 1; }
	}
	return 0;
}

bool Response::checkAuth() {
	if (_cnf.getHder("Authorization").empty()) { return 0; }
	std::string creds = _cnf.getHder("Authorization");
	std::string tkn = ft::b64decode(creds.substr(creds.find(' ') + 1));
	return (tkn == _cnf.getAuth());
}

int Response::genErr(int cds) {
	if (!_cnf.getErr()[cds].empty()) {
		std::string target = ft::uchar(_cnf.getErr()[cds]);
		std::string cur_target = ft::uchar("/" + _cnf.getTarget());
		if (target != cur_target) {
		_cnf.getRMeth() = "GET";
		_rdirect = 1;
		_rcds = cds;
		_rtarget = target;
		return 0;
		}
	}
	_body += "<html>\r\n";
	_body += "<head><title>WebServ!</title></head>\r\n";
	_body += "<body>\r\n";
	_body += "<center><h1>" + ft::ntos(cds) + " " + g_status[cds] + "</h1></center>\r\n";
	_body += "<hr><center>" + _hdr["Server"] + "</center>\r\n";
	_body += "</body>\r\n";
	_body += "</html>\r\n";
	_hdr["Content-Type"] = g_mimes.getMime(".html");
	_hdr["Content-Length"] = ft::ntos(_body.length());
	if (cds == 401) { _hdr["WWW-Authenticate"] = "Basic realm=\"Restricted Content\""; }
	if (cds == 408 || cds == 503) { _hdr["Connection"] = "close"; }
	if (cds == 503) { _hdr["Retry-After"] = "21"; }
	return cds;
}

std::string Response::getMtd() {
	std::string tmp;
	std::vector<std::string> mtd = _cnf.getMeth();
	std::vector<std::string>::iterator it = mtd.begin();
	while (it != mtd.end()) {
		tmp += *it;
		it++;
		if (it != mtd.end()) { tmp += ", "; }
	}
	return tmp;
}

void Response::Rhandler() {
	std::string & meth = _cnf.getRMeth();
	_fl.setPath(_cnf.getRDir() + "/" + _cnf.getTarget());
	if (_errcds > 1) { _cds = _errcds; }
	else if (!_cnf.allowed(meth)) { _cds = 405; _hdr["Allow"] = getMtd(); }
	else if (_cnf.getMBL() > 0 and _cnf.getBody().length() > _cnf.getMBL()) { _cds = 413; }
	else if (_cnf.getAuth() != "off" and !checkAuth()) { _cds = 401; }
	else { _cds = MTDHandler(); }
	if (_cds >= 300 and !_body.length()) { _cds = genErr(_cds); }
	if (!_rdirect) { genRes(); }
}

int Response::MTDHandler() {
	std::string & meth = _cnf.getRMeth();
	std::string path;

	if (meth == "GET") {
		if (_fl.isDir()) {
			std::string idx = _fl.getIdx(_cnf.getIdx());
			if (idx.length()) {
			_rdirect = 1;
			_rtarget = ft::uchar("/" + _cnf.getRTarget() + "/" + idx);
			return 200;
			}
			else if (!_cnf.getAindex()) { return 404; }
		}
		path = _fl.getPath();
		if (!_fl.isDir()) {
			if (!_fl.isFile()) { return 404; }
			_fl.setDirs();
			std::vector<std::string> & dirs = _fl.getDirs();
			if (!_cnf.getHder("Accept-Language").empty()) {
				if (local(dirs))
				_fl.setPath(path.substr(0, path.find_last_of("/") + 1) + dirs.front(), 1);
			}
			if (!_cnf.getHder("Accept-Charset").empty()) {
				_encode = encode(dirs);
				_fl.setPath(path.substr(0, path.find_last_of("/") + 1) + dirs.front(), 1);
			}
			if (!_fl.fopen()) { return 403; }
			_hdr["Last-Modified"] = _fl.isMod();
		}
	}
	if (checkCGI(_fl.getMime())) {
		CGIHandler cgi(_fl, _cnf, _cnf.getHdr(), _cnf.getBody());
		cgi.initCGI(_idw);
		if ((_cds = cgi.exeCGI()) > 400) { return _cds; }
		cgi.getHDR(_hdr);
		_body = cgi.getBody();
		_hdr["Content-Length"] = ft::ntos(_body.length());
		return _cds;
	}
	if (meth == "POST") {
		std::string path = _cnf.getUri() + "/" + _cnf.getTarget();

		if (!_cnf.getUpload().empty()) {
			FHandler dir(_cnf.getRDir() + "/" + _cnf.getUpload());

			path = _cnf.getUri() + "/" + _cnf.getUpload() + "/" + _cnf.getTarget();
			if (dir.isFile() and !dir.isDir()) { dir.funlink(); }

			if (!dir.isFile()) {
				if (mkdir(dir.getPath().c_str(), 0755) == -1) { throw std::runtime_error(strerror(errno)); }
			}
			_fl.setPath(dir.getPath() + "/" + _cnf.getTarget());
		}
		_hdr["Location"] = ft::uchar(path);
	}
	return (this->*(Response::_meth[meth]))();
}


static std::string getDate() {
	struct tm	*tm;
	struct timeval tv;
	char buf[32];
	gettimeofday(&tv, NULL);
	tm = gmtime(&tv.tv_sec);
	int ret = strftime(buf, 32, "%a, %b %d %Y %T GMT", tm);
	return std::string(buf, ret);
}

void Response::genRes() {
	_hdr["Server"] = "21NG1NY";
	if (_cds < 400 and _rcds) { _cds = _rcds; }
	std::string cds;

	if (_hdr.count("Status")) { cds = _hdr["Status"]; _hdr.erase("Status"); }
	else { cds = ft::ntos(_cds) + " " + g_status[_cds]; }
	_res = _res + _cnf.getProtocol() + " " + cds + "\r\n";
	_hdr["Date"] = getDate();
	std::map<std::string, std::string>::iterator it = _hdr.begin();
	for (; it != _hdr.end(); it++) { _res += it->first + ": " + it->second + "\r\n"; }
	_res += "\r\n";
	_hlen = _res.size();
	if (!_body.empty()) { _blen = _body.size(); _res += _body; _body.clear(); }
}

int Response::GET() {
	if (_cnf.getAindex() and _fl.isDir()) {
		_hdr["Content-Type"] = g_mimes.getMime(".html");
		_body = _fl.autoindex(_cnf.getRTarget());
		_hdr["Content-Length"] = ft::ntos(_body.length());
	}
	else {
		_hdr["Content-Type"] = g_mimes.getMime(_fl.getMime());
		if (!_encode.empty()) { _hdr["Content-Type"] += "; charset=" + _encode; }
		_body = _fl.getCnt();
		_hdr["Content-Length"] = ft::ntos(_body.length());
	}
	return 200;
}

int Response::POST() {
	int cds = 200;
	_body = _cnf.getBody();
	pthread_mutex_lock(&g_write);
	if (!_fl.isFile()) { _fl.fcreate(_body); cds = 201; }
	else { _fl.fappend(_body); cds = 200; }
	pthread_mutex_unlock(&g_write);
	_hdr["Content-Length"] = ft::ntos(_body.length());
	return cds;
}

int Response::DELETE() {
	if (!_fl.isFile()) { return 404; }
	_fl.funlink();
	_body += "<!DOCTYPE html>\r\n\
				<html>\r\n\
				<body>\r\n\
				<h1>DELETED!</h1>\r\n\
				</body>\r\n\
				</html>";
	_hdr["Content-Type"] = g_mimes.getMime(".html");
	_hdr["Content-Length"] = ft::ntos(_body.length());
	return 200;
}

bool Response::local(std::vector<std::string> & loc){
	int m = 0;
	int q = 10;
	std::string path = _fl.getPath();
	std::string lcl = _cnf.getHder("Accept-Language");
	std::vector<std::string> addlcl;
	std::vector<std::string> clcl;
	_hdr["Content-Language"] = "en";
	while (1) {
		std::string str = lcl.substr(0, lcl.find_first_of(" ,;\0"));
		addlcl.clear();
		if (str.find("*") == size_t(-1)){
			std::vector<std::string>::iterator it = loc.begin();
			for (; it != loc.end() ; it++)
			if (it->find("." + str) != size_t(-1)) { addlcl.push_back(*it); }
		}
		else { addlcl = loc; }
		if (!addlcl.empty() and (q > m)) {
			clcl = addlcl;
			if (str[0] != '*') { _hdr["Content-Language"] = str; }
			m = q;
		}
		if (lcl.find(".") != size_t(-1)) { q = ft::stoi(lcl.substr(lcl.find_first_of(".") + 1, 1)); }
		if (lcl.find(",") == size_t(-1)) {
			if (!clcl.empty()) { loc = clcl; return 1; }
			return (0);
		}
		lcl = lcl.substr(lcl.find_first_of(" ,;"));
		lcl = lcl.substr(lcl.find_first_of("abcdefghijklmnoprstuvwxyz*"));
	}
}

std::string Response::encode(std::vector<std::string> & enc) {
	int m = 0;
	int q = 10;
	std::string path = _fl.getPath();
	std::string ecd = _cnf.getHder("Accept-Charset");
	std::string ret;
	std::vector<std::string> addecd;
	std::vector<std::string> cecd;
	ret = "";
	while (1) {
		std::string str = ecd.substr(0, ecd.find_first_of(" ,;\0"));
		if (str.find("*") == size_t(-1)){
			std::vector<std::string>::iterator it = enc.begin();
			for(; it != enc.end() ; it++) {
				if (it->find("." + str) != size_t(-1)) { addecd.push_back(*it); }
			}
		}
		else { addecd = enc; }
		if (!addecd.empty() and (q > m)) {
			cecd = addecd;
		if(str[0] != '*') { ret = str; }
		m = q;
		}
		if (ecd.find(".") != size_t(-1)) { q = ft::stoi(ecd.substr(ecd.find_first_of(".") + 1, 1)); }
		if (ecd.find(",") == size_t(-1)){
			if (!cecd.empty()) { enc = cecd; }
			break ;
		}
		ecd = ecd.substr(ecd.find_first_of(" ,;"));
		ecd = ecd.substr(ecd.find_first_of("abcdefghijklmnoprstuvwxyz*"));
	}
	return ret;
}

bool Response::brakeCon() {
	if (_hdr.find("Connection") != _hdr.end() and _hdr["Connection"] == "close") { return 1; }
	return 0;
}

bool Response::getRdirect() { return _rdirect; }

std::string Response::getRtarget() { return _rtarget; }

int Response::ressend(int fd) {
	int ret = send(fd, _res.c_str() + _sent, _res.length() - _sent, 0);
	if (ret <= 0) { std::cout <<  strerror(errno) << std::endl; return -1; }
	_sent += ret;
	if (_sent >= _res.length()) { return 0; }
	return 1;
}

std::string Response::reslog() {
	std::string ret = "[status: " + ft::ntos(_cds) + " " + g_status[_cds] + "]";
	if (_hdr.count("Content-Length")) { ret = ret + " [length: " + _hdr["Content-Length"] + "]"; }
	return ret;
}
