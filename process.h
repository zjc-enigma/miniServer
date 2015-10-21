#ifndef PROCESS_H
#define PROCESS_H

#include "utility.h"
#include "jparse.h"
#include "sql.h"

#define MAX_JSON_LEN 2000
#define MAX_COMMENT_LEN 2000
#define MAX_DESCRIPTION_LEN 500

//OP code list
#define    USER_REG 1
#define    USER_LOGIN 2
#define    USER_LOGOUT 3
#define    USER_UPDATE 4
#define    USER_DEL 5
#define    POS_MARK 6
#define    POS_MAP 7
#define    POS_INFO 8
#define    ARTICLE_FETCH 9
#define    ARTICLE_READ 10
#define    ARTICLE_RELEASE 11
#define    ARTICLE_UPDATE 12
#define    ARTICLE_REVIEW 13

int ProcessRecvData(MYSQL *conn, char *recv, char *echo, int sockfd);

#endif //PROCESS_H
