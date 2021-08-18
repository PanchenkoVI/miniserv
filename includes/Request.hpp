#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include "CNTHandler.hpp"
#include "GNLHandler.hpp"
#include "Response.hpp"
#include "Utils.hpp"

class Request {
	public:
		friend class RQTHandler;

		Request();
		~Request();

		int setStat(std::string & buf);
		int setSline();
		int setHdr();
		int setOhdr();
		int setBody();
		int setChunk();
		int subChunk();

		bool checkStat();
		int getStat();
		time_t getStart();
		time_t getPrev();



	private:
		enum SRVstat {
			SLINE,
			HDR,
			OHDR,
			BODY,
			CHUNK,
			DONE,
			ERR
		};

		enum Chunk {
			CHUNK_DATA,
			CHUNK_SIZE,
		};

		size_t _len;
		int _blen;
		int _csize;

		SRVstat _srvstat;
		Chunk _cstat;

		std::string _buf;
		std::string _meth;
		std::string _target;
		std::string _rqstr;
		std::string _proto;
		std::map<std::string, std::string, ft::cmp> _hdr;
		std::string _rbody;

		struct timeval _start;
		struct timeval _prev;

};
