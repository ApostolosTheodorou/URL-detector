#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "word_list.h"
#include "url_list.h"
#include "parser.h"
#include "ready_queue.h"

int killed= 0; //if cntrl C is received the signal handler will set this to 1

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

void child_finished (int signum) {
    signal(SIGCHLD, child_finished);
}

void terminate_all (int signum) {
    signal(SIGINT, terminate_all);
    killed= 1;
}

int main (int argc, char* argv[]) {

    char* path, *prev_dir, buf[1024], buf2[100], *current_file, *pipe_name, *ith_char;
    int status, listener_pipe[2], bytes_read, bytes_written, filenames_count, i, fd, ith= 0;
    pid_t pid, listener_pid;
    word_list_ptr filenames;
    worker_ptr ready_workers, all_workers; 
    create_workers_list(&ready_workers); create_workers_list(&all_workers);

    signal(SIGCHLD, child_finished);
    signal(SIGINT, terminate_all);

    //Create the directories where the new files and fifos will be sent
    if (mkdir("../pipes/", 0744)) {
        perror("mkdir");
        exit (-12);
    }
    if (mkdir("../output_files/", 0744)) {
        perror("mkdir");
        exit (-12);
    }
    //Check the correct use of the program
    switch (argc)
    {
    case 1:
        path= malloc(3 * sizeof(char));
        strcpy(path, "./");
        break;
    case 3:
        if (strcmp(argv[1], "-p") != 0) {
            perror("Use like this:\n./sniffer -p <path>\n");
            return -3;
        }
        path= malloc ((strlen(argv[2])+1) * sizeof(char));
        strcpy(path, argv[2]);
    default:
        break;
    }
    
    //That is the manager's code
    
    //Create the pipe to interact with the listener's process
    if (pipe(listener_pipe) == -1) {
        perror("listener's pipe");
        return -1;
    }

    //Create the listener's process
    switch (pid= fork())
    {
    case -1:
        perror("listener's fork");
        break;
    case 0: //listener's code
        close(listener_pipe[0]); //close the reading for the listener's process
        if (dup2(listener_pipe[1], 1) == -1) {//match stdout with the writing end of the pipe
            perror("dup2");
            return -2;
        }
        close(listener_pipe[1]);
        execlp("inotifywait", "inotifywait", "-m", path, NULL);
        break;
    default: //manager's code
        close(listener_pipe[1]); //close the writing for the manager's process
        listener_pid= pid;
        break;
    }

    while (!killed) { //manager's infinite loop
 
        //Read the listener's pipe
        bytes_read= read(listener_pipe[0], buf, 1024);

        //find the filenames in the buffer and save them in the list
        find_filenames (buf, &filenames, bytes_read); 
        
        //count how many filenames exist in the filenames list
        filenames_count= count_filenames(filenames); 

        for (i= 0; i < filenames_count ; i++) { //For every new file
            if (!is_empty_wl(filenames)) {
                current_file= remove_from_start_wl(&filenames); //get the file's name
                
                //if there are no workers available
                if (is_empty_workers_list(ready_workers)) {
                    //create a named pipe with name pipe_# 
                    ith++;
                    inttoascii(ith, &ith_char);
                    pipe_name= malloc(((strlen("../pipes/pipe_")+strlen(ith_char)+1)) * sizeof(char));
                    strcpy(pipe_name, "../pipes/pipe_");
                    strcat(pipe_name, ith_char);
                    if (mkfifo(pipe_name, 0666) == -1) {
                        perror("pipe (mkfifo");
                        return -4;
                    }
                   
                    //Create a new worker to handle this file
                    pid= fork();
                    switch (pid)
                    {
                    case 0: //worker's code
                        //worker opens the pipe
                        if ((fd= open(pipe_name, O_RDONLY)) < 0) {
                            perror("worker open pipe");
                            return -8;
                        }
                        while (1) { //worker's infinite loop
                            if (read(fd, buf2, 100) < 0) { //read the file's name
                                perror("worker reading pipe");
                                return -9;
                            }
                            find_urls(buf2); //Does the job that has to be done
                            kill(getppid(), SIGCHLD);
                            raise(SIGSTOP); //Send a message to hisself to stop
                        }
                        break;

                    case -1:
                        perror("fork new worker");
                        return -7;

                    default: //manager's code
                        //open that pipe and get a file descriptor for it
                        if ((fd= open(pipe_name, O_WRONLY)) < 0) {
                            perror("open pipe");
                            return -5;
                        }
                        //write the file's name in the pipe
                        if ((bytes_written= write(fd, current_file, strlen(current_file)+1)) == -1) {
                            perror("write in pipe");
                            return -6;
                        }
                        //inserts the new worker in the list of workers
                        insert_at_end_workers_list(&all_workers, pid, fd, pipe_name);
                        pid= waitpid(-1, &status, WUNTRACED); //wait for the specific worker to finish
                        //when worker has finished insert him in the ready queue
                        insert_at_end_workers_list(&ready_workers, pid, get_fd_workers_list(all_workers, pid), pipe_name);
                        break;
                    }
                }
                else { //if there is available worker
                    pid= remove_from_start_workers_list(&ready_workers);
                    fd= get_fd_workers_list(all_workers, pid);
                    kill(pid, SIGCONT);
                    //write the file's name in the pipe
                    if ((bytes_written= write(fd, current_file, strlen(current_file)+1)) == -1) {
                        perror("write in pipe");
                        return -6;
                    }  
                    pid= waitpid(-1, &status, WUNTRACED); //wait for the specific worker to finish
                    //when worker has finished insert him in the ready queue
                    insert_at_end_workers_list(&ready_workers, pid, get_fd_workers_list(all_workers, pid), pipe_name);
                }
                
            }
            free (current_file);
            free(pipe_name);
            free(ith_char);
        }
        //if the process received SIGINT kill all the child processes 
        //and free the allocated memory
        if (killed) { 
             free(path);
             clear_workers_list(ready_workers);
             free(ready_workers);
             worker_ptr temp= all_workers;
             while (temp != NULL) { 
                 fd= get_fd_workers_list(all_workers, temp->pid);
                 close(fd);
                 kill(temp->pid, SIGKILL);
                 temp= temp->next;
             }
             kill(listener_pid, SIGKILL);
             clear_workers_list(all_workers);
             free(all_workers); 
             free(temp);
        }
    }
    
}