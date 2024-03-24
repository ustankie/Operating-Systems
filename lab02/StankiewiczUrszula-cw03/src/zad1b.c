#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>

#define max(a, b) ((a > b) ? a : b)
void reverse(char *block, int size);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments!\n");
        return 0;
    }

    char block[1024];
    size_t block_size = sizeof(block);
    int in, out, real_size, clock_out;
    clock_t start = clock();

    if (access(argv[1], F_OK) != 0)
    {
        printf("The file \'%s\' does not exist!\n", argv[1]);
        return 0;
    }

    in = open(argv[1], O_RDONLY);
    out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    int curr_position = SEEK_CUR;
    curr_position = max(0, lseek(in, -block_size, SEEK_END));

    while ((real_size = read(in, block, block_size)) > 0 && curr_position >= 0)
    {
        reverse(block, real_size);
        write(out, block, real_size);
        curr_position = lseek(in, -2 * block_size, SEEK_CUR);
    }

    clock_out = open("pomiar_zad_1.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;

    char buff[16];
    int a = sprintf(buff, "%f", seconds);
    buff[a] = 's';
    buff[a + 1] = '\n';

    char title[] = "Blocks: ";

    write(clock_out, title, sizeof(title) - 1);
    write(clock_out, buff, a + 2);

    close(clock_out);
    close(in);
    close(out);
}

void reverse(char *block, int size)
{
    for (int i = 0; i < (size / 2); i++)
    {
        char temp = block[i];
        block[i] = block[size - i - 1];
        block[size - i - 1] = temp;
    }
}