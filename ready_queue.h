#ifndef READY_QUEUE_H
#define READY_QUEUE_H

#include "parser.h"
#include <unistd.h>

typedef struct worker* worker_ptr;

typedef struct worker {
    int pid;
    int fd;
    char* fifo;
    worker_ptr next;
}worker;



void create_workers_list (worker_ptr* list);
int is_empty_workers_list (worker_ptr list);
void insert_at_end_workers_list (worker_ptr * list, int new_pid, int new_fd, char* new_fifo);
void clear_workers_list (worker_ptr list) ;
void print_workers_list(worker_ptr list);
int remove_from_start_workers_list(worker_ptr *list);
int get_fd_workers_list(worker_ptr list, int pid);
char* get_fifo_workers_list(worker_ptr list, int pid);

#endif


