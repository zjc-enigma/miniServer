#ifndef SQL_H
#define SQL_H

#include <my_global.h>
#include <m_string.h>
#include <mysql.h>
#include <math.h>

#define MAX_COMMAND_LEN 100
#define LAST_INSERT_ID "SELECT LAST_INSERT_ID()"
#define USER_DEL_SQL       "DELETE FROM user WHERE name="
#define USER_QUERY_ID     "SELECT id, name, passwd, email, online FROM user WHERE id= "
#define USER_QUERY_NAME_SQL     "SELECT id, name, passwd, email, online FROM user WHERE name= "
#define USER_UPDATE_SQL    "UPDATE user SET "
#define USER_ADD    "INSERT INTO user (name, passwd, email) VALUES ("
#define POS_ADD     "INSERT INTO map (east, north, description, name) VALUES ("
#define ARTICLE_ADD     "INSERT INTO article (authorName, authorId, name, article) VALUES ("
#define POS_QUERY_EN     "SELECT id, east, north, description FROM map WHERE east= "
#define POS_QUERY_NAME     "SELECT id, east, north, description FROM map WHERE name= "

#define POS_QUERY_LOCAL_SQL    "SELECT east, north FROM map"

#define ARTICLE_QUERY_ID    "SELECT id, authorName, authorId, name, article FROM article WHERE id="

#define HALF_BRACKET    ")"

//extern MYSQL *conn;

//init data base connection
void InitDataBase(MYSQL *conn);

//conbine two string, no check
char * Add2StrEnd(char *origin, char *tail);

//replace for strcat
char * CombineStr(char * str1, char * str2);

//construct SQL instruction
char * GetAddInstruction(MYSQL *connection, char *command, char *values[], int size);

//add Quotation to a string
char * AddQuot(char *str);

//Add a  user
void AddUser(MYSQL *conn, char *userName, char *passwd, char *email);

//Delete a row by name of TAB user
void DeleteUser(MYSQL *conn, char *userName);

int QueryUserById(MYSQL *conn, int id, char *userName, char *passwd, char *email, int *online);

char * GetUpdateInstruction(char *prefix, char *value, char *id, int haveQuot);

void UpdateUser(MYSQL *conn, int id, char *userName, char *passwd, char *email, int online);

int LoginUser(MYSQL *conn, char *userName, char *passwd);

void LogoutUser(MYSQL *conn,int id);

int GetLastInsertId(MYSQL *conn);

void AddPosMark(MYSQL *conn, float eastDegree, float northDegree, char *description, char *posName);

int GetPosInfo(MYSQL *conn, char *userName, char *description);

float GetPosDistence(float east1, float north1, float east2, float north2);

void GetNearestPosSet(float *eastSet, float *northSet, int setSize, float curEast, float curNorth, float posEast, float posNorth);

void GetPosMap(MYSQL *conn,  float *east, float *north, float curEast, float curNorth, int *posNum);

int QueryArticleById(MYSQL *conn, int articleId, char *authorName, int *authorId, char *articleName, char *article, char *comment);

void GetArticleRead(MYSQL *conn, int articleId, char *article, char *comment);

void AddArticle(MYSQL *conn, char *authorName, int authorId, char *articleName, char *article);

void UpdateArticle(MYSQL *conn, int articleId, char *articleName, char *article);

void ReviewArticle(MYSQL *conn, int articleId, int userId, char *comment);

#endif //SQL_H




