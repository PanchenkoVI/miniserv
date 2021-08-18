#include "RQTHandler.hpp"


RQTHandler::RQTHandler(Request & req, Sock & sock, std::vector<CNFHandler> &sbuf, CNTHandler & cnt):
	_req(req), _sock(sock),  _sbuf(sbuf), _cnt(cnt) {}

RQTHandler::~RQTHandler() {}

void RQTHandler::init() {
	CNFHandler *loc = 0;
	CNFHandler *srv = lockSrv(_sbuf);

	if (_req.getStat() > 2) { loc = getRLoc(srv, _req._target); }
	_srv = srv;
	_loc = srv;
	_target = _req._target;
	if (loc) {
		_loc = loc;
		if (_req._target.find(loc->_uri) != size_t(-1)) { _target.erase(0, _loc->_uri.length()); }
	}
}

void RQTHandler::redirected(std::string target) {
	CNFHandler *loc = 0;
	if (_req.getStat() > 2) { loc = getRLoc(_srv, target); }
	_target = target;
	if (loc) {
		_loc = loc;
		if (_target.find(loc->_uri) != size_t(-1)) { _target.erase(0, _loc->_uri.length()); }
	}
}

CNFHandler *RQTHandler::lockSrv(std::vector<CNFHandler> & sbuf) {
	std::vector<CNFHandler*> locked;
	for (std::vector<CNFHandler>::iterator it = sbuf.begin(); it != sbuf.end(); it++) {
		std::vector<Sock>::iterator _ = it->_con.begin();
		for (; _ != it->_con.end(); _++) {
			if ((_->_ip == _sock._ip || _->_ip == "0.0.0.0") and _->_p == _sock._p) {
				locked.push_back(&(*it));
				break;
			}
		}
	}
	if (locked.size() == 1) { return locked.front(); }
	std::string host = _req._hdr["Host"].substr(0, _req._hdr["Host"].find(':'));
	for (std::vector<CNFHandler*>::iterator it = locked.begin(); it != locked.end(); it++) {
		std::vector<std::string> srvs = (*it)->getSRVs();
		std::vector<std::string>::iterator isrv = srvs.begin();
		for (; isrv != srvs.end(); isrv++) { if (host == *isrv) { return *it; } }
	}
	return locked.front();
}

CNFHandler *RQTHandler::getRegex(std::vector<CNFHandler*> & loc, std::string & target) {
	regex_t regex;
	std::vector<CNFHandler*>::iterator it = loc.begin();
	for (; it != loc.end(); it++) {
		int fl = REG_NOSUB | REG_EXTENDED;
		if ((*it)->_mod == 3) { fl |= REG_ICASE; }
		int err = regcomp(&regex, (*it)->_uri.c_str(), fl);
		if (err == 0) {
			int str = regexec(&regex, target.c_str(), 0, 0, 0);
			regfree(&regex);
			if (str == 0) { return *it; }
		}
	}
	return 0;
}

CNFHandler *RQTHandler::getRLoc(CNFHandler *srv, std::string & target) {
	CNFHandler *loc = 0;
	std::vector<CNFHandler*> rloc;
	for (std::vector<CNFHandler>::iterator it = srv->_loc.begin(); it != srv->_loc.end(); it++) {
		if (it->_mod != 2 and it->_mod != 3) {
			if (it->_mod == 1 and it->_uri == target) { return &(*it); }
			else if (target.find(it->_uri) == 0) {
				if (loc and loc->_uri.length() < it->_uri.length()) { loc = &(*it); }
				else if (!loc) { loc = &(*it); }
			}
		}
		else { rloc.push_back(&(*it)); }
	}
	if (loc and loc->_mod == 4) { return loc; }
	if (loc and !loc->_loc.empty()) {
		for (std::vector<CNFHandler>::iterator it = loc->_loc.begin(); it != loc->_loc.end(); it++) {
			if (it->_mod == 2 || it->_mod == 3) { rloc.insert(rloc.begin(), &(*it)); }
		}
	}
	CNFHandler *regex = getRegex(rloc, target);
	if (regex) { return regex; }
	return loc;
}

bool RQTHandler::allowed(std::string & meth) {
	std::vector<std::string> mtd = _loc->_meth;

	if (mtd.empty()) { return 1; }
	if (std::find(mtd.begin(), mtd.end(), meth) != mtd.end()) {return 1; }
	return 0;
}

std::string &RQTHandler::getTarget() { return _target; }
std::string &RQTHandler::getRTarget() { return _req._target; }
std::string &RQTHandler::getRQstr() { return _req._rqstr; }
void RQTHandler::setTarget(std::string target) { _target = target; }
std::string &RQTHandler::getIp() { return _sock._ip; }
size_t &RQTHandler::getPort() { return _sock._p; }
CNTHandler &RQTHandler::getCnt() { return _cnt; }
std::string &RQTHandler::getRDir() { return _loc->_root; }
std::string &RQTHandler::getAuth() { return _loc->_creds; }
std::string &RQTHandler::getUri() { return _loc->_uri; }
std::map<std::string, std::string> &RQTHandler::getCGI() { return _loc->_cgi; }
std::string &RQTHandler::getCGIBin() { return _loc->_cgi_bin; }
size_t &RQTHandler::getMBL() { return _loc->_mbLen; }
bool RQTHandler::getAindex() { return _loc->_autoindex; }
std::string &RQTHandler::getUpload() { return _loc->_upload; }
std::vector<std::string> &RQTHandler::getIdx() { return _loc->_idx; }
std::map<int, std::string> &RQTHandler::getErr() { return _loc->_err; }
std::vector<std::string> &RQTHandler::getMeth() { return _loc->_meth; }
std::string &RQTHandler::getBody() { return _req._rbody; }
std::string &RQTHandler::getRMeth() { return _req._meth; }
std::string &RQTHandler::getHder(std::string key) { return _req._hdr[key]; }
std::map<std::string, std::string, ft::cmp> &RQTHandler::getHdr() { return _req._hdr; }
std::string &RQTHandler::getProtocol() { return _req._proto; }

std::string RQTHandler::log() {
	std::string str;
	str = str + "[" + getRMeth() + "]";
	str = str + " [" + getRTarget() + "]";
	str = str + " [" + getUri() + "]";
	std::map<std::string, std::string>::iterator it = getHdr().begin();
	for (; it != getHdr().end(); it++) { str = str + "\n" + it->first + ": " + it->second; }
	return str;
}
