#include "Request.hpp"

Request::Request():
	_blen(0), _csize(0), _srvstat(SLINE), _proto("HTTP/1.1") { gettimeofday(& _start, 0); }

Request::~Request() {}

int Request::setStat(std::string & buf) {
	gettimeofday(&_prev, 0);

	size_t stat = 0;
	_buf += buf;
	buf.clear();
	if (_srvstat == SLINE) { stat = setSline(); }
	if (_srvstat == HDR) { stat = setHdr(); }
	if (_srvstat == OHDR) { stat = setOhdr(); }
	if (_srvstat == BODY) { stat = setBody(); }
	if (_srvstat == CHUNK) { stat = setChunk(); }
	if (_srvstat == DONE or stat == 1) { _srvstat = DONE; return stat; }
	else if (_srvstat == ERR or stat > 1) { _srvstat = ERR; return stat; }
	return stat;
}

static bool uriChecker(std::string & uri) {
	int fl = 0;
	std::string _uri = uri;
	while (_uri.find('/') != size_t(-1)) {
		_uri = _uri.substr(_uri.find('/') + 1);
		if (_uri.empty()) { break; }
		std::string sub = _uri.substr(0, _uri.find('/'));
		if (sub.find("..", 0) != size_t(-1)) { fl--; }
		else { fl++; }
	}
	return fl >= 0;
}

std::string _mtd[] = {
	"GET",
	"POST",
	"DELETE"
};
std::set<std::string> mtd(_mtd, _mtd + sizeof(_mtd) / sizeof(_mtd[0]));

int Request::setSline() {
	if (_buf.find("\r\n") != size_t(-1)) {
		std::string sub = _buf.substr(0, _buf.find(' '));
		if (mtd.count(sub)) { _meth = sub; _buf.erase(0, _meth.length() + 1); }
		else { return 501; }
		if (_buf.find(' ') == 0) { return 400; }
		sub = _buf.substr(0, _buf.find(' '));
		if (sub[0] != '/') { return 400; }
		if (!uriChecker(sub)) { return 403; }
		if (sub.length() < 100000) { _target = sub; _buf.erase(0, _target.length() + 1); }
		else { return 414; }
		if (_target.find('?') != size_t(-1)) {
			_rqstr = _target.substr(_target.find('?') + 1);
			_target.erase(_target.find('?'));
		}
		if (_buf.find(' ') == 0) { return 400; }
		size_t end = _buf.find("\r\n");
		sub = _buf.substr(0, end);
		if (sub == "HTTP/1.1") { _proto = sub; _buf.erase(0, end + 2); }
		else { return 505; }
		_srvstat = HDR;
	}
	return 0;
}

int Request::setHdr() {
	size_t prev;
	size_t end;
	std::string hder;
	std::string val;
	while ((end = _buf.find("\r\n")) != size_t(-1)) {
		if (_buf.find("\r\n") == 0) { _buf.erase(0, end + 2); _srvstat = OHDR; break; }
		if ((prev = _buf.find(':', 0)) != size_t(-1)) {
			if (prev == 0 or _buf[prev - 1] == ' ') { return 400; }
			hder = _buf.substr(0, prev);
			val = _buf.substr(prev + 1, end - prev - 1);
			if (hder == "Host" and _hdr.count(hder)) { return 400; }
			if (hder.length() > 1000 or val.length() > 4000) { return 400; }
			_hdr[hder] = ft::sltrim(ft::srtrim(val, ' '), ' ');
			if (_hdr[hder].empty()) { _hdr.erase(hder); }
		}
		else { return 400; }
		_buf.erase(0, end + 2);
	}
	return 0;
}

int Request::setOhdr() {
	_blen = 0;

	if (_hdr.find("Host") == _hdr.end() or _hdr["Host"].empty()) { return 400; }
	if (_hdr["Host"].find("@") != size_t(-1)) { return 400; }

	if (_hdr.find("Transfer-Encoding") != _hdr.end() and _hdr["Transfer-Encoding"] == "chunked") {
		_srvstat = CHUNK;
		_cstat = CHUNK_SIZE;
	}
	else if (_hdr.find("Content-Length") != _hdr.end()) {
		if (_hdr["Content-Length"].find_first_not_of("0123456789") != size_t(-1)) { return 400; }
		try {
			_len = ft::stoi(_hdr["Content-Length"]);
			if (_len < 0) { throw std::invalid_argument(strerror(errno)); }
		}
		catch (std::exception &e) { return 400; }
		_srvstat = BODY;
	}
	else { return 1; }
	if (_meth != "POST") { return 1; }
	return 0;
}

int Request::setBody() {
	if (_buf.length() >= _len) {
		_rbody.insert(_blen, _buf, 0, _len);
		_blen += _buf.length();
		_buf.clear();
		if (_rbody.length() == _len) { return 1; }
		else { return 400; }
	}
	return 0;
}

int Request::subChunk() {
	size_t end, prev;
	std::string hder;
	std::string val;

	while ((end = _buf.find("\r\n")) != size_t(-1)) {
		if (_buf.find("\r\n") == 0) { _buf.erase(0, end + 2); break; }
		if ((prev = _buf.find(':', 0)) != size_t(-1)) {
			hder = _buf.substr(0, prev);
			val = _buf.substr(prev + 1, end - prev - 1);
			_hdr[hder] = ft::sltrim(val, ' ');
		}
		else { return 400; }
		_buf.erase(0, end + 2);
	}
	return 1;
}

int Request::setChunk() {
	size_t end;

	while ((end = _buf.find("\r\n")) != size_t(-1)) {
		if (_cstat == CHUNK_SIZE) {
			std::string hex = _buf.substr(0, end);
			_csize = ft::stoh(hex);
			_buf.erase(0, end + 2);
			_cstat = CHUNK_DATA;
		}
		else if (_cstat == CHUNK_DATA) {
			if (_csize == 0) {
				if (!_buf.empty()) { return subChunk(); }
			return 1;
		}
		_rbody += _buf.substr(0, end);
		_buf.erase(0, end + 2);
		_csize = 0;
		_cstat = CHUNK_SIZE;
		}
	}
	return 0;
}

bool Request::checkStat() {
	if (_srvstat != DONE) { _srvstat = ERR; return 1; }
	return 0;
}

int Request::getStat() { return _srvstat; }

time_t Request::getStart() { return _start.tv_sec; }
time_t Request::getPrev() { return _prev.tv_sec; }