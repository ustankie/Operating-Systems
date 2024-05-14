#pragma once
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

#define grid_width 30
#define grid_height 30

typedef struct
{
	char *src;
	char *dst;
	int begin;
	int end;
    int thread_num;
} Args;


char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
void update_cells(Args *args);