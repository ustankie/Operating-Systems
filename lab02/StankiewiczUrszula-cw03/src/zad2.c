#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>


int main()
{
    DIR *cur_dir = opendir("./");
    struct dirent *cur_file;
    struct stat buff;

    if (cur_dir == NULL)
    {
        perror("Error opening directory");
        return 1;
    }

    long long sum_size = 0;

    while ((cur_file = readdir(cur_dir)))
    {
        if (stat(cur_file->d_name, &buff) == -1)
        {
            perror("Error getting file info");
            continue;
        }

        if (!S_ISDIR(buff.st_mode))
        {
            printf("%s: %ldB\n", cur_file->d_name, buff.st_size);
            sum_size += buff.st_size;
        }
        
    }

    printf("\nTotal: %lldB\n", sum_size);
    closedir(cur_dir);
}