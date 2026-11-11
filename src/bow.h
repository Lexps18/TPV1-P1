#pragma once

#include <SDL3/SDL.h>   // SDL_Event
#include "vector2D.h"
#include "sceneObject.h"

class Texture;
class Game;

class Bow : public SceneObject
{
public:
	static constexpr float WIDTH = 105;
	static constexpr float HEIGHT = 100;
	static constexpr float SPEED = 300;

private:
	Vector2D<> direction;		// {0, -1} arriba, {0, 1} abajo, {0, 0} quieto
	int arrows;					//flechas que quedan
	bool loaded;

	void shoot(); //crea una flecha y se la da agame

public:
	Bow(Game* game, Texture* texture, Point2D<> position, int numArrows);
	Bow(Game* game, std::istream& line);

	void render() const override;
	bool update(float delta) override;
	void handleEvent(const SDL_Event& event);
	void handleKeyUp(SDL_Keycode key);

	int getArrows() const;
	void setArrows(int numArrows);

	void saveObject(std::ostream& out) const override;
};
