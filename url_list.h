#ifndef URL_LIST_H
#define URL_LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct url_list_node* url_list_ptr;

typedef struct url_list_node{
    char* url;
    int appearances;
    url_list_ptr next;
}url_list_node;

void create_urll (url_list_ptr* list);
int is_empty_urll (url_list_ptr list);
void insert_at_end_urll (url_list_ptr * list, char* new_url);
void clear_urll (url_list_ptr list) ;
void print_urll(url_list_ptr list);

#endif