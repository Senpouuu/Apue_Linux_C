#include <stdio.h>
#include <setjmp.h>
static jmp_buf save;	

static void d(void) {
    printf("%s(): D Begin.\n", __FUNCTION__);
    printf("Jumpping....\n");
	longjmp(save,0);
	printf("%s(): End.\n", __FUNCTION__);
}

static void c(void) {
    printf("%s(): Begin.\n", __FUNCTION__);
    printf("%s(): Call d().\n", __FUNCTION__);
    d();
    printf("%s(): d() returned.\n", __FUNCTION__);
    printf("%s(): End.\n", __FUNCTION__);
}

static void b(void) {
    printf("%s(): Begin.\n", __FUNCTION__);
    printf("%s(): Call c().\n", __FUNCTION__);
    c();
    printf("%s(): c() returned.\n", __FUNCTION__);
    printf("%s(): End.\n", __FUNCTION__);
}

void a(void) {
	printf("%s(): Begin.\n", __FUNCTION__);
		
	int ret = setjmp(save);
	
	if(ret == 0)
	{
		printf("%s(): Call b().\n", __FUNCTION__);
   	 	b();
   		printf("%s(): b() returned.\n", __FUNCTION__);
	}	
	else
	{
		printf("%s() Jumped Back! Code is : %d\n",__FUNCTION__,ret);
	}
	
    printf("%s(): End.\n", __FUNCTION__);
}

int main(void) {
    a();
    return 0;
}
