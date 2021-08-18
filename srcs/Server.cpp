#include "Server.hpp"


SRVcds g_status;
MIMEs g_mimes;

bool Server::status = false;

Server::Server(std::vector<CNFHandler> & srv, CNFPath & path) : _srv(srv), _path(path), _mfd(0) {
	FD_ZERO(& _fds);
	FD_ZERO(& _readfds);
	FD_ZERO(& _writefds);
}

Server::Server(Server const  & src) : _srv(src._srv), _path(src._path) { *this = src; }

Server::~Server() {}

Server	&Server::operator=(const Server &rhs) {
	_fds = rhs._fds;
	_path = rhs._path;
	_fdset = rhs._fdset;
	_srv = rhs._srv;
	_srvSock = rhs._srvSock;
	_cnt = rhs._cnt;
	_mfd = rhs._mfd;
	return *this;
}

void Server::addSd(int fd) {
	_fdset.push_back(fd);
	_fdset.sort();
	FD_SET(fd, &_fds);
	if (fd > _mfd) { _mfd = fd; }
}

void Server::rmSd(int fd) {
	std::list<int>::iterator it = _fdset.begin();
	std::list<int>::iterator ite = _fdset.end();
	for (; it != ite; it++) {
		if (*it == fd) { _fdset.erase(it); break ; }
	}
	FD_CLR(fd, &_fds);
  if (fd == _mfd) { _mfd = *_fdset.rbegin(); }
}

void Server::initSrv() {
	std::vector<Sock> sock;
	int sd = 0;
	int qlen = 128;
	int opt = 1;

	std::vector<CNFHandler>::iterator it = _srv.begin();
	std::vector<CNFHandler>::iterator ite = _srv.end();

	for (; it != ite; it++) {
		if (it->getCon().empty()) { it->getCon().push_back(Sock("0.0.0.0", 80)); }
    	std::vector<Sock>::iterator its = it->getCon().begin();
		std::vector<Sock>::iterator itse = it->getCon().end();
		for (; its != itse; its++) {
			if (std::find(sock.begin(), sock.end(), *its) == sock.end()) {
				if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) { throw std::runtime_error(strerror(errno)); }
				fcntl(sd, F_SETFL, O_NONBLOCK);
				struct sockaddr_in addr;
				memset(& addr, 0, sizeof(addr));
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr(its->_ip.c_str());
				addr.sin_port = htons(its->_p);

				setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, & opt, sizeof(int));
				if (bind(sd, (struct sockaddr *) & addr, sizeof(addr)) < 0) { throw std::runtime_error(strerror(errno)); }
				if (listen(sd, qlen) < 0) { throw std::runtime_error(strerror(errno)); }
				_srvSock[sd] = Sock(its->_ip, its->_p);
				srvSTD.srvstd("21NG1NY ->> " + ft::ntos(its->_ip) + ":" + ft::ntos(its->_p), GREEN);
				addSd(sd);
				sock.push_back(*its);
			}
		}
	}
	if (sock.empty()) { throw std::runtime_error(strerror(errno)); }
}

static std::string ft_itos(void const *s) {
	size_t const *src = (size_t const*)s;
	std::ostringstream os;
	os << ft::ntos(src[0]) << '.' << ft::ntos(src[1]) << '.' << ft::ntos(src[2]) << '.' << ft::ntos(src[3]);
	return os.str();
}

static void *getAddr(struct sockaddr *addr) { return &(((struct sockaddr_in*)addr)->sin_addr); }

void Server::cconnect(int fd) {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	FD_CLR(fd, &_readfds);
	int cd = accept(fd, (struct sockaddr *)& addr, & addrlen);
	if (cd < 0) { return ; }
	srvSTD.srvstd(_name + ft::ntos(_srvSock[fd]._ip) + ":" + ft::ntos(_srvSock[fd]._p) + " connected" , GREEN);
	fcntl(cd, F_SETFL, O_NONBLOCK);
	std::string caddr = ft_itos(getAddr((struct sockaddr *)& addr));
	_cnt[cd] = new CNTHandler(cd, caddr, _srvSock[fd], _idw, _cnt.size() >= CNT);
	addSd(cd);
}

void Server::disconnect(int fd) {
	rmSd(fd);
	if (_cnt.find(fd) != _cnt.end()) {
		srvSTD.srvstd(_name + "disconnected", GREEN);
		delete _cnt[fd];
		_cnt.erase(fd);
	}
}

bool Server::srecv(int fd) {
	Request *res = _cnt[fd]->getREQ();
	if (!res) { res = _cnt[fd]->getREQ(1); }
	char buf[BUF];
	FD_CLR(fd, &_readfds);
	int rec = recv(fd, buf, BUF, 0);
	if (rec <= 0) { return 0; }
	std::string buffer(buf, rec);
	int ret = res->setStat(buffer);
	if (ret >= 1) {
		_cnt[fd]->setCNF(_srv);
		srvSTD.srvstd(_name + "<<- " + _cnt[fd]->getCNF()->log());
		_cnt[fd]->setRES(_srv, ret);
	}
	return 1;
}

bool Server::ssend(int fd) {
	FD_CLR(fd, &_writefds);
	Response *res = _cnt[fd]->getRES();
	if (res) {
		int ret = res->ressend(fd);
		if (ret < 0) { return 0;}
		else if (ret == 0) {
			bool status = res->brakeCon() or _cnt[fd]->disCon();
			srvSTD.srvstd(_name + "->> " + res->reslog());
			_cnt[fd]->clear();
			if (status) { return false; }
		}
	}
	return 1;
}

void Server::timeChecker(CNTHandler *cnt) {
	if (cnt->timeout()) { cnt->setRES(_srv, 408); }
	if (cnt->disCon()) { cnt->setRES(_srv, 503); }
}

static void sigHandler(int sig) { (void)sig; std::cout << "\b\b \b\b"; Server::status = 0; }

void Server::srvHandler(int idw) {
	static int tmp = idw;
	_idw = idw;
	_name = "21NG1NY ";
	if (_idw > 0) { _name = ft::ntos(_idw) + "NG1NY "; }
	if (!_idw or _idw == 1) { signal(SIGINT, sigHandler); signal(SIGQUIT, sigHandler); }
	srvSTD.srvstd(_name + "started", GREEN);
	status = 1;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	while (status) {
		_readfds = _fds;
		_writefds = _fds;

		int res = select(_mfd + 1, & _readfds, & _writefds, NULL, & timeout);

		if (res >= 0) {
			std::map<int, Sock>::iterator it = _srvSock.begin();
			std::map<int, Sock>::iterator ite = _srvSock.end();
			for (; it != ite; it++) {
				if (FD_ISSET(it->first, &_readfds)) {
					pthread_mutex_lock(&g_accept);
					cconnect(it->first);
					pthread_mutex_unlock(&g_accept);
				}
			}
			std::map<int, CNTHandler*>::iterator itcx;
			for (std::map<int, CNTHandler*>::iterator itc = _cnt.begin(), itcx = itc; itc != _cnt.end(); itc = itcx) {
				int cd = itc->first;
				itcx++;
				if (FD_ISSET(cd, & _readfds) && !srecv(cd)) { disconnect(cd); continue; }
				timeChecker(itc->second);
				if (FD_ISSET(cd, & _writefds) && !ssend(cd)) { disconnect(cd); continue; }
			}
		}
		else if (res == -1 && status) { throw std::runtime_error(strerror(errno)); }
		if (tmp < idw) { tmp = idw; }
		usleep(tmp * 2000);
	}
	std::map<int, CNTHandler*>::iterator icx;
	for (std::map<int, CNTHandler*>::iterator ic = _cnt.begin(), icx = ic; ic != _cnt.end(); ic = icx) { icx++; disconnect(ic->first); }
	srvSTD.srvstd(_name + "stopped", GREEN);
}
