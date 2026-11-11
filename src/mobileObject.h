#pragma once
#include "sceneObject.h"

class MobileObject: public SceneObject
{
protected:
	Vector2D<> velocity;

public:
	MobileObject(Game* game, Texture* texture, Point2D<> position, float width, float height, Vector2D<> velocity);
	MobileObject(Game* game, Texture* texture, std::istream& line);

	bool update(float delta) override;

	virtual void saveObject(std::ostream& out) const override;
};

