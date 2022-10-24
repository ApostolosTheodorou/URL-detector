#ifndef WORD_LIST_H
#define WORD_LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct word_list_node* word_list_ptr;

typedef struct word_list_node{
    char* word;
    word_list_ptr next;
}word_list_node;

typedef int bool;

void create_wl (word_list_ptr* list);
bool is_empty_wl (word_list_ptr list);
void insert_at_end_wl (word_list_ptr * list, char* new_word);
void clear_wl (word_list_ptr list) ;
void print_wl(word_list_ptr list);
char* remove_from_start_wl(word_list_ptr *list);

#endif