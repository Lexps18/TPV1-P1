#ifndef BALLOON_H
#define BALLOON_H

#include "vector2D.h"

//declaracion anticipada: como solo usamos Texture*, un puntero, basta con decirle 
// al compilador que la clase existe. El #include completo va en el .cpp.
// Así, si cambias texture.h, no se recompila todo lo que incluya balloon.h
class Texture;

class Balloon
{
	Point2D<> position; //esquina superior izquierda
	float width, height; //tamaño pantalla
	int color;
	Vector2D<> velocity;
	Texture* texture; //No es dueño, la textura la borra Game

public:
	Balloon(Texture* texture, Point2D<> position, Vector2D<> velocity, int color);

	void render() const;

	//mueve globo, devuelve false cuando sale de la pantalla, hay q borralo
	bool update(float delta);
};

#endif // BALLOON_H

