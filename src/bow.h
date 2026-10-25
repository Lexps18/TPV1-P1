#pragma once

#include <SDL3/SDL.h>   // SDL_Event
#include "vector2D.h"

class Texture;
class Game;

class Bow
{
public:
	static constexpr float WIDTH = 105;
	static constexpr float HEIGHT = 100;
	static constexpr float SPEED = 300;

private:
	Game* game;					//entregarle flechas que dipara
	Texture* texture;
	Point2D<> position;
	Vector2D<> direction;		// {0, -1} arriba, {0, 1} abajo, {0, 0} quieto
	int arrows;					//flechas que quedan
	bool loaded;

	void shoot(); //crea una flecha y se la da agame

public:
	Bow(Game* game, Texture* texture, Point2D<> position, int numArrows);

	void render() const;
	bool update(float delta);
	void handleEvent(const SDL_Event& event);
	void handleKeyUp(SDL_Keycode key);

	int getArrows() const;
};
