#pragma once

#include <string>
#include "gameError.h"

/*
	Error de formato al leer un archivo del juego (nivel o partida guardada).
	Guarda el archivo y la línea donde está el error.
 */
class FileFormatError : public GameError
{
private:
	std::string filename;
	int line;

public:
	FileFormatError(const std::string& filename, int line, const std::string& message);

	const std::string& getFilename() const;
	int getLine() const;
};
