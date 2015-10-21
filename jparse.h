#ifndef JPARSE_H
#define JPARSE_H

#include <json/json.h>
#include <stddef.h>

#define J_RENAME "{ \"USER_ID\" : 12, \"USER_NAME\" : \"ted\", \"PASSWD\" : \"\", \"EMAIL\" : \"\", \"X\" : 128, \"Y\" : 200, \"OP\" : \"UPDATE_USER\", \"NEW_NAME\" : \"健成\", \"NEW_PASSWD\" : \"\", \"NEW_EMAIL\" : \"\", \"COMMENT\" : \"remember the promise\" }"

int Turn2JsonStr(char *originalStr, char *jsonStr);

int QueryJsonStr(char *jsonStr, char *pattern, char *result);

int QueryJsonFloat(char *jsonStr, char *pattern, float *result);

int QueryJsonInt(char *jsonStr, char *pattern, int *result);

void User2JsonStr(char *jsonStr, int id, char *userName, char *passwd, char *email, float x, float y, char *op, char *newName, char *newPasswd, char *newemail, char *comment);

void DeQuotations(char *str);

void Status2JsonStr(char *echo, int id, int success);

void PosDescription2JsonStr(char *jsonStr, char *description, int success);

void Article2JsonStr(char *jsonStr, char *article, char *comment, int success);

void PosMap2JsonStr(char *jsonStr, float *east, float *north);

#endif //JPARSE_H
