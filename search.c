#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

void search_file(char *dir_name,char *file_name);

void main(int argc,char *argv[]){

    char *file_name; 
    if(argc == 1){ 
        printf("Please provide the file name to search for\n");
        return;
    }

    file_name = argv[argc-1];

    if(argc == 2){
        search_file(".",file_name);
    }else if(argc == 3){
         search_file(argv[argc-2],file_name);
    }else if(argc >= 4){

    }

}

void search_file(char *dir_name,char *file_name){

    struct dirent *de;
    DIR *dr = opendir(dir_name);
    char *err_dir = strcmp(dir_name, ".") == 0  ? "current" : dir_name;

    if(dr == NULL){
         printf("Could not open %s directory",err_dir);
         return;
    }
  
    while ((de = readdir(dr)) != NULL)
         if(strcmp(de->d_name, file_name) == 0)
            printf("%s\n", de->d_name);

    closedir(dr); 
}