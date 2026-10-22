#include "bow.h"
#include <iostream>
#include "texture.h"
#include "game.h" 
#include "arrow.h"

Bow::Bow(Game* game, Texture* texture, Point2D<> position, int numArrows)
	: game(game), texture(texture), position(position),	direction (0, 0), arrows(numArrows), loaded(false)
{
}

void 
Bow::render() const {
	SDL_FRect target{ position.getX(), position.getY(), WIDTH, HEIGHT };
	int frame;
	if (loaded)
		frame = 0;   // arco con flecha
	else
		frame = 1;   // arco sin flecha
	texture->renderFrame(target, 0, frame);
}

bool 
Bow::update(float delta) {
	position += direction * (SPEED * delta);

	if (position.getY() < 0) {
		position = { position.getX(), 0 };
	}
	else if (position.getY() + HEIGHT > Game::WINDOW_HEIGHT) {
		position = Point2D<>(position.getX(), Game::WINDOW_HEIGHT - HEIGHT);
	}

	return true; //el arco nunca se elimina durante la partida
}

void 
Bow::handleEvent(const SDL_Event& event) 
{
	if (event.type == SDL_EVENT_KEY_DOWN) {
		switch (event.key.key) {
		case SDLK_UP:
			direction = { 0, -1 };		// dirección arriba
			break;
		case SDLK_DOWN:
			direction = { 0, 1 };		// dirección abajo
			break;
		case SDLK_LEFT:
			if (arrows > 0)
				loaded = true;			// cargar, solo si quedan flechas
			break;
		case SDLK_RIGHT:
			if (loaded) {
				loaded = false;
				arrows--;
				shoot();
				std::cout << "Numero de flechas: " << arrows << "\n";
			}
			break;
		}
	}
	else if (event.type == SDL_EVENT_KEY_UP)
		handleKeyUp(event.key.key);		// soltar una tecla va en su propio método
}

// Al soltar arriba o abajo, sigue hacia la otra si está pulsada, o se para
void
Bow::handleKeyUp(SDL_Keycode key)
{
	if (key != SDLK_UP && key != SDLK_DOWN)
		return;	

	// cogemos las teclas pulsadas actualmente
	const bool* keys = SDL_GetKeyboardState(nullptr);
	if (keys[SDL_SCANCODE_UP])
		direction = { 0, -1 };
	else if (keys[SDL_SCANCODE_DOWN])
		direction = { 0, 1 };
	else
		direction = { 0, 0 };
}

void 
Bow::shoot() {
	Point2D<> start(position.getX() + (WIDTH / 2), position.getY() + (HEIGHT / 2));
	game->addArrow(new Arrow(game, game->getTexture(Game::ARROW), start, Vector2D<>(Arrow::SPEED, 0)));
}

int
Bow::getArrows() {
	return arrows;
}
