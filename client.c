#include "mailbox.h"
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>

int join(void);
int check_input(void);
int check_rec(mail_t *mail);
void output(mail_t *mail);
void leave(void);
void send(void);
void send_pri(void);

char name[SIZE_OF_SHORT_STRING];
char input[SIZE_OF_LONG_STRING];
char pri_input[SIZE_OF_LONG_STRING];
int id;
int pri_id;
int len;
int check;
int state;
mailbox_t server;
mailbox_t client;

int main(void){
	
	printf("Plz enter your name:\n");
	scanf("%s", name);

	srand(time(NULL));
	id= (rand()%10000)+1;
	client=mailbox_open(id);
	server=mailbox_open(0);
	
	if(join()==1){

		printf("-----------------------------\n");
        printf(" Welcomet to the chatroom!\n");
        printf(" Your id is %d!\n",id);
		printf(" Enter \"LEAVE\" to leave!\n");
        printf(" Enter \"WHISPER\" to whisper!\n");
        printf("-----------------------------\n");

	}else{

		printf("Failed to join !\nPlz try again!\nhaha 88\n");
		return 0;
	}

    struct termios tty_state;
    tcgetattr(0,&tty_state);
    state=fcntl(0,F_GETFL);
    state|=O_NONBLOCK;
    fcntl(0,F_SETFL, state);

	while(1){

		check=check_input();

		if(check==1){
			leave();
			break;
		}else if(check==2){
            send_pri();
        //whisper
		}else if(check==0){
            send();
		}//user input
		
		mail_t mail;
		check=check_rec(&mail);
		if(check){
			output(&mail);
		}//recv
	}
	
	return 0;
}

int join(){
    mail_t mail;

    mail.from = id;
    mail.type = JOIN;
	strcpy(mail.sstr, name);

    if (mailbox_check_full(server)==0) {

        mailbox_send(server, &mail);
        return 1;

    }else{
    	return 0;
	}
}

void leave(){

    mail_t mail;
    mail.from = id;
    mail.type = LEAVE;
    
    if(mailbox_check_full(server)==0){

        mailbox_send(server, &mail);
    }

    mailbox_close(server);
    mailbox_close(client);
    mailbox_unlink(id);
    printf("88\n");
}

void send(){
    mail_t mail;

    strcpy(mail.lstr, input);
    mail.type = BROADCAST;
    mail.from = id;

    if(mailbox_check_full(server)==0){
        mailbox_send(server, &mail);
    }
}

void send_pri(){
    mail_t mail;

    strcpy(mail.lstr, pri_input);
    mail.type = WHISPER;
    mail.from = id;
    mail.to=pri_id;

    if(mailbox_check_full(server)==0){
        mailbox_send(server, &mail);
    }
}

void output(mail_t *mail){

	switch(mail->type){

		case JOIN:
            printf("----------%s joined----------\n", mail->sstr);
			break;
		case BROADCAST:
            printf("\n%s(id-%d):%s", mail->sstr, mail->from, mail->lstr);
			break;
		case  LEAVE:
            printf("-------%s(id-%d) left-------\n", mail->sstr, mail->from);
			break;
        case  WHISPER:
            printf("\n===WHISPER===(id-%d):%s\n", mail->from, mail->lstr);
			break;
		default:
			break;	
	}
}

int check_input(){
    
	int len;
    char str[SIZE_OF_LONG_STRING];
    if(fgets(str, SIZE_OF_LONG_STRING, stdin)){

        len=strlen(str);

        if(len>0&&str[len-1]=='\n'&&str[0]!='\n'){

            strcpy(input, str);
            if(strcmp("LEAVE\n", input)==0){
		        return 1;
	        }else if(strcmp("WHISPER\n", input)==0){

		        struct termios tty_state;
                tcgetattr(0,&tty_state);
                state=fcntl(0,F_GETFL);
                state&=~O_NONBLOCK;
                fcntl(0,F_SETFL, state);//blocking to wait the input

                char tem[10];
                printf("plz enter id, name\n");
                scanf("%s\n%s",tem, pri_input);
                pri_id=atoi(tem);

                tcgetattr(0,&tty_state);
                state=fcntl(0,F_GETFL);
                state|=O_NONBLOCK;
                fcntl(0,F_SETFL, state);//back to nonblocking

                return 2;

	        }else{     
                return 0; 
            }//normal input
        }
    }else{
        return -1;//no input
    }
}

int check_rec(mail_t *mail){
	
	if (mailbox_check_empty(client)==0) {

        mailbox_recv(client, mail);
        return 1;

    }else{
    	return 0;
	}
	
}
