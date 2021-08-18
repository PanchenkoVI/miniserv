#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>

#include "SRVstd.hpp"
#include "CNFPath.hpp"
#include "CNTHandler.hpp"
#include "CNFHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Utils.hpp"

#define CNT 1000
#define BUF 65535


extern pthread_mutex_t g_accept;
extern pthread_mutex_t g_write;
extern SRVstd srvSTD;

class CNTHandler;
class CNFHandler;
struct Sock;


class Server {
	public:
		Server(std::vector<CNFHandler> & srv, CNFPath & path);
		Server(Server const  & src);
		~Server();

		Server & operator=(Server const & rhs);

		void initSrv();
		void srvHandler(int idw = 0);

		bool srecv(int fd);
		bool ssend(int fd);

		void cconnect(int fd);
		void disconnect(int fd);
		void addSd(int fd);
		void rmSd(int fd);
		void close(int fd);
		void timeChecker(CNTHandler *cnt);

		static bool status;

	private:
		std::vector<CNFHandler> & _srv;
		fd_set _fds;
		fd_set _readfds;
		fd_set _writefds;
		CNFPath & _path;
		std::list<int> _fdset;
		std::map<int, Sock> _srvSock;
		std::map<int, CNTHandler*> _cnt;
		int _mfd;
		std::string _name;
		int _idw;
};
