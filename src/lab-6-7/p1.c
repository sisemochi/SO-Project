#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

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

    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in < 0)
    {
        error("Eroare deschidere fisier intrare");
    }

    char bmp_header[54];
    if (read(fd_in, bmp_header, sizeof(bmp_header)) != sizeof(bmp_header))
    {
        error("Eroare citire header BMP");
    }

    int lungime = *(int *)&bmp_header[22];
    int inaltime = *(int *)&bmp_header[18];
    int dimensiune = *(int *)&bmp_header[2];
    int uid = *(int *)&bmp_header[6];

    struct stat file_stat;

    if (fstat(fd_in, &file_stat) < 0)
    {
        error("Eroare obtinere statistica");
    }

    char time_str[20];
    struct tm *tm_info;
    tm_info = localtime(&file_stat.st_mtime);
    strftime(time_str, 20, "%d.%m.%Y", tm_info);

    char user_perm[4], group_perm[4], others_perm[4];
    sprintf(user_perm, "%c%c%c", (file_stat.st_mode & S_IRUSR) ? 'R' : '-', (file_stat.st_mode & S_IWUSR) ? 'W' : '-', (file_stat.st_mode & S_IXUSR) ? 'X' : '-');
    sprintf(group_perm, "%c%c%c", (file_stat.st_mode & S_IRGRP) ? 'R' : '-', (file_stat.st_mode & S_IWGRP) ? 'W' : '-', (file_stat.st_mode & S_IXGRP) ? 'X' : '-');
    sprintf(others_perm, "%c%c%c", (file_stat.st_mode & S_IROTH) ? 'R' : '-', (file_stat.st_mode & S_IWOTH) ? 'W' : '-', (file_stat.st_mode & S_IXOTH) ? 'X' : '-');

    FILE *output_file = fopen("statistica.txt", "w");
    if (output_file == NULL)
    {
        error("Eroare deschidere fisier statistica");
    }

    fprintf(output_file, "nume fisier: %s\n", argv[1]);
    fprintf(output_file, "inaltime: %d\n", inaltime);
    fprintf(output_file, "lungime: %d\n", lungime);
    fprintf(output_file, "dimensiune: %d\n", dimensiune);
    fprintf(output_file, "identificatorul utilizatorului: %d\n", uid);
    fprintf(output_file, "timpul ultimei modificari: %s\n", time_str);
    fprintf(output_file, "contorul de legaturi: %ld\n", file_stat.st_nlink);
    fprintf(output_file, "drepturi de acces user: %s\n", user_perm);
    fprintf(output_file, "drepturi de acces grup: %s\n", group_perm);
    fprintf(output_file, "drepturi de acces altii: %s\n", others_perm);

    close(fd_in);
    fclose(output_file);

    return 0;
}
