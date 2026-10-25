#pragma once

#include <SDL3/SDL.h>

class Game;
class Texture;

class InfoBar
{
private:
	const Game* game;		// para sacar la puntuación y flechas
	Texture* arrowTexture;	// para dibujar las flechas
	SDL_Renderer* renderer;	// para escribir texto

public:
	InfoBar(const Game* game, Texture* arrowTexture, SDL_Renderer* renderer);

	void render() const;
};

