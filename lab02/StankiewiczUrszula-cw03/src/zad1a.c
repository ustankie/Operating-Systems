#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments!\n");
        return 0;
    }

    if (access(argv[1], F_OK) != 0)
    {
        printf("The file \'%s\' does not exist!\n", argv[1]);
        return 0;
    }

    char c;
    int in, out, clock_out;
    clock_t start = clock();

    in = open(argv[1], O_RDONLY);
    out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    int curr_position = SEEK_CUR;
    curr_position = lseek(in, -1, SEEK_END);

    while (read(in, &c, 1) == 1 && curr_position >= 0)
    {
        int written_bytes = write(out, &c, 1);
        curr_position = lseek(in, -2 * written_bytes, SEEK_CUR);
    }

    clock_out = open("pomiar_zad_1.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;

    char buff[16];
    int a = sprintf(buff, "%f", seconds);
    buff[a] = 's';
    buff[a + 1] = '\n';

    char title[] = "Bytes: ";

    write(clock_out, title, sizeof(title) - 1);
    write(clock_out, buff, a + 2);

    close(clock_out);
    close(in);
    close(out);
}