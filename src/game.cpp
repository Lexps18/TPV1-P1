#include "game.h"

#include <string>
#include <fstream>
#include <limits>

#include <SDL3_image/SDL_image.h>

#include "texture.h"
#include "balloon.h"
#include "bow.h"
#include "arrow.h"
#include "rectangle.h"
#include "InfoBar.h"

using namespace std;

// Constantes
constexpr const char* const WINDOW_TITLE = "Bow & Arrow 1.0";
constexpr const char* IMAGE_ROOT = "../assets/images/";
constexpr const char* LEVEL_ROOT = "../assets/levels/";
constexpr const char* LEVEL_FILE = "1.txt";

constexpr int POINTS_BALLOON = 10;
constexpr SDL_Color DEFAULT_BG_COLOR = { 0, 170, 85, SDL_ALPHA_OPAQUE };	
constexpr bool SHOW_HITBOXES = false;	// dibuja las cajas de colisión
constexpr float BOW_X = 10;			// distancia del arco al borde izquierdo
constexpr float BALLOON_MIN_X = 150;		// para que los globos no salgan encima del arco
constexpr float BALLOON_MIN_SPEED = 50;		// en píxeles por segundo
constexpr float BALLOON_MAX_SPEED = 200;
constexpr float MS_PER_SECOND = 1000.0f;	// para pasar de milisegundos a segundos

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
	, bow(nullptr)
	, randomGenerator(random_device()())
	, nextBallonTime(0)
	, initialArrows(0)
	, balloonDelay(0)
	, infoBar(nullptr)
	, balloonPoints(POINTS_BALLOON)
	, backgroundColor(DEFAULT_BG_COLOR)
	, score(0)
	, exit(false)
	
{
	// Lee la configuración lo primero: si falla, aún no hay nada que liberar
	loadConfig(string(LEVEL_ROOT) + LEVEL_FILE);

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
	//arco
	bow = new Bow(this, getTexture(BOW), { BOW_X, ((WINDOW_HEIGHT - Bow::HEIGHT) / 2)}, initialArrows);

	//infobar
	infoBar = new InfoBar(this, getTexture(ARROW), renderer);
}

Game::~Game()
{
	// Liberar los objetos libres
	delete bow;
	delete infoBar;

	for (Balloon* balloon : balloons) {
		delete balloon;
	}

	for (Arrow* arrow : arrows) {
		delete arrow;
	}

	// Liberar cada textura
	for (Texture* texture : textures)
		delete texture;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void
Game::render() const
{
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	SDL_RenderClear(renderer);

	bow->render();

	for (Arrow* arrow : arrows) {
		arrow->render();
	}

	for (Balloon* balloon : balloons) {
		balloon->render();
	}

	if (SHOW_HITBOXES)
		renderHitboxes();

	infoBar->render();

	SDL_RenderPresent(renderer);
}

void
Game::update()
{
	// Tiempo de un ciclo en segundos 
	constexpr float delta = FRAME_PERIOD / MS_PER_SECOND;

	generateRandomBallon();

	bow->update(delta);

	//recorremos con indice porque vamos a borrar los elementos
	for (size_t i = 0; i < arrows.size(); ) {
		if (arrows[i]->update(delta)) {
			++i; //sigue vivo, pasamos al siguiente
		}
		else {
			delete arrows[i]; // libera el globo
			arrows.erase(arrows.begin() + i); // quita su puntero del vector
			//sin ++i, el siguiente globo baja a la posicion i
		}
	}

	if (arrows.empty() && bow->getArrows() <= 0) {
		cout << "Se acabo, tu puntuacion es: " << score << endl;
		exit = true;
	}


	
	for (size_t i = 0; i < balloons.size(); ) {
		if (balloons[i]->update(delta)) {
			++i;
		}
		else {
			delete balloons[i]; 
			balloons.erase(balloons.begin() + i); 
			
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
			SDL_Delay(Uint32(FRAME_PERIOD - frameTime));
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

void  
Game::addArrow(Arrow* arrow) {
	arrows.push_back(arrow);
}

void
Game::generateRandomBallon() {

	if (SDL_GetTicks() < nextBallonTime)
		return;

	nextBallonTime = SDL_GetTicks() + balloonDelay;
	float pos_x = uniform_real_distribution<float>(BALLOON_MIN_X, WINDOW_WIDTH - Balloon::SIZE)(randomGenerator);
	float vel_y = uniform_real_distribution<float>(BALLOON_MIN_SPEED, BALLOON_MAX_SPEED)(randomGenerator);
	int color = uniform_int_distribution<int>(0, getTexture(BALLOONS)->getNumRows() - 1)(randomGenerator);

	balloons.push_back(new Balloon(getTexture(BALLOONS), Point2D<>(pos_x, WINDOW_HEIGHT), Vector2D<>(0, -vel_y), color));
}

bool
Game::checkCollision(const Rectangle& rect) {
	for (Balloon* balloon : balloons) {
		if (balloon->hit(rect)) {
			score += balloonPoints;
			cout << "PUNTOS: " << score << endl;
			return true;
		}
	}
	return false;
}

void
Game::renderHitboxes() const
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);	

	for (Arrow* arrow : arrows) {
		SDL_FRect rect = arrow->getTip().to_sdl();
		SDL_RenderRect(renderer, &rect);
	}

	for (Balloon* balloon : balloons) {
		SDL_FRect rect = balloon->getRect().to_sdl();
		SDL_RenderRect(renderer, &rect);
	}
}

void
Game::loadConfig(const string& filename) {
	ifstream file(filename);
	if (!file)
		throw "No se encontro el archivo de configuracion: "s + filename;

	string key;
	while(file >> key) {
		if (key == "arrows")
			file >> initialArrows;
		else if (key == "baldelay")
			file >> balloonDelay;
		else if (key == "balpoints")
			file >> balloonPoints;
		else if (key == "bgcolor") {
			int r = 0, g = 0, b = 0;	// leídos como int
			file >> r >> g >> b;
			backgroundColor = { Uint8(r), Uint8(g), Uint8(b), SDL_ALPHA_OPAQUE };
		}
		else
			file.ignore(numeric_limits<streamsize>::max(), '\n');

		if (!file)	// no había un número donde tocaba
			throw "Formato erroneo en "s + filename + " (opcion " + key + ")";
	}
}

int 
Game::getScore() const
{
	return score;
}

int 
Game::getArrowsLeft() const
{
	return bow->getArrows();
}
