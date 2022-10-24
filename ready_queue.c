#include "ready_queue.h"

//Initializes a workers list
void create_workers_list (worker_ptr* list) {
    *list= NULL;
}

//Returns 1 when the workers list is empty
int is_empty_workers_list (worker_ptr list) {
    if (list == NULL)
        return 1;
    else 
        return 0;
}

//Inserts a new worker (pid and fd) at the end of the list
void insert_at_end_workers_list (worker_ptr * list, int new_pid, int new_fd, char* new_fifo) {
    
    //if the list is empty, insert the new worker as the first word
    if (is_empty_workers_list(*list)) {           
        (*list)= malloc(sizeof(worker));
        (*list)->pid= new_pid;
        (*list)->fd= new_fd;
        (*list)->fifo= malloc((strlen(new_fifo)+1) * sizeof(char));
        strcpy((*list)->fifo, new_fifo);
        (*list)->next= NULL;
    }
    else {
    //if the list already contains some workers, 
    //insert the new new worker after the last worker
        list= &((*list)->next);
        while (*list != NULL) { //find the last node of the list
            list= &((*list)->next);
        }
        *list= malloc(sizeof(worker));      
        (*list)->pid= new_pid;
        (*list)->fd= new_fd;
        (*list)->fifo= malloc((strlen(new_fifo)+1) * sizeof(char));
        strcpy((*list)->fifo, new_fifo);
        (*list)->next= NULL; 
    }
}

//Clears the allocated memory for the workers list
void clear_workers_list (worker_ptr list) {
    if (list->next == NULL) {
        close(list->fd);
        free ((list)->fifo);
    }
    else {
        clear_workers_list(list->next);
        close(list->fd);
        free ((list)->fifo);
        free (list->next);
    }
}

//prints the list given
void print_workers_list(worker_ptr list) {
    while (list != NULL) {
        printf("PID: %d fd: %d --> ", list->pid, list->fd);
        list= list->next;
    }
    printf("\n");
}

//Removes the first node of the list and returns it's pid
int remove_from_start_workers_list(worker_ptr *list) {
    worker_ptr temp;
    int pid;

    if (is_empty_workers_list(*list)){
        perror("Trying to remove worker from empty list");
        return -1;
    }
    temp= (*list)->next;
    pid= (*list)->pid;
    free((*list));
    (*list)= temp;
    return pid;
}

//Gets the fd that corresponds to the named pipe of the worker with the given pid
int get_fd_workers_list(worker_ptr list, int pid) {
    while (list->pid != pid) {
        list= list->next;
    }
    return list->fd;
}

char* get_fifo_workers_list(worker_ptr list, int pid) {
    while (list->pid != pid) {
        list= list->next;
    }
    return list->fifo;
}