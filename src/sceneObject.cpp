#include "sceneObject.h"

SceneObject::SceneObject(Game* game, Texture* texture, Point2D<> position, float width, float height)
	:GameObject(game), texture(texture), position(position), width(width), height(height) {}


bool
SceneObject::hit(const Rectangle&) {
	return false;
}

int
SceneObject::getPoints(int combo) {
	return 0;
}
