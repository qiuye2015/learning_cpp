#include <stdio.h>
#include <string.h>

extern char ** environ;

void spt_init(int argc, char *argv[]);
void test_argv_environ(int argc , char *argv[]);

int main(int argc, char *argv[])
{
	spt_init(argc,argv);
	printf("*********************************************************************************\n");
	test_argv_environ(argc,argv);
	return 0;
}
void spt_init(int argc, char *argv[]) {
	char **envp = environ;
	char *base, *end, *nul, *tmp;
	int i, error;
	/*
	   注意这里的base, 被赋值argv[0],由于上面说了argv和environ
	   是内存连续的, 在经历下面的操作以后argv 和 environ这边连续
	   的内存将退化成char*类型的base
	   */
	if (!(base = argv[0]))
		return;
	// nul表示argv[0], 也就是base字符串, 
	// 也就是进程名字符串的结束位置(不包括\0)
	nul = &base[strlen(base)];
	end = nul + 1;
	printf("argv[0]=%s,strlen(base)=%d nul=%s end=%s\n",argv[0],strlen(base),nul,end);
	/*
	   虽然argv和environ是连续的内存, 但是其中包含\0, 而且不知道
	   environ的长度,所以得遍历二者, 将这片内存退化成char*, 
	   也就是base*/
	/*step 1: 遍历argv*/
	for (i = 0; i < argc || (i >= argc && argv[i]); i++) {
		if (!argv[i] || argv[i] < end)
			continue;
		end = argv[i] + strlen(argv[i]) + 1;
		printf("i=%d,argv[i]=%s end=%s\n",i,argv[i],end);
	}
	/*step 2: 遍历 environ*/
	for (i = 0; envp[i]; i++) {
		if (envp[i] < end)
			continue;

		end = envp[i] + strlen(envp[i]) + 1;
		printf("i=%d,envp[i]=%s end=%s\n",i,envp[i],end);
	}
} /* spt_init() */
//验证argv和environ执行连续内存的测试程序
void test_argv_environ(int argc , char *argv[]){
	int i;
	printf("argc=%d\n" , argc);
	for (i = 0; i < argc; ++i){
		printf("argv[i]=%x, argv[%d]:%s\n" ,argv[i], i , argv[i]);
	}
	printf("evriron=%x\n" , environ[0]);
}
