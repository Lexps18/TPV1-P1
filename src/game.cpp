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

// Evento propio que manda el callback de los diálogos al cerrarse
constexpr Uint32 DIALOG_EVENT = SDL_EVENT_USER;

// Tipos de archivo que ofrecen los diálogos de guardar y cargar
constexpr SDL_DialogFileFilter SAVE_FILTERS[] = {
	{ "Partidas guardadas (*.txt)", "txt" },
	{ "Todos los archivos", "*" }
};
constexpr int NUM_SAVE_FILTERS = 2;

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
	{"butterfly.png", 4, 10},
	{"digits.png", 1, 10}
};

Game::Game()
	: window(nullptr)
	, renderer(nullptr)
	, textures{}		// todas a nullptr
	, bow(nullptr)
	, arrowsInFlight(0)
	, butterfliesAlive(0)
	, randomGenerator(random_device()())
	, balloonTimer(0)
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
	, dialogOpen(false)
	, dialogType(SAVE_DIALOG)
{
	// Lee la configuración lo primero: si falla, aún no hay nada que liberar
	loadConfig(string(LEVEL_ROOT) + LEVEL_FILE);

	// Carga SDL y crea la ventana con su renderizador
	if (!SDL_Init(SDL_INIT_VIDEO))
		throw SDLError("SDL_Init");

	try {
		if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE,
		                                 WINDOW_WIDTH, WINDOW_HEIGHT,
		                                 0, &window, &renderer))
			throw SDLError("SDL_CreateWindowAndRenderer");

		// Carga las texturas al inicio
		string imgRoot = IMAGE_ROOT;

		for (size_t i = 0; i < textures.size(); i++) {
			auto [name, nrows, ncols] = textureList[i];
			textures[i] = new Texture(renderer, (imgRoot + name).c_str(), nrows, ncols);
		}
	}
	catch (...) {
		// Si falla el constructor, el destructor no se llamará
		freeResources();
		throw;
	}
	//arco
	bow = new Bow(this, getTexture(BOW), { BOW_X, ((WINDOW_HEIGHT - Bow::HEIGHT) / 2)}, initialArrows);
	objects.push_back(bow);

	//mariposas
	generateRandomButterfly(initialButterflies);

	//infobar
	infoBar = new InfoBar(this, getTexture(ARROW), getTexture(NUMBER), renderer);
}

Game::~Game()
{
	delete infoBar;

	for (SceneObject* object : objects)
		delete object;

	freeResources();
}

void
Game::freeResources()
{
	for (Texture* texture : textures)
		delete texture;

	if (renderer != nullptr)
		SDL_DestroyRenderer(renderer);
	if (window != nullptr)
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
Game::update(float delta)
{
	generateRandomBallon(delta);

	// El arco está en objects, así que ya se actualiza en este bucle
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
	// Si el nivel tenía mariposas y ya no queda ninguna viva, se pierde
	if (!exit && initialButterflies > 0 && butterfliesAlive == 0) {
		cout << "Has perdido: no quedan mariposas. Puntuacion: " << score << endl;
		exit = true;
	}

}

void
Game::run()
{
	Uint64 lastFrame = SDL_GetTicks();

	while (!exit) {
		// Momento en que empieza este ciclo
		Uint64 frameStart = SDL_GetTicks();

		// Delta: tiempo transcurrido desde el ciclo anterior (incluida la espera), en segundos
		float delta = (frameStart - lastFrame) / MS_PER_SECOND;
		lastFrame = frameStart;

		handleEvents();
		// Mientras hay un diálogo abierto el juego está en pausa: no se actualiza
		if (!dialogOpen)
			update(delta);
		render();

		// Si el ciclo ha tardado menos de FRAME_PERIOD, esperar el resto
		Uint64 frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < FRAME_PERIOD)
			SDL_Delay(Uint32(FRAME_PERIOD - frameTime));
	}
}

void
Game::handleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT)
			exit = true;
		else if (event.type == DIALOG_EVENT)
			handleDialogEvent();
		else if (dialogOpen)
			continue;	// en pausa: se ignora el teclado hasta que se cierre el diálogo
		else if (event.type == SDL_EVENT_KEY_DOWN && (event.key.mod & SDL_KMOD_CTRL))
		{
			// Ctrl + S guarda, Ctrl + O carga
			if (event.key.key == SDLK_S)
				openSaveDialog();
			else if (event.key.key == SDLK_O)
				openLoadDialog();
		}
		else
			bow->handleEvent(event);
	}

}

void
Game::openSaveDialog()
{
	dialogOpen = true;			// pausa el juego
	dialogType = SAVE_DIALOG;	// para saber luego que hay que guardar
	SDL_ShowSaveFileDialog(dialogCallback, this, window, SAVE_FILTERS, NUM_SAVE_FILTERS, nullptr);
}

void
Game::openLoadDialog()
{
	dialogOpen = true;			// pausa el juego
	dialogType = LOAD_DIALOG;	// para saber luego que hay que cargar
	SDL_ShowOpenFileDialog(dialogCallback, this, window, SAVE_FILTERS, NUM_SAVE_FILTERS, nullptr, false);
}

void SDLCALL
Game::dialogCallback(void* userdata, const char* const* filelist, int)
{
	// userdata es el "this" que pasamos al abrir el diálogo
	Game* game = static_cast<Game*>(userdata);

	// Apuntamos el archivo elegido ("" si el usuario canceló o hubo error)
	if (filelist != nullptr && filelist[0] != nullptr)
		game->dialogFile = filelist[0];
	else
		game->dialogFile = "";

	// Avisamos al bucle principal con un evento. No guardamos ni cargamos aquí
	// porque esta función puede ejecutarse en otro hilo a la vez que el juego.
	SDL_Event event{};
	event.type = DIALOG_EVENT;
	SDL_PushEvent(&event);
}

void
Game::handleDialogEvent()
{
	dialogOpen = false;	// se quita la pausa

	if (dialogFile.empty())	// el usuario canceló: no se hace nada
		return;

	if (dialogType == SAVE_DIALOG) {
		// Si el usuario no ha escrito la extensión, se añade
		if (!dialogFile.ends_with(".txt"))
			dialogFile += ".txt";
		saveState(dialogFile);
	}
	else
		loadState(dialogFile);
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
Game::butterflyBorn()
{
	butterfliesAlive++;
}

void
Game::butterflyDied()
{
	butterfliesAlive--;
}

void
Game::generateRandomBallon(float delta) {
	// Se cuenta tiempo de juego (delta), no tiempo real
	balloonTimer += delta * MS_PER_SECOND;
	if (balloonTimer < balloonDelay)
		return;

	balloonTimer -= balloonDelay;
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
Game::saveState(const string& filename) const {
	ofstream file(filename);
	if (!file)
		throw GameError("No se puede escribir el archivo " + filename);

	file << level << ' ' << score << '\n';	// cabecera: nivel y puntuación
	for (SceneObject* object : objects) {
		object->saveObject(file);
		file << '\n';
	}
	cout << "Partida guardada" << '\n';
}

void
Game::loadState(const string& filename) {
	ifstream file(filename);
	if (!file)
		throw FileNotFoundError(filename);

	string text;
	getline(file, text);
	istringstream header(text);
	int savedLevel = 0, savedScore = 0;
	if (!(header >> savedLevel >> savedScore))
		throw FileFormatError(filename, 1, "cabecera no valida");

	level = savedLevel;
	score = savedScore;
	goal = 0;
	loadConfig(string(LEVEL_ROOT) + to_string(level) + ".txt");

	// Borra todos los objetos actuales, arco incluido
	for (SceneObject* object : objects)
		delete object;

	objects.clear();
	bow = nullptr;

	int lineNumber = 1;
	while (getline(file, text)) {
		lineNumber++;
		istringstream line(text);	// se lee de la línea como si fuera un archivo

		char type;
		if (!(line >> type))		// línea vacía
			continue;

		objects.push_back(loadObject(type, line, filename, lineNumber));

		if (!line)	// no había un número donde tocaba
			throw FileFormatError(filename, lineNumber, "datos no validos");
	}

}

SceneObject*
Game::loadObject(char type, istream& line, const string& filename, int lineNumber)
{
	switch (type) {
	case 'W':
		if (bow != nullptr)
			throw FileFormatError(filename, lineNumber, "hay mas de un arco");
		bow = new Bow(this, line);
		return bow;
	case 'B':
		return new Balloon(this, balloonPoints, line);
	case 'A':
		return new Arrow(this, line);
	case 'F':
		return new Butterfly(this, line);
	default:
		throw FileFormatError(filename, lineNumber, "tipo de objeto desconocido: "s + type);
	}
}


