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

typedef struct unvisited_node {
	int x;
	int y;
} unvisited_node_t;

void draw_figures(SDL_Renderer *prenderer, figure_t ***game_table);
bool is_safe(figure_t potential_figure, int x, int y, figure_t ***game_table);
bool check_adjency(figure_t selected_figure, int x, int y, figure_t ***game_table);
void shuffle(figure_t *choices);
void dig(figure_t ***game_table, unvisited_node_t *unvisited_nodes, int size, unvisited_node_t *visited_nodes);
bool check_solutions(figure_t ***game_table, unvisited_node_t *visited_nodes, int size, int *count_solutions, int index); 

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

void initialize_game_table(figure_t ***game_table, unvisited_node_t *unvisited_nodes){
	*game_table = malloc(6 * sizeof(figure_t *));
	int count = 0;
	for(int i = 0; i<6; i++){
		(*game_table)[i] = malloc(6*sizeof(figure_t));
		for(int j = 0; j < 6; j++){
			(*game_table)[i][j] = EMPTY;
			unvisited_nodes[count].x = i;
			unvisited_nodes[count].y = j;
			count++;
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

bool check_sequence(int x, int y, figure_t ***game_table) {
	int sequence_row_moon = 0;
	int sequence_col_moon = 0;
	int sequence_row_sun = 0;
	int sequence_col_sun = 0;

	for(int i = 0; i < 6; i++) {
		// MOON
		if((*game_table)[x][i] == MOON) {
			sequence_col_moon++;
			if(sequence_col_moon == 3) {
				return true;
				break;
			}
		} else {
			sequence_col_moon = 0;;
		}

		if((*game_table)[i][y] == MOON) {
			sequence_row_moon++;
			if(sequence_row_moon == 3) {
				return true;
				break;
			}
		} else {
			sequence_row_moon = 0;
		}

		// SUN
		if((*game_table)[x][i] == SUN) {
			sequence_col_sun++;
			if(sequence_col_sun == 3) {
				return true;
				break;
			}

		} else {
			sequence_col_sun = 0;
		}

		if((*game_table)[i][y] == SUN) {
			sequence_row_sun++;
			if(sequence_row_sun == 3) {
				return true;
				break;
			}
		} else {
			sequence_row_sun = 0;
		}
	}

	return false;
}

void shuffle(figure_t *choices) {
	figure_t rand_figure = (rand() % (SUN - MOON +1)) + MOON;
	figure_t opp_figure = opposite_figure(rand_figure);
	choices[0] = rand_figure;
	choices[1] = opp_figure;
}

bool solve(figure_t ***game_table, int x, int y) {
	if(y == 6) {
		return true;
	}

	figure_t choices[2] = {0,0};
	shuffle(choices);

	int next_x = (x == 5) ? 0 : x + 1;
	int next_y = (x < 5) ? y : y + 1;
	
	for(int i = 0; i < 2; i++) {
		if(is_safe(choices[i], x, y, game_table)) {
			(*game_table)[x][y] = choices[i];
			if(solve(game_table, next_x, next_y)) return true;
			(*game_table)[x][y] = EMPTY;
		}
	}
	return false;
}

void shuffle_unvisited_nodes(unvisited_node_t *unvisited_nodes, int length) {
	int swap_index;
	for(int i = 0; i < length; i++) {
		swap_index = rand() % length;
		// Swap x's
		unvisited_nodes[i].x = unvisited_nodes[i].x ^ unvisited_nodes[swap_index].x;
		unvisited_nodes[swap_index].x = unvisited_nodes[i].x ^ unvisited_nodes[swap_index].x;
		unvisited_nodes[i].x = unvisited_nodes[i].x ^ unvisited_nodes[swap_index].x;

		// Swap y's
		unvisited_nodes[i].y = unvisited_nodes[i].y ^ unvisited_nodes[swap_index].y;
		unvisited_nodes[swap_index].y = unvisited_nodes[i].y ^ unvisited_nodes[swap_index].y;
		unvisited_nodes[i].y = unvisited_nodes[i].y ^ unvisited_nodes[swap_index].y;
	}
}

bool check_gap(figure_t ***game_table, int x, int y, figure_t selected_figure) {
	//(*game_table)[x][y] = selected_figure;
	int count_selected_col = 0;
	int count_selected_row = 0;
	for(int z = 0; z < 6; z++){
		if((*game_table)[x][z] == selected_figure){
			count_selected_col++;
		}
		if((*game_table)[z][y] == selected_figure){
			count_selected_row++;
		}
	}
	
	int gap_count = 0;
	if(count_selected_col == 3) {
		for(int i = 0; i < 6; i++) {
			if((*game_table)[x][i] == selected_figure) {
				gap_count = 0;
			} else {
				gap_count++;
				if(gap_count == 3) {
					(*game_table)[x][i] = EMPTY;
					return true;
					break;
				}
			}
		}
	}

	if(count_selected_row == 3) {
		for(int i = 0; i < 6; i++) {
			if((*game_table)[i][y] == selected_figure) {
				gap_count = 0;
			} else {
				gap_count++;
				if(gap_count == 3) {
					(*game_table)[i][y] = EMPTY;
					return true;
					break;
				}
			}
		}
	
	}

	(*game_table)[x][y] = EMPTY;
	return false;
}


bool check_last_round(int x, int y, unvisited_node_t last_visited) {
	if(x == last_visited.x && y == last_visited.y) {
		printf("hola\n");
		return false;
	}
	return true;
}

bool check_forced_moves(figure_t ***game_table) {
	int forced_moves = 0;
	static unvisited_node_t last_visited = { .x = -1, .y = -1 };
	printf("Last move -> x: %d, y: %d\n", last_visited.x, last_visited.y);

	bool resolvable = false;
	int safe_count = 0;
	figure_t figures[] = { MOON, SUN };
	for(int i = 0; i < 6; i++){
		for(int j = 0; j < 6; j++) {
			for(int z = 0; z < 2; z++) {
				if((*game_table)[i][j] == EMPTY) {
					if(is_safe(figures[z], i, j, game_table) == 1) {
						safe_count++;
					}
					
					if(z == 1 && safe_count == 1) {
						printf("Forced move -> x: %d, y: %d, figure: %d\n", i, j, figures[z]);
						resolvable = true;
						last_visited.x = i;
						last_visited.y = j;
						forced_moves++;
						break;
					}
				}
			}
		}
	}

	return resolvable;
}


bool check_solutions(figure_t ***game_table, unvisited_node_t *visited_nodes, int size, int *count_solutions, int index) {
	figure_t choices[2] = {0,0};
	shuffle(choices);

	if(index == size) {
		(*count_solutions)++;
		if((*count_solutions) > 1) {
			return false;
		}
		return false;
	}

	for(int i = 0; i < 2; i++) {
		if(is_safe(choices[i], visited_nodes[index].x, visited_nodes[index].y, game_table)) {
			(*game_table)[visited_nodes[index].x][visited_nodes[index].y] = choices[i];
			if(!check_sequence(visited_nodes[index].x, visited_nodes[index].y, game_table)){
				if(check_solutions(game_table, visited_nodes, size, count_solutions, index + 1)) return true;
			}
			(*game_table)[visited_nodes[index].x][visited_nodes[index].y] = EMPTY;
		}
	}

	return false;
}

void dig(figure_t ***game_table, unvisited_node_t *unvisited_nodes, int size, unvisited_node_t *visited_nodes) {
	int visited_count = 0;
	int count_solutions = 0;

	for(int	i = 0; i < size; i++) {
		figure_t temp = (*game_table)[unvisited_nodes[i].x][unvisited_nodes[i].y];
		(*game_table)[unvisited_nodes[i].x][unvisited_nodes[i].y] = EMPTY;
		visited_nodes[i] = unvisited_nodes[i];
		for(int jx = 0; jx < size - (size - i); jx++){
			printf("visited_nodes: x->%d y->%d\n", visited_nodes[jx].x, visited_nodes[jx].y);
		}
		visited_count++;
		printf("Visited_count: %d\n", visited_count);
		count_solutions = 0;
		bool result = check_solutions(game_table, visited_nodes, visited_count, &count_solutions, 0);
		printf("Count: %d\n", count_solutions);
		if(count_solutions > 1 || count_solutions < 1) {
			//printf("FIgure: %d\n", temp);
			printf("coords: x: %d, y: %d\n", unvisited_nodes[i].x, unvisited_nodes[i].y);
			(*game_table)[unvisited_nodes[i].x][unvisited_nodes[i].y] = temp;
			//visited_count--;
		} 
	}
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

	srand(time(NULL));

	// We create the 2D Array
	figure_t **game_table;
	
	unvisited_node_t unvisited_nodes[36];
	unvisited_node_t *visited_nodes = malloc(36 * sizeof(unvisited_node_t));

	initialize_game_table(&game_table, unvisited_nodes);
	shuffle_unvisited_nodes(unvisited_nodes, 36);

	solve(&game_table, 0, 0);
	
	dig(&game_table, unvisited_nodes, 36, visited_nodes);
	free(visited_nodes);

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

					//printf("%d\n",is_safe(selected_figure, coord_to_table(event.button.x), coord_to_table(event.button.y), &game_table));
					game_table[coord_to_table(event.button.x)][coord_to_table(event.button.y)] = selected_figure;
				    //check_gap(&game_table, coord_to_table(event.button.x), coord_to_table(event.button.y), selected_figure);
                    int result = check_sequence(coord_to_table(event.button.x), coord_to_table(event.button.y), &game_table);
					printf("Result: %d\n", result);
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
