#include "CNTHandler.hpp"

CNTHandler::CNTHandler(int fd, std::string &addr, Sock & sock, int idw, bool disCon):
	_fd(fd), _addr(addr), _sock(sock), _idw(idw), _disCon(disCon) {
	_req = NULL;
	_cnf = NULL;
	_res = NULL;
}

CNTHandler::~CNTHandler() { close(_fd); clear(); }

void CNTHandler::clear() { rmCNT(_req); rmCNT(_cnf); rmCNT(_res); }

void CNTHandler::setCNF(std::vector<CNFHandler> & srv) {
	_cnf = new RQTHandler(*_req, _sock, srv, *this);
	_cnf->init();
}

void CNTHandler::setRES(std::vector<CNFHandler> &srv, int errcds) {
	if (!_req) { _req = getREQ(1); }
	if (!_cnf) { setCNF(srv); }

	int flag = 0;
	_res = new Response(*_cnf, _idw, errcds);
	for (int _ = 1; _ != 0; flag++) {
		_ = 0;
		_res->Rhandler();
		if (_res->getRdirect()) {
			_ = 1;
			_cnf->redirected(_res->getRtarget());
			srvSTD.srvstd("[" + _res->getRtarget() + "] REDIRECTED",LYELLOW);
			_res->clear();
		}
		if (flag > 5) {
			rmCNT(_res);
			_res = new Response(*_cnf, _idw, 500);
			_res->Rhandler();
			break ;
		}
	}
	rmCNT(_req);
}

bool CNTHandler::timeout() {
	if (_req) {
		struct timeval time;
		gettimeofday(&time, NULL);
		time_t current_time = time.tv_sec;
		if (current_time - _req->getStart() > INITIME
		or current_time - _req->getPrev() > RESTIME) {
			if (_req->checkStat()) { return 1; }
		}
	}
	return 0;
}

bool CNTHandler::disCon() {
	if (_disCon) { return 1; }
	return 0;
}

int CNTHandler::getFd() { return _fd; }

std::string &CNTHandler::getAddr() { return _addr; }

Request *CNTHandler::getREQ(bool flag) {
	if (!_req and flag) { _req = new Request(); }
	return _req;
}

RQTHandler *CNTHandler::getCNF() { return _cnf; }

Response *CNTHandler::getRES() { return _res; }