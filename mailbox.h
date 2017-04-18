#ifndef _MAILBOX_H
#define _MAILBOX_H

#define SIZE_OF_SHORT_STRING    64
#define SIZE_OF_LONG_STRING     512
#define JOIN                    1
#define BROADCAST               2
#define LEAVE                   3
#define WHISPER                 4

typedef void *mailbox_t;

typedef struct __MAIL {
    int from;
    int type;
    int to;
    char sstr[SIZE_OF_SHORT_STRING];
    char lstr[SIZE_OF_LONG_STRING];
} mail_t;

mailbox_t mailbox_open(int id);
int mailbox_unlink(int id);
int mailbox_close(mailbox_t box);
int mailbox_send(mailbox_t box, mail_t *mail);
int mailbox_recv(mailbox_t box, mail_t *mail);
int mailbox_check_empty(mailbox_t box);
int mailbox_check_full(mailbox_t box);

#endif
