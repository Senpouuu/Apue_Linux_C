#include <stdio.h>
#include <stdlib.h>

int main(int argc,char**argv)
{
	#pragma omp parallel
	{
		puts("Hello");
		puts("world");

	}


	exit(0);
}

