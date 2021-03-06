#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>

char __DIR_[1000];
char *__FILE_;
int __DEPTH_FLAG = -1;

char *normalize_pattern(char *pattern);
char* remove_consecutive_occurences(char *word,char character);
bool is_a_directory(struct dirent *de);
bool is_it_our_file(struct dirent *de, char *file_name);
bool in_array(char **array, int array_size, char *needle);
int count_depth(char *path, char *start, char separator);
int count_flag_occurences(char **flags, int nb_flags, char *f);
bool verifiy_flags(char **flags, int nb_flags);
bool does_the_file_match_the_wildcard_pattern(char *file, char *pattern);
void print_stat(char *path, char **flags, int nb_flags, char *reel_file);
void search_file_with_options(char *dir_name, char **flags, int nb_flags, char *file_name);

void main(int argc, char *argv[])
{

    char *file_name;

    if (argc == 1)
    {
        printf("Please provide the file name to search for\n");
        return;
    }

    __FILE_ = normalize_pattern(argv[argc - 1]);
    if (argc == 2)
    {
        strcpy(__DIR_, ".");
        search_file_with_options(__DIR_, NULL, 0, __FILE_);
    }
    else if (argc == 3)
    {
        strcpy(__DIR_, argv[1]);
        search_file_with_options(__DIR_, NULL, 0, __FILE_);
    }
    else if (argc >= 4)
    {
        strcpy(__DIR_, argv[1]);
        char *flags[argc - 3];
        for (int i = 2; i < argc - 1; i++)
            flags[i - 2] = argv[i];

        if (verifiy_flags(flags, argc - 3) == false)
        {
            printf("Unknown flag provided to the command\n");
        }
        else
        {
            search_file_with_options(__DIR_, flags, argc - 3, __FILE_);
        }
    }
}

char *normalize_pattern(char *pattern){
    int i= 0;

    while(pattern[i] != '\0'){

        if(pattern[i] == '*' && pattern[i+1] == '?'){
            pattern[i] ='?';
            pattern[i+1] = '*';
            pattern = remove_consecutive_occurences(pattern,'*');
        }
        i++;
    }

    return remove_consecutive_occurences(pattern,'*');
}
char* remove_consecutive_occurences(char *word,char character){
    int i= 0,k=0;
    char static new_word[1000];

    while(word[i] != '\0'){

        if(word[i] != character || word[i] == character && word[i+1] == '\0' || word[i] == character && word[i+1] != character ){
            new_word[k] = word[i];
            k++;
        }
        i++;
    }

    new_word[k] = '\0';
    return new_word;
}

bool is_it_our_file(struct dirent *de, char *file_name)
{
    return (de->d_type != DT_DIR && does_the_file_match_the_wildcard_pattern(de->d_name, __FILE_) == true) ? true : false;
}

bool is_a_directory(struct dirent *de)
{
    return (de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) ? true : false;
}

bool in_array(char **array, int array_size, char *needle)
{

    for (int i = 0; i < array_size; i++)
        if (strcmp(array[i], needle) == 0)
            return true;

    return false;
}

int count_flag_occurences(char **flags, int nb_flags, char *f)
{
    int cpt = 0;

    for (int i = 0; i < nb_flags; i++)
        if (strcmp(flags[i], f) == 0)
            cpt++;

    return cpt;
}
bool verifiy_flags(char **flags, int nb_flags)
{

    char *allowed_flags[6] = {"-d", "-m", "-s", "-t", "-p", "-a"};
    int count_depth_flag = 0;
    for (int i = 0; i < nb_flags; i++){
        if (in_array(allowed_flags, 6, flags[i]) == false)
        {

            if (strcmp(flags[i], "-0") == 0)
            {
                __DEPTH_FLAG = 0;
                count_depth_flag++;
                if (count_depth_flag > 1)
                {
                    printf("too many depth flags passed to the command\n");
                    return false;
                }
                continue;
            }
            char *tmp = flags[i];
            if(tmp[0] != '-'){
                return false;
            }
            tmp[0] = '0';
            int a = atoi(tmp);
            if (a == 0)
                return false;
            __DEPTH_FLAG = a;
            count_depth_flag++;
            if (count_depth_flag > 1)
            {
                printf("too many depth flags passed to the command\n");
                return false;
            }
        }else if( count_flag_occurences(flags,nb_flags,flags[i]) > 1){
            printf("flags are redundant\n");
            return false;
        }else if(strcmp(flags[i], "-a") != 0 && in_array(allowed_flags, 6, flags[i]) == true && in_array(flags, nb_flags, "-a") == true){
            printf("-a should not be used with other meta data flags\n");
            return false;
        }
    }
    return true;
}

void search_file_with_options(char *dir_name, char **flags, int nb_flags, char *file_name)
{

    DIR *dr = opendir(dir_name);
    struct dirent *de;
    char new_dir_name[1000], full_path[1000];

    if (dr == NULL)
    {
        //printf("Could not open %s directory\n", dir_name);
        return;
    }
    while ((de = readdir(dr)) != NULL)
    {
        if (is_it_our_file(de, file_name) == true)
        {
            if (strcmp(dir_name, "/") == 0)
                sprintf(full_path, "%s%s", "/", de->d_name);
            else
                sprintf(full_path, "%s/%s", dir_name, de->d_name);

            if (flags != NULL)
                print_stat(full_path, flags, nb_flags, de->d_name);
            else
                printf("%s\n", full_path);
        }
        else if (is_a_directory(de) == true)
        {
            if (strcmp(dir_name, "/") == 0)
                sprintf(new_dir_name, "%s%s", dir_name, de->d_name);
            else
                sprintf(new_dir_name, "%s/%s", dir_name, de->d_name);

            search_file_with_options(new_dir_name, flags, nb_flags, file_name);
        }
    }
    closedir(dr);
}

int count_depth(char *path, char *start, char separator)
{

    int occurence = 0, i = 0;

    while (start[i] != '\0')
        i++;

    while (path[i] != '\0')
    {
        i++;
        if (path[i] == separator)
            occurence++;
    }

    return occurence;
}

bool does_the_file_match_the_wildcard_pattern(char *file, char *pattern)
{

    bool match = strcmp(file, pattern) == 0 ? true : false;
    int i = 0, k = 0;

    if (match == true)
        return true;

    match = true;

    while (pattern[i] != '\0' || file[k] != '\0')
    {
        if(pattern[i] == '\0' && file[k] != '\0'){
            return false;
        }
        
        if (pattern[i] == '?' && pattern[i + 1] == '\0' && file[k + 1] != '\0' || pattern[i] == '?' && pattern[i + 1] != '\0' && file[k + 1] == '\0')
        {
            return false;
        }
        else if (pattern[i] == '?')
        {
            i++;
            k++;
        }
        else if (pattern[i] == '*')
        {
            if (pattern[i + 1] != '\0' && pattern[i + 1] == file[k])
            {
                i++;
            }
            else if (pattern[i + 1] != '\0')
            {
                while (file[k] != pattern[i + 1] && file[k] != '\0')
                {
                    k++;
                }
                i++;
                if (file[k] == '\0')
                {
                   return false;
                }
            }
            else if (pattern[i + 1] == '\0')
            {
                return true;
            }
        }
        else
        {
            if (file[k] != pattern[i])
            {
                return false;
            }
            i++;
            k++;
        }
    }

    return match;
}

void print_stat(char *path, char **flags, int nb_flags, char *reel_file)
{

    if (__DEPTH_FLAG == -1 || count_depth(path, __DIR_, '/') <= __DEPTH_FLAG)
    {
        struct stat sb;
        if (stat(path, &sb) == -1)
        {
            printf("Error occured while trying to read file metadata of %s\n",path);
            return;
        }

        printf("%s ", path);
        for (int i = 0; i < nb_flags; i++)
        {
            if (strcmp(flags[i], "-d") == 0)
            {
                printf("\033[0;31m");//red
                printf("-d %s ", ctime(&sb.st_atime));
                printf("\033[0m");//reset
            }
            else if (strcmp(flags[i], "-m") == 0)
            {
                printf("\033[0;34m");//blue
                printf("-m %s ", ctime(&sb.st_mtime));
                printf("\033[0m");//reset
            }
            else if (strcmp(flags[i], "-s") == 0)
            {
                printf("\033[0;37m");//white
                printf("-s %lld bytes ", (long long)sb.st_size);
                printf("\033[0m");//reset
            }
            else if (strcmp(flags[i], "-t") == 0)
            {
                printf("\033[0;35m");//purple
                printf("-t ");
                switch (sb.st_mode & S_IFMT)
                {
                case S_IFBLK:
                    printf("block device ");
                    break;
                case S_IFCHR:
                    printf("character device ");
                    break;
                case S_IFDIR:
                    printf("directory ");
                    break;
                case S_IFIFO:
                    printf("FIFO/pipe ");
                    break;
                case S_IFLNK:
                    printf("symlink ");
                    break;
                case S_IFREG:
                    printf("regular file ");
                    break;
                case S_IFSOCK:
                    printf("socket ");
                    break;
                default:
                    printf("unknown? ");
                    break;
                }
                printf("\033[0m");//reset
            }
            else if (strcmp(flags[i], "-p") == 0)
            {
                printf("\033[0;32m");//green
                printf("-p %lo ", (unsigned long)sb.st_mode);
                printf("\033[0m");//reset
            }
            else if (strcmp(flags[i], "-a") == 0)
            {
                printf("\033[0;31m");//red
                printf("-d %s ", ctime(&sb.st_atime));
                printf("\033[0m");//reset
                printf("\033[0;37m");//white
                printf("-s %lld bytes ", (long long)sb.st_size);
                printf("\033[0m");//reset
                printf("\033[0;34m");//blue
                printf("-m %s ", ctime(&sb.st_mtime));
                printf("\033[0m");//reset
                printf("\033[0;35m");//purple
                printf("-t ");
                switch (sb.st_mode & S_IFMT)
                {
                case S_IFBLK:
                    printf("block device ");
                    break;
                case S_IFCHR:
                    printf("character device ");
                    break;
                case S_IFDIR:
                    printf("directory ");
                    break;
                case S_IFIFO:
                    printf("FIFO/pipe ");
                    break;
                case S_IFLNK:
                    printf("symlink ");
                    break;
                case S_IFREG:
                    printf("regular file ");
                    break;
                case S_IFSOCK:
                    printf("socket ");
                    break;
                default:
                    printf("unknown? ");
                    break;
                }
                printf("\033[0m");//reset
                printf("\033[0;32m");//green
                printf("-p %lo ", (unsigned long)sb.st_mode);
                printf("\033[0m");//reset
            }
            else
            {
            }
        }
        printf("\n\n");
    }
}
