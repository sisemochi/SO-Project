#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Numar invalid argumente!");
        exit(1);
    }

    DIR *director;
    struct dirent *directorStruct;
    if ((director = opendir(argv[1])) == NULL)
    {
        error("Eroare deschidere fisier intrare");
    }

    while ((directorStruct = readdir(director)) != NULL)
    {
        // Ignoram directorul curent si parintele
        if (strcmp(directorStruct->d_name, ".") == 0 || strcmp(directorStruct->d_name, "..") == 0)
        {
            continue;
        }
        printf("%s\n", directorStruct->d_name);
    }


    return 0;
}
