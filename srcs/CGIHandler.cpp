#include "CGIHandler.hpp"

CGIHandler::CGIHandler(FHandler & file, RQTHandler & cnf, std::map<std::string, std::string, ft::cmp> & req):
	_file(file), _cnf(cnf), _req(req) {
	_rbody = _file.getCnt();
}

CGIHandler::CGIHandler(FHandler &file, RQTHandler & cnf, std::map<std::string, std::string, ft::cmp> & req, std::string & rbody):
_file(file), _cnf(cnf), _req(req) {
	if (rbody.empty() and _cnf.getRMeth() != "POST") { _rbody = _file.getCnt(); }
	else { _rbody = rbody; }
}

void CGIHandler::initCGI(int idw) {
	char *cwd = getcwd(NULL, 0);
	if (!cwd) { return ; }
	_pwd = cwd;
	free(cwd);

	_env = 0;
	_av[0] = 0;
	_av[1] = 0;
	_av[2] = 0;
	_mime = _file.getMime();
	_exec = _cnf.getCGI()[_mime];
	if (_cnf.getCGIBin()[0] == '/') { _cpath = _cnf.getCGIBin() + "/" + _exec; }
	else { _cpath = _pwd + "/" + _cnf.getCGIBin() + "/" + _exec; }
	std::string cpath = "/tmp/webserv_cgi_tmp_" + ft::ntos(idw);
	_tmp.setPath(cpath.c_str());
	_tmp.fopen(1);
	if (idw) { srvSTD.srvstd(ft::ntos(idw) + "NG1NY CGI ->> " + _cpath); }
	else { srvSTD.srvstd("21NG1NY CGI ->> " + _cpath); }
}

static void efree(char **env) {
	int _ = 0;
	while (env[_]) {
		if (env[_]) { free(env[_]); }
		env[_] = NULL;
		_++;
	}
	free(env);
}

CGIHandler::~CGIHandler() {
	free(_av[0]);
	free(_av[1]);
	if (_env) { efree(_env); }
	_tmp.fclose();
	_tmp.funlink();
}

int CGIHandler::exeCGI() {
	_fpath = _pwd + "/" + _file.getPath();

	if (!setENV()) { return 500; }
	if (!(_av[0] = ft::strdup(_cpath.c_str()))) { return 500; }
	if (!(_av[1] = ft::strdup(_fpath.c_str()))) { return 500; }
	_av[2] = NULL;
	int pip[2];
	if (pipe(pip) != 0) {return 500; }
	pid_t pid = fork();
	if (pid == 0) {
		if (chdir(_fpath.substr(0, _fpath.find_last_of('/')).c_str()) == -1) { return 500; }
	close(pip[1]);
	if (dup2(pip[0], 0) == -1) { return 500; }
	if (dup2(_tmp.getFd(), 1) == -1) { return 500; }
	close(pip[0]);
	execve(_av[0], _av, _env);
	exit(1);
	}
	else if (pid > 0) {
		close(pip[0]);
		if (_rbody.length() and write(pip[1], _rbody.c_str(), _rbody.length()) <= 0) { return 500; }
		close(pip[1]);
		int status;
		if (waitpid(pid, &status, 0) == -1) { return 500; }
		if (WIFEXITED(status) and WEXITSTATUS(status)) { return 502; }
	}
	else { return 502; }
	_body = _tmp.getCnt();
	return 200;
}

void CGIHandler::getHDR(std::map<std::string, std::string> & hdr) {
	std::string tmp;
	size_t crlf;
	size_t sub;

	while ((crlf = _body.find("\r\n")) != size_t(-1)) {
		if (_body.find("\r\n") == 0) { _body.erase(0, crlf + 2); break; }
		if ((sub = _body.find(':', 0)) != size_t(-1)) {
			tmp = _body.substr(0, sub);
			hdr[tmp] = ft::sltrim(_body.substr(sub + 1, crlf - sub - 1), ' ');
		}
		_body.erase(0, crlf + 2);
	}
	if (hdr.count("Content-Length")) { size_t size = ft::stoi(hdr["Content-Length"]); _body.erase(size); }
}

std::string & CGIHandler::getBody() { return _body; }

bool CGIHandler::setENV() {
	if (_cnf.getRMeth() == "POST") {
		_cenv["CONTENT_TYPE"] = _req["Content-Type"];
		_cenv["CONTENT_LENGTH"] = ft::ntos(_rbody.length());
	}
	_cenv["GATEWAY_INTERFACE"] = "CGI/1.1";
	_cenv["PATH_INFO"] = _fpath;
	_cenv["PATH_TRANSLATED"] = _fpath;
	_cenv["QUERY_STRING"] = _cnf.getRQstr();
	_cenv["REMOTE_ADDR"] = _cnf.getCnt().getAddr();
	if (_cnf.getAuth() != "off") {
		_cenv["AUTH_TYPE"] = "Basic";
		_cenv["REMOTE_IDENT"] = _cnf.getAuth().substr(0, _cnf.getAuth().find(':'));
		_cenv["REMOTE_USER"] = _cnf.getAuth().substr(0, _cnf.getAuth().find(':'));
	}
	_cenv["REQUEST_METHOD"] = _cnf.getRMeth();
	_cenv["REQUEST_URI"] = _fpath;
	_cenv["SCRIPT_NAME"] = _cpath;
	_cenv["SERVER_NAME"] = _cnf.getIp();
	_cenv["SERVER_PROTOCOL"] = _cnf.getProtocol();
	_cenv["SERVER_PORT"] = ft::ntos(_cnf.getPort());
	_cenv["SERVER_SOFTWARE"] = "21NG1NY";
	_cenv["REDIRECT_STATUS"] = "200";

	for (std::map<std::string, std::string, ft::cmp>::iterator it = _req.begin(); it != _req.end(); it++) {
		if (!it->second.empty()) {
			std::string tmp = "HTTP_" + ft::stoup(it->first);
			std::replace(tmp.begin(), tmp.end(), '-', '_');
			_cenv[tmp] = it->second;
		}
	}
	if (!(_env = (char **)malloc(sizeof(char *) * (_cenv.size() + 1)))) { return 0; }
	int _ = 0;
	for (std::map<std::string, std::string>::iterator it = _cenv.begin(); it != _cenv.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		if (!(_env[_] = ft::strdup(tmp.c_str()))) { return 0; }
		_++;
	}
	_env[_] = NULL;
	return 1;
}