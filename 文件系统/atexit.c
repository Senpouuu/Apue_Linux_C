#include <stdio.h>
#include <stdlib.h>


static void hookfun1()
{
	puts("Hook 1 Trigged!\r\n");
}


static void hookfun2()
{
	puts("Hook 2 Trigged!\r\n");
}


int main(int argc,char**argv)
{
	puts("Begin!\r\n");
	
	atexit(hookfun1);
	atexit(hookfun2);

	
	puts("End!\r\n");

		
	exit(0);
}

