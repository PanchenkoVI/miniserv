#pragma once

#include <iostream>

#include "CNFPath.hpp"
#include "Request.hpp"
#include "RQTHandler.hpp"
#include "CNFHandler.hpp"
#include "Response.hpp"

#define INITIME 200
#define RESTIME 100

class RQTHandler;
class CNFPath;
class Response;

class CNTHandler {
	public:
		CNTHandler(int fd, std::string & addr, Sock & sock, int idw, bool disCon = 0);
		~CNTHandler();

		template<typename T>
		void rmCNT(T *&p) {
			if (p) { delete p; p = NULL; }
		}

		int getFd();
		std::string &getAddr();
		void clear();


		bool timeout();
		bool disCon();
		void setCNF(std::vector<CNFHandler> & srv);
		void setRES(std::vector<CNFHandler> & srv, int errcds = 0);

		Request *getREQ(bool flag = 0);
		RQTHandler *getCNF();
		Response *getRES();

	private:
		int _fd;
		std::string _addr;
		Sock & _sock;
		int _idw;
		int _disCon;
		Request *_req;
		Response *_res;
		RQTHandler *_cnf;
};