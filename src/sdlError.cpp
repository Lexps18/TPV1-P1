#include "sdlError.h"

#include <SDL3/SDL.h>

SDLError::SDLError(const std::string& context)
	: GameError(context + ": " + SDL_GetError())
{}
