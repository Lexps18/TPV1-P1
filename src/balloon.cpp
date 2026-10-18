#include "balloon.h"
#include "texture.h"

Balloon::Balloon(Texture* texture, Point2D<> position, Vector2D<> velocity, int color) 
	: texture(texture), position(position), color(color), velocity(velocity)
{
}

void Balloon::render() const {
	//rectangulo de destino, donde y de que tamaño se dibuja
	SDL_FRect dest{ position.getX(), position.getY(), SIZE, SIZE };
	texture->renderFrame(dest, color, 0);

}

bool Balloon::update(float delta) {
	position += velocity * delta;

	if (position.getY() + SIZE < 0)
		return false;

	return true;
}
