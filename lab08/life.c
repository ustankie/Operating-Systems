#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <signal.h>

#define max_threads (grid_width * grid_height)



pthread_t threads[max_threads];

void handle(int sig_num){

}


int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Wrong number of arguments!\n");
		return -1;
	}

	
	int threads_num = atoi(argv[1]);

	printf("Number of threads: %d\n", threads_num);
	if (threads_num > max_threads)
	{
		printf("Number of threads cannot be greater than the grid size!\n");
		return -1;
	}

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	
	

	char *foreground = create_grid();
	char *background = create_grid();
	// char *tmp;

	init_grid(foreground);

	

	int cells_per_thread = floor(max_threads / threads_num);

	printf("Cells per thread: %d\n\n",cells_per_thread);
	signal(SIGUSR1,handle);

	int start_cell = 0;

	for (int i = 0; i < threads_num; i++)
	{
		int cells = cells_per_thread;
		if (i<max_threads%threads_num)
		{
			cells+=1;
		}

		Args args;
		args.src = foreground;
		args.dst = background;
		args.begin = start_cell;
		args.end = start_cell + cells;
		args.thread_num=i;

		printf("Thread %d: Begin: %d, end: %d\n",args.thread_num,args.begin,args.end);

		if (pthread_create(&threads[i], NULL, &update_cells, &args) != 0)
		{
			perror("pthread_create");
			return -1;
		}

		start_cell += cells;
	}
	initscr(); // Start curses mode
	


	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		for (int j = 0; j < threads_num; j++)
		{
			if (pthread_kill(threads[j], SIGUSR1) < 0)
			{
				perror("pthread_kill");
				return -1;
			}
		}

		// Step simulation
		// update_grid(foreground, background);
		// tmp = foreground;
		// foreground = background;
		// background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);



	return 0;
}
