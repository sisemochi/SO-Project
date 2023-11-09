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

void printareDateBMP(struct dirent *fisier, char *caleFisier, struct stat statFisier){
    int fd;
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);
    if ((fd = open(caleNoua, O_RDONLY)) == -1)
    {
        error("Eroare deschidere fisier intrare");
    }
    char *header = (char *)malloc(54 * sizeof(char));
    if (read(fd, header, 54) == -1)
    {
        error("Eroare citire header");
    }

    int fisierIesire = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fisierIesire == -1)
    {
        error("Eroare deschidere fisier iesire");
    }

    write(fisierIesire, "nume fisier:", 12);
    write(fisierIesire, fisier->d_name, strlen(fisier->d_name));
    write(fisierIesire, "\n", 1);

    int *inaltime = (int *)malloc(sizeof(int));
    memcpy(inaltime, header + 22, 4);
    write(fisierIesire, "inaltime: ", 10);
    char *inaltimeChar = (char *)malloc(10 * sizeof(char));
    sprintf(inaltimeChar, "%d", *inaltime);
    write(fisierIesire, inaltimeChar, strlen(inaltimeChar));
    write(fisierIesire, "\n", 1);

    int *lungime = (int *)malloc(sizeof(int));
    memcpy(lungime, header + 18, 4);
    write(fisierIesire, "lungime: ", 9);
    char *lungimeChar = (char *)malloc(10 * sizeof(char));
    sprintf(lungimeChar, "%d", *lungime);
    write(fisierIesire, lungimeChar, strlen(lungimeChar));
    write(fisierIesire, "\n", 1);

    int *dimensiune = (int *)malloc(sizeof(int));
    memcpy(dimensiune, header + 2, 4);
    write(fisierIesire, "Dimensiune: ", 12);
    char *dimensiuneChar = (char *)malloc(10 * sizeof(char));
    sprintf(dimensiuneChar, "%d", *dimensiune);
    write(fisierIesire, dimensiuneChar, strlen(dimensiuneChar));
    write(fisierIesire, "\n", 1);

    unsigned int identificatorulUtilizatorului= statFisier.st_uid;
    write(fisierIesire, "Identificatorul utilizatorului: ", 32);
    char *identificatorulUtilizatoruluiChar = (char *)malloc(10 * sizeof(char));
    sprintf(identificatorulUtilizatoruluiChar, "%d", identificatorulUtilizatorului);
    write(fisierIesire, identificatorulUtilizatoruluiChar, strlen(identificatorulUtilizatoruluiChar));
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Timpul ultimei modificari: ", 27);
    char *timpulUltimeiModificariChar = (char *)malloc(10 * sizeof(char));
    sprintf(timpulUltimeiModificariChar, "%s", ctime(&statFisier.st_mtime));
    write(fisierIesire, timpulUltimeiModificariChar, strlen(timpulUltimeiModificariChar));

    write(fisierIesire, "numarul de legaturi: ", 21);
    char *numarulDeLegaturiChar = (char *)malloc(10 * sizeof(char));
    sprintf(numarulDeLegaturiChar, "%d", statFisier.st_nlink);
    write(fisierIesire, numarulDeLegaturiChar, strlen(numarulDeLegaturiChar));
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces user: ", 24);
    if (statFisier.st_mode & S_IRUSR)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWUSR)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXUSR)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces grup: ", 24);
    if (statFisier.st_mode & S_IRGRP)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWGRP)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXGRP)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces altii: ", 25);
    if (statFisier.st_mode & S_IROTH)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWOTH)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXOTH)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n\n", 2);

    close(fd);
}

void printareDateRegulareNuBMP(struct dirent *fisier, char *caleFisier, struct stat statFisier){
    int fd;
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);
    if ((fd = open(caleNoua, O_RDONLY)) == -1)
    {
        error("Eroare deschidere fisier intrare");
    }

    int fisierIesire = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fisierIesire == -1)
    {
        error("Eroare deschidere fisier iesire");
    }

    write(fisierIesire, "nume fisier:", 12);
    write(fisierIesire, fisier->d_name, strlen(fisier->d_name));
    write(fisierIesire, "\n", 1);

    long *dimensiune = (long *)malloc(sizeof(long));
    if (stat(caleNoua, &statFisier) == -1)
    {
        error("Eroare stat");
    }
    *dimensiune = statFisier.st_size;
    write(fisierIesire, "Dimensiune: ", 12);
    char *dimensiuneChar = (char *)malloc(10 * sizeof(char));
    sprintf(dimensiuneChar, "%ld", *dimensiune);
    write(fisierIesire, dimensiuneChar, strlen(dimensiuneChar));
    write(fisierIesire, "\n", 1);

    unsigned int identificatorulUtilizatorului= statFisier.st_uid;
    write(fisierIesire, "Identificatorul utilizatorului: ", 32);
    char *identificatorulUtilizatoruluiChar = (char *)malloc(10 * sizeof(char));
    sprintf(identificatorulUtilizatoruluiChar, "%d", identificatorulUtilizatorului);
    write(fisierIesire, identificatorulUtilizatoruluiChar, strlen(identificatorulUtilizatoruluiChar));
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Timpul ultimei modificari: ", 27);
    char *timpulUltimeiModificariChar = (char *)malloc(10 * sizeof(char));
    sprintf(timpulUltimeiModificariChar, "%s", ctime(&statFisier.st_mtime));
    write(fisierIesire, timpulUltimeiModificariChar, strlen(timpulUltimeiModificariChar));

    write(fisierIesire, "numarul de legaturi: ", 21);
    char *numarulDeLegaturiChar = (char *)malloc(10 * sizeof(char));
    sprintf(numarulDeLegaturiChar, "%d", statFisier.st_nlink);
    write(fisierIesire, numarulDeLegaturiChar, strlen(numarulDeLegaturiChar));
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces user: ", 24);
    if (statFisier.st_mode & S_IRUSR)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWUSR)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXUSR)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces grup: ", 24);
    if (statFisier.st_mode & S_IRGRP)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWGRP)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXGRP)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces altii: ", 25);
    if (statFisier.st_mode & S_IROTH)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWOTH)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXOTH)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n\n", 2);

    close(fd);

}

void printareDateDirector(struct dirent *fisier, char *caleFisier, struct stat statFisier){}

void printareDateLink(struct dirent *fisier, char *caleFisier, struct stat statFisier){
    int fd;
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);
    if ((fd = open(caleNoua, O_RDONLY)) == -1)
    {
        error("Eroare deschidere fisier intrare");
    }

    int fisierIesire = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fisierIesire == -1)
    {
        error("Eroare deschidere fisier iesire");
    }

    write(fisierIesire, "nume legatura:", 12);
    write(fisierIesire, fisier->d_name, strlen(fisier->d_name));
    write(fisierIesire, "\n", 1);

    long *dimensiuneLegatura = (long *)malloc(sizeof(long));
    if (stat(caleNoua, &statFisier) == -1)
    {
        error("Eroare stat");
    }
    *dimensiuneLegatura = statFisier.st_size;
    write(fisierIesire, "Dimensiune legatura: ", 21);
    char *dimensiuneLegaturaChar = (char *)malloc(10 * sizeof(char));
    sprintf(dimensiuneLegaturaChar, "%ld", *dimensiuneLegatura);
    write(fisierIesire, dimensiuneLegaturaChar, strlen(dimensiuneLegaturaChar));
    write(fisierIesire, "\n", 1);

    long *dimensiune = (long *)malloc(sizeof(long));
    if (stat(caleNoua, &statFisier) == -1)
    {
        error("Eroare stat");
    }
    *dimensiune = statFisier.st_size;
    write(fisierIesire, "Dimensiune fisier: ", 12);
    char *dimensiuneChar = (char *)malloc(10 * sizeof(char));
    sprintf(dimensiuneChar, "%ld", *dimensiune);
    write(fisierIesire, dimensiuneChar, strlen(dimensiuneChar));
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces user: ", 24);
    if (statFisier.st_mode & S_IRUSR)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWUSR)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXUSR)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces grup: ", 24);
    if (statFisier.st_mode & S_IRGRP)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWGRP)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXGRP)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n", 1);

    write(fisierIesire, "Drepturi de acces altii: ", 25);
    if (statFisier.st_mode & S_IROTH)
    {
        write(fisierIesire, "r", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IWOTH)
    {
        write(fisierIesire, "w", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    if (statFisier.st_mode & S_IXOTH)
    {
        write(fisierIesire, "x", 1);
    }
    else
    {
        write(fisierIesire, "-", 1);
    }
    write(fisierIesire, "\n\n", 2);

    close(fd);
}

void decizieFisier(struct dirent *fisier, char *caleFisier)
{
    struct stat statFisier;
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);
    if (stat(caleNoua, &statFisier) == -1)
    {
        error("Eroare stat");
    }
    if (S_ISDIR(statFisier.st_mode))
    {
        printf("%s este un Director\n\n", fisier->d_name);
    }
    else if (S_ISREG(statFisier.st_mode))
    {
        //verificam daca este un .bmp
        char *extensie = strrchr(fisier->d_name, '.');
        if (extensie != NULL && strcmp(extensie, ".bmp") == 0)
        {
            printf("Fisierul %s este un .bmp\n\n", fisier->d_name);
            printareDateBMP(fisier, caleFisier, statFisier);
        }
        else
        {
            printf("Fisierul %s nu este un .bmp\n\n", fisier->d_name);
            printareDateRegulareNuBMP(fisier, caleFisier, statFisier);
        }
    }
    else if (S_ISLNK(statFisier.st_mode))
    {
        printf("%s este un Link\n\n", fisier->d_name);
        printareDateLink(fisier, caleFisier, statFisier);
    }
    else
    {
        printf("%s este Altceva\n\n", fisier->d_name);
    }

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
        decizieFisier(directorStruct, argv[1]);
    }


    return 0;
}
