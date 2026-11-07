#pragma once
#include <stdexcept>

/*
	Superclase de todos los errores del juego. No añade nada a logic_error,
	pero permite capturar solo los errores propios del juego con un catch.
*/

class GameError : public std::logic_error {
public:
	using std::logic_error::logic_error;
};
