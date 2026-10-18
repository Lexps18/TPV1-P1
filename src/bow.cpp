#include "bow.h"

#include <iostream>
#include "texture.h"
#include "game.h"  // para Game::WINDOW_HEIGHT
#include "arrow.h"

using namespace std;

//velocidad vertical del arco
constexpr float BOW_SPEED = 300.0f;

constexpr float BOW_HEIGHT = 130.0f;

// Velocidad con la que sale la flecha
constexpr float ARROW_SPEED = 500.0f;

Bow::Bow(Game* game, Texture* texture, int numArrows): 
	game(game),
	texture(texture),
	width(BOW_HEIGHT* texture->getFrameWidth() / texture->getFrameHeight()),
	height(BOW_HEIGHT),
	position(0, (Game::WINDOW_HEIGHT - height) / 2), //pegado a la izquierda, centrado
	direction(0),
	arrows(numArrows),
	loaded(false)
{ }

void Bow::render() const {
	SDL_FRect dest{ position.getX(), position.getY(), width, height };

	texture->renderFrame(dest, 0, loaded ? 0 : 1);
}

bool Bow::update(float delta) {
	//mover segun direccion dejada por handleEvent
	position += Vector2D<>(0, direction * BOW_SPEED * delta);

	//no dejar que sw salogo ni por arriba nipor abajo
	if (position.getY() < 0) {
		position = Point2D<>(position.getX(), 0);
	}
	else if (position.getY() + height > Game::WINDOW_HEIGHT) {
		position = Point2D<>(position.getX(), Game::WINDOW_HEIGHT - height);
	}

	return true; //el arco nunca se elimina durante la partida
}

void Bow::handleEvent(const SDL_Event& event){
	if (event.type == SDL_EVENT_KEY_DOWN) {
		onKeyDown(event.key.key);
	}
	else if (event.type == SDL_EVENT_KEY_UP) {
		onKeyUp(event.key.key);
	}
}

void Bow::onKeyDown(SDL_Keycode key) {
	switch (key) {
	case SDLK_UP: direction = -1; break;
	case SDLK_DOWN: direction = 1; break;

	case SDLK_LEFT: //cargar solo si no esta catgado ya y si quedan flechas
		if (!loaded && arrows > 0) {
			loaded = true;
		}
		break;

	case SDLK_RIGHT: //disparar solo si esta cargado
		if (loaded) {
			loaded = false;
			--arrows;
			cout << "Flechas restantes: " << arrows << '\n';
			shoot();
		}
		break;
	}
}


void Bow::onKeyUp(SDL_Keycode key) {
	//solo nos paramos si se suelta la tecla en la direccion actual
	if ((key == SDLK_UP && direction == -1) || (key == SDLK_DOWN && direction == 1)) {
		direction = 0;
	}
}

//void Bow::shoot() {
//
//}
