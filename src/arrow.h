#pragma once

#include "vector2D.h"

class Game;
class Texture;
class Rectangle;

class Arrow
{
public:
	static constexpr float WIDTH = 80;
	static constexpr float SPEED = 150;
	static constexpr float TIP_WIDTH = 15;

private:
	Game* game; //consultar colisions
	Texture* texture;
	float width, height;
	Point2D<> position;
	Vector2D<> velocity;

public:
	Arrow(Game* game, Texture* texture, Point2D<> start, Vector2D<> velocity);
	void render() const;

	//devuelve false cuando ha salido por la derecha
	bool update(float delta);

	// Rectángulo de la punta, el único trozo que explota globos
	Rectangle getTip() const;
};

