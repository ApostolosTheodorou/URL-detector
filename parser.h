#ifndef PARSER_H
#define PARSER_H

#include "word_list.h"
#include "url_list.h"

void tokenize (char* buf, word_list_ptr *list, int bytes_in_buffer);
void find_filenames (char* buf, word_list_ptr *filenames, int bytes_in_buffer);
int find_urls (char* filename);
void inttoascii(int number, char** num);
int count_filenames(word_list_ptr filenames);

#endif