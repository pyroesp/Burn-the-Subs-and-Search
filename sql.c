#include "sql.h"


char *SQL_queryRead(char *filename){
    int i;
    int file_size;
    char *content = NULL;
    FILE *fp = NULL;

    fp = fopen(filename, "rb");
    if (fp == NULL)
        return NULL;

    for(i = 0; fgetc(fp) != EOF; i++);
    file_size = i;
    rewind(fp);

    content = (char*)malloc(sizeof(char) * (file_size + 1));
    if (content == NULL)
        return NULL;
    memset(content, 0, sizeof(char) * (file_size + 1));
    
    for (i = 0; i < file_size; i++)
        content[i] = fgetc(fp);

    fclose(fp);
    return content;
}

char *SQL_queryAddArgument(char *sp, int type, void *data){
    int *data_int = NULL;
    char *data_char = NULL;
    char int_to_str[16];
    char *new_sp = NULL;

    switch(type){
        case SQL_SP_INT:
            data_int = (int*)data;
            snprintf(int_to_str, 16, "%d", *data_int);
            new_sp = (char*)malloc(sizeof(char) * (strlen(sp) + strlen(int_to_str)));
            if(new_sp == NULL)
                return NULL;
            sprintf(new_sp, sp, *data_int);
            break;
        case SQL_SP_STRING:
            data_char = (char*)data;
            new_sp = (char*)malloc(sizeof(char) * (strlen(sp) + strlen(data_char)));
            if(new_sp == NULL)
                return NULL;
            sprintf(new_sp, sp, data_char);
            break;
        case SQL_SP_NONE:
        default:
            return sp;
    }

    free(sp);
    return new_sp;
}

int SQL_callbackReadData(void *p, int columns, char **column_data, char **column_title){
    int i;
    SQL_table *sub = (SQL_table*)p;

    if (sub == NULL)
        return -1;

    sub->data_size++;
    sub->id = (int*)realloc(sub->id, sizeof(int) * sub->data_size);
    sub->name = (char**)realloc(sub->name, sizeof(char*) * sub->data_size);
    sub->name[sub->data_size - 1] = (char*)malloc(sizeof(char*) * SQL_USERNAME_SIZE);
    sub->pos_x = (int*)realloc(sub->pos_x, sizeof(int) * sub->data_size);
    sub->pos_y = (int*)realloc(sub->pos_y, sizeof(int) * sub->data_size);
    sub->font_size = (int*)realloc(sub->font_size, sizeof(int) * sub->data_size);
    sub->entry_time = (char**)realloc(sub->entry_time, sizeof(char*) * sub->data_size);
    sub->entry_time[sub->data_size - 1] = (char*)malloc(sizeof(char) * SQL_ENTRY_TIME_SIZE);

    for (i = 0; i < columns; i++){
        if (!strcmp(column_title[i], SQL_TITLE_ID)){
            sscanf(column_data[i], "%d", &sub->id[sub->data_size - 1]);
        } else if (!strcmp(column_title[i], SQL_TITLE_USERNAME)){
            strcpy(sub->name[sub->data_size - 1], column_data[i]);
        } else if (!strcmp(column_title[i], SQL_TITLE_POS_X)){
            sscanf(column_data[i], "%d", &sub->pos_x[sub->data_size - 1]);
        } else if (!strcmp(column_title[i], SQL_TITLE_POS_Y)){
            sscanf(column_data[i], "%d", &sub->pos_y[sub->data_size - 1]);
        } else if (!strcmp(column_title[i], SQL_TITLE_FONT_SIZE)){
            sscanf(column_data[i], "%d", &sub->font_size[sub->data_size - 1]);
        } else if (!strcmp(column_title[i], SQL_TITLE_ENTRY)){
            strcpy(sub->entry_time[sub->data_size - 1], column_data[i]);
        }
    }

    return 0;
}
