#pragma once

#include "vector2D.h"

class Rectangle;
class Texture;

class Balloon
{
public:
	static constexpr float SIZE = 64;
	static constexpr float EXPLOSION_FRAME_TIME = 0.08f;

private:
	Texture* texture;
	Point2D<> position;
	Vector2D<> velocity;
	int color;
	int frame;
	float explosionTime;

public:
	Balloon(Texture* texture, Point2D<> position, Vector2D<> velocity, int color);

	void render() const;
	bool update(float delta);
	bool hit(const Rectangle&);

	// Rectángulo que ocupa el globo
	Rectangle getRect() const;
};

