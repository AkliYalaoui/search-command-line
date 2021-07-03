#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>

char * __DIR_;
char * __FILE_;
int  __DEPTH_FLAG = -1;

int is_a_directory(struct dirent *de);
int is_it_our_file(struct dirent *de,char *file_name);
int in_array(char **array,int array_size,char *needle);
int count_depth(char* path,char *start,char separator);
int count_flag_occurences(char **flags,int nb_flags,char *f);
int verifiy_flags(char **flags,int nb_flags);
void print_stat(char *path,char **flags,int nb_flags);
void recursive_file_search(char *dir_name ,char *file_name);
void search_file(char *dir_name,char *file_name);
void search_file_with_options(char *dir_name,char **flags,int nb_flags,char *file_name);

void main(int argc,char *argv[]){

    char *file_name; 

    if(argc == 1){ 
        printf("Please provide the file name to search for\n");
        return;
    }

    __FILE_ = argv[argc-1];
    __DIR_ = argv[1];

    if(argc == 2){
        strcpy(__DIR_,".");
        search_file(__DIR_,__FILE_);
    }else if(argc == 3){ 
         search_file(__DIR_,__FILE_);
    }else if(argc >= 4){
         char *flags[argc-3];
        for(int i = 2; i < argc - 1;i++)
            flags[i-2] = argv[i];

        if(verifiy_flags(flags,argc-3) == 1){
            printf("Unknown flag provided to the command\n");
        }else{
            search_file_with_options(__DIR_,flags,argc-3,__FILE_);
        }
    }
}

int is_it_our_file(struct dirent *de,char *file_name){
    return de->d_type != DT_DIR && strcmp(de->d_name,file_name) == 0 ? 0 : 1;
}

int is_a_directory(struct dirent *de){
    return de->d_type == DT_DIR && strcmp(de->d_name,".") != 0 && strcmp(de->d_name,"..") != 0 ? 0:1;
}

int in_array(char **array,int array_size,char *needle){

    for (int i = 0; i < array_size; i++)
        if(strcmp(array[i],needle) == 0) return 0;

    return 1;
}

int count_flag_occurences(char **flags,int nb_flags,char *f){
    int cpt = 0;

    for (int i = 0; i < nb_flags; i++)
        if(strcmp(flags[i],f) == 0) cpt++;

    return cpt;
    
}

int verifiy_flags(char **flags,int nb_flags){

    char *allowed_flags[6]={"-d","-m","-s","-t","-p","-a"};
    int count_depth_flag = 0;
    for (int i = 0; i < nb_flags; i++)
        if(in_array(allowed_flags,6,flags[i]) == 1){
            
            if(strcmp(flags[i],"-0")== 0){
                __DEPTH_FLAG = 0;
                count_depth_flag++;
                if(count_depth_flag > 1){
                    printf("too many depth flags passed to the command\n");
                    return 1;
                }
                continue;
            }
            char *tmp = flags[i];
            tmp[0] = '0';
            int a = atoi(tmp);
            if(a == 0) return 1;
            __DEPTH_FLAG = a;
            count_depth_flag++;
            if(count_depth_flag > 1){
                printf("too many depth flags passed to the command\n");
                return 1;
            }
        }

    return 0;   
    
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

void search_file_with_options(char *dir_name,char **flags,int nb_flags,char *file_name){
 
    DIR *dr = opendir(dir_name);
    struct dirent *de;
    char new_dir_name[1000], full_path[1000];

    if(dr == NULL) return;
    while ((de = readdir(dr)) != NULL){

        if(is_it_our_file(de,file_name) == 0){
            if(strcmp(dir_name, "/") == 0)
                sprintf(full_path, "%s%s", "/", de->d_name); 
            else
                sprintf(full_path, "%s/%s", dir_name, de->d_name);

            print_stat(full_path,flags,nb_flags);
        }   
        else if(is_a_directory(de) == 0){

            if(strcmp(dir_name, "/") == 0)
                sprintf(new_dir_name, "%s%s", dir_name, de->d_name);
            else
                sprintf(new_dir_name, "%s/%s", dir_name, de->d_name);

            search_file_with_options(new_dir_name,flags,nb_flags,file_name);
        }
    }

}

int count_depth(char *path,char *start,char separator){
    
    int occurence = 0,i=0;

    while (start[i] != '\0')
        i++;

    while (path[i] != '\0')
    {   
        i++;
        if(path[i] ==  separator)
            occurence++;
    }
    
    return occurence;
}

void print_stat(char *path,char **flags,int nb_flags){

    if(__DEPTH_FLAG == -1 || count_depth(path,__DIR_,'/') <= __DEPTH_FLAG){
        struct stat sb;
        if (stat(path, &sb) == -1) {
            printf("Error occured while trying to read file metadata\n");
            return;
        }

        printf("%s ",path);
        printf("-l %d",count_depth(path,__DIR_,'/'));
        for (int i = 0; i < nb_flags; i++)
        {
            if(strcmp(flags[i],"-d") == 0){
                printf("-d %s ",ctime(&sb.st_atime));
            }else if(strcmp(flags[i],"-m") == 0){
                printf("-m %s ",ctime(&sb.st_mtime));
            }else if(strcmp(flags[i],"-s") == 0){
                printf("-s %lld bytes ",(long long) sb.st_size);
            }else if(strcmp(flags[i],"-t") == 0){
                printf("-t ");
                switch (sb.st_mode & S_IFMT) {
                    case S_IFBLK:  printf("block device ");            break;
                    case S_IFCHR:  printf("character device ");        break;
                    case S_IFDIR:  printf("directory ");               break;
                    case S_IFIFO:  printf("FIFO/pipe ");               break;
                    case S_IFLNK:  printf("symlink ");                 break;
                    case S_IFREG:  printf("regular file ");            break;
                    case S_IFSOCK: printf("socket ");                  break;
                    default:       printf("unknown? ");                break;
                }
            }else if(strcmp(flags[i],"-p") == 0){
                printf("-p %lo ",(unsigned long) sb.st_mode);
            }else if(strcmp(flags[i],"-a") == 0){
                printf("-d %s ",ctime(&sb.st_atime));
                printf("-s %lld bytes ",(long long) sb.st_size);
                printf("-m %s ",ctime(&sb.st_mtime));
                printf("-t ");
                switch (sb.st_mode & S_IFMT) {
                    case S_IFBLK:  printf("block device ");            break;
                    case S_IFCHR:  printf("character device ");        break;
                    case S_IFDIR:  printf("directory ");               break;
                    case S_IFIFO:  printf("FIFO/pipe ");               break;
                    case S_IFLNK:  printf("symlink ");                 break;
                    case S_IFREG:  printf("regular file ");            break;
                    case S_IFSOCK: printf("socket ");                  break;
                    default:       printf("unknown? ");                break;
                }
                printf("-p %lo ",(unsigned long) sb.st_mode);
            }else{
                
            }
        }
        printf("\n\n");   
    }
}