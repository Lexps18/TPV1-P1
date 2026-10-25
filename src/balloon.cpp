#include "balloon.h"
#include "texture.h"
#include "rectangle.h"

Balloon::Balloon(Texture* texture, Point2D<> position, Vector2D<> velocity, int color) 
	: texture(texture), position(position), velocity(velocity), color(color), frame(0), explosionTime(0)
{
}

void 
Balloon::render() const {
	//rectangulo de destino, donde y de que tamaño se dibuja
	SDL_FRect dest{ position.getX(), position.getY(), SIZE, SIZE };
	texture->renderFrame(dest, color, frame);

}

bool 
Balloon::update(float delta) {
	position += velocity * delta;

	if (frame > 0) {
		explosionTime += delta;
		if (explosionTime >= EXPLOSION_FRAME_TIME) {
			explosionTime -= EXPLOSION_FRAME_TIME;
			frame++;
		}
		if (frame >= texture->getNumColumns())
			return false;
	}

	return position.getY() + SIZE >= 0;
}

bool
Balloon::hit(const Rectangle& obj) {

	if (frame > 0 || !obj.hasIntersection(getRect()))
		return false;

	frame = 1; // empieza la explosión
	return true;
}

Rectangle
Balloon::getRect() const
{
	return Rectangle({position.getX() + SIZE/4, position.getY()}, SIZE/2, SIZE);
}
