#pragma once

#include <string>
#include "gameError.h"

/*
	Error al no encontrar un archivo que el juego intenta abrir.
*/
class FileNotFoundError : public GameError
{
public:
	FileNotFoundError(const std::string& filename);
};
