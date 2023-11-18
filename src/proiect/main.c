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

typedef struct {
    char nume[256];
    int inaltime;
    int lungime;
    long size;
    long uid;
    long timpulModificarii;
    int numarLegaturi;
    char drepturiUser[3];
    char drepturiGrup[3];
    char drepturiAltii[3];
    char type;
    // Type of file ('B' for BMP, 'D' for Directory, 'L' for Link, 'R' for Regular, 'O' for Others)
} FileInfo;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void writeCheck(int fd, char *buffer, unsigned long size)
{
    if (write(fd, buffer, size) == -1)
    {
        error("Eroare scriere");
    }
}

void printareStatistica(FileInfo fileInfo, char *fisierIesire){
    int fd;
    if ((fd = open(fisierIesire, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1)
    {
        error("Eroare deschidere fisier iesire");
    }

    if(fileInfo.type == 'B'){
        writeCheck(fd, "nume fisier:", 12);
        writeCheck(fd, fileInfo.nume, strlen(fileInfo.nume));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "inaltime: ", 10);
        char *inaltimeChar = (char *)malloc(10 * sizeof(char));
        sprintf(inaltimeChar, "%d", fileInfo.inaltime);
        writeCheck(fd, inaltimeChar, strlen(inaltimeChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "lungime: ", 9);
        char *lungimeChar = (char *)malloc(10 * sizeof(char));
        sprintf(lungimeChar, "%d", fileInfo.lungime);
        writeCheck(fd, lungimeChar, strlen(lungimeChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Dimensiune: ", 12);
        char *dimensiuneChar = (char *)malloc(10 * sizeof(char));
        sprintf(dimensiuneChar, "%ld", fileInfo.size);
        writeCheck(fd, dimensiuneChar, strlen(dimensiuneChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Identificatorul utilizatorului: ", 32);
        char *identificatorulUtilizatoruluiChar = (char *)malloc(10 * sizeof(char));
        sprintf(identificatorulUtilizatoruluiChar, "%ld", fileInfo.uid);
        writeCheck(fd, identificatorulUtilizatoruluiChar, strlen(identificatorulUtilizatoruluiChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Timpul ultimei modificari: ", 27);
        char *timpulUltimeiModificariChar = (char *)malloc(10 * sizeof(char));
        sprintf(timpulUltimeiModificariChar, "%ld", fileInfo.timpulModificarii);
        writeCheck(fd, timpulUltimeiModificariChar, strlen(timpulUltimeiModificariChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "numarul de legaturi: ", 21);
        char *numarulDeLegaturiChar = (char *)malloc(10 * sizeof(char));
        sprintf(numarulDeLegaturiChar, "%d", fileInfo.numarLegaturi);
        writeCheck(fd, numarulDeLegaturiChar, strlen(numarulDeLegaturiChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces user: ", 24);
        writeCheck(fd, fileInfo.drepturiUser, 3);
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces grup: ", 24);
        writeCheck(fd, fileInfo.drepturiGrup, 3);
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces altii: ", 25);
        writeCheck(fd, fileInfo.drepturiAltii, 3);
        writeCheck(fd, "\n\n", 2);
    }

    if(fileInfo.type=='R'){
        writeCheck(fd, "nume fisier:", 12);
        writeCheck(fd, fileInfo.nume, strlen(fileInfo.nume));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Dimensiune: ", 12);
        char *dimensiuneChar = (char *)malloc(10 * sizeof(char));
        sprintf(dimensiuneChar, "%ld", fileInfo.size);
        writeCheck(fd, dimensiuneChar, strlen(dimensiuneChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Identificatorul utilizatorului: ", 32);
        char *identificatorulUtilizatoruluiChar = (char *)malloc(10 * sizeof(char));
        sprintf(identificatorulUtilizatoruluiChar, "%ld", fileInfo.uid);
        writeCheck(fd, identificatorulUtilizatoruluiChar, strlen(identificatorulUtilizatoruluiChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Timpul ultimei modificari: ", 27);
        char *timpulUltimeiModificariChar = (char *)malloc(10 * sizeof(char));
        sprintf(timpulUltimeiModificariChar, "%ld", fileInfo.timpulModificarii);
        writeCheck(fd, timpulUltimeiModificariChar, strlen(timpulUltimeiModificariChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "numarul de legaturi: ", 21);
        char *numarulDeLegaturiChar = (char *)malloc(10 * sizeof(char));
        sprintf(numarulDeLegaturiChar, "%d", fileInfo.numarLegaturi);
        writeCheck(fd, numarulDeLegaturiChar, strlen(numarulDeLegaturiChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces user: ", 24);
        writeCheck(fd, fileInfo.drepturiUser, 3);
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces grup: ", 24);
        writeCheck(fd, fileInfo.drepturiGrup, 3);
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces altii: ", 25);
        writeCheck(fd, fileInfo.drepturiAltii, 3);
        writeCheck(fd, "\n\n", 2);
    }

    if(fileInfo.type == 'L'){
        writeCheck(fd, "nume legatura: ", 15);
        writeCheck(fd, fileInfo.nume, strlen(fileInfo.nume));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "dimensiune legatura: ", 21);
        char *dimensiuneChar = (char *)malloc(10 * sizeof(char));
        sprintf(dimensiuneChar, "%ld", fileInfo.size);
        writeCheck(fd, dimensiuneChar, strlen(dimensiuneChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "dimensiune fisier target: ", 26);
        char *dimensiuneFisierTargetChar = (char *)malloc(10 * sizeof(char));
        sprintf(dimensiuneFisierTargetChar, "%ld", fileInfo.size);
        writeCheck(fd, dimensiuneFisierTargetChar, strlen(dimensiuneFisierTargetChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces user: ", 24);
        writeCheck(fd, fileInfo.drepturiUser, 3);
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces grup: ", 24);
        writeCheck(fd, fileInfo.drepturiGrup, 3);
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces altii: ", 25);
        writeCheck(fd, fileInfo.drepturiAltii, 3);
        writeCheck(fd, "\n\n", 2);
    }

    if(fileInfo.type == 'D'){
        writeCheck(fd, "nume director: ", 15);
        writeCheck(fd, fileInfo.nume, strlen(fileInfo.nume));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Identificatorul utilizatorului: ", 32);
        char *identificatorulUtilizatoruluiChar = (char *)malloc(10 * sizeof(char));
        sprintf(identificatorulUtilizatoruluiChar, "%ld", fileInfo.uid);
        writeCheck(fd, identificatorulUtilizatoruluiChar, strlen(identificatorulUtilizatoruluiChar));
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces user: ", 24);
        writeCheck(fd, fileInfo.drepturiUser, 3);
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces grup: ", 24);
        writeCheck(fd, fileInfo.drepturiGrup, 3);
        writeCheck(fd, "\n", 1);

        writeCheck(fd, "Drepturi de acces altii: ", 25);
        writeCheck(fd, fileInfo.drepturiAltii, 3);
        writeCheck(fd, "\n\n", 2);
    }

    close(fd);

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

    FileInfo fileInfo;
    strcpy(fileInfo.nume, fisier->d_name);
    fileInfo.inaltime = *(int *)(header + 22);
    fileInfo.lungime = *(int *)(header + 18);
    fileInfo.size = *(int *)(header + 2);
    fileInfo.uid = statFisier.st_uid;
    fileInfo.timpulModificarii = statFisier.st_mtime;
    fileInfo.numarLegaturi = statFisier.st_nlink;
    if (statFisier.st_mode & S_IRUSR)
    {
        fileInfo.drepturiUser[0] = 'r';
    }
    else
    {
        fileInfo.drepturiUser[0] = '-';
    }
    if (statFisier.st_mode & S_IWUSR)
    {
        fileInfo.drepturiUser[1] = 'w';
    }
    else
    {
        fileInfo.drepturiUser[1] = '-';
    }
    if (statFisier.st_mode & S_IXUSR)
    {
        fileInfo.drepturiUser[2] = 'x';
    }
    else
    {
        fileInfo.drepturiUser[2] = '-';
    }
    if (statFisier.st_mode & S_IRGRP)
    {
        fileInfo.drepturiGrup[0] = 'r';
    }
    else
    {
        fileInfo.drepturiGrup[0] = '-';
    }
    if (statFisier.st_mode & S_IWGRP)
    {
        fileInfo.drepturiGrup[1] = 'w';
    }
    else
    {
        fileInfo.drepturiGrup[1] = '-';
    }
    if (statFisier.st_mode & S_IXGRP)
    {
        fileInfo.drepturiGrup[2] = 'x';
    }
    else
    {
        fileInfo.drepturiGrup[2] = '-';
    }
    if (statFisier.st_mode & S_IROTH)
    {
        fileInfo.drepturiAltii[0] = 'r';
    }
    else
    {
        fileInfo.drepturiAltii[0] = '-';
    }
    if (statFisier.st_mode & S_IWOTH)
    {
        fileInfo.drepturiAltii[1] = 'w';
    }
    else
    {
        fileInfo.drepturiAltii[1] = '-';
    }
    if (statFisier.st_mode & S_IXOTH)
    {
        fileInfo.drepturiAltii[2] = 'x';
    }
    else
    {
        fileInfo.drepturiAltii[2] = '-';
    }
    fileInfo.type = 'B';

    printareStatistica(fileInfo, "statistica.txt");

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

    FileInfo fileInfo;
    strcpy(fileInfo.nume, fisier->d_name);
    fileInfo.size = statFisier.st_size;
    fileInfo.uid = statFisier.st_uid;
    fileInfo.timpulModificarii = statFisier.st_mtime;
    fileInfo.numarLegaturi = statFisier.st_nlink;
    if (statFisier.st_mode & S_IRUSR)
    {
        fileInfo.drepturiUser[0] = 'r';
    }
    else
    {
        fileInfo.drepturiUser[0] = '-';
    }
    if (statFisier.st_mode & S_IWUSR)
    {
        fileInfo.drepturiUser[1] = 'w';
    }
    else
    {
        fileInfo.drepturiUser[1] = '-';
    }
    if (statFisier.st_mode & S_IXUSR)
    {
        fileInfo.drepturiUser[2] = 'x';
    }
    else
    {
        fileInfo.drepturiUser[2] = '-';
    }
    if (statFisier.st_mode & S_IRGRP)
    {
        fileInfo.drepturiGrup[0] = 'r';
    }
    else
    {
        fileInfo.drepturiGrup[0] = '-';
    }
    if (statFisier.st_mode & S_IWGRP)
    {
        fileInfo.drepturiGrup[1] = 'w';
    }
    else
    {
        fileInfo.drepturiGrup[1] = '-';
    }
    if (statFisier.st_mode & S_IXGRP)
    {
        fileInfo.drepturiGrup[2] = 'x';
    }
    else
    {
        fileInfo.drepturiGrup[2] = '-';
    }
    if (statFisier.st_mode & S_IROTH)
    {
        fileInfo.drepturiAltii[0] = 'r';
    }
    else
    {
        fileInfo.drepturiAltii[0] = '-';
    }
    if (statFisier.st_mode & S_IWOTH)
    {
        fileInfo.drepturiAltii[1] = 'w';
    }
    else
    {
        fileInfo.drepturiAltii[1] = '-';
    }
    if (statFisier.st_mode & S_IXOTH)
    {
        fileInfo.drepturiAltii[2] = 'x';
    }
    else
    {
        fileInfo.drepturiAltii[2] = '-';
    }
    fileInfo.type = 'R';

    printareStatistica(fileInfo, "statistica.txt");

    close(fd);

}

void printareDateDirector(struct dirent *fisier, char *caleFisier, struct stat statFisier){
    int fd;
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);
    if ((fd = open(caleNoua, O_RDONLY)) == -1)
    {
        error("Eroare deschidere fisier intrare");
    }

    FileInfo fileInfo;
    strcpy(fileInfo.nume, fisier->d_name);
    fileInfo.uid = statFisier.st_uid;
    fileInfo.numarLegaturi = statFisier.st_nlink;
    if (statFisier.st_mode & S_IRUSR)
    {
        fileInfo.drepturiUser[0] = 'r';
    }
    else
    {
        fileInfo.drepturiUser[0] = '-';
    }
    if (statFisier.st_mode & S_IWUSR)
    {
        fileInfo.drepturiUser[1] = 'w';
    }
    else
    {
        fileInfo.drepturiUser[1] = '-';
    }
    if (statFisier.st_mode & S_IXUSR)
    {
        fileInfo.drepturiUser[2] = 'x';
    }
    else
    {
        fileInfo.drepturiUser[2] = '-';
    }
    if (statFisier.st_mode & S_IRGRP)
    {
        fileInfo.drepturiGrup[0] = 'r';
    }
    else
    {
        fileInfo.drepturiGrup[0] = '-';
    }
    if (statFisier.st_mode & S_IWGRP)
    {
        fileInfo.drepturiGrup[1] = 'w';
    }
    else
    {
        fileInfo.drepturiGrup[1] = '-';
    }
    if (statFisier.st_mode & S_IXGRP)
    {
        fileInfo.drepturiGrup[2] = 'x';
    }
    else
    {
        fileInfo.drepturiGrup[2] = '-';
    }
    if (statFisier.st_mode & S_IROTH)
    {
        fileInfo.drepturiAltii[0] = 'r';
    }
    else
    {
        fileInfo.drepturiAltii[0] = '-';
    }
    if (statFisier.st_mode & S_IWOTH)
    {
        fileInfo.drepturiAltii[1] = 'w';
    }
    else
    {
        fileInfo.drepturiAltii[1] = '-';
    }
    if (statFisier.st_mode & S_IXOTH)
    {
        fileInfo.drepturiAltii[2] = 'x';
    }
    else
    {
        fileInfo.drepturiAltii[2] = '-';
    }
    fileInfo.type = 'D';

    printareStatistica(fileInfo, "statistica.txt");

    close(fd);

}

void printareDateLink(struct dirent *fisier, char *caleFisier, struct stat statFisier){
    int fisierIesire = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fisierIesire == -1)
    {
        error("Eroare deschidere fisier iesire");
    }

    FileInfo fileInfo;
    strcpy(fileInfo.nume, fisier->d_name);
    fileInfo.size = statFisier.st_size;
    fileInfo.uid = statFisier.st_uid;
    fileInfo.numarLegaturi = statFisier.st_nlink;
    if (statFisier.st_mode & S_IRUSR)
    {
        fileInfo.drepturiUser[0] = 'r';
    }
    else
    {
        fileInfo.drepturiUser[0] = '-';
    }
    if (statFisier.st_mode & S_IWUSR)
    {
        fileInfo.drepturiUser[1] = 'w';
    }
    else
    {
        fileInfo.drepturiUser[1] = '-';
    }
    if (statFisier.st_mode & S_IXUSR)
    {
        fileInfo.drepturiUser[2] = 'x';
    }
    else
    {
        fileInfo.drepturiUser[2] = '-';
    }
    if (statFisier.st_mode & S_IRGRP)
    {
        fileInfo.drepturiGrup[0] = 'r';
    }
    else
    {
        fileInfo.drepturiGrup[0] = '-';
    }
    if (statFisier.st_mode & S_IWGRP)
    {
        fileInfo.drepturiGrup[1] = 'w';
    }
    else
    {
        fileInfo.drepturiGrup[1] = '-';
    }
    if (statFisier.st_mode & S_IXGRP)
    {
        fileInfo.drepturiGrup[2] = 'x';
    }
    else
    {
        fileInfo.drepturiGrup[2] = '-';
    }
    if (statFisier.st_mode & S_IROTH)
    {
        fileInfo.drepturiAltii[0] = 'r';
    }
    else
    {
        fileInfo.drepturiAltii[0] = '-';
    }
    if (statFisier.st_mode & S_IWOTH)
    {
        fileInfo.drepturiAltii[1] = 'w';
    }
    else
    {
        fileInfo.drepturiAltii[1] = '-';
    }
    if (statFisier.st_mode & S_IXOTH)
    {
        fileInfo.drepturiAltii[2] = 'x';
    }
    else
    {
        fileInfo.drepturiAltii[2] = '-';
    }
    fileInfo.type = 'L';

    printareStatistica(fileInfo, "statistica.txt");

    close(fisierIesire);
}

void decizieFisier(struct dirent *fisier, char *caleFisier)
{
    struct stat statFisier;
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);

    if (lstat(caleNoua, &statFisier) == -1)
    {
        error("Eroare lstat");
    }

    if (S_ISLNK(statFisier.st_mode))
    {
        printf("%s este un Link\n\n", fisier->d_name);
        printareDateLink(fisier, caleFisier, statFisier);
    }
    else if (S_ISDIR(statFisier.st_mode))
    {
        printf("%s este un Director\n\n", fisier->d_name);
        printareDateDirector(fisier, caleFisier, statFisier);
    }
    else if (S_ISREG(statFisier.st_mode))
    {
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
