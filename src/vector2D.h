#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <concepts> //std::floating_point
#include <cmath> //std::sqrt
#include <iostream> //std::ostream, para operator<<


/**
 * Vector bidimensional genérico.
 */
template<std::floating_point T = float>
class Vector2D
{
	T x, y;

public:
	Vector2D(T x, T y) : x(x), y(y) { }
	Vector2D() : Vector2D(0, 0) { }

	// Coordenadas del vector
	T getX() const { return x; }
	T getY() const { return y; }

	// Operadores
	//suma y resta, devuelven un vector nuevo pero no toca ninguno de los dos
	Vector2D operator+(const Vector2D& otro) const {
		return {x + otro.x, y + otro.y};
	}
	Vector2D operator-(const Vector2D& otro) const {
		return { x - otro.x, y - otro.y };
	}

	// dos operator* distintos el programa elige uno segun el tipo de arguemnto
	Vector2D operator*(T k) const { // vector * numero
		return { x * k, y * k };
	}
	T operator*(const Vector2D& otro) const {
		return x * otro.x + y * otro.y; // producto escalar
	}

	//si modifica el vector asi q no es const
	//referencia a si mismo para que se comporte como un += de un int
	Vector2D& operator+=(const Vector2D& otro){
		x += otro.x;
		y += otro.y;
		return *this;
	}

	T length() const {
		return std::sqrt(x * x + y * y); //pitagoras
	}

	// Operadores de entrada/salida
	friend std::ostream& operator<<(std::ostream& out, const Vector2D& v) {
		return out << '{' << v.x << ", " << v.y << '}';
	}
};

//mismo tipo solo nombre diferente
//para un punto en lugar de direccion
template<std::floating_point T = float>
using Point2D = Vector2D<T>;

#endif // VECTOR2D_H
