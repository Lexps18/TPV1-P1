#pragma once

#include "vector2D.h"

class Texture;

class Balloon
{
public:
	static constexpr float SIZE = 64;

private:
	Texture* texture;
	Point2D<> position;
	Vector2D<> velocity;
	int color;

public:
	Balloon(Texture* texture, Point2D<> position, Vector2D<> velocity, int color);

	void render() const;
	bool update(float delta);
};

