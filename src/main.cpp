//
// Tecnología de la Programación de Videojuegos 1
// Facultad de Informática UCM
//
// Plantilla de proyecto con SDL
//
#include <iostream>
#include <string>

#include "game.h"

using namespace std;

int main(int argc, char* argv[])
{
	try {
		Game().run();
	}
	catch (const string& error) {	// todos los errores del juego son string
		cerr << "Error: " << error << endl;
	}

	return 0;
}
