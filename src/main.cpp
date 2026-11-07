//
// Tecnología de la Programación de Videojuegos 1
// Facultad de Informática UCM
//
// Plantilla de proyecto con SDL
//
#include <iostream>
#include <string>

#include "game.h"
#include "gameError.h"

using namespace std;

int main(int argc, char* argv[])
{
	try {
		Game().run();
	}
	catch (const GameError& error) {	// todos los errores del juego son string
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", error.what(), nullptr);
	}

	return 0;
}
