#include "fileNotFoundError.h"

FileNotFoundError::FileNotFoundError(const std::string& filename)
	: GameError("No se encuentra el archivo " + filename)
{}
