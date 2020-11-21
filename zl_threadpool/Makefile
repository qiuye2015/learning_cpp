all: threadpool98 threadpool03 threadpool11 test
CFLAGS = -g -Wall

threadpool98:ThreadPoolCpp98/main.cpp ThreadPoolCpp98/ThreadPool.cpp
	g++ $(CFLAGS) -o threadpool98 ThreadPoolCpp98/main.cpp ThreadPoolCpp98/ThreadPool.cpp -lpthread

threadpool03:ThreadPoolCpp03/main.cpp ThreadPoolCpp03/ThreadPool.cpp
	g++ $(CFLAGS) -o threadpool03 -std=c++0x ThreadPoolCpp03/main.cpp ThreadPoolCpp03/ThreadPool.cpp -lpthread

threadpool11:ThreadPoolCpp11/main.cpp ThreadPoolCpp11/ThreadPool.cpp
	g++ $(CFLAGS) -o threadpool11 -std=c++11 ThreadPoolCpp11/main.cpp ThreadPoolCpp11/ThreadPool.cpp -lpthread

test:ThreadPool/example.cpp
	g++ $(CFLAGS) -o test  -std=c++11 ThreadPool/example.cpp -lpthread
clean:
	rm -f threadpool98 threadpool03 threadpool11 test
