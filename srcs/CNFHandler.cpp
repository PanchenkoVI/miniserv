#include "CNFHandler.hpp"

CNFHandler::CNFHandler():
	_creds("off"), _autoindex(false), _mbLen(0), _cgi_bin("cgi-bin") {
	_mod = NONE;
	initCNF();
}

CNFHandler::~CNFHandler() {}

CNFHandler & CNFHandler::operator=(CNFHandler const & rhs) {
	_mbLen = rhs._mbLen;
	_root = rhs._root;
	_err = rhs._err;
	_idx = rhs._idx;
	_cgi = rhs._cgi;
	_cgi_bin = rhs._cgi_bin;
	_autoindex = rhs._autoindex;
	_creds = rhs._creds;
	return (*this);
}

std::map<std::string, CNFHandler::type> CNFHandler::_cmd;

void CNFHandler::initCNF() {
	CNFHandler::_cmd["server_name"] = & CNFHandler::server_name;
	CNFHandler::_cmd["root"] = &CNFHandler::root;
	CNFHandler::_cmd["auth"] = & CNFHandler::auth;
	CNFHandler::_cmd["listen"] = & CNFHandler::listen;
	CNFHandler::_cmd["location"] = & CNFHandler::location;
	CNFHandler::_cmd["client_max_body_size"] = &CNFHandler::client_max_body_size;
	CNFHandler::_cmd["allow_only"] = &CNFHandler::allow_only;
	CNFHandler::_cmd["index"] = &CNFHandler::index;
	CNFHandler::_cmd["autoindex"] = &CNFHandler::autoindex;
	CNFHandler::_cmd["upload"] = &CNFHandler::upload;
	CNFHandler::_cmd["error_page"] = & CNFHandler::error_page;
	CNFHandler::_cmd["cgi"] = &CNFHandler::cgi;
	CNFHandler::_cmd["cgi_bin"] = &CNFHandler::cgi_bin;
}

void CNFHandler::sepChecker(std::vector<std::string>::iterator & it) {
	if (*it != "{") { throw std::invalid_argument(strerror(errno)); }
	while (*(++it) != "}") {
		if (CNFHandler::_cmd[*it]) { (this->*(CNFHandler::_cmd[*it]))(++it); }
		else { throw std::invalid_argument(strerror(errno)); }
	}
}

void CNFHandler::server_name(std::vector<std::string>::iterator & it) {
	while (*it != ";") { _srvName.push_back(*it++); }
}

void CNFHandler::listen(std::vector<std::string>::iterator & it) {
	std::string ipSrv = "0.0.0.0";
	size_t pSrv = 8080;
	std::string iCnf = *it;

	if (iCnf.find(':') != size_t(-1)) {
		ipSrv = iCnf.substr(0, iCnf.find(':'));
		std::string subpSrv = iCnf.substr(iCnf.find(':') + 1);
		if (subpSrv.find_first_not_of("0123456789") != size_t(-1)) { throw std::invalid_argument(strerror(errno)); }
		pSrv = ft::stoi(subpSrv);
		if (pSrv > 65535) { throw std::invalid_argument(strerror(errno)); }
	}
	else if (iCnf.find_first_not_of("0123456789") != size_t(-1)) { ipSrv = iCnf; }
	else { pSrv = ft::stoi(iCnf); }
	Sock con(ipSrv, pSrv);
	if (std::find(_con.begin(), _con.end(), con) != _con.end()) { throw std::invalid_argument(strerror(errno)); }
	_con.push_back(Sock(ipSrv, pSrv));
	if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
}

void CNFHandler::client_max_body_size(std::vector<std::string>::iterator & it) {
	if (it->find_first_not_of("0123456789") != size_t(-1)) { throw std::invalid_argument(strerror(errno)); }
	_mbLen = ft::stoi(*it);
	if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
}

void CNFHandler::error_page(std::vector<std::string>::iterator & it) {
	std::vector<int> errn;
	while (it->find_first_not_of("0123456789") == size_t(-1)) { errn.push_back(ft::stoi(*it++)); }
	std::vector<int>::iterator _it = errn.begin();
	std::vector<int>::iterator _ite = errn.end();
	for ( ; _it != _ite; _it++) { _err[*_it] = *it; }
	if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
}

void CNFHandler::auth(std::vector<std::string>::iterator & it) {
	_creds = *it;
	if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
}

void CNFHandler::root(std::vector<std::string>::iterator & it) {
	_root = *it;
	if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
}

void CNFHandler::index(std::vector<std::string>::iterator &it) {
	while (*it != ";") { _idx.push_back(*it++); }
}

void CNFHandler::allow_only(std::vector<std::string>::iterator &it) {
	while (*it != ";") { _meth.push_back(*it++); }
}

void CNFHandler::autoindex(std::vector<std::string>::iterator & it) {
	if (*it == "on") { _autoindex = true; }
	else if (*it == "off") { _autoindex = false; }
	else { throw std::invalid_argument(strerror(errno)); }
	if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
}

void CNFHandler::upload(std::vector<std::string>::iterator & it) {
	_upload = *it;
	if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
}

void CNFHandler::cgi_bin(std::vector<std::string>::iterator & it) {
	_cgi_bin = *it;
	if (*++it != ";") { throw std::invalid_argument(strerror(errno)); }
}

std::string _[] = {
	"=",
	"~",
	"~*",
	"^~"
};

std::set<std::string> locmod(_, _ + sizeof(_) / sizeof(_[0]));

void CNFHandler::locHandler(std::vector<std::string>::iterator & it, std::vector<CNFHandler> & loc) {
	if (locmod.count(*it)) { if (*it == "=") { _mod = EXACT; }
		else if (*it == "~") { _mod = CASE_SENSITIVE_REG; }
		else if (*it == "~*") { _mod = CASE_INSENSITIVE_REG; }
		else if (*it == "^~") { _mod = LONGEST; }
		else { throw std::invalid_argument(strerror(errno)); }
		it++;
	}
	else { _mod = NONE; }
	_uri = *it++;
	if (*it != "{") { throw std::invalid_argument(strerror(errno)); }
	while (*(++it) != "}") {
		if (CNFHandler::_cmd[*it]) { (this->*(CNFHandler::_cmd[*it]))(++it); }
		else { throw std::invalid_argument(strerror(errno)); }
	}
	loc.push_back(*this);;
}

void CNFHandler::location(std::vector<std::string>::iterator & it) {
	CNFHandler loc;
	loc = *this;
	loc.locHandler(it, _loc);
}

void CNFHandler::cgi(std::vector<std::string>::iterator & it) {
	std::string & _ = *it++;
	std::string & cmd = *it++;
	_cgi[_] = cmd;
	if (*it != ";") { throw std::invalid_argument(strerror(errno)); }
}

std::string & CNFHandler::getUri() { return _uri; }
std::vector<Sock> & CNFHandler::getCon() { return _con; }
std::vector<std::string> & CNFHandler::getSRVs() { return _srvName; }
std::vector<CNFHandler> &CNFHandler::getLoc() { return _loc; }
