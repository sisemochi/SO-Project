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

void error(char *msg) {
    perror(msg);
    exit(1);
}

void bmpAlbNegru(int fd, char *header) {
    int latime = *(int *) &header[18];
    int inaltime = *(int *) &header[22];
    int padding = (4 - (latime * 3) % 4) % 4;
    int pixelDataOffset = *(int *) &header[10];
    lseek(fd, pixelDataOffset, SEEK_SET);

    unsigned char pixeli[3];
    for (int y = 0; y < inaltime; y++) {
        for (int x = 0; x < latime; x++) {
            if (read(fd, pixeli, 3) != 3) {
                error("Reading pixeli failed");
                close(fd);
            }

            unsigned char gray = (unsigned char) (0.299 * pixeli[2] + 0.587 * pixeli[1] + 0.114 * pixeli[0]);
            lseek(fd, -3, SEEK_CUR);
            memset(pixeli, gray, sizeof(pixeli));
            if (write(fd, pixeli, 3) != 3) {
                error("Writing pixeli failed");
                close(fd);
            }
        }
        lseek(fd, padding, SEEK_CUR);
    }

}


void writeCheck(int fd, char *buffer, unsigned long size) {
    if (write(fd, buffer, size) == -1) {
        error("Eroare scriere");
    }
}

void printareStatistica(FileInfo fileInfo, char *fisierIesire, char *caracter, int *fdParinte) {
    int fd;
    if ((fd = open(fisierIesire, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1) {
        error("Eroare deschidere fisier iesire");
    }
    int nrLinii = 0;

    if (fileInfo.type == 'B') {
        writeCheck(fd, "nume fisier:", 12);
        writeCheck(fd, fileInfo.nume, strlen(fileInfo.nume));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "inaltime: ", 10);
        char *inaltimeChar = (char *) malloc(10 * sizeof(char));
        sprintf(inaltimeChar, "%d", fileInfo.inaltime);
        writeCheck(fd, inaltimeChar, strlen(inaltimeChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "lungime: ", 9);
        char *lungimeChar = (char *) malloc(10 * sizeof(char));
        sprintf(lungimeChar, "%d", fileInfo.lungime);
        writeCheck(fd, lungimeChar, strlen(lungimeChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Dimensiune: ", 12);
        char *dimensiuneChar = (char *) malloc(10 * sizeof(char));
        sprintf(dimensiuneChar, "%ld", fileInfo.size);
        writeCheck(fd, dimensiuneChar, strlen(dimensiuneChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Identificatorul utilizatorului: ", 32);
        char *identificatorulUtilizatoruluiChar = (char *) malloc(10 * sizeof(char));
        sprintf(identificatorulUtilizatoruluiChar, "%ld", fileInfo.uid);
        writeCheck(fd, identificatorulUtilizatoruluiChar, strlen(identificatorulUtilizatoruluiChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Timpul ultimei modificari: ", 27);
        char *timpulUltimeiModificariChar = (char *) malloc(10 * sizeof(char));
        sprintf(timpulUltimeiModificariChar, "%ld", fileInfo.timpulModificarii);
        writeCheck(fd, timpulUltimeiModificariChar, strlen(timpulUltimeiModificariChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "numarul de legaturi: ", 21);
        char *numarulDeLegaturiChar = (char *) malloc(10 * sizeof(char));
        sprintf(numarulDeLegaturiChar, "%d", fileInfo.numarLegaturi);
        writeCheck(fd, numarulDeLegaturiChar, strlen(numarulDeLegaturiChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces user: ", 24);
        writeCheck(fd, fileInfo.drepturiUser, 3);
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces grup: ", 24);
        writeCheck(fd, fileInfo.drepturiGrup, 3);
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces altii: ", 25);
        writeCheck(fd, fileInfo.drepturiAltii, 3);
        writeCheck(fd, "\n\n", 2);
        nrLinii++;
    }

    if (fileInfo.type == 'R') {
        writeCheck(fd, "nume fisier:", 12);
        writeCheck(fd, fileInfo.nume, strlen(fileInfo.nume));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Dimensiune: ", 12);
        char *dimensiuneChar = (char *) malloc(10 * sizeof(char));
        sprintf(dimensiuneChar, "%ld", fileInfo.size);
        writeCheck(fd, dimensiuneChar, strlen(dimensiuneChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Identificatorul utilizatorului: ", 32);
        char *identificatorulUtilizatoruluiChar = (char *) malloc(10 * sizeof(char));
        sprintf(identificatorulUtilizatoruluiChar, "%ld", fileInfo.uid);
        writeCheck(fd, identificatorulUtilizatoruluiChar, strlen(identificatorulUtilizatoruluiChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Timpul ultimei modificari: ", 27);
        char *timpulUltimeiModificariChar = (char *) malloc(10 * sizeof(char));
        sprintf(timpulUltimeiModificariChar, "%ld", fileInfo.timpulModificarii);
        writeCheck(fd, timpulUltimeiModificariChar, strlen(timpulUltimeiModificariChar));
        writeCheck(fd, ".\n", 2);
        nrLinii++;

        writeCheck(fd, "numarul de legaturi: ", 21);
        char *numarulDeLegaturiChar = (char *) malloc(10 * sizeof(char));
        sprintf(numarulDeLegaturiChar, "%d", fileInfo.numarLegaturi);
        writeCheck(fd, numarulDeLegaturiChar, strlen(numarulDeLegaturiChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces user: ", 24);
        writeCheck(fd, fileInfo.drepturiUser, 3);
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces grup: ", 24);
        writeCheck(fd, fileInfo.drepturiGrup, 3);
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces altii: ", 25);
        writeCheck(fd, fileInfo.drepturiAltii, 3);
        writeCheck(fd, "\n\n", 2);
        nrLinii++;

        int fd2[2]; // fd2[0] - citire, fd2[1] - scriere
        if (pipe(fd2) == -1) {
            error("Eroare pipe");
        }

        int fd_stdin[2]; // Pipe pentru script
        if (pipe(fd_stdin) == -1) {
            perror("pipe");
            exit(1);
        }

        char comanda[200];
        memset(comanda, 0, sizeof comanda);
        strcat(comanda, "cat ");
        strcat(comanda, fisierIesire);
        strcat(comanda, " | ");
        strcat(comanda, "./src/proiect/propozitii.sh ");
        strcat(comanda, caracter);

        printf("\n-----------\nRulam comanda: %s\n", comanda);

        pid_t pid;
        pid = fork();
        if (pid == -1) {
            error("Eroare fork");
        } else if (pid == 0) {
            // proces child

            close(fd2[0]); // inchide citirea din pipe
            close(fd_stdin[1]); // inchide scrierea in pipe a scriptului

            dup2(fd2[1], STDOUT_FILENO);
            dup2(fd_stdin[0], STDIN_FILENO);
            close(fd2[1]); // inchide scriere in pipe
            close(fd_stdin[0]); // inchide citirea din pipe a scriptului

            system(comanda);

            exit(1);
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                printf("In parinte dupa script");
                // proces parinte
                close(fd2[1]); // inchide scrierea in pipe
                close(fd_stdin[0]); // inchide citirea din pipe a scriptului

                close(fd_stdin[1]); // inchide scrierea in pipe a scriptului pentru a trimite EOF

                char buffer[100];
                read(fd2[0], buffer, sizeof(buffer)); // citeste din pipe outputul scriptului
                printf("\n-------------------------\nOutput comanda: \n%s\n----------------------\n", buffer);

                close(fd2[0]); // inchide citirea din pipe

                //scriere buffer in fd_parinte
                if (fdParinte != NULL) {
//                    printf("Scriere in pipe non-null\nVom scrie: %s\n", buffer);
                    ssize_t bytesWritten = write(fdParinte[1], "hei", 3);
                    if (bytesWritten == -1) {
                        printf("---------------------\nEroare scriere in pipe\n");
                        error("Error writing to pipe");
                    }
                }

            } else {
                printf("Child terminated abnormally\n");
            }
        }
    }

    if (fileInfo.type == 'L') {
        writeCheck(fd, "nume legatura: ", 15);
        writeCheck(fd, fileInfo.nume, strlen(fileInfo.nume));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "dimensiune legatura: ", 21);
        char *dimensiuneChar = (char *) malloc(10 * sizeof(char));
        sprintf(dimensiuneChar, "%ld", fileInfo.size);
        writeCheck(fd, dimensiuneChar, strlen(dimensiuneChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "dimensiune fisier target: ", 26);
        char *dimensiuneFisierTargetChar = (char *) malloc(10 * sizeof(char));
        sprintf(dimensiuneFisierTargetChar, "%ld", fileInfo.size);
        writeCheck(fd, dimensiuneFisierTargetChar, strlen(dimensiuneFisierTargetChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces user: ", 24);
        writeCheck(fd, fileInfo.drepturiUser, 3);
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces grup: ", 24);
        writeCheck(fd, fileInfo.drepturiGrup, 3);
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces altii: ", 25);
        writeCheck(fd, fileInfo.drepturiAltii, 3);
        writeCheck(fd, "\n\n", 2);
        nrLinii++;

    }

    if (fileInfo.type == 'D') {
        writeCheck(fd, "nume director: ", 15);
        writeCheck(fd, fileInfo.nume, strlen(fileInfo.nume));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Identificatorul utilizatorului: ", 32);
        char *identificatorulUtilizatoruluiChar = (char *) malloc(10 * sizeof(char));
        sprintf(identificatorulUtilizatoruluiChar, "%ld", fileInfo.uid);
        writeCheck(fd, identificatorulUtilizatoruluiChar, strlen(identificatorulUtilizatoruluiChar));
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces user: ", 24);
        writeCheck(fd, fileInfo.drepturiUser, 3);
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces grup: ", 24);
        writeCheck(fd, fileInfo.drepturiGrup, 3);
        writeCheck(fd, "\n", 1);
        nrLinii++;

        writeCheck(fd, "Drepturi de acces altii: ", 25);
        writeCheck(fd, fileInfo.drepturiAltii, 3);
        writeCheck(fd, "\n\n", 2);
        nrLinii++;
    }

    close(fd);
    exit(nrLinii);

}

void printareDateBMP(struct dirent *fisier, char *caleFisier, struct stat statFisier, char *fisierIesire) {
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);

    int fd;
    if ((fd = open(caleNoua, O_RDWR)) == -1) {
        error("Eroare deschidere fisier intrare");
    }
    char *header = (char *) malloc(54 * sizeof(char));
    if (read(fd, header, 54) == -1) {
        error("Eroare citire header");
    }

    bmpAlbNegru(fd, header);

    FileInfo fileInfo;
    strcpy(fileInfo.nume, fisier->d_name);
    fileInfo.inaltime = *(int *) (header + 22);
    fileInfo.lungime = *(int *) (header + 18);
    fileInfo.size = *(int *) (header + 2);
    fileInfo.uid = statFisier.st_uid;
    fileInfo.timpulModificarii = statFisier.st_mtime;
    fileInfo.numarLegaturi = statFisier.st_nlink;
    if (statFisier.st_mode & S_IRUSR) {
        fileInfo.drepturiUser[0] = 'r';
    } else {
        fileInfo.drepturiUser[0] = '-';
    }
    if (statFisier.st_mode & S_IWUSR) {
        fileInfo.drepturiUser[1] = 'w';
    } else {
        fileInfo.drepturiUser[1] = '-';
    }
    if (statFisier.st_mode & S_IXUSR) {
        fileInfo.drepturiUser[2] = 'x';
    } else {
        fileInfo.drepturiUser[2] = '-';
    }
    if (statFisier.st_mode & S_IRGRP) {
        fileInfo.drepturiGrup[0] = 'r';
    } else {
        fileInfo.drepturiGrup[0] = '-';
    }
    if (statFisier.st_mode & S_IWGRP) {
        fileInfo.drepturiGrup[1] = 'w';
    } else {
        fileInfo.drepturiGrup[1] = '-';
    }
    if (statFisier.st_mode & S_IXGRP) {
        fileInfo.drepturiGrup[2] = 'x';
    } else {
        fileInfo.drepturiGrup[2] = '-';
    }
    if (statFisier.st_mode & S_IROTH) {
        fileInfo.drepturiAltii[0] = 'r';
    } else {
        fileInfo.drepturiAltii[0] = '-';
    }
    if (statFisier.st_mode & S_IWOTH) {
        fileInfo.drepturiAltii[1] = 'w';
    } else {
        fileInfo.drepturiAltii[1] = '-';
    }
    if (statFisier.st_mode & S_IXOTH) {
        fileInfo.drepturiAltii[2] = 'x';
    } else {
        fileInfo.drepturiAltii[2] = '-';
    }
    fileInfo.type = 'B';

    char *caleIesire = malloc(100);
    strcpy(caleIesire, fisierIesire);
    strcat(caleIesire, "/");
    strcat(caleIesire, fisier->d_name);
    strcat(caleIesire, "_statistica.txt");

    printareStatistica(fileInfo, caleIesire, NULL, NULL);

    close(fd);

}

void printareDateRegulareNuBMP(struct dirent *fisier, char *caleFisier, struct stat statFisier, char *fisierIesire,
                               char *caracter, int *fdParinte) {
    int fd;
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);
    if ((fd = open(caleNoua, O_RDONLY)) == -1) {
        error("Eroare deschidere fisier intrare");
    }

    FileInfo fileInfo;
    strcpy(fileInfo.nume, fisier->d_name);
    fileInfo.size = statFisier.st_size;
    fileInfo.uid = statFisier.st_uid;
    fileInfo.timpulModificarii = statFisier.st_mtime;
    fileInfo.numarLegaturi = statFisier.st_nlink;
    if (statFisier.st_mode & S_IRUSR) {
        fileInfo.drepturiUser[0] = 'r';
    } else {
        fileInfo.drepturiUser[0] = '-';
    }
    if (statFisier.st_mode & S_IWUSR) {
        fileInfo.drepturiUser[1] = 'w';
    } else {
        fileInfo.drepturiUser[1] = '-';
    }
    if (statFisier.st_mode & S_IXUSR) {
        fileInfo.drepturiUser[2] = 'x';
    } else {
        fileInfo.drepturiUser[2] = '-';
    }
    if (statFisier.st_mode & S_IRGRP) {
        fileInfo.drepturiGrup[0] = 'r';
    } else {
        fileInfo.drepturiGrup[0] = '-';
    }
    if (statFisier.st_mode & S_IWGRP) {
        fileInfo.drepturiGrup[1] = 'w';
    } else {
        fileInfo.drepturiGrup[1] = '-';
    }
    if (statFisier.st_mode & S_IXGRP) {
        fileInfo.drepturiGrup[2] = 'x';
    } else {
        fileInfo.drepturiGrup[2] = '-';
    }
    if (statFisier.st_mode & S_IROTH) {
        fileInfo.drepturiAltii[0] = 'r';
    } else {
        fileInfo.drepturiAltii[0] = '-';
    }
    if (statFisier.st_mode & S_IWOTH) {
        fileInfo.drepturiAltii[1] = 'w';
    } else {
        fileInfo.drepturiAltii[1] = '-';
    }
    if (statFisier.st_mode & S_IXOTH) {
        fileInfo.drepturiAltii[2] = 'x';
    } else {
        fileInfo.drepturiAltii[2] = '-';
    }
    fileInfo.type = 'R';

    char *caleIesire = malloc(100);
    strcpy(caleIesire, fisierIesire);
    strcat(caleIesire, "/");
    strcat(caleIesire, fisier->d_name);
    strcat(caleIesire, "_statistica.txt");

    printareStatistica(fileInfo, caleIesire, caracter, fdParinte);

    close(fd);

}

void printareDateDirector(struct dirent *fisier, char *caleFisier, struct stat statFisier, char *fisierIesire) {
    int fd;
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);
    if ((fd = open(caleNoua, O_RDONLY)) == -1) {
        error("Eroare deschidere fisier intrare");
    }

    FileInfo fileInfo;
    strcpy(fileInfo.nume, fisier->d_name);
    fileInfo.uid = statFisier.st_uid;
    fileInfo.numarLegaturi = statFisier.st_nlink;
    if (statFisier.st_mode & S_IRUSR) {
        fileInfo.drepturiUser[0] = 'r';
    } else {
        fileInfo.drepturiUser[0] = '-';
    }
    if (statFisier.st_mode & S_IWUSR) {
        fileInfo.drepturiUser[1] = 'w';
    } else {
        fileInfo.drepturiUser[1] = '-';
    }
    if (statFisier.st_mode & S_IXUSR) {
        fileInfo.drepturiUser[2] = 'x';
    } else {
        fileInfo.drepturiUser[2] = '-';
    }
    if (statFisier.st_mode & S_IRGRP) {
        fileInfo.drepturiGrup[0] = 'r';
    } else {
        fileInfo.drepturiGrup[0] = '-';
    }
    if (statFisier.st_mode & S_IWGRP) {
        fileInfo.drepturiGrup[1] = 'w';
    } else {
        fileInfo.drepturiGrup[1] = '-';
    }
    if (statFisier.st_mode & S_IXGRP) {
        fileInfo.drepturiGrup[2] = 'x';
    } else {
        fileInfo.drepturiGrup[2] = '-';
    }
    if (statFisier.st_mode & S_IROTH) {
        fileInfo.drepturiAltii[0] = 'r';
    } else {
        fileInfo.drepturiAltii[0] = '-';
    }
    if (statFisier.st_mode & S_IWOTH) {
        fileInfo.drepturiAltii[1] = 'w';
    } else {
        fileInfo.drepturiAltii[1] = '-';
    }
    if (statFisier.st_mode & S_IXOTH) {
        fileInfo.drepturiAltii[2] = 'x';
    } else {
        fileInfo.drepturiAltii[2] = '-';
    }
    fileInfo.type = 'D';

    char *caleIesire = malloc(100);
    strcpy(caleIesire, fisierIesire);
    strcat(caleIesire, "/");
    strcat(caleIesire, fisier->d_name);
    strcat(caleIesire, "_statistica.txt");

    printareStatistica(fileInfo, caleIesire, NULL, NULL);

    close(fd);

}

void printareDateLink(struct dirent *fisier, char *caleFisier, struct stat statFisier, char *fisierIesire) {
    char *caleFinal = malloc(100);
    strcpy(caleFinal, fisierIesire);
    strcat(caleFinal, "/");
    strcat(caleFinal, fisier->d_name);
    strcat(caleFinal, "_statistica.txt");

    FileInfo fileInfo;
    strcpy(fileInfo.nume, fisier->d_name);
    fileInfo.size = statFisier.st_size;
    fileInfo.uid = statFisier.st_uid;
    fileInfo.numarLegaturi = statFisier.st_nlink;
    if (statFisier.st_mode & S_IRUSR) {
        fileInfo.drepturiUser[0] = 'r';
    } else {
        fileInfo.drepturiUser[0] = '-';
    }
    if (statFisier.st_mode & S_IWUSR) {
        fileInfo.drepturiUser[1] = 'w';
    } else {
        fileInfo.drepturiUser[1] = '-';
    }
    if (statFisier.st_mode & S_IXUSR) {
        fileInfo.drepturiUser[2] = 'x';
    } else {
        fileInfo.drepturiUser[2] = '-';
    }
    if (statFisier.st_mode & S_IRGRP) {
        fileInfo.drepturiGrup[0] = 'r';
    } else {
        fileInfo.drepturiGrup[0] = '-';
    }
    if (statFisier.st_mode & S_IWGRP) {
        fileInfo.drepturiGrup[1] = 'w';
    } else {
        fileInfo.drepturiGrup[1] = '-';
    }
    if (statFisier.st_mode & S_IXGRP) {
        fileInfo.drepturiGrup[2] = 'x';
    } else {
        fileInfo.drepturiGrup[2] = '-';
    }
    if (statFisier.st_mode & S_IROTH) {
        fileInfo.drepturiAltii[0] = 'r';
    } else {
        fileInfo.drepturiAltii[0] = '-';
    }
    if (statFisier.st_mode & S_IWOTH) {
        fileInfo.drepturiAltii[1] = 'w';
    } else {
        fileInfo.drepturiAltii[1] = '-';
    }
    if (statFisier.st_mode & S_IXOTH) {
        fileInfo.drepturiAltii[2] = 'x';
    } else {
        fileInfo.drepturiAltii[2] = '-';
    }
    fileInfo.type = 'L';


    printareStatistica(fileInfo, caleFinal, NULL, NULL);

}

void decizieFisier(struct dirent *fisier, char *caleFisier, char *fisierIesire, char *caracter) {
    int fdParinte[2]; // fdParinte[0] - citire, fdParinte[1] - scriere
    if (pipe(fdParinte) == -1) {
        error("Eroare pipe");
    }
    int isRegular = 0;

    pid_t pid;
    pid = fork();
    if (pid == -1) {
        error("Eroare fork");
    } else if (pid == 0) {
        //Proces child
        struct stat statFisier;
        char caleNoua[100];
        strcpy(caleNoua, caleFisier);
        strcat(caleNoua, "/");
        strcat(caleNoua, fisier->d_name);

        if (lstat(caleNoua, &statFisier) == -1) {
            error("Eroare lstat");
        }

        if (S_ISLNK(statFisier.st_mode)) {
            printf("%s este un Link\n\n", fisier->d_name);
            printareDateLink(fisier, caleFisier, statFisier, fisierIesire);
        } else if (S_ISDIR(statFisier.st_mode)) {
            printf("%s este un Director\n\n", fisier->d_name);
            printareDateDirector(fisier, caleFisier, statFisier, fisierIesire);
        } else if (S_ISREG(statFisier.st_mode)) {
            char *extensie = strrchr(fisier->d_name, '.');
            if (extensie != NULL && strcmp(extensie, ".bmp") == 0) {
                printf("Fisierul %s este un .bmp\n\n", fisier->d_name);
                printareDateBMP(fisier, caleFisier, statFisier, fisierIesire);
            } else {
                printf("Fisierul %s nu este un .bmp\n\n", fisier->d_name);
                isRegular = 1;
                printareDateRegulareNuBMP(fisier, caleFisier, statFisier, fisierIesire, caracter, fdParinte);
            }
        } else {
            printf("%s este Altceva\n\n", fisier->d_name);
        }

//        exit(0);//terminam procesul copil daca nu a fost terminat pana acum
    } else if (pid > 0) {
        //proces parinte
        printf("Asteptam procesul copil...\n");
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            if (isRegular) {
                //codul asta nu ruleaza?
                //aici ar trebui facuta suma, din buffer
                printf("Procesul copil a terminat cu succes\n");
                close(fdParinte[1]); // inchide scrierea in pipe

                char buffer[100];
                ssize_t bytesRead;
                bytesRead = read(fdParinte[0], buffer, sizeof(buffer));
                printf("bytesRead: %ld\n", bytesRead);
                while ((bytesRead = read(fdParinte[0], buffer, sizeof(buffer))) >= 0) {
                    printf("In while\n");
                    buffer[bytesRead] = '\0';
                    printf("\n-------------------------\nOutput comanda din parintele mare: \n");
                    fwrite(buffer, sizeof(char), bytesRead, stdout);
                    printf("\n----------------------\n");
                }

                close(fdParinte[0]); //inchide citirea din pipe
            }


            close(fdParinte[0]);
            close(fdParinte[1]);

            int codIesire = WEXITSTATUS(status);

            int fd = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

            write(fd, fisier->d_name, strlen(fisier->d_name));
            write(fd, " ", 1);
            char *codIesireChar = malloc(50);
            sprintf(codIesireChar, "%d", codIesire);
            write(fd, codIesireChar, strlen(codIesireChar));
            write(fd, "\n", 1);

            printf("Procesul copil a terminat cu codul de iesire %d\n\n", WEXITSTATUS(status));
        } else {
            printf("Procesul copil a terminat cu eroare\n");
        }
    }

}


int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Numar invalid argumente!\n");
        printf("Folosire corecta: ./program <cale_fisier_intrare> <cale_fisier_iesire> <caracter>\n");
        exit(1);
    }

    DIR *director;
    struct dirent *directorStruct;
    if ((director = opendir(argv[1])) == NULL) {
        error("Eroare deschidere fisier intrare");
    }

    while ((directorStruct = readdir(director)) != NULL) {
        // Ignoram directorul curent si parintele
        if (strcmp(directorStruct->d_name, ".") == 0 || strcmp(directorStruct->d_name, "..") == 0) {
            continue;
        }
        decizieFisier(directorStruct, argv[1], argv[2], argv[3]);
    }


    return 0;
}
