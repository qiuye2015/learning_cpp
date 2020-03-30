#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <iostream>
using namespace std;

int main()
{
    int fd = open("test.txt", O_RDONLY);
    if(fd < 0)
    {
        perror("open failed");
        return 1;
    }

    char buf[1024] = {0};
    int offset = 5;
    ssize_t ret = 0;
    int count = 10;
    if((ret = pread(fd, buf, count, offset)) == -1)
    {
        perror("pread failed");
        return 1;
    }

    std::cout << "read buf = " << buf << std::endl;
    return 0;
}
