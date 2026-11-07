#include "arrow.h"
#include "game.h"
#include "texture.h"
#include "rectangle.h"

Arrow::Arrow(Game* game, Texture* texture, Point2D<> start, Vector2D<> velocity)
	: MobileObject(game, texture, start, WIDTH, WIDTH / texture->getAspectRatio(), velocity)
	, combo(0)
{
	position = position - Vector2D<>(0, height / 2);
	game->arrowCreated();	// el juego lleva la cuenta de las flechas en vuelo
}

Arrow::~Arrow()
{
	game->arrowDestroyed();
}

void
Arrow::render() const
{
	SDL_FRect dest{ position.getX(), position.getY(), width, height };
	texture->render(dest);   // arrow.png tiene un solo frame: render, no renderFrame
}


bool
Arrow::update(float delta)
{
	MobileObject::update(delta);

	if (game->checkCollision(getTip(), combo)) {
		combo++;
	}

	// Sigue viva mientras su borde izquierdo no haya pasado el borde derecho de la ventana
	return position.getX() < Game::WINDOW_WIDTH;
}

Rectangle
Arrow::getTip() const
{
	return Rectangle({ position.getX() + WIDTH - TIP_WIDTH, position.getY() }, TIP_WIDTH, height);
}
