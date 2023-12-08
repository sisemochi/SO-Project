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

int nrPropozitiiCorecteTotale = 0;

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
    // tipul fisierului ('B' = BMP, 'D' = Directory, 'L' = Link, 'R' = Regular, 'O' = Others)
} FileInfo;

void error(char *msg) {
    perror(msg);
    exit(1);
}

void bmpAlbNegru(int fd, char *header) {
    //explicatie *(int *) &header[x]
    //&header[x] : adresa celui de al x-ulea element din header
    //(int *) : considera adresa ca un pointer la int
    //* : ia valoarea de la adresa respectiva


    int latime = *(int *) &header[18];
    int inaltime = *(int *) &header[22];

    //explicatie padding
    //informatia fiecarui pixel la fisiere bmp e de 3 bytes(pt rosu, verde, albastru)
    //lungimea unei linii este de (3 * latime) bytes
    //aceasta TREBUIE sa fie multiplu de 4
    //calculam cu primul % cati bytes suntem peste limita
    //scadem 4 ca sa calculam cati bytes mai avem pana la urmatorul multiplu de 4
    //inca un %4 pentru cazul in care latime*3 era deja multiplu de 4
    //padding e deci folosit pt a verifica ca fiecare linie de pixeli e pe 4 bytes
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

            //formula de pe cv
            unsigned char gray = (unsigned char) (0.299 * pixeli[2] + 0.587 * pixeli[1] + 0.114 * pixeli[0]);

            //mergem inapoi in memorie de unde am citit pixelii pe care ii modificam
            lseek(fd, -3, SEEK_CUR);

            //rescriem memoria cu pixelii gri
            memset(pixeli, gray, sizeof(pixeli));
            if (write(fd, pixeli, 3) != 3) {
                error("Writing pixeli failed");
                close(fd);
            }
        }
        lseek(fd, padding, SEEK_CUR);
    }

}

//un write cu error handling integrat
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
        //fisier .bmp
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
        //fisier regular nu .bmp
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
                memset(buffer, 0, sizeof(buffer));
                read(fd2[0], buffer, sizeof(buffer)); // citeste din pipe outputul scriptului
                printf("\n-------------------------\nOutput comanda: \n%s\n----------------------\n", buffer);

                close(fd2[0]); // inchide citirea din pipe

                //scriere buffer in fd_parinte
                if (fdParinte != NULL) {
//                    printf("Scriere in pipe non-null\nVom scrie: %s\n", buffer);
                    ssize_t bytesWritten = write(fdParinte[1], buffer, sizeof(buffer));
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
        //fisier symbolic link
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
        //fisier director
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
    //generare nume fisier statistica
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

    //generare nume fisier statistica
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

    //generare nume fisier statistica
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
    //generare nume fisier statistica
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
    int pipeParinteCopil[2]; // pipeParinteCopil[0] - citire, pipeParinteCopil[1] - scriere
    if (pipe(pipeParinteCopil) == -1) {
        error("Eroare pipe");
    }
    int isRegular = 0;  //flag pentru a stii daca fisierul e regular si nu e .bmp

    struct stat statFisier;

    //creare path catre fisier/document
    char caleNoua[100];
    strcpy(caleNoua, caleFisier);
    strcat(caleNoua, "/");
    strcat(caleNoua, fisier->d_name);

    if (lstat(caleNoua, &statFisier) == -1) {
        error("Eroare lstat");
    }

    if(S_ISREG(statFisier.st_mode) && strcmp(fisier->d_name, ".bmp") != 0){
        isRegular = 1; //setare flag
    };


    //initializare fork
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        error("Eroare fork");
    } else if (pid == 0) {
        //Proces copil

        if (S_ISLNK(statFisier.st_mode)) {
            //fisierul e symbolic link
            printf("%s este un Link\n\n", fisier->d_name);
            printareDateLink(fisier, caleFisier, statFisier, fisierIesire);
        } else if (S_ISDIR(statFisier.st_mode)) {
            //fisierul e director
            printf("%s este un Director\n\n", fisier->d_name);
            printareDateDirector(fisier, caleFisier, statFisier, fisierIesire);
        } else if (S_ISREG(statFisier.st_mode)) {
            //fisierul e regular
            char *extensie = strrchr(fisier->d_name, '.');
            if (extensie != NULL && strcmp(extensie, ".bmp") == 0) {
                //fisierul e regular si .bmp
                printf("Fisierul %s este un .bmp\n\n", fisier->d_name);
                printareDateBMP(fisier, caleFisier, statFisier, fisierIesire);
            } else {
                //fisierul e regular dar nu .bmp
                printf("Fisierul %s nu este un .bmp\n\n", fisier->d_name);
                printareDateRegulareNuBMP(fisier, caleFisier, statFisier, fisierIesire, caracter, pipeParinteCopil);
            }
        } else {
            printf("%s este Altceva\n\n", fisier->d_name);
        }
    } else if (pid > 0) {
        //proces parinte
        printf("Asteptam procesul copil...\n");
        int status;
        waitpid(pid, &status, 0);

        //verificare status proces copil
        if (WIFEXITED(status)) {
            if (isRegular) {
                //citim din pipe daca am prelucrat un fisier regular fara .bmp
                close(pipeParinteCopil[1]); // inchide scrierea in pipe

                char buffer[100];
                ssize_t bytesRead;
                bytesRead = read(pipeParinteCopil[0], buffer, sizeof(buffer));

                char *outputComanda = buffer;   //salvam output-ul scriptului din buffer
                outputComanda++;    //trecem la al 2-lea caracter, primul fiind \n

                printf("buffer parinte final: %s", outputComanda);

                int nrPropozitiiCorecte = atoi(outputComanda);  //transformam din string in int

                nrPropozitiiCorecteTotale += nrPropozitiiCorecte;   //calculam global toate propozitiile corecte

                close(pipeParinteCopil[0]); //inchide citirea din pipe
            }


            close(pipeParinteCopil[0]);
            close(pipeParinteCopil[1]);

            int codIesire = WEXITSTATUS(status);

            //scriere in fisierul de statistica
            int fd = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

            write(fd, fisier->d_name, strlen(fisier->d_name));
            write(fd, " ", 1);
            char *codIesireChar = malloc(50);
            sprintf(codIesireChar, "%d", codIesire); //convertim codul de iesire de la int la char*
            write(fd, codIesireChar, strlen(codIesireChar));
            write(fd, "\n", 1);

            printf("Procesul copil a terminat cu codul de iesire %d\n\n", WEXITSTATUS(status));
        } else {
            printf("Procesul copil a terminat cu eroare\n");
        }
        isRegular = 0;  //resetam flagul pentru urmatorul fisier
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

    printf("\nAu fost gasite %d propozitii corecte in fisierele de statistica\n", nrPropozitiiCorecteTotale);


    return 0;
}
