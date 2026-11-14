#include "sceneObject.h"

SceneObject::SceneObject(Game* game, Texture* texture, Point2D<> position, float width, float height)
	:GameObject(game), texture(texture), position(position), width(width), height(height) {}

SceneObject::SceneObject(Game* game, Texture* texture, std::istream& line)
	:GameObject(game), texture(texture) {
	float pX, pY;
	line >> pX >> pY >> width >> height;
	position = { pX, pY };
}


bool
SceneObject::hit(const Rectangle&) {
	return false;
}

int
SceneObject::getPoints(int combo) const {
	return 0;
}

void
SceneObject::saveObject(std::ostream& out) const{
	out << ' ' << position.getX() << ' ' << position.getY() << ' ' << width << ' ' << height;
}
