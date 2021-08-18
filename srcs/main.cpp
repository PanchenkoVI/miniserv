#include "CNFPath.hpp"
#include "GNLHandler.hpp"
#include "Server.hpp"
#include "SRVstd.hpp"

#include <pthread.h>

pthread_mutex_t g_accept;
pthread_mutex_t g_write;

SRVstd srvSTD;

struct Worker {
	size_t id;
	Server *srv;
	pthread_t ptd;
};
std::vector<Worker> g_workers;


void *initWkr(void *arg) {
	Server *serv = ((Worker*)arg)->srv;
	int id = ((Worker*)arg)->id;
	serv->srvHandler(id);
	return 0;
}

int main(int ac, char **av) {
	try {
		CNFPath path(ac, av);
		path.setPath();
		GNLHandler file(path.getPath());
		file.cnfParser();
		srvSTD.srvstd( "->> " + file.getFile() + " loaded", LYELLOW);
		file.clear();
		Server serv(file.getSRVConf(), path);
		serv.initSrv();

		if (file.getWNum() > 0) {
		g_workers.resize(file.getWNum());
		pthread_mutex_init(& g_accept, NULL);
		pthread_mutex_init(& g_write, NULL);

		for (size_t _ = 0; _ < file.getWNum(); _++) {
			g_workers[_].id = _ + 1;
			g_workers[_].srv = new Server(serv);
			pthread_create(&g_workers[_].ptd, NULL, initWkr, &g_workers[_]);
			usleep(300);
		}
		for (size_t _ = 0; _ < file.getWNum(); _++) {
			pthread_join(g_workers[_].ptd, NULL);
			delete g_workers[_].srv;
		}
		pthread_mutex_destroy(&g_write);
		pthread_mutex_destroy(&g_accept);
		} else { serv.srvHandler(); }
	}
	catch (std::exception &e) { srvSTD.srvstd(e.what(), RED); return 1; }
	return 0;
}