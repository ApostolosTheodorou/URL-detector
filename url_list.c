#include "url_list.h"

void create_urll (url_list_ptr* list) {
    *list= NULL;
}

int is_empty_urll (url_list_ptr list) {
    if (list == NULL)
        return 1;
    else 
        return 0;
}

void insert_at_end_urll (url_list_ptr * list, char* new_url) {
    int length;
    
    length= strlen(new_url);   //find the new url's length
    //if the list is empty, insert the new_url as the first url
    if (is_empty_urll(*list)) {           
        (*list)= malloc(sizeof(url_list_node));
        (*list)->url= malloc(length+1 * sizeof(char));
        strcpy((*list)->url, new_url);
        (*list)->appearances= 1;
        (*list)->next= NULL;
    }
    else {
    //if the list already contains some urls, 
    //insert the new new url after the last url in case it does NOT already exist
    //if it exists, then increase it's appearances by 1
        while (*list != NULL) { //find the last node of the list
            if (strcmp((*list)->url, new_url) == 0) { //if url was found
                (*list)->appearances++;
                return;
            }
            list= &((*list)->next);
        }
        *list= malloc(sizeof(url_list_node));      
        (*list)->url= malloc(length+1 * sizeof(char));
        strcpy((*list)->url, new_url);
        (*list)->appearances= 1;
        (*list)->next= NULL; 
    }
}

//frees the allocated nodes apart from the first node which is freed in main
void clear_urll (url_list_ptr list) {
    if (list->next == NULL) {
        free (list->url);
    }
    else {
        clear_urll(list->next);
        free (list->url);
        free (list->next);
    }
}

void print_urll(url_list_ptr list) {
    printf("( ");
    while (list != NULL) {
        printf("\"%s %d\", ", list->url, list->appearances);
        list= list->next;
    }
    printf(")\n");
}