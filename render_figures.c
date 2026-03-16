#include "render_figures.h"
#include <SDL2/SDL.h>

void SDL_RenderDrawFigure(SDL_Renderer *prenderer, SDL_Texture *ptexture, int x, int y) {
	SDL_Rect figure_rect;
	figure_rect.w = 110;
	figure_rect.h = 110;
	figure_rect.x = x;
	figure_rect.y = y;

	SDL_RenderCopy(prenderer, ptexture, NULL, &figure_rect);
}
