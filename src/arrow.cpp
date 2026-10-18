#include "arrow.h"
#include "game.h"
#include "texture.h"

Arrow::Arrow(Game* game, Texture* texture, Point2D<> start, Vector2D<> velocity)
	: game(game)
	, texture(texture)
	, width(WIDTH)
	, height(WIDTH * texture->getFrameHeight() / texture->getFrameWidth())
	, position(start.getX(), start.getY() - height / 2)   // centrada en vertical
	, velocity(velocity)
{
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
	position += velocity * delta;

	// TODO comprobar colisión 

	// Sigue viva mientras su borde izquierdo no haya pasado el borde derecho de la ventana
	return position.getX() < Game::WINDOW_WIDTH;
}
