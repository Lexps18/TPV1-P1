#pragma once

#include <string>
#include "gameError.h"

/*
	Error al inicializar o usar SDL. El mensaje incluye el que da SDL_GetError.
*/
class SDLError : public GameError
{
public:
	// context indica qué se estaba haciendo cuando falló
	SDLError(const std::string& context);
};
