#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


#define BUFLEN 1000

#define NUM_THREADS 1000


pthread_t threads[NUM_THREADS];
int thread_count = 0;




struct thread_data
{

    char* pattern;
    int file_index;
    char file_name[BUFLEN];

};

struct thread_data thread_data_array[NUM_THREADS];



void print_string(char* s)
{

    printf("%s ", s);
    return;

    while (*s)
    {
        putchar(*s++);
        //fflush(stdout);
    }
    putchar(' ');
    //fflush(stdout);

}


void print_number(int i)
{

    char buf[BUFLEN];
    sprintf(buf, "%d", i);
    print_string(buf);

}


void print_newline()
{

    putchar('\n');
    //fflush(stdout);

}


void print_match(char* file_name, int file_index, int line_number, char* line)
{

    printf("%d %s %d %s\n", file_index, file_name, line_number, line);
    
}


FILE* open_file(char* file_name)
{

    FILE* fp = fopen(file_name, "r");
    return fp;

}


void* add_all_matching_lines(void* threadArg)
{

    //Unpack Arguments
    struct thread_data* my_data;
    my_data = (struct thread_data*) threadArg;

    char* pattern = my_data -> pattern;
    int file_index = my_data -> file_index;
    char* file_name = my_data -> file_name;

    if (!file_name)
        printf("file_name is nullptr\n");


    //Begin Thread Work
    FILE* fp = open_file(file_name);

    int line_number = 1;


    char lineBuf[BUFLEN];
    while ( (fgets(lineBuf, BUFLEN, fp)) != NULL )
    {
        lineBuf[strlen(lineBuf) - 1] = '\0';


        if ( strstr(lineBuf, pattern) )
        {


            print_match(file_name, file_index, line_number, lineBuf);
        }

        ++line_number;
    }


    fclose(fp);
    
    pthread_exit(NULL); 
}


int isRegular(char* path)
{
    struct stat file_stat;
    stat(path, &file_stat);
    return S_ISREG(file_stat.st_mode);
}

int isDir(char* path)
{
    struct stat file_stat;
    stat(path, &file_stat);
    return S_ISDIR(file_stat.st_mode);
}


int is_readable(char* file_name)
{

    FILE* fp = fopen(file_name, "r");
    int is_readable = 0;

    if (fp)
        is_readable = 1;

    fclose(fp);


    return is_readable;

}



void search(char* pattern, int file_index, char* file_name)
{


    if (!is_readable (file_name))
    {
        return;
    }
    if (isRegular (file_name))
    {


        thread_data_array[thread_count].pattern = pattern;
        thread_data_array[thread_count].file_index = file_index;
        strcpy(thread_data_array[thread_count].file_name, file_name);


        int rc;
        if ( rc = pthread_create(&threads[thread_count], NULL, add_all_matching_lines, (void*) &thread_data_array[thread_count]) )
        {
            perror("Problem with create thread\n");
            exit(-1);

        }

        
        ++thread_count;


        

    }
    else if (isDir(file_name))
    {
        DIR* dir = opendir(file_name);
        struct dirent* file;

        while((file = readdir(dir)) != NULL)
        {

            if (strcmp(file -> d_name, ".") == 0 || strcmp(file -> d_name, "..") == 0)
            {
                continue;
            }


            char newPath [BUFLEN];
            sprintf(newPath, "%s/%s", file_name, file -> d_name);

            search(pattern, file_index, newPath);
        }

        closedir(dir);

    }
   

}


int main(int argc, char* argv[])
{

    int file_index = 1;

    if (argc < 3)
    {
        perror("Wrong number of arguments to main");
    }


    for (int i = 2; i < argc; ++i)
    {
        //printf("i = %d\n", i);
        search(argv[1], file_index++, argv[i]);
        
    }

    for (int i = 0; i < thread_count; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    pthread_exit(NULL);

    return 0;

}




