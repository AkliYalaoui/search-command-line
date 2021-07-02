#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

int is_a_directory(struct dirent *de);
int is_it_our_file(struct dirent *de,char *file_name);
void recursive_file_search(char *dir_name ,char *file_name);
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
         search_file(argv[1],file_name);
    }else if(argc >= 4){

    }

}

int is_it_our_file(struct dirent *de,char *file_name){
    return de->d_type != DT_DIR && strcmp(de->d_name,file_name) == 0 ? 0 : 1;
}

int is_a_directory(struct dirent *de){
    return de->d_type == DT_DIR && strcmp(de->d_name,".") != 0 && strcmp(de->d_name,"..") != 0 ? 0:1;
}
void recursive_file_search(char *dir_name ,char *file_name){

    DIR *dr = opendir(dir_name);
    struct dirent *de;
    char new_dir_name[1000];

    if(dr == NULL) return;
    while ((de = readdir(dr)) != NULL){

        if(is_it_our_file(de,file_name) == 0){
            if(strcmp(dir_name, "/") == 0)
                printf("/%s\n",de->d_name); 
            else
                printf("%s/%s\n",dir_name,de->d_name); 
        }   
        else if(is_a_directory(de) == 0){

            if(strcmp(dir_name, "/") == 0)
                sprintf(new_dir_name, "%s%s", dir_name, de->d_name);
            else
                sprintf(new_dir_name, "%s/%s", dir_name, de->d_name);

            recursive_file_search(new_dir_name,file_name);
        }
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
    closedir(dr);
    recursive_file_search(dir_name,file_name); 
}