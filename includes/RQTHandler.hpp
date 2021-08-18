#pragma once

#include <regex.h>

#include "CNFPath.hpp"
#include "CNTHandler.hpp"
#include "Request.hpp"
#include "Utils.hpp"

class CNTHandler;
class CNFPath;
class Request;
class CNFHandler;
struct Sock;

class RQTHandler {
	public:
		RQTHandler(Request &req, Sock & sock, std::vector<CNFHandler> & sbuf, CNTHandler & cnt);
		~RQTHandler();

		void init();

		CNFHandler *lockSrv(std::vector<CNFHandler> & sbuf);
		CNFHandler *getRLoc(CNFHandler *srv, std::string & target);
		void redirected(std::string target);

		CNFHandler *getRegex(std::vector<CNFHandler*> & loc, std::string & target);
		bool allowed(std::string & meth);

		std::string &getRDir();
		std::string &getUri();
		std::string &getTarget();
		std::string &getRTarget();
		std::string &getRQstr();
		std::string &getCGIBin();
		void setTarget(std::string target);
		std::string &getBody();
		std::string &getAuth();
		std::map<std::string, std::string> &getCGI();
		std::map<std::string, std::string, ft::cmp> &getHdr();
		std::string &getHder(std::string key);
		size_t & getMBL();
		std::string &getProtocol();
		std::string &getUpload();
		bool getAindex();
		std::vector<std::string> &getIdx();
		std::map<int, std::string> &getErr();
		std::vector<std::string> &getMeth();
		std::string &getRMeth();
		std::string &getIp();
		size_t &getPort();
		CNTHandler & getCnt();
		std::string log();

	private:
		Request & _req;
		Sock & _sock;
		std::vector<CNFHandler> & _sbuf;
		std::string _target;
		CNTHandler & _cnt;
		CNFHandler *_srv;
		CNFHandler *_loc;
};
