#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "./buffer.h"
#include "SDL.h"
#include "SDL_ttf.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
int char_width, char_height;

void scc(int code)
{
	if (code < 0) {
		fprintf(stderr, "SDL ERROR: %s \n", SDL_GetError());
		exit(1);
	}
}

void *scp(void *ptr) {
	if (ptr == NULL) {
		fprintf(stderr, "SDL ERROR: %s \n", SDL_GetError());
		exit(1);
	}
	return ptr;
}

void initSDL() {
	scc(SDL_Init(SDL_INIT_VIDEO));
	TTF_Init();
	
	window = scp(SDL_CreateWindow("Text Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN));
	SDL_MaximizeWindow(window);
	renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));
	
	font = TTF_OpenFont("NotoSansMono-Regular.ttf", 24);
}

void renderCharacter(char character, int x, int y,Uint32 color) {

	Uint8 r = (color >> 16) & 0xFF;
	Uint8 g = (color >> 8) & 0xFF;
	Uint8 b = (color) & 0xFF;
	SDL_Surface* surface = scp(TTF_RenderGlyph_Solid(font, character, (SDL_Color){r, g, b, 255}));
	SDL_Texture* texture = scp(SDL_CreateTextureFromSurface(renderer, surface));
	SDL_Rect destRect = {x, y, surface->w, surface->h};
	scc(SDL_RenderCopy(renderer, texture, NULL, &destRect));
	
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void renderText(const char* text, size_t text_size, int x, int y) {

	int current_x = x;
	for (size_t i = 0; i < text_size; ++i) {
		renderCharacter(text[i], current_x, y, 0xFFFFFF);
		int char_width;
		TTF_GlyphMetrics(font, (Uint16)text[i], NULL, NULL, NULL, NULL, &char_width);
		current_x += char_width;
	}
}

void closeSDL() {
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}

Editor editor = {0};

void renderCursor() {
	scc(TTF_SizeText(font, "A", &char_width, &char_height));
	SDL_Rect cursorRect = {
		.x = editor.cursor_col * char_width,
		.y = editor.cursor_row * char_height,
		.w = char_width,
		.h = char_height,
	};
	scc(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255));
	scc(SDL_RenderFillRect(renderer, &cursorRect));

	const char *c = editorCharUnderCursor(&editor);
	if (c) {
		renderCharacter(*c, editor.cursor_col * char_width, editor.cursor_row * char_height, 0x000000);
	}
}

int main(int argc, char **argv) {
	
	const char *file_path = NULL;
	
	if(argc > 1) {
		file_path = argv[1];
	}

	if (file_path) {
		editorLoadFromFile(&editor, file_path);
	}
	

	initSDL();

	SDL_Event e;
	int quit = 0;
	
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
				case SDL_QUIT: {
						quit = 1;
					}
					break;
				case SDL_KEYDOWN: {
						switch (e.key.keysym.sym) {
							case SDLK_DELETE: {
									editorDelete(&editor);
								}
							break;
							case SDLK_BACKSPACE: {
									editorBackspace(&editor);
								}
								break;
							case SDLK_LEFT: {
								if (editor.cursor_col > 0) 
								{
									editor.cursor_col -= 1;
								}
							}
							break;
							case SDLK_RIGHT: {
								editor.cursor_col += 1;
							}
							break;
							case SDLK_UP: {
								if (editor.cursor_row > 0) {
									editor.cursor_row -= 1;
								}
							}
							break;
							case SDLK_DOWN: {
									editor.cursor_row += 1;
							}
							break;
							case SDLK_RETURN: {
								editorInsertNewLine(&editor);
							}
							break;
							case SDLK_F2: {
								editorSaveToFile(&editor,"firstfile");
							}
							break;
						}
					}
					break;
				case SDL_TEXTINPUT: {
						editorInsert(&editor, e.text.text);
					}
				break;	
	
			}
		}
			
		scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255));
		scc(SDL_RenderClear(renderer));
		
		for (size_t row=0; row < editor.size; ++row) {
			renderText(editor.lines[row].chars, editor.lines[row].size, 0, row * char_height);
		}
		renderCursor();

		SDL_RenderPresent(renderer);
	}

	closeSDL();
	return 0;
}
