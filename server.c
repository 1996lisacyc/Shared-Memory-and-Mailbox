#include "mailbox.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>

struct id_name_map{
    int id;
    char name[SIZE_OF_SHORT_STRING];
    mailbox_t box;
}map[100];

int check_input(void);
void maintain(void);
void process(mail_t *mail);
void broadcast(mail_t *mail);
void broadcast_pri(mail_t *mail);
void client88(mail_t *mail);
void add(mail_t *mail);
int client_counter=0;
mailbox_t server;
int i;

int main(void){
	
	int check=0;
    int state;
	for(i=0; i<100; i++){
		map[i].id=0;
        strcpy(map[i].name,"");
		map[i].box=NULL;
	}

    server = mailbox_open(0);
    printf("------------------------------------\n");
    printf("Enter \"LEAVE\" to close the server!\n");
    printf("------------------------------------\n");

    //fcntl(0, F_SETFL, 0);

    struct termios tty_state;
    tcgetattr(0,&tty_state);
    state=fcntl(0,F_GETFL);
    state|=O_NONBLOCK;
    fcntl(0,F_SETFL, state);

    while (1) {
    	
    	maintain();

		check=check_input();
        if(check){
        	break;
		}
    }
	
	mailbox_close(server);
    mailbox_unlink(0);
    
	return 0;
}

void maintain(){
	
	if (mailbox_check_empty(server) == 0) {
        mail_t mail;
        mailbox_recv(server, &mail);
        process(&mail);
    }
}

void process(mail_t *mail){

    switch(mail->type) {
        case JOIN:
            add(mail);
            printf("JOIN: %s(id-%d)\n", mail->sstr, mail->from);
            break;
        case LEAVE:
            client88(mail);
            printf("LEAVE: %s(id-%d)\n", mail->sstr, mail->from);
            break;
        case BROADCAST:
            broadcast(mail);
            printf("BROADCAST: (id-%d):%s\n", mail->from, mail->lstr);
            break;
        case WHISPER:
            broadcast_pri(mail);
            printf("WHISPER: (id-%d):%s\n", mail->from, mail->lstr);
            break;
        default:
            break;
    }
}

void broadcast(mail_t *mail){
	
    int cur=0;
    
    for(i=0; i<client_counter; i++){
    	if(map[i].id==mail->from){
    		cur=i;		
		}
	}
	mail_t mail2;
    memcpy(&mail2, mail, sizeof(mail2));
    strcpy(mail2.sstr, map[cur].name);
        
    for(i=0; i<client_counter; i++){
        if ((i!=cur)&&(mailbox_check_full(map[i].box)==0)) {
            mailbox_send(map[i].box, &mail2);
        }
	}
}

void broadcast_pri(mail_t *mail){
	
    int cur=0;
    int exist=0;
    
    for(i=0; i<client_counter; i++){
    	if(map[i].id==mail->to){
    		cur=i;
            exist=1;		
		}
	}

    if(exist==0){
        printf("======WHISPER ERROR!!======\n");
        
    }else{
        mail_t mail2;
        memcpy(&mail2, mail, sizeof(mail2));
        strcpy(mail2.sstr, map[cur].name);
        
        mailbox_send(map[cur].box, &mail2);
    }
}

void add(mail_t *mail){
	
    map[client_counter].id= mail->from;
    strcpy(map[client_counter].name,mail->sstr);
    map[client_counter].box=mailbox_open(map[client_counter].id);

	mail_t mail2;
    memcpy(&mail2, mail, sizeof(mail2));
    mail2.from = 0;
    
    for(i=0; i<client_counter; i++){

        if (mailbox_check_full(map[i].box) == 0) {
            mailbox_send(map[i].box, &mail2);
        }
	}

    client_counter++;
}

void client88(mail_t *mail){
	
	int cur=0;
    
    for(i=0; i<client_counter; i++){
    	if(map[i].id==mail->from){
    		cur=i;		
		}
	}//index of the one who left in map
	
	mail_t mail2;
    memcpy(&mail2, mail, sizeof(mail2));
    strcpy(mail2.sstr, map[cur].name);
    
	for(i=0; i<client_counter; i++){
        if ((i!=cur)&&(mailbox_check_full(map[i].box)==0)) {
            mailbox_send(map[i].box, &mail2);
        }
	}
	
	mailbox_close(map[cur].box);
	
	client_counter--;
	
	for(i=cur; i<client_counter; i++){
		map[i].id=map[i+1].id;
		map[i].box=map[i+1].box;
        strcpy(map[i].name,map[i+1].name);
	}//update map
    
}

int check_input(){
	
	char input[SIZE_OF_LONG_STRING];
	scanf("%s", input);
	
	if(strcmp("LEAVE", input) == 0){
		return 1;
	}else{
		return 0;
	}

}
