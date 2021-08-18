# include "CNFPath.hpp"

CNFPath::CNFPath(int ac, char **av) : _ac(ac), _av(av), _path("./config/default.conf") {}
CNFPath::~CNFPath() {};

void CNFPath::setPath() {
	for (int i = 1; i < _ac; i++) {
		std::string arg = _av[i];
		_path = arg;
		if (i != _ac - 1) { throw std::invalid_argument(strerror(errno)); }
	}
}

std::string &CNFPath::getPath() { return _path; }
