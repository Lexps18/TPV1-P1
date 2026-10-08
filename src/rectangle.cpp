//
// Tecnología de la Programación de Videojuegos 1
// Facultad de Informática UCM
//
// Clase rectángulo independiente de la biblioteca gráfica
//

#include "rectangle.h"

#include <cassert>

using namespace std;
using scalar_t = Rectangle::scalar_t;

// Comprueba si un número está estrictamente entre otros dos
bool
between(scalar_t v, scalar_t lower, scalar_t upper)
{
	return v > lower && v < upper;
}

// Comprueba si los intervalos (x1, x1 + w1) y (x2, x2 + w2) solapan
bool
intervalOverlaps(scalar_t x1, scalar_t w1, scalar_t x2, scalar_t w2)
{
	return between(x1, x2, x2 + w2) || between(x2, x1, x1 + w1);
}

//
// Rectangle
//

Rectangle::Rectangle(Point2D<> topleft, scalar_t width, scalar_t height)
  : corner(topleft)
  , width(width)
  , height(height)
{
	assert(width >= 0);
	assert(height >= 0);
}

Rectangle::Rectangle()
  : Rectangle({0, 0}, 0, 0)
{
}

bool
Rectangle::empty() const
{
	return width == 0 && height == 0;
}

bool
Rectangle::isInside(Point2D<> point) const
{
	return between(point.getX(), corner.getX(), corner.getX() + width) &&
	       between(point.getY(), corner.getY(), corner.getY() + height);
}

bool
Rectangle::hasIntersection(const Rectangle& other) const
{
	return intervalOverlaps(corner.getX(), width, other.corner.getX(), other.width) &&
	       intervalOverlaps(corner.getY(), height, other.corner.getY(), other.height);
}

Rectangle
Rectangle::getIntersection(const Rectangle& other) const
{
	// Calcula los extremos más restrictivos de cada coordenada
	scalar_t x0 = max(corner.getX(), other.corner.getX());
	scalar_t y0 = max(corner.getY(), other.corner.getY());

	scalar_t x1 = min(corner.getX() + width, other.corner.getX() + other.width);
	scalar_t y1 = min(corner.getY() + height, other.corner.getY() + other.height);

	return {{x0, y0}, max(0.f, x1 - x0), max(0.f, y1 - y0)};
}

void
Rectangle::move(Vector2D<> v)
{
	corner += v;
}

Rectangle
Rectangle::moved(Vector2D<> v) const
{
	return {corner + v, width, height};
}

Rectangle
Rectangle::scaled(scalar_t sx, scalar_t sy) const
{
	return {corner + Vector2D<>{.5f * (1 - sx) * width, .5f * (1 - sy) * height}, sx * width, sy * height};
}

//
// SDL connection
//

#ifdef WITH_SDL

SDL_FRect
Rectangle::to_sdl() const
{
	return {
		corner.getX(),
		corner.getY(),
		width,
		height,
	};
}

#endif
