#include "parser.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PERM 0664

//Tokenizes the input buffer that is given as an argument
//The tokens are inserted in the linked list that is also given as a parameter
void tokenize (char* buf, word_list_ptr *list, int bytes_in_buffer) {
    char current_word[100], current_char;
    int i, j=0;

    for (i=0 ; i < bytes_in_buffer ; i++) {
        if (buf[i] != ' ' && buf[i] != '\t' && buf[i] != '\n') { //if not whitespace
            current_word[j++]= buf[i];
            if (i == bytes_in_buffer-1) { //if the last byte is not a whitespace 
                                          //don't miss the last token
                current_word[j]= '\0'; 
                insert_at_end_wl(list, current_word);
            }
        }
        else { //when a whitespace is reached, then a full token has been read
            if (j == 0)  //ignore white spaces in a row 
                continue;
            current_word[j]= '\0'; 
            insert_at_end_wl(list, current_word);
            j= 0;
        }
    }
}

//Receives a buffer with inotifywait's output, finds the files' names in it
//and inserts them into a list
void find_filenames (char* buf, word_list_ptr *filenames, int bytes_in_buffer) {
    bool previous_token_was_keyword= 0;
    word_list_ptr tokens, current_token; create_wl(&tokens);
    
    create_wl(filenames);
    tokenize(buf, &tokens, bytes_in_buffer);
    current_token= tokens;
    while (current_token != NULL) {
        if (strcmp(current_token->word, "CREATE") == 0 || strcmp(current_token->word, "MOVED_TO") == 0){
            previous_token_was_keyword= 1; //set the flag to true      
        }
        current_token= current_token->next;
        if (previous_token_was_keyword) { //then this token must be a file's name
            insert_at_end_wl(filenames, current_token->word); //insert token in filenames
            previous_token_was_keyword= 0; //reset the flag
        }

    }
}

//Reads a file whoose name is given as an argument
//Finds the urls in it and inserts them into url_list
//After all the urls have been inserted, a new file with a
//specific format and name <filename>.out is produced in another directory
int find_urls (char* filename) {
    word_list_ptr tokens, current_token; create_wl(&tokens);
    url_list_ptr list, list_iterator; create_urll(&list);
    char buf[8192], url[256], *appear, *output_path, *output_line;
    int input_file, output_file, path_length, length, i, j;
    ssize_t n;

    //open the input file
    if ((input_file= open(filename, O_RDONLY)) == -1) {
        perror("open input_file");
        return -2;
    } 
    
    //create the output path
    path_length= strlen("../output_files/") + strlen(filename) + strlen(".out") +1;
    output_path= malloc(path_length * sizeof(char));
    strcpy(output_path, "../output_files/");
    strcat(output_path, filename);
    strcat(output_path, ".out");

    //open output file
    if ((output_file= open(output_path, O_WRONLY|O_CREAT|O_TRUNC, PERM)) == -1) {
        perror("open output_file");
        close(input_file); 
        free (output_path);
        return -3;
    }

    //read the input file and save it's contents in a buffer
    //after detect and save all the urls from the buffer into a list
    while ((n= read(input_file, buf, 8192)) > 0) {
        tokenize(buf, &tokens, n);
        current_token= tokens;
        while (current_token != NULL) { //for every token
            //if the current token is a url following the http protocol
            if (strlen(current_token->word) > 7 && (current_token->word)[0] == 'h'
                                    && (current_token->word)[1] == 't'
                                    && (current_token->word)[2] == 't'
                                    && (current_token->word)[3] == 'p'
                                    && (current_token->word)[4] == ':'
                                    && (current_token->word)[5] == '/'
                                    && (current_token->word)[6] == '/') {
                //strip the url from initial www. (if it exist)
                //and insert it in the urls' list
                if (strlen(current_token->word) > 11 && (current_token->word[7]) == 'w'
                                              && (current_token->word[8]) == 'w'
                                              && (current_token->word[9]) == 'w'
                                              && (current_token->word[10]) == '.') {
                    for (i=11, j=0 ; i < strlen(current_token->word) ; i++, j++) {
                        if (current_token->word[i] == '/') { //end of the location
                            break;
                        }
                        url[j]= current_token->word[i];
                    }
                    url[j]= '\0';
                    insert_at_end_urll(&list, url); //insert the new url in the list of urls
                }
                else { //www. not included
                    for (i=7, j=0 ; i < strlen(current_token->word) ; i++, j++) {
                        if (current_token->word[i] == '/') { //end of the location
                            break;
                        }
                        url[j]= current_token->word[i];
                    }
                    url[j]= '\0';
                    insert_at_end_urll(&list, url); //insert the new url in the list of urls
                }
            }
            current_token= current_token->next; //check the next token in the list
        }
    }

    //write the contents of the urls' list in the output file
    list_iterator= list;
    while (list_iterator != NULL) {
        inttoascii(list_iterator->appearances, &appear);
        length= strlen(list_iterator->url) + strlen(" ") + 
                strlen(appear) + 1;
        output_line= malloc(length * sizeof(char));
        strcpy(output_line, list_iterator->url);
        strcat(output_line, " ");
        strcat(output_line, appear);
        strcat(output_line, "\n");
        write(output_file, output_line, length);
        list_iterator= list_iterator->next;
        free (appear);
        free (output_line);
    }

    //close the files and free memory
    close(input_file);
    close(output_file);
    clear_wl(tokens);
    clear_urll(list);
    free (output_path);
    free (tokens);
    free (list);

    return 0;   
}

//converts an integer until 20 digits in a string
void inttoascii(int number, char** num) {
    char str[20];
    int i, j, modulo;

    i= 0;
    while (number / 10 != 0) {
        modulo= number % 10;
        str[i++]= '0' + modulo;
        number/= 10;
    }
    modulo= number % 10;
    str[i]= '0' + modulo;

    //reverse
    *num= malloc (i+1 * sizeof(char));
    j= 0;
    while (i >= 0) {
        (*num)[j++]= str[i--];
    }
    (*num)[j]= '\0';
}

//Returns the number of filenames that the given list of filenames contains
int count_filenames(word_list_ptr filenames) {
    int count= 0;

    while (filenames != NULL) {
        count++;
        filenames= filenames->next;
    }    
    return count;
}