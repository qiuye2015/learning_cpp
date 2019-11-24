#g++ $1 -g -o test -lboost_thread -lpthread
g++ $1 -g -o test -ltcmalloc -lprofiler -g
