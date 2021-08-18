#pragma once

#include <iostream>
#include <string>
#include "SRVstd.hpp"
#include "RQTHandler.hpp"
#include "SRVcds.hpp"
#include "MIMEs.hpp"
#include "FHandler.hpp"
#include "CGIHandler.hpp"
#include "Base64.hpp"

class RQTHandler;

extern pthread_mutex_t g_write;
extern SRVcds g_status;
extern MIMEs g_mimes;

class Response {
	public:
		Response(RQTHandler & cnf, int idw, int errcds = 0);
		~Response();

		void clear();
		void initMeth();
		bool checkCGI(std::string mime);
		bool checkAuth();
		int genErr(int cds);
		std::string getMtd();
		void Rhandler();
		int MTDHandler();
		void genRes();

		int GET();
		int POST();
		int DELETE();

		bool brakeCon();
		bool getRdirect();
		std::string getRtarget();
		int ressend(int fd);
		std::string encode(std::vector<std::string> & enc);
		bool local(std::vector<std::string> & loc);
		int getStatus();
		std::string reslog();

		typedef int (Response::*mtd)();

	private:
		RQTHandler & _cnf;
		int _idw;
		int _errcds;
		int _cds;
		size_t _sent;
		size_t _hlen;
		size_t _blen;
		int _rcds;
		bool _rdirect;
		std::string _encode;
		FHandler _fl;
		std::string _res;
		std::string _body;
		std::string _rtarget;
		std::map<std::string, Response::mtd> _meth;
		std::map<std::string, std::string> _hdr;
};