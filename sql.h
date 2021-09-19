#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"


/** SQL DB **/
#define SQL_DATABASE "DB/dataBase.db"

/** SQL Stored Procedure **/
#define SQL_GETALLNAMES "SP/GetAllNames.sql"
#define SQL_GETINFOBYID "SP/GetAllNames.sql"
#define SQL_GETINFOBYNAME "SP/GetAllNames.sql"

/** SQL Stored Procedure arguments **/
#define SQL_SP_NONE 0
#define SQL_SP_INT 1
#define SQL_SP_STRING 2

/** SQL Column Titles **/
#define SQL_TITLE_ID "id"
#define SQL_TITLE_USERNAME "userName"
#define SQL_TITLE_POS_X "positionX"
#define SQL_TITLE_POS_Y "positionY"
#define SQL_TITLE_FONT_SIZE "fontSize"
#define SQL_TITLE_ENTRY "entryTime"

#define SQL_USERNAME_SIZE 64
#define SQL_ENTRY_TIME_SIZE 32


typedef struct{
    char **name;
    int data_size;
    int *id;
    int *pos_x;
    int *pos_y;
    int *font_size;
    char **entry_time;
}SQL_table;

// Open SP file
char *SQL_queryRead(char *filename);
// Replace %s in query
char *SQL_queryAddArgument(char *sp, int type, void *data);

// Read columns into the sql structure
int SQL_callbackReadData(void *p, int columns, char **column_data, char **column_title);
