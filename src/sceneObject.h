#pragma once
#include "gameObject.h"
#include "vector2D.h"
#include <iostream>

class Texture;
class Rectangle;

class SceneObject : public GameObject
{
protected:
	Texture* texture;
	Point2D<> position;
	float width;
	float height;

public:
	SceneObject(Game* game, Texture* texture, Point2D<> position, float width, float height);
	virtual bool hit(const Rectangle& rect);
	virtual int getPoints(int combo);
	virtual void saveObject(std::ostream& out) const;
};

