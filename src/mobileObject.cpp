#include "mobileObject.h"

MobileObject::MobileObject(Game* game, Texture* texture, Point2D<> position, float width, float height, Vector2D<> velocity)
	:SceneObject(game, texture, position, width, height), velocity(velocity) {}

bool
MobileObject::update(float delta) {
	position += velocity * delta;
	return true;
}

