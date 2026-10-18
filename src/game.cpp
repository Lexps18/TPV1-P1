#include "game.h"

#include <string>

#include <SDL3_image/SDL_image.h>

#include "texture.h"
#include "balloon.h"
#include "bow.h"
#include "arrow.h"

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
		// Si falla una textura, el destructor no se llamará
		for (Texture* texture : textures)
			delete texture;

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();

		throw;
	}

	// Configura que se pueden utilizar capas translúcidas
	// SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	//prueb de globos
	balloons.push_back(new Balloon(getTexture(BALLOONS), Point2D<>(200, WINDOW_HEIGHT), Vector2D<>(0, -100), 0));
	balloons.push_back(new Balloon(getTexture(BALLOONS), Point2D<>(500, WINDOW_HEIGHT), Vector2D<>(0, -200), 3));

	//arco
	bow = new Bow(this, getTexture(BOW), { 10, ((WINDOW_HEIGHT - Bow::HEIGHT) / 2)}, INITIAL_ARROWS);
}

Game::~Game()
{
	// Liberar los objetos libres
	delete bow;

	for (Balloon* balloon : balloons) {
		delete balloon;
	}

	for (Arrow* arrow : arrows) {
		delete arrow;
	}

	// Liberar cada textura
	for (int i = 0; i < textures.size(); i++) {
		delete textures[i];
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

	bow->render();

	for (Arrow* arrow : arrows) {
		arrow->render();
	}

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

	for (size_t i = 0; i < arrows.size(); ) {
		if (arrows[i]->update(delta)) {
			++i; //sigue vivo, pasamos al siguiente
		}
		else {
			delete arrows[i]; //1.libera el globo
			arrows.erase(arrows.begin() + i); //2.quita su puntero del vector
			//sin ++i, el siguiente globo baja a la posicion i
		}
	}

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

		handleEvents();
		update(); 
		render(); 

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

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT)
			exit = true;

		bow->handleEvent(event); 
	}
}

void Game::addArrow(Arrow* arrow) {
	arrows.push_back(arrow);
}

// bool
// Game::checkCollision(const Rectangle& rect)
// {
//	// TODO
//	return false;
// }
