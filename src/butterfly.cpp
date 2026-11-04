#include "butterfly.h"
#include "texture.h"
#include "rectangle.h"
#include "game.h"

#include <cmath>
#include <numbers>

using namespace std;

Butterfly::Butterfly(Game* game, Texture* texture, Point2D<> position, Vector2D<> velocity)
	: MobileObject(game, texture, position, SIZE, SIZE, velocity),
	frame(0), frameTime(0), directionTime(DIRECTION_CHANGE_TIME), dead(false)
{}

void
Butterfly::render() const {
	//rectangulo de destino, donde y de que tamaño se dibuja
	SDL_FRect dest{ position.getX(), position.getY(), SIZE, SIZE };
	if (velocity.getX() > 0 || dead) {
		texture->renderFrame(dest, 0, frame);
	}
	else {
		texture->renderFrame(dest, 1, frame);
	}
}

bool
Butterfly::update(float delta) {
	MobileObject::update(delta);

	if(dead)
		return position.getY() < Game::WINDOW_HEIGHT;

	bounce();

	directionTime += delta;
	if (directionTime >= DIRECTION_CHANGE_TIME) {
		directionTime -= DIRECTION_CHANGE_TIME;
		changeDirection();
	}

	frameTime += delta;
	if (frameTime >= TIME_PER_FRAME) {
		frameTime -= TIME_PER_FRAME;
		frame = (frame + 1) % texture->getNumColumns();
	}

	return true;
}

bool
Butterfly::hit(const Rectangle& obj) {

	if (dead || !obj.hasIntersection(getRect()))
		return false;

	dead = true;
	frame = 0;
	velocity = { 0, FALL_SPEED };
	return true;
}

void
Butterfly::changeDirection() {
	float angle = game->getRandom(0, 2 * numbers::pi_v<float>);	// dirección: 0 a 360 grados, en radianes
	velocity = Vector2D<>(cos(angle) * SPEED, sin(angle) * SPEED);
}

void
Butterfly::bounce()
{
	if (position.getX() < 0)
		velocity = { std::abs(velocity.getX()), velocity.getY() };	// hacia la derecha
	else if (position.getX() + width > Game::WINDOW_WIDTH)
		velocity = { -std::abs(velocity.getX()), velocity.getY() };	// hacia la izquierda

	if (position.getY() < 0)
		velocity = { velocity.getX(), std::abs(velocity.getY()) };	// hacia abajo
	else if (position.getY() + height > Game::WINDOW_HEIGHT)
		velocity = { velocity.getX(), -std::abs(velocity.getY()) };	// hacia arriba
}

Rectangle
Butterfly::getRect() const
{
	return Rectangle({ position.getX() + (SIZE - HITBOX_WIDTH) / 2, position.getY() }, HITBOX_WIDTH, SIZE);
}

int
Butterfly::getPoints() {
	return BUTTERFLY_POINTS;
}

