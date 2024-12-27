#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char** argv)
{

    if(argc < 2)
    {
        printf("Usage: <size>\n");
        exit(1);
    }

    printf("The size of the file is: %s\n",argv[1]);

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0) 
    {
        perror("open");
        exit(1);   
    }

    struct stat f_st;    
    if(fstat(fd, &f_st) < 0)
    {
        perror("fstat");   
        exit(1);
    }

    char* str = mmap(NULL,f_st.st_size,PROT_READ,MAP_SHARED,fd,0);
    
    if(str == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    close(fd);

    int count = 0;

    for (size_t i = 0; i < f_st.st_size; i++) {
        printf("%c", str[i]);
    }
    printf("\n");

    munmap(str,f_st.st_size);
    exit(0);
} 