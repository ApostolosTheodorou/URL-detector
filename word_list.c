#include "word_list.h"

void create_wl (word_list_ptr* list) {
    *list= NULL;
}

bool is_empty_wl (word_list_ptr list) {
    if (list == NULL)
        return 1;
    else 
        return 0;
}

void insert_at_end_wl (word_list_ptr * list, char* new_word) {
    int length;
    
    length= strlen(new_word);   //find the new word's length

    //if the list is empty, insert the new_word as the first word
    if (is_empty_wl(*list)) {           
        (*list)= malloc(sizeof(word_list_node));
        (*list)->word= malloc(length+1 * sizeof(char));
        strcpy((*list)->word, new_word);
        (*list)->next= NULL;
    }
    else {
    //if the list already contains some words, 
    //insert the new new word after the last word
        list= &((*list)->next);
        while (*list != NULL) { //find the last node of the list
            list= &((*list)->next);
        }
        *list= malloc(sizeof(word_list_node));      
        (*list)->word= malloc(length+1 * sizeof(char));
        strcpy((*list)->word, new_word);
        (*list)->next= NULL; 
    }
}

//frees the allocated nodes apart from the first node which is freed in main
void clear_wl (word_list_ptr list) {
    if (list->next == NULL) {
        free (list->word);
    }
    else {
        clear_wl(list->next);
        free (list->word);
        free (list->next);
    }
}

void print_wl(word_list_ptr list) {
    printf("( ");
    while (list != NULL) {
        printf("\"%s\", ", list->word);
        list= list->next;
    }
    printf(")\n");
}

char* remove_from_start_wl(word_list_ptr *list) {
    word_list_ptr temp;
    char* file;

    if (is_empty_wl(*list)){
        perror("Trying to remove file from empty list");
        return NULL;
    }
    temp= (*list)->next;
    file= malloc(strlen(((*list)->word)+1) * sizeof(char));
    strcpy(file, (*list)->word);
    free((*list));
    (*list)= temp;
    return file;
}