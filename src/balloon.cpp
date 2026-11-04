#include "balloon.h"
#include "texture.h"
#include "rectangle.h"

Balloon::Balloon(Game* game, Texture* texture, Point2D<> position, Vector2D<> velocity, int color, int balloon_points) 
	: MobileObject(game, texture, position, SIZE, SIZE, velocity), 
	color(color), frame(0), explosionTime(0), balloon_points(balloon_points)
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
	MobileObject::update(delta);

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
	return Rectangle({position.getX() + (SIZE - HITBOX_WIDTH) / 2, position.getY()}, HITBOX_WIDTH, SIZE);
}

int
Balloon::getPoints() {
	return balloon_points;
}
