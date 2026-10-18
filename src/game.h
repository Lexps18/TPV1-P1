#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <array>
#include <istream>
#include <vector>

// #include "rectangle.h"

// Declaraciones anticipadas
class Texture;
class Balloon;
class Bow;
class Arrow;

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
		NUM_TEXTURES
	};

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::array<Texture*, NUM_TEXTURES> textures;
	std::vector<Balloon*> balloons;
	Bow* bow;
	std::vector<Arrow*> arrows;

	void render() const;
	void update();
	void handleEvents();

	bool exit;

public:
	Game();
	~Game();

	// Obtiene una textura por su nombre
	Texture* getTexture(TextureName name) const;

	//añadir una flecha
	void addArrow(Arrow* arrow);

	// Ejecuta el bucle principal del juego
	void run();

	// Comprueba si hay algún objeto colocado en ese rectángulo
	// bool checkCollision(const Rectangle& rect);
};

inline Texture*
Game::getTexture(TextureName name) const
{
	return textures[name];
}

#endif // GAME_H
