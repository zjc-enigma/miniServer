#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jparse.h"

//turn recv string into json format
int Turn2JsonStr(char *originalStr, char *jsonStr)
{
    struct json_object *tmp;
    if((tmp = json_tokener_parse(originalStr)) < 0)
    {
	perror("Turn2JsonStr invalid string\n");
	return -1;
    }
    strcpy(jsonStr, json_object_to_json_string(tmp));
    json_object_put(tmp);
    return 0;
}

//look up the string value match pattern
int QueryJsonStr(char *jsonStr, char *pattern, char *result)
{
    struct json_object *tmp;
    struct json_object *tmpPattern;

    if((tmp = json_tokener_parse(jsonStr)) < 0)
    {
	perror("QueryJsonStr invalid string\n");
	return -1;
    }
    if((tmpPattern = json_object_object_get(tmp, pattern)) < 0)
    {
	perror("QueryJsonStr invalid string\n");
	return -1;
    }
    strcpy(result, json_object_to_json_string(tmpPattern));
    json_object_put(tmp);
    json_object_put(tmpPattern);
    return 0;
}
//look up the float value match pattern
int QueryJsonFloat(char *jsonStr, char *pattern, float *result)
{
    struct json_object *tmp;
    struct json_object *tmpPattern;
    if((tmp = json_tokener_parse(jsonStr)) < 0)
    {
	perror("QueryJsonStr invalid string\n");
	return -1;
    }
    if((tmpPattern = json_object_object_get(tmp, pattern)) < 0)
    {
	perror("QueryJsonStr invalid string\n");
	return -1;
    }
    *result = json_object_get_double(tmpPattern);
    json_object_put(tmp);
    json_object_put(tmpPattern);
    return 0;
}

int QueryJsonInt(char *jsonStr, char *pattern, int *result)
{
    struct json_object *tmp;
    struct json_object *tmpPattern;

    if((tmp = json_tokener_parse(jsonStr)) < 0)
    {
	perror("QueryJsonStr invalid string\n");
	return -1;
    }
    if((tmpPattern = json_object_object_get(tmp, pattern)) < 0)
    {
	perror("QueryJsonStr invalid string\n");
	return -1;
    }

    *result = json_object_get_int(tmpPattern);
    json_object_put(tmp);
    json_object_put(tmpPattern);
    return 0;
}

void User2JsonStr(char *jsonStr, int id, char *userName, char *passwd, char *email, float x, float y, char *op, char *newName, char *newPasswd, char *newemail, char *comment)
{
    sprintf(jsonStr, "{\"USER_ID\" : %d, \"USER_NAME\" : \"%s\",\"PASSWD\" : \"%s\",\"EMAIL\" : \"%s\", \"X\" : %f, \"Y\": %f, \"OP\" : \"%s\",\"NEW_NAME\" : \"%s\", \"NEW_PASSWD\" : \"%s\", \"NEW_EMAIL\":\"%s\", \"COMMENT\" : \"%s\"}\n", id, userName, passwd, email, x, y, op, newName, newPasswd, newemail, comment);
}

void Status2JsonStr(char *jsonStr, int id, int success)
{
    if(success)
	sprintf(jsonStr, "{\"USER_ID\" : %d, \"STATUS\" : \"successful\"}\n", id);
    else
	sprintf(jsonStr, "{\"USER_ID\" : %d, \"STATUS\" : \"failed\"}\n", id);
}

void DeQuotation(char *str)
{
    int len = strlen(str);
    if(str != NULL && len > 2)
    {
	for(int i = 0; i < len - 2; i++)
	{
	    *(str + i) = *(str + i + 1);
	}
	*(str + len - 2) = (char)0;
    }
}

void PosDescription2JsonStr(char *jsonStr, char *description, int success)
{
    if(jsonStr != NULL)
    {
	if(success)
	    sprintf(jsonStr, "{\"POS_DESCRIPTION\" : \"%s\", \"STATUS\" : \"successful\"}\n", description);
	else
	    sprintf(jsonStr, "{\"POS_DESCRIPTION\" : \"\", \"STATUS\" : \"failed\"}\n");
    }
}

void Article2JsonStr(char *jsonStr, char *article, char *comment, int success)
{
    if(jsonStr != NULL)
    {
	if(success)
	    sprintf(jsonStr, "{\"ARTICLE\" : \"%s\", \"ARTICLE_COMMENT\" : \"%s\", \"STATUS\" : \"successful\"}\n", article, comment);
	else
	    sprintf(jsonStr, "{\"ARTICLE\" : \"\", \"ARTICLE_COMMENT\" : \"\", \"STATUS\" : \"failed\"}\n");
    }
}

void PosMap2JsonStr(char *jsonStr, float *east, float *north)
{
    sprintf(jsonStr,  "{\"E\" : %f, \"N\" : %f, \"E\" : %f, \"N\" : %f, \"E\" : %f, \"N\" : %f, \"E\" : %f, \"N\" : %f, \"E\" : %f, \"N\" : %f}\n", east[0], north[0], east[1], north[1], east[2], north[2], east[3], north[3], east[4], north[4]);
}
