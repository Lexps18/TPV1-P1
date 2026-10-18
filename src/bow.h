#ifndef BOW_H
#define BOW_H

#include <SDL3/SDL.h>   // SDL_Event
#include "vector2D.h"

class Texture;
class Game;

class Bow
{
	Game* game; //entregarle flechas que dipara
	Texture* texture;
	float width, height;
	Point2D<> position;
	int direction; //-1 sube, 0 quieto, 1 baja
	int arrows; //flechas que quedan
	bool loaded;

	//separan el manejo de pulsar y soltar para que handleEvents no sea enorme
	void onKeyDown(SDL_Keycode key);
	void onKeyUp(SDL_Keycode key);

	void shoot(); //crea una flecha y se la da agame


public:
	Bow(Game* game, Texture* texture, int numArrows);

	void render() const;
	bool update(float delta);
	void handleEvent(const SDL_Event& event);
};

#endif // BOW_H
