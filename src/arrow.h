#ifndef ARROW_H
#define ARROW_H

#include "vector2D.h"

class Game;
class Texture;

class Arrow
{
	Game* game; //consultar colisions
	Texture* texture;
	float width, height;
	Point2D<> position;
	Vector2D<> velocity;

public:
	//start es el punto donde debe quedar el centro vertical del extremo izquierdo de la flecha
	Arrow(Game* game, Texture* texture, Point2D<> start, Vector2D<> velocity);

	void render() const;

	//devuelve false cuando ha salido por la derecha
	bool update(float delta);
};

#endif // ARROW_H
