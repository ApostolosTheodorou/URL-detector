#!/bin/bash

#Warning: If you have any other directory named 'output_files' in your
#system this script will fail. In order to fix this problem hard code the 
#path to syspro project 'output_files' directory in line 8 

pathToDir=$(find / -type d -name output_files 2> /dev/null)
cd $pathToDir                               #go to the directory with the output files
for tld in $*; do                     #for every top level domain given as an argument
    files=$(find ./ | grep -e .out$)              #find all the files in the directory
    for file in "$files"                                        #for every single file
    do
        cat $file | grep -e "$tld" |cut -d' ' -f 2 > nums.txt #find the urls ending in tld
        lines=$(wc -l nums.txt | cut -d' ' -f 1)       #count how many urls were found
        sum=0
        for ((i=1; i <= $lines; i++)) do             #for every url sum the apperances
            current_line=$(head -n $i nums.txt| tail -1)
            let sum=$sum+$current_line
        done
        echo $tld $sum
    done
done
