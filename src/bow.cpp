#include "bow.h"
#include <iostream>
#include "texture.h"
#include "game.h" 
#include "arrow.h"

Bow::Bow(Game* game, Texture* texture, Point2D<> position, int numArrows)
	: SceneObject(game, texture, position, WIDTH, HEIGHT), direction (0, 0), arrows(numArrows), loaded(false)
{
}

Bow::Bow(Game* game, std::istream& line)
	:SceneObject(game, game->getTexture(Game::BOW), line)
	, direction(0, 0)
	, arrows(0)
	, loaded(false)
{
	line >> arrows >> loaded;
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
	// cogemos las teclas pulsadas actualmente
	const bool* keys = SDL_GetKeyboardState(nullptr);
	if (keys[SDL_SCANCODE_UP])
		direction = { 0, -1 };
	else if (keys[SDL_SCANCODE_DOWN])
		direction = { 0, 1 };
	else
		direction = { 0, 0 };

	if (event.type == SDL_EVENT_KEY_DOWN) {
		switch (event.key.key) {
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
}

void 
Bow::shoot() {
	Point2D<> start(position.getX() + (WIDTH / 2), position.getY() + (HEIGHT / 2));
	game->addObject(new Arrow(game, game->getTexture(Game::ARROW), start, Vector2D<>(Arrow::SPEED, 0)));
}

int
Bow::getArrows() const {
	return arrows;
}

void
Bow::setArrows(int numArrows) {
	arrows = numArrows;
}

void
Bow::saveObject(std::ostream& out) const {
	out << "W ";	// W de boW: la B ya es de los globos
	SceneObject::saveObject(out);
	out << ' ' << arrows << ' ' << loaded;
}
