#include "sql.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//init a database connection
void InitDataBase(MYSQL *conn)
{
    conn = mysql_init(NULL);
    if(mysql_real_connect(conn, "localhost", "root", "", "test_server", 0, NULL, 0) == NULL){
        //	perror("no database connection...\n");
        printf("no database connection...\n");
        exit(1);
    }
}
//conbine two string, no check
char * Add2StrEnd(char *head, char *tail)
{
    char *tmp = head;
    while(*tail != '\0')
        {
            *tmp++ = *tail++;
        }
    //point to the end of conbined string
    return tmp;
}


//replace for strcat
char * CombineStr(char * str1, char * str2)
{
    char *newStr = (char *)malloc(MAX_COMMAND_LEN * sizeof(char));
    char *tmp = strmov(newStr, str1);
    tmp = Add2StrEnd(tmp, str2);
    *tmp = (char)0;
    return newStr;
}


//construct SQL instruction
char *GetAddInstruction(MYSQL *connection, char *command, char *values[], int size)
{
    char *tmp = (char *)malloc(MAX_COMMAND_LEN * sizeof(char));
    char *tmppos;
    tmppos = strmov(tmp, command);

    for(int i = 0; i < size; i++)
        {
            //	tmppos += mysql_real_escape_string(connection, tmppos, values[i], strlen(values[i]));
            tmppos = Add2StrEnd(tmppos, values[i]);
            if(i < size - 1)
                tmppos += mysql_real_escape_string(connection, tmppos, ", ", 2);
        }
    tmppos = strmov(tmppos, HALF_BRACKET);
    *tmppos++ = (char)0;
    return tmp;
}
//add Quotation to a string
char * AddQuot(char *str)
{
    int len = strlen(str);
    char *newStr = (char *)malloc((len + 3) * sizeof(char));
    char *tmp = newStr;
    for(int i = 0; i < len + 2; i++)
        {
            if(i == 0 || i == len + 1)
                {
                    *tmp++ = 39;
                    continue;
                }
            else
                {
                    *tmp++ = *str++;
                }
        }
    *tmp = (char)0;
    return newStr;
}

//Insert a row to TAB user
void AddUser(MYSQL *conn, char *userName, char *passwd, char *email)
{

    char *instruction;
    int size = 3;
    char *values[size];

    values[0] =  AddQuot(userName);
    values[1] = AddQuot(passwd);
    values[2] = AddQuot(email);

    if((instruction = GetAddInstruction(conn, USER_ADD, values, size)) == NULL){
        fprintf(stderr, "error insert %s\n", userName);
    }
    else{
        printf("%s\n", instruction);	    
        mysql_query(conn, instruction);
    }

    //free 
    for(int i = 0; i < size; i++)
        {
            free(values[i]);    
            values[i] = NULL;
        }
}

//Delete a row by name of TAB user
void DeleteUser(MYSQL *conn, char *userName)
{
    char *instruction;
    userName = AddQuot(userName);
    instruction = CombineStr(USER_DEL_SQL, userName);
    printf("%s\n", instruction);
    mysql_query(conn, instruction);

    free(instruction);
    free(userName);
    instruction = NULL;
    userName = NULL;
}

int QueryUserById(MYSQL *conn, int id, char *userName, char *passwd, char *email, int *online)
{
    char *instruction;
    char tmp[10];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char userId[50];

    sprintf(userId, "%d", id);

    instruction = CombineStr(USER_QUERY_ID, userId);
    mysql_query(conn, instruction);
    free(instruction);
    instruction = NULL;
    if((result = mysql_store_result(conn)) == NULL)
        {
            perror("sql result error\n");
            return -1;
        }
    if((row = mysql_fetch_row(result)) != NULL)
        {
            if(userName != NULL)
                strcpy(userName, row[1]);
            if(passwd != NULL)
                strcpy(passwd, row[2]);
            if(email != NULL)
                strcpy(email, row[3]);
            strcpy(tmp, row[4]);
            if(online != NULL)
                *online = atoi(tmp);
        }
    else
        {
            perror("row error\n");
            return -1;
        }
    mysql_free_result(result);
    result = NULL;
    return 0;
}


int QueryUserByName(MYSQL *conn, int *id, char *userName, char *passwd, char *email, int *online)
{
    char *instruction;
    char tmpId[50];
    char tmpOnline[10];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char *tmpUserName = AddQuot(userName);
    instruction = CombineStr(USER_QUERY_NAME_SQL, tmpUserName);
//    printf("instruction %s\n", instruction);
    mysql_query(conn, instruction);
    free(instruction);
    instruction = NULL;
    if((result = mysql_store_result(conn)) == NULL)
    {
	perror("sql result error\n");
	return -1;
    }
    if((row = mysql_fetch_row(result)) != NULL)
    {
	strcpy(tmpId, row[0]);
	printf("id : %s\n", tmpId);
	if(passwd != NULL)
	    strcpy(passwd, row[2]);
	if(email != NULL)
	    strcpy(email, row[3]);
	strcpy(tmpOnline, row[4]);
	if(id != NULL)
	    *id = atoi(tmpId);
	if(online != NULL)
	    *online = atoi(tmpOnline);
    }
    else
    {
	perror("row error\n");
	return -1;
    }
    mysql_free_result(result);
    result = NULL;
    free(tmpUserName);
    tmpUserName = NULL;
    return 0;
}

char * GetUpdateInstruction(char *prefix, char *value, char *id, int haveQuot)
{
    char *tmp1, *tmp2, *tmp3;
    if(haveQuot)
	tmp1 = AddQuot(value);
    else
	tmp1 = value;
    tmp2 = CombineStr(prefix, tmp1);
    if(haveQuot)
    {
	free(tmp1);
	tmp1 = NULL;
    }
    tmp1 = CombineStr(" WHERE id=", id);
    tmp3 = CombineStr(tmp2, tmp1);
    free(tmp2);
    free(tmp1);
    tmp2 = NULL;
    tmp1 = NULL;
    
    tmp1 = CombineStr(USER_UPDATE_SQL, tmp3);
    free(tmp3);
    tmp3 = NULL;
    return tmp1;
}

void UpdateUser(MYSQL *conn, int id, char *userName, char *passwd, char *email, int online)
{
    char *instruction;
    char userId[50];
    char tmpOnline[10];
    sprintf(userId, "%d", id);
    if(userName != NULL)
        {
            instruction = GetUpdateInstruction("name=", userName, userId, TRUE);
            printf("%s\n", instruction);
            mysql_query(conn, instruction);
            free(instruction);
            instruction = NULL;
        }
    
    if(passwd != NULL)
        {
            instruction = GetUpdateInstruction("passwd=", passwd, userId, TRUE);
            printf("%s\n", instruction);
            mysql_query(conn, instruction);
            free(instruction);
            instruction = NULL;
        }

    if(email != NULL)
        {
            instruction = GetUpdateInstruction("email=", email, userId, TRUE);
            printf("%s\n", instruction);
            mysql_query(conn, instruction);
            free(instruction);
            instruction = NULL;
        }
    if(online == 0 || online == 1)
        {
            sprintf(tmpOnline, "%d", online);
            instruction = GetUpdateInstruction("online=", tmpOnline, userId, FALSE);//no quotation
            printf("%s\n", instruction);
            mysql_query(conn, instruction);
            free(instruction);
            instruction = NULL;
        }
}

int LoginUser(MYSQL *conn, char *userName, char *userPasswd)
{
    int id;
    char passwd[20];
    char email[50];
    int online;
    char *instruction;
    QueryUserByName(conn, &id, userName, passwd, email, &online);
    if(strcmp(passwd, userPasswd) == 0){
	if(id > 0)// && strcmp(passwd, userPasswd) == 0)
	{
	    UpdateUser(conn, id, NULL, NULL, NULL, TRUE);
	    return id;
	}
	else
	    return -1;   
    }
    else
	return -1;
}

void LogoutUser(MYSQL *conn, int id)
{
    UpdateUser(conn, id, NULL, NULL, NULL, FALSE);
}

//get last insert row id
int  GetLastInsertId(MYSQL *conn)
{

    MYSQL_RES *result;
    MYSQL_ROW row;
    int lastId;
    mysql_query(conn, LAST_INSERT_ID);

    if((result = mysql_store_result(conn)) == NULL)
        {
            perror("result error\n");
            return -1;
        }
    if((row = mysql_fetch_row(result)) == NULL)
        {
            perror("row error\n");
            return -1;
        }
    lastId = atoi(row[0]);
    mysql_free_result(result);
    result = NULL;
    return lastId;
}

void AddPosMark(MYSQL *conn, float eastDegree, float northDegree, char *description, char *posName)
{

    char *instruction;
    int size = 4;
    char *values[size];
    values[0] =(char *)malloc(MAX_COMMAND_LEN * sizeof(char));
    values[1] =(char *)malloc(MAX_COMMAND_LEN * sizeof(char));
    sprintf(values[0], "%f", eastDegree);
    sprintf(values[1], "%f", northDegree);
    values[2] = AddQuot(description);
    values[3] = AddQuot(posName);


    if((instruction = GetAddInstruction(conn, POS_ADD, values, size)) == NULL){
        printf("error add pos\n");
    }
    else{
        printf("%s\n", instruction);	    
        mysql_query(conn, instruction);
    }
    printf("instruction %s\n", instruction);


    //free 
    for(int i = 0; i < size; i++)
        {
            free(values[i]);    
            values[i] = NULL;
        }
}

int GetPosInfo(MYSQL *conn, char *posName, char *description)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    char *instruction;
    char *tmpPosName;
    tmpPosName = AddQuot(posName);
    instruction = CombineStr(POS_QUERY_NAME, tmpPosName);
    printf("instruction %s\n", instruction);
    mysql_query(conn, instruction);
    if((result = mysql_store_result(conn)) == NULL)
        {
            perror("result error\n");
            return -1;
        }
    if((row = mysql_fetch_row(result)) == NULL)
        {
            perror("row error\n");
            return -1;
        }
    if(description != NULL)
        strcpy(description, row[3]);
    free(instruction);
    instruction = NULL;
    free(tmpPosName);
    tmpPosName = NULL;
    return 0;
}

float GetPosDistence(float east1, float north1, float east2, float north2)
{
    return pow(abs(east1 - east2), 2) + pow(abs(north1 - north2), 2);
}
//check if the point should be add into nearest Set
void GetNearestPosSet(float *eastSet, float *northSet, int setSize, float curEast, float curNorth, float posEast, float posNorth)
{
    for(int i = 0; i < setSize; i++)
        {
            if(eastSet[i] == 0 && northSet[i] == 0)
                {
                    eastSet[i] = posEast;
                    northSet[i] = posNorth;
                    break;
                }
	
            if(GetPosDistence(eastSet[i], northSet[i], curEast, curNorth) > GetPosDistence(posEast, posNorth, curEast, curNorth))
                {
                    eastSet[i] = posEast;
                    northSet[i] = posNorth;
                    break;
                }
        }
}
//select 20 point to return
void GetPosMap(MYSQL *conn,  float *eastSet, float *northSet, float curEast, float curNorth, int *posNum)
{
    MYSQL_RES *result;
    MYSQL_ROW row;

    mysql_query(conn, POS_QUERY_LOCAL_SQL);
    if((result = mysql_store_result(conn)) == NULL)
        {
            perror("result error\n");
        }
    int i = 0;
    while(((row = mysql_fetch_row(result)) != NULL))
        {
            if((curEast != atof(row[0])) && (curNorth != atof(row[1])))
                {
                    GetNearestPosSet(eastSet, northSet, 5, curEast, curNorth, atof(row[0]), atof(row[1]));
                    i++;
                }
        }
    if(i < 5)
        *posNum = i;
    else
        *posNum = 5;
}

int QueryArticleById(MYSQL *conn, int articleId, char *authorName, int *authorId, char *articleName, char *article, char *comment)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    char *instruction;
    char tmpAuthorId[50];
    char tmpArticleId[50];
    //instead of itoa
    sprintf(tmpArticleId, "%d", articleId);
    instruction = CombineStr(ARTICLE_QUERY_ID, tmpArticleId);
    mysql_query(conn, instruction);
    free(instruction);
    instruction = NULL;
    if((result = mysql_store_result(conn)) == NULL)
        {
            perror("result error\n");
            return -1;
        }
    if((row = mysql_fetch_row(result)) != NULL)
        {
            if(authorName != NULL)
                strcpy(authorName, row[1]);
            strcpy(tmpAuthorId, row[2]);
            if(articleName != NULL)
                strcpy(articleName, row[3]);
            if(article != NULL)
                strcpy(article, row[4]);
            if(comment != NULL)
                strcpy(comment, row[5]);
            if(authorId != NULL)
                *authorId = atoi(tmpAuthorId);
        }
    else
        {
            perror("row error\n");
            return -1;
        }
    mysql_free_result(result);
    result = NULL;
    return 0;
}

void GetArticleRead(MYSQL *conn, int articleId, char *article, char *comment)
{
    QueryArticleById(conn, articleId, NULL, NULL, NULL, article, comment);
}

void AddArticle(MYSQL *conn, char *authorName, int authorId, char *articleName, char *article)
{
    char *instruction;
    char tmpAuthorId[50];
    int size = 4;
    char *values[size];
    printf(tmpAuthorId, "%d", authorId);
    values[0] =  AddQuot(authorName);
    values[1] = AddQuot(tmpAuthorId);
    values[2] = AddQuot(articleName);
    values[4] = AddQuot(article);

    if((instruction = GetAddInstruction(conn, ARTICLE_ADD, values, size)) == NULL){
        fprintf(stderr, "error add article %s\n", articleName);
    }
    else{
        printf("%s\n", instruction);	    
        mysql_query(conn, instruction);
    }
    //free 
    for(int i = 0; i < size; i++)
        {
            free(values[i]);    
            values[i] = NULL;
        }
}

void UpdateArticle(MYSQL *conn, int articleId, char *articleName, char *article)
{
	char *instruction;
	char tmpArticleId[50];
	sprintf(tmpArticleId, "%d", articleId);
	if(articleName != NULL)
        {
            instruction = GetUpdateInstruction("name=", articleName, tmpArticleId, TRUE);
            printf("%s\n", instruction);
            mysql_query(conn, instruction);
            free(instruction);
            instruction = NULL;
        }
    
	if(article != NULL)
        {
            instruction = GetUpdateInstruction("article=", article, tmpArticleId, TRUE);
            printf("%s\n", instruction);
            mysql_query(conn, instruction);
            free(instruction);
            instruction = NULL;
        }
}

void ReviewArticle(MYSQL *conn, int articleId, int userId, char *comment)
{
    char *instruction;
    char tmpArticleId[50];
    char tmpUserId[50];
    char userName[20];
    sprintf(tmpArticleId, "%d", articleId);
    sprintf(tmpUserId, "%d", userId);
    QueryUserById(conn, userId, userName, NULL, NULL, NULL);
    char formatComment[1000];
    char oldComment[1000];
    QueryArticleById(conn, articleId, NULL, NULL, NULL, NULL, oldComment);
    sprintf(formatComment, "%s : %s\n", userName, comment);
    strcat(oldComment, formatComment);

    instruction = GetUpdateInstruction("comment=", formatComment, tmpArticleId, TRUE);
    printf("%s\n", instruction);
    mysql_query(conn, instruction);
    free(instruction);
    instruction = NULL;
}


