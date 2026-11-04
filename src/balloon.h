#pragma once

#include "mobileObject.h"

class Rectangle;

class Balloon : public MobileObject
{
public:
	static constexpr float SIZE = 64;
	static constexpr float HITBOX_WIDTH = SIZE / 2;	// más estrecha que la imagen, que tiene bordes transparentes
	static constexpr float EXPLOSION_FRAME_TIME = 0.08f;

private:
	int color;
	int frame;
	float explosionTime;
	int balloon_points;

public:
	Balloon(Game* game, Texture* texture, Point2D<> position, Vector2D<> velocity, int color, int balloon_points);

	void render() const override;
	bool update(float delta) override;
	bool hit(const Rectangle&) override;
	int getPoints() override;

	// Rectángulo que ocupa el globo
	Rectangle getRect() const;
};

