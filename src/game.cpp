#include "game.h"

#include <string>

#include <SDL3_image/SDL_image.h>

#include "texture.h"
#include "balloon.h"
#include "bow.h"

using namespace std;

// Constantes
constexpr const char* const WINDOW_TITLE = "Bow & Arrow 1.0";
constexpr const char* IMAGE_ROOT = "../assets/images/";
constexpr const char* LEVEL_ROOT = "../assets/levels/";

constexpr int INITIAL_ARROWS = 10;

// Estructura para especificar las texturas que hay que
// cargar y el tamaño de su matriz de frames
struct TextureSpec
{
	const char* name;
	int nrows = 1;
	int ncols = 1;
};

constexpr array<TextureSpec, Game::NUM_TEXTURES> textureList{
	TextureSpec{"bow.png", 1, 2},
	{"arrow.png"},
	{"balloons.png", 7, 6},
};

Game::Game()
  : window(nullptr)
	, renderer(nullptr)
	, exit(false)
	, bow(nullptr)
{
	// Carga SDL y crea la ventana con su renderizador
	if (!SDL_Init(SDL_INIT_VIDEO))
		throw "SDL_Init: "s + SDL_GetError();

	if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE,
	                                 WINDOW_WIDTH, WINDOW_HEIGHT,
	                                 0, &window, &renderer))
		throw "window or renderer: "s + SDL_GetError();

	// Carga las texturas al inicio
	try {
		textures.fill(nullptr);
		string imgRoot = IMAGE_ROOT;

		for (size_t i = 0; i < textures.size(); i++) {
			auto [name, nrows, ncols] = textureList[i];
			textures[i] = new Texture(renderer, (imgRoot + name).c_str(), nrows, ncols);
		}
	}
	catch (...) {
		// TODO: liberar la memoria reservada porque el destructor no se llamara
		freeResources(); 
		throw; // relanza la excepción para que llegue al main
	}

	// Configura que se pueden utilizar capas translúcidas
	// SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// TODO: crear los objetos del juego
	//prueba: globos puestos a mano
	balloons.push_back(new Balloon(getTexture(BALLOONS), Point2D<>(200, WINDOW_HEIGHT), Vector2D<>(0, -100), 0));
	balloons.push_back(new Balloon(getTexture(BALLOONS), Point2D<>(500, WINDOW_HEIGHT), Vector2D<>(0, -200), 3));

	//arco
	bow = new Bow(getTexture(BOW), INITIAL_ARROWS);
}

Game::~Game()
{
	// TODO: liberar la memoria reservada por la clase
	freeResources();
}

void Game::freeResources() 
{
	//primero los objetos del juego
	for (Balloon* balloon : balloons) {
		delete balloon;
	}
	balloons.clear();

	delete bow;
	bow = nullptr;

	//delete de bullptr no hace nada, por eso el textures.fill(nullptr) del constructor:
	//asi las texturas que no llegaron a cargarse se pueden "borrar" sin problema
	for (Texture* texture : textures) {
		delete texture;
	}

	SDL_DestroyRenderer(renderer); 
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void
Game::render() const
{
	SDL_SetRenderDrawColor(renderer, 0, 170, 85, 255);    // fondo verde

	SDL_RenderClear(renderer);

	// TODO
	bow->render();

	for (Balloon* balloon : balloons) {
		balloon->render();
	}

	SDL_RenderPresent(renderer);
}

void
Game::update()
{
	// Tiempo de un ciclo en segundos (los objetos se mueven a velocidad x delta)
	constexpr float delta = FRAME_PERIOD / 1000.0f;

	bow->update(delta);

	//recorremos con indice (no con for-each) porque vamos a borrar los elementos
	for (size_t i = 0; i < balloons.size(); ) {
		if (balloons[i]->update(delta)) {
			++i; //sigue vivo, pasamos al siguiente
		}
		else {
			delete balloons[i]; //1.libera el globo
			balloons.erase(balloons.begin() + i); //2.quita su puntero del vector
			//sin ++i, el siguiente globo baja a la posicion i
		}
	}
}

void
Game::run()
{
	while (!exit) {
		//momento en que empieza este ciclo
		Uint64 frameStart = SDL_GetTicks();

		handleEvents(); //1. leer teclado/raton/cerrar ventana
		update(); //2. mover los objetos
		render(); //3. pintarlos

		//si el ciclo ha tardado menos de Frame_Period, esperar
		Uint64 frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < FRAME_PERIOD) {
			SDL_Delay(static_cast<Uint32>(FRAME_PERIOD - frameTime));
		}
	}
}

void
Game::handleEvents()
{
	SDL_Event event;

	// Only quit is handled directly, everything else is delegated
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT)
			exit = true;

		// TODO
		bow->handleEvent(event); 
	}
}

// bool
// Game::checkCollision(const Rectangle& rect)
// {
//	// TODO
//	return false;
// }
