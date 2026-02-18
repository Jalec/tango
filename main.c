#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "render_figures.h"

#define WINDOW_TITLE "Tango"
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720

#define TABLE_WIDTH 720
#define TABLE_HEIGHT 720
#define TABLE_START_X 0
#define TABLE_START_Y 0

typedef enum {
	EMPTY = 0,
	MOON = 1,
	SUN = 2
} figure_t;


void draw_figures(SDL_Renderer *prenderer, figure_t ***game_table);
bool is_safe(figure_t potential_figure, int x, int y, figure_t ***game_table);
bool check_adjency(figure_t selected_figure, int x, int y, figure_t ***game_table); 
int solver(figure_t ***game_table); 

void create_table_game(SDL_Renderer *prenderer) {
	const SDL_Rect table_frame = { .x = TABLE_START_X, .y = TABLE_START_Y, .w = TABLE_WIDTH, .h = TABLE_HEIGHT };
	SDL_SetRenderDrawColor(prenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(prenderer, &table_frame);
	
	for (int i = 1; i < 6; i++) {
		SDL_RenderDrawLine(prenderer, TABLE_START_X,TABLE_START_Y + (i * 120), TABLE_WIDTH,TABLE_START_Y + (i * 120)); // Horizontal lines 
		SDL_RenderDrawLine(prenderer, TABLE_START_X + (i * 120), TABLE_START_Y, TABLE_START_X + (i * 120), TABLE_HEIGHT); // Vertical lines
	}
}

void render_updated_frame(SDL_Renderer *prenderer, figure_t *** game_table) {		
	SDL_SetRenderDrawColor(prenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(prenderer);
	create_table_game(prenderer);
	draw_figures(prenderer, game_table);
	SDL_RenderPresent(prenderer);
}

void initialize_game_table(figure_t ***game_table){
	*game_table = malloc(6 * sizeof(figure_t *));
	for(int i = 0; i<6; i++){
		(*game_table)[i] = malloc(6*sizeof(figure_t));
		for(int j = 0; j < 6; j++){
			(*game_table)[i][j] = EMPTY;
		}
	}
}

bool check_balance(figure_t selected_figure, int x, int y, figure_t ***game_table) {
	bool status = 1;
	int repeated_x_figures = 0;
	int repeated_y_figures = 0;
	
	for(int i = 0; i < 6; i++) {
		if((*game_table)[i][y] == selected_figure){
			repeated_x_figures++;
		}
		if((*game_table)[x][i] == selected_figure){
			repeated_y_figures++;
		}	
	}

	if((repeated_x_figures >= 3) || (repeated_y_figures >= 3)){
		status = 0;
	}
	
	// EXPERIMENTING
	// printf("x_figures: %d, y_figures: %d\n", repeated_x_figures, repeated_y_figures);

	return status;
}

bool is_safe(figure_t potential_figure, int x, int y, figure_t ***game_table) {
	if(check_balance(potential_figure, x, y, game_table) == 1 && check_adjency(potential_figure, x, y, game_table) == 1) {
		return 1;
	}
	return 0;
}

figure_t opposite_figure(figure_t figure) {
	if(figure == SUN) {
		return MOON;
	}
	return SUN;
}


void backtracking(int *x, int *y, figure_t ***game_table) {
	if(*x == 0) {
		(*y)--;
		*x = 5;
	} else {
		(*x)--;
	}
	
	if(is_safe(opposite_figure((*game_table)[*x][*y]), *x, *y, game_table)) {
		(*game_table)[*x][*y] = opposite_figure((*game_table)[*x][*y]);
		if(*x < 5) {
			(*x)++;
		} else {
			(*x) = 0;
			(*y)++;
		}
	} else {
		(*game_table)[*x][*y] = EMPTY;
		printf("Check\n");
		backtracking(x, y, game_table);
	}
}

void rand_solving_game_table(figure_t ***game_table){
	int x = 0;
	int y = 0;
	while ((*game_table)[x][y] == EMPTY) {
		printf("Checking x: %d, y: %d\n", x, y);
		if((*game_table)[x][y] != EMPTY) {
			if(x < 5) {
				x++;
			} else {
				x = 0;
				y++;
			}
		} else {
			figure_t random_figure = (rand() % (SUN - MOON +1)) + MOON;
			if (is_safe(random_figure, x, y, game_table)) {
				(*game_table)[x][y] = random_figure;
				if(x < 5) {
					x++;
				} else {
					x = 0;
					y++;
				}
			} else if(is_safe(opposite_figure(random_figure), x, y, game_table)) {
				(*game_table)[x][y] = opposite_figure(random_figure);
				if(x < 5) {
					x++;
				} else {
					x = 0;
					y++;
				}
			} else {
				backtracking(&x, &y, game_table);
			}
		}
	} 		
}

bool answer_is_valid(figure_t ***game_table) {
	for(int i = 0; i < 6; i++){
		for(int j = 0; j < 6; j++) {
			if(!is_safe((*game_table)[i][j], i, j, game_table)){
				printf("x: %d, y: %d\n", i, j);
				return 0;
			}	
		}
	}
	return 1;
}

void free_game_table(figure_t ***game_table){
	for(int i = 0; i < 6; i++) {
		free((*game_table)[i]);
	}
	free(*game_table);
}


void draw_figures(SDL_Renderer *prenderer, figure_t ***game_table){
	for(int i = 0; i < 6; i++){
		for(int j = 0; j < 6; j++) {
			switch((*game_table)[i][j]) {
				case MOON:
					SDL_RenderDrawCircle(prenderer, 60 + (i*120), 60 + (j*120), 30);
					break;
				case SUN:
					const SDL_Rect sun = { .x = 40 + (i*120), .y = 40 + (j*120), .w = 40, .h = 40 };
					SDL_RenderDrawRect(prenderer, &sun);
					break;
			}	
		}
	}	
}

int coord_to_table(int coord) {
	double result = (double)(coord - 60) / 120;
	return (int)round(result);
}

figure_t get_next_figure(figure_t current) {
	switch(current){
		case MOON:
			return SUN;
			break;
		case SUN:
			return EMPTY;
			break;
		case EMPTY:
			return MOON;
			break;
		default:
			return EMPTY;
	}
}

bool check_adjency(figure_t selected_figure, int x, int y, figure_t ***game_table) {
	bool status = 1;

	// Check rows
	if(x != 0 && x != 1 ) {
		if((*game_table)[x - 1][y] == selected_figure && (*game_table)[x - 2][y] == selected_figure) {
			status = 0;
		}
	}
	if(x != 4 && x != 5) {
		if((*game_table)[x + 1][y] == selected_figure && (*game_table)[x + 2][y] == selected_figure) {
			status = 0;
		}
	}

	// Check columns
	if(y != 0 && y != 1 ) {
		if((*game_table)[x][y - 1] == selected_figure && (*game_table)[x][y - 2] == selected_figure) {
			status = 0;
		}
	}
	if(y != 4 && y != 5) {
		if((*game_table)[x][y + 1] == selected_figure && (*game_table)[x][y + 2] == selected_figure) {
			status = 0;
		}
	}

	return status;
}

int main () {
	// We initialize the SDL Library
    SDL_Init(SDL_INIT_VIDEO);
	
	// We create a window
    SDL_Window *pwindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if(pwindow == NULL) {
        printf("BAD WINDOW");
        return 1;
    }

	// We create a renderer that we will use in the window
   	SDL_Renderer *prenderer = SDL_CreateRenderer(pwindow,-1, 0);

	// We create the 2D Array
	figure_t **game_table;

	srand(time(NULL));
	initialize_game_table(&game_table);
	rand_solving_game_table(&game_table);

	if(answer_is_valid(&game_table)) {
		printf("Valid grid!");
	}
	//printf("cells: %d", rand_solving_game_table(&game_table));
		
	render_updated_frame(prenderer, &game_table);

	int game_running = 1;
	while (game_running) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					game_running = 0;
					break;
				case SDL_KEYDOWN:
					if(!strcmp(SDL_GetKeyName(event.key.keysym.sym), "Escape")) {
						game_running = 0;
						break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					figure_t selected_figure = get_next_figure(game_table[coord_to_table(event.button.x)][coord_to_table(event.button.y)]);

					printf("%d\n",is_safe(selected_figure, coord_to_table(event.button.x), coord_to_table(event.button.y), &game_table));
					game_table[coord_to_table(event.button.x)][coord_to_table(event.button.y)] = selected_figure;
				    

					render_updated_frame(prenderer, &game_table);
					break;
			}
		}
	
		SDL_Delay(16);
	}
	
	free_game_table(&game_table);
    SDL_DestroyWindow(pwindow);
	SDL_Quit();

    return 0;
}
