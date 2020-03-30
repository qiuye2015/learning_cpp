#include<stdarg.h>
#include<stdio.h>

int sum(int, ...);
int log(const char*, ...);
int main(void)
{
	//printf("10、20 和 30 的和 = %d\n",  sum(3, 10, 20, 30) );
	//printf("4、20、25 和 30 的和 = %d\n",  sum(4, 4, 20, 25, 30) );
	log("%d,%s",3,"word");
	return 0;
}

int sum(int num_args, ...)
{
	int val = 0;
	va_list ap;
	int i;

	va_start(ap, num_args);
	for(i = 0; i < num_args; i++)
	{
		val += va_arg(ap, int);
	}
	va_end(ap);

	return val;
}
int log(const char* fmt, ...){
	char msg[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	fprintf(stdout,"[fjp...] %s\n",msg);
}
