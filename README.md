# URL finder 

The main porpuse of this project is to monitor a directory of the file system and detect URLs contained in files of this directory.During the 
excecution of the program, the directory may change by adding/deleting/moving new or already existing files and subdirectories. The found URLs
are stored in a new directory, named "output_files", in a file with name <original_file's_name>.out, where original file's name is the name of 
the file in which the URL was detected. Apart from the main program, a bash script "finder.sh" is provided, whose job is to count the number of 
URLs that end at the given top level domains (com, gr, edu etc.).

## Compilation and Execution
To compile simply give the command make.

To execute run: ~$./sniffer [-p path_to_directory]
(if no path is given, the default is the current directory)

Note: To rerun the program, first run "make clean" and delete manually output_files and pipes subdirectories. Then recompile using "make".

To terminate the program press <Ctrl+C>.

To run the script: ~$./finder.sh <top_level_domain_1> <top_level_domain_2> ... <top_level_domain_n>

## Concepts Used

This project involves the following:

-system calls for process creation (fork/exec)

-interprocess communication with named pipes

-low level I/O

-signal handling

-shell scripting (bash)


