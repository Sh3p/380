/*
  Filename   : mem.c
  Author     : Christian Shepperson
  Course     : CSCI 380-01
  Assignment : Memory Mapping
  Description: Acts as the linux command copy
*/


//Libraries
/*******************/
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

//local declaration
/******************/
void unix_error(char* msg);


int main(int argc, char* argv[]){
    
    if(argc != 3){
        printf("Too many Arguments\n");
        exit(1);
    }
    struct stat sbuf;
    char* s_addr;
    char* d_addr;
    
    int src = open(argv[1], O_RDONLY, 0600);
        
        if(src < 1){
            unix_error("source file does not exist");
        }
    
    fstat(src, &sbuf);

    int dest = open(argv[2], O_RDWR | O_CREAT, 0600);

    int ftrunc = ftruncate(dest, sbuf.st_size);
    
    if(ftrunc < 0){
            unix_error("source file does not exist");
        }

    d_addr = mmap(NULL, sbuf.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, dest, 0);

    s_addr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_SHARED, src, 0);

    memcpy(d_addr, s_addr, sbuf.st_size);

    int dmunm = munmap(d_addr, sbuf.st_size);
     
     if(dmunm < 0){
            unix_error("source file does not exist");
        }

    int smum = munmap(s_addr, sbuf.st_size);
    
    if(smum < 0){
            unix_error("source file does not exist");
        }
    
    close(src);
    
    close (dest);
}
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(0);
}