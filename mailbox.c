#include "mailbox.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

typedef void *mailbox_t;

mailbox_t mailbox_open(int id) {
	char shm_name[100];
	int fd=-1;
	
	sprintf(shm_name, "/__mailbox_%d", id);
	fd = shm_open(shm_name, O_CREAT | O_TRUNC | O_RDWR, 0777);  
    
	if(fd==-1){  
         return NULL; 
     }  
    
    return ((void *)fd); 
}

int mailbox_unlink(int id) {
	
    char shm_name[100];

    sprintf(shm_name, "/__mailbox_%d", id);

    return shm_unlink(shm_name);
}

int mailbox_close(mailbox_t box){
	return close(((int*)box));
}

int mailbox_send(mailbox_t box, mail_t *mail){
	int fd=((int*)box);
	lseek(fd, 0, SEEK_END);
	
	if(write(fd, mail, sizeof(*mail)) <= 0){
		return -1;
	}else{
		return 0;
	}
}

int mailbox_recv(mailbox_t box, mail_t *mail){
	int fd=((int*)box);
    off_t size=lseek(fd, 0, SEEK_END);//size
    char buf[size];
    lseek(fd,0,SEEK_SET);

    int check=read(fd, buf, size);
	
	if(check==-1){
		return -1;
	}else{
        ftruncate(fd, size - sizeof(*mail));
        memcpy(mail, buf, sizeof(*mail));
		return 0;
	}

}

int mailbox_check_empty(mailbox_t box){
	int fd=((int*)box);
	off_t size=lseek(fd, 0, SEEK_END);
	
	if(size<0){
		return -1;
	}else if(size==0){
		return 1; //empty
	}else if(size>0){
		return 0;
	}	
	
}

int mailbox_check_full(mailbox_t box){
	int fd=((int*)box);
	
	int check=lseek(fd, 0, SEEK_CUR);
	
	if(check==-1){
		return 1;
	}else{
		return 0;
	}
}

