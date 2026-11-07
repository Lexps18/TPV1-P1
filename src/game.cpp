#include "game.h"

#include <string>
#include <fstream>
#include <limits>
#include <sstream>

#include <SDL3_image/SDL_image.h>

#include "texture.h"
#include "balloon.h"
#include "bow.h"
#include "arrow.h"
#include "rectangle.h"
#include "InfoBar.h"
#include "butterfly.h"

#include "sdlError.h"
#include "fileNotFoundError.h"
#include "fileFormatError.h"

using namespace std;

// Constantes
constexpr const char* const WINDOW_TITLE = "Bow & Arrow 1.0";
constexpr const char* IMAGE_ROOT = "../assets/images/";
constexpr const char* LEVEL_ROOT = "../assets/levels/";
constexpr const char* LEVEL_FILE = "1.txt";
constexpr const char* SAVE_FILE = "saved.txt";

constexpr SDL_Color DEFAULT_BG_COLOR = { 0, 170, 85, SDL_ALPHA_OPAQUE };	
constexpr float BOW_X = 10;			// distancia del arco al borde izquierdo
constexpr float BALLOON_MIN_X = 150;		// para que los globos no salgan encima del arco
constexpr float DEFAULT_BALLOON_SPEED = 200;	// si el nivel no trae balspeed
constexpr float BALLOON_SPEED_VARIATION = 0.5f;
constexpr float MS_PER_SECOND = 1000.0f;	// para pasar de milisegundos a segundos
constexpr float BUTTERFLY_MIN_X = 150;	

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
	{"butterfly.png", 4, 10}	
};

Game::Game()
	: window(nullptr)
	, renderer(nullptr)
	, bow(nullptr)
	, arrowsInFlight(0)
	, randomGenerator(random_device()())
	, nextBallonTime(0)
	, initialArrows(0)
	, balloonDelay(0)
	, balloonPoints(0)
	, balloonSpeed(DEFAULT_BALLOON_SPEED)
	, infoBar(nullptr)
	, initialButterflies(0)
	, level(1)
	, goal(0)
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
		throw SDLError("SDL_CreateWindowAndRenderer");

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
	objects.push_back(bow);

	//mariposas
	generateRandomButterfly(initialButterflies);

	//infobar
	infoBar = new InfoBar(this, getTexture(ARROW), renderer);
}

Game::~Game()
{
	delete infoBar;

	for (SceneObject* object : objects)
		delete object;

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

	for (SceneObject* object : objects)
		object->render();

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

	for (auto it = objects.begin(); it != objects.end(); ) {
		if ((*it)->update(delta)) {
			++it;
		}
		else {
			delete* it;
			it = objects.erase(it);
		}
	}

	if (score >= goal && goal != 0) {
		nextLevel();
	}

	if (!exit && bow->getArrows() <= 0 && arrowsInFlight == 0) {
		cout << "Se acabo, tu puntuacion es: " << score << endl;
		exit = true;
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
Game::addObject(SceneObject* object) {
	objects.push_back(object);
}

void
Game::arrowCreated()
{
	arrowsInFlight++;
}

void
Game::arrowDestroyed()
{
	arrowsInFlight--;
}

void
Game::generateRandomBallon() {

	if (SDL_GetTicks() < nextBallonTime)
		return;

	nextBallonTime = SDL_GetTicks() + balloonDelay;
	float pos_x = uniform_real_distribution<float>(BALLOON_MIN_X, WINDOW_WIDTH - Balloon::SIZE)(randomGenerator);
	float vel_y = uniform_real_distribution<float>(balloonSpeed * (1 - BALLOON_SPEED_VARIATION), balloonSpeed * (1 + BALLOON_SPEED_VARIATION))(randomGenerator);
	int color = uniform_int_distribution<int>(0, getTexture(BALLOONS)->getNumRows() - 1)(randomGenerator);

	objects.push_back(new Balloon(this, getTexture(BALLOONS), Point2D<>(pos_x, WINDOW_HEIGHT), Vector2D<>(0, -vel_y), color, balloonPoints));
}

void
Game::generateRandomButterfly(int numButterfly) {
	for (int i = 0; i < numButterfly; i++) {
		float pos_x = uniform_real_distribution<float>(BUTTERFLY_MIN_X, WINDOW_WIDTH - Butterfly::SIZE)(randomGenerator);
		float pos_y = uniform_real_distribution<float>(0, WINDOW_HEIGHT - Butterfly::SIZE)(randomGenerator);

		objects.push_back(new Butterfly(this, getTexture(BUTTERFLY), Point2D<>(pos_x, pos_y), Vector2D<>(0, 0)));
	}
}

bool
Game::checkCollision(const Rectangle& rect, int combo) {
	for (SceneObject* object : objects) {
		if (object->hit(rect)) {
			score += object->getPoints(combo);
			cout << "PUNTOS: " << score << endl;
			return true;
		}
	}
	return false;
}

/*
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
*/

void
Game::loadConfig(const string& filename)
{
	ifstream file(filename);
	if (!file)
		throw FileNotFoundError(filename);

	string text;
	int lineNumber = 0;
	while (getline(file, text)) {
		lineNumber++;
		istringstream line(text);	// se lee de la línea como si fuera un archivo

		string key;
		if (!(line >> key))		// línea vacía
			continue;

		if (key == "arrows")
			line >> initialArrows;
		else if (key == "baldelay")
			line >> balloonDelay;
		else if (key == "balpoints")
			line >> balloonPoints;
		else if (key == "goal")
			line >> goal;
		else if (key == "balspeed") {
			line >> balloonSpeed;
			balloonSpeed *= MS_PER_SECOND;	// el archivo lo da en píxeles por milisegundo
			if (line && balloonSpeed <= 0)
				throw FileFormatError(filename, lineNumber, "balspeed tiene que ser mayor que 0");
		}
		else if (key == "butterflies")
			line >> initialButterflies;
		else if (key == "bgcolor") {
			int r = 0, g = 0, b = 0;	// leídos como int, un Uint8 se leería como carácter
			line >> r >> g >> b;
			if (line && (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255))
				throw FileFormatError(filename, lineNumber, "componente de color fuera de 0-255");
			backgroundColor = { Uint8(r), Uint8(g), Uint8(b), SDL_ALPHA_OPAQUE };
		}

		if (!line)	// no había un número donde tocaba
			throw FileFormatError(filename, lineNumber, "valor no valido para " + key);
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

float
Game::getRandom(float min, float max)
{
	return uniform_real_distribution<float>(min, max)(randomGenerator);
}

void
Game::clearObjects() {
	for (SceneObject* object : objects)
		if(object != bow)
			delete object;

	objects.clear();
	objects.push_back(bow);
}

void
Game::nextLevel() {
	string filename = string(LEVEL_ROOT) + to_string(level + 1) + ".txt";

	// Si no existe el siguiente nivel, se acaba la partida con victoria
	if (!ifstream(filename)) {
		cout << "Has ganado! Puntuacion final: " << score << endl;
		exit = true;
		return;
	}

	level++;
	goal = 0;	// si el nivel no trae goal, nunca se pasa de él
	loadConfig(filename);
	clearObjects();
	bow->setArrows(initialArrows);
	generateRandomButterfly(initialButterflies);
}

void
Game::saveState() const {
	ofstream file(SAVE_FILE);
	if (!file)
		throw GameError("No se puede escribir el archivo "s + SAVE_FILE);

	file << level << ' ' << score << '\n';	// cabecera: nivel y puntuación
	file << "# Balloons" << '\n';		// '\n' cambia de línea


}
