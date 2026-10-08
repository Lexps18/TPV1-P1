//
// Tecnología de la Programación de Videojuegos 1
// Facultad de Informática UCM
//
// Clase rectángulo independiente de la biblioteca gráfica
//

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "vector2D.h"

// Incluye soporte para SDL solo si está disponible
#if __has_include(<SDL3/SDL_rect.h>)
	#include <SDL3/SDL_rect.h>
	#define WITH_SDL
#endif

/*
 * Este archivo depende del tipo Vector2D y requiere que este implemente los siguientes métodos:
 *
 * - Un constructor Vector2D(T, T) para construir un vector dadas sus coordenadas
 * - T getX() const y T getY() const para obtener las coordenadas
 * - Vector2D operator+(const Vector2D&) const para la suma de vectores
 * - Vector2D operator-(const Vector2D&) const para la resta de vectores
 * - Vector2D& operator+=(const Vector2D&) const para la suma de un vector sobre otro
 */

/// Rectángulo alineado con los ejes coordenados
class Rectangle
{
public:
	// Tipo de los escalares que usa el vector (cualquiera que sea)
	using scalar_t = decltype(std::declval<Point2D<>>().getX());

private:
	Point2D<> corner;
	scalar_t width;
	scalar_t height;

public:
	/// Crea un rectángulo a partir de su esquina superior izquierda y sus dimensiones
	Rectangle(Point2D<> topleft, scalar_t width, scalar_t height);
	/// Crea un rectángulo degenerado en el origen
	Rectangle();

	/// Obtiene la anchura del rectángulo
	scalar_t getWidth() const;
	/// Obtiene la altura del rectángulo
	scalar_t getHeight() const;
	/// Obtiene la esquina superior izquierda
	Point2D<> getTopLeftCorner() const;

	/// Si el rectángulo no tiene área
	bool empty() const;
	/// Comprueba si un punto está dentro del rectángulo
	bool isInside(Point2D<> point) const;
	/// Comprueba si el rectángulo interseca con otro rectángulo
	bool hasIntersection(const Rectangle& other) const;
	/// Obtiene la intersección entre dos rectángulos
	Rectangle getIntersection(const Rectangle& other) const;

	/// Devuelve un rectángulo movido en la dirección del vector
	Rectangle moved(Vector2D<> v) const;
	/// Devuelve un rectángulo escalado que mantiene su centro
	Rectangle scaled(scalar_t sx, scalar_t sy) const;
	/// Mueve el rectángulo en la dirección del vector
	void move(Vector2D<> v);

// Conexión con la SDL
#ifdef WITH_SDL
	SDL_FRect to_sdl() const;
#endif
};

inline Rectangle::scalar_t
Rectangle::getWidth() const
{
	return width;
}

inline Rectangle::scalar_t
Rectangle::getHeight() const
{
	return height;
}

inline Point2D<>
Rectangle::getTopLeftCorner() const
{
	return corner;
}

#endif // RECTANGLE_H
