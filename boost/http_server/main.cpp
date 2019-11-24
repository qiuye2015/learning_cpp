//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"
#include <gperftools/profiler.h>
#include <signal.h>

void gprofStartAndStop(int signum) {
	static int isStarted = 0;
	if (signum != SIGUSR1) return;

	//通过isStarted标记未来控制第一次收到信号量开启性能分析，第二次收到关闭性能分析。
	if (!isStarted){
		isStarted = 1;
		ProfilerStart("test.prof");
		printf("ProfilerStart success\n");
	}else{
		ProfilerStop();
		printf("ProfilerStop success\n");
	}
}
int main(int argc, char* argv[])
{
	signal(SIGUSR1, gprofStartAndStop);
	try
	{
		// Check command line arguments.
		if (argc != 4)
		{
			std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
			std::cerr << "  For IPv4, try:\n";
			std::cerr << "    receiver 0.0.0.0 80 .\n";
			std::cerr << "  For IPv6, try:\n";
			std::cerr << "    receiver 0::0 80 .\n";
			return 1;
		}

		// Initialise the server.
		http::server::server s(argv[1], argv[2], argv[3]);

		// Run the server until stopped.
		s.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}
	return 0;
}
