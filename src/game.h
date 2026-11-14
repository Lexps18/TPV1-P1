#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <array>
#include <istream>
#include <list>
#include <random>
#include <string>

// Declaraciones anticipadas
class Texture;
class SceneObject;
class Bow;
class Rectangle;
class InfoBar;

/**
 * Clase principal del juego.
 */
class Game
{
public:
	// Número de actualizaciones por segundo
	static constexpr int FRAME_RATE = 30;
	// Periodo entre actualizaciones en milisegundos
	static constexpr int FRAME_PERIOD = 1000 / FRAME_RATE;
	// Tamaño real de la ventana
	static constexpr int WINDOW_WIDTH = 800;
	static constexpr int WINDOW_HEIGHT = 600;

	enum TextureName
	{
		BOW = 0,
		ARROW,
		BALLOONS,
		BUTTERFLY,
		NUMBER,
		NUM_TEXTURES
	};

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::array<Texture*, NUM_TEXTURES> textures;
	std::list<SceneObject*> objects;		// todos los objetos de la escena
	Bow* bow;
	int arrowsInFlight;		// flechas disparadas que aún no han salido de la ventana
	int butterfliesAlive;	// mariposas vivas 
	std::mt19937_64 randomGenerator;
	float balloonTimer;		// milisegundos de juego desde el último globo
	int initialArrows;
	Uint64 balloonDelay;
	int balloonPoints;
	float balloonSpeed;
	InfoBar* infoBar;	// barra con flechas y puntuación
	int initialButterflies;
	int level;
	int goal;
	SDL_Color backgroundColor;
	int score;			// Puntuación jugador

	void render() const;
	void update(float delta);
	void handleEvents();

	void generateRandomBallon(float delta);
	void generateRandomButterfly(int numButterfly);
	// Lee las opciones del archivo de configuración
	void loadConfig(const std::string& filename);

	void renderHitboxes() const;
	void nextLevel();
	void saveState() const;
	void loadState();
	// Libera texturas y recursos de SDL (lo usan el constructor si falla y el destructor)
	void freeResources();

	// Crea el objeto de una línea del archivo de guardado según su letra
	SceneObject* loadObject(char type, std::istream& line, int lineNumber);

	bool exit;

public:
	Game();
	~Game();

	// Obtiene una textura por su nombre
	Texture* getTexture(TextureName name) const;

	//añadir una flecha
	void addObject(SceneObject* object);

	// Las flechas avisan al crearse y al destruirse
	void arrowCreated();
	void arrowDestroyed();

	// Las mariposas avisan al nacer y al morir, para saber cuántas quedan vivas
	void butterflyBorn();
	void butterflyDied();

	// Ejecuta el bucle principal del juego
	void run();

	// Comprueba si hay algún objeto colocado en ese rectángulo
	bool checkCollision(const Rectangle& rect, int combo);
	void givePoints(int combo);

	// Datos que muestra la barra de información
	int getScore() const;
	int getArrowsLeft() const;

	// Número real aleatorio entre min y max
	float getRandom(float min, float max);

	void clearObjects();
};

inline Texture*
Game::getTexture(TextureName name) const
{
	return textures[name];
}

#endif // GAME_H
