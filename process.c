#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ProcessRecvData(MYSQL* conn, char *recv, char *echo, int sockfd)
{
    int operation;
    char jsonStr[MAX_JSON_LEN];
    int userId;
    int online;
    int articleId;
    char userName[20];
    char passwd[20];
    char email[50];
    char posName[50];
    char comment[1000];
    char article[3000];
    float eastDegree, northDegree;
    float eastSet[5] = {0};
    float northSet[5] = {0};
    int mapPointNum;
    char description[MAX_DESCRIPTION_LEN];
    //pre-process string 
    if(Turn2JsonStr(recv, jsonStr) == -1)
	return -1;
    printf("json %s\n", jsonStr);
    //get user Id, which is necessary for all kinds of request
    if(QueryJsonInt(jsonStr, "USER_ID", &userId) == -1)
	return -1;

    //get OP section
    if(QueryJsonInt(jsonStr, "OP", &operation) == -1)
	return -1;
    if(&operation == NULL)
    {
	perror("bad operation\n");
	return -1;
    }

//    if(userId <= 0 && (operation != USER_REG || operation != USER_LOGIN))
//    {
//	perror("invalid user id\n");
//	return -1;
//    }

    switch(operation)
    {
	//register a user account
    case USER_REG:
	printf("OP : USER_REG\n");
	if(QueryJsonStr(jsonStr, "USER_NAME", userName) == -1)
	    return -1;
	if(QueryJsonStr(jsonStr, "USER_PASSWD", passwd) == -1)
	    return -1;
	if(QueryJsonStr(jsonStr, "USER_EMAIL", email) == -1)
	    return -1;
	DeQuotation(userName);
	DeQuotation(passwd);
	DeQuotation(email);
	if(userName != NULL && passwd != NULL && email != NULL)
	{
	    AddUser(conn, userName, passwd, email);
	    userId = GetLastInsertId(conn);
	    Status2JsonStr(echo, userId, TRUE);
	}
	else
	{
	    perror("invalid user data to add\n");
	    Status2JsonStr(echo, 0, FALSE);
	}
	break;

	//user login
    case USER_LOGIN:
	printf("OP : USER_LOGIN\n");
	if(QueryJsonStr(jsonStr, "USER_NAME", userName) == -1)
	    return -1;
	if(QueryJsonStr(jsonStr, "USER_PASSWD", passwd) == -1)
	    return -1;
	DeQuotation(userName);
	DeQuotation(passwd);
	if((userId = LoginUser(conn, userName, passwd)) == -1)
	    Status2JsonStr(echo, 0, FALSE);
	else
	    Status2JsonStr(echo, userId, TRUE);
	break;

	//user logout
    case USER_LOGOUT:
	printf("OP : USER_LOGOUT\n");
	LogoutUser(conn, userId);
	Status2JsonStr(echo, userId, TRUE);
	break;

	//update user data
    case USER_UPDATE:
	//get new values
	printf("OP : USER_UPDATE\n");
	if(QueryJsonStr(jsonStr, "USER_NAME", userName) == -1)
	    return -1;
	if(QueryJsonStr(jsonStr, "USER_PASSWD", passwd) == -1)
	    return -1;
	if(QueryJsonStr(jsonStr, "USER_EMAIL", email) == -1)
	    return -1;
	if(QueryJsonInt(jsonStr, "USER_ONLINE", &online) == -1)
	    return -1;
	DeQuotation(userName);
	DeQuotation(passwd);
	DeQuotation(email);
	//update values, null for no change
	UpdateUser(conn, userId, userName, passwd, email, online);
	//update not change user id
	Status2JsonStr(echo, userId, TRUE);
	break;

	//delete user
    case USER_DEL:
	printf("OP : USER_DEL\n");
	if(QueryJsonStr(jsonStr, "USER_NAME", userName) == -1)
	    return -1;
	DeQuotation(userName);
	DeleteUser(conn, userName);
	Status2JsonStr(echo, userId, TRUE);
	break;

	//mark a pos on the map
    case POS_MARK:
	printf("OP : POS_MARK\n");
	if(QueryJsonFloat(jsonStr, "POS_COORDINATE_E", &eastDegree) == -1)
	    return -1;
	if(QueryJsonFloat(jsonStr, "POS_COORDINATE_N", &northDegree) == -1)
	    return -1;
	if(QueryJsonStr(jsonStr, "POS_DESCRIPTION", description) == -1)
	    return -1;
	if(QueryJsonStr(jsonStr, "POS_NAME", posName) == -1)
	    return -1;
	DeQuotation(description);
	DeQuotation(posName);
	AddPosMark(conn, eastDegree, northDegree, description, posName);
	Status2JsonStr(echo, userId, TRUE);
	break;
	
	//get 20 marked positions nearby
    case POS_MAP:
	printf("OP : POS_MAP\n");
	if(QueryJsonFloat(jsonStr, "POS_COORDINATE_E", &eastDegree) == -1)
	    return -1;
	if(QueryJsonFloat(jsonStr, "POS_COORDINATE_N", &northDegree) == -1)
	    return -1;
	GetPosMap(conn, eastSet, northSet, eastDegree, northDegree, &mapPointNum);
	PosMap2JsonStr(echo, eastSet, northSet);
	printf("point Num %d\n", mapPointNum);
	break;
	
	//get the comment of the position
    case POS_INFO:
	if(QueryJsonStr(jsonStr, "POS_NAME", posName) == -1)
	    return -1;
	DeQuotation(posName);
	GetPosInfo(conn, posName, description);
	if(strlen(description) != 0)
	    PosDescription2JsonStr(echo, description, TRUE);
	else
	    PosDescription2JsonStr(echo, description, TRUE);
	break;
	
	//get 20 other's article
    case ARTICLE_FETCH:
	break;

	//read article
    case ARTICLE_READ:
	if(QueryJsonInt(jsonStr, "ARTCLE_ID", &articleId) == -1)
	    return -1;
	GetArticleRead(conn, articleId, article, comment);
	Article2JsonStr(echo, article, comment, TRUE);
	break;

	//release article
    case ARTICLE_RELEASE:
	break;

    case ARTICLE_REVIEW:
	break;

    default:
	perror("unknown operation\n");
	break;
    }
    return 0;
}
