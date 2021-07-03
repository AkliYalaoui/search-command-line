# About this command

## Search-command-line
A file search command programmed in C language under Linux. 
## Search    [Directory_name]    [-options]    File_name
Search for all "File_name" files existing in the directory
"Directory_name" and all its descendants.
## Directory_name is optional
if it is not specified , the search will be performed on the current directory :
search file_name is equivalent to search . file_name
## This command accepts several options
-d for showing file's last access time
-m for showing file's last modification time 
-s for showing file's size 
-t for showing file's type
-p for showing file's permissions
-a an alias for combining the previous flags ( -a is equivalent to -d -m -s -t -p)
-n where n is a positive integer , it specifies the depth of the file counting from the specified directory in the command
## File name can contain wildcards symbols
like ? (for a single character) and * (for 0 or multiple characters)

# How to install

## clone this project
git clone https://github.com/AkliYalaoui/search-command-line.git
cd search-command-line
gcc search.c
./a.out [Directory_name]    [-options]    File_name