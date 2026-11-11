#pragma once

#include "mobileObject.h"

class Rectangle;

class Balloon : public MobileObject
{
public:
	static constexpr float SIZE = 64;
	static constexpr float HITBOX_WIDTH = SIZE / 2;	// más estrecha que la imagen, que tiene bordes transparentes
	static constexpr float EXPLOSION_FRAME_TIME = 0.08f;
	static constexpr int COMBO_BONUS = 5;

private:
	int color;
	int frame;
	float explosionTime;
	int balloon_points;

public:
	Balloon(Game* game, Texture* texture, Point2D<> position, Vector2D<> velocity, int color, int balloon_points);
	Balloon(Game* game, int balloonPoints, std::istream& line);

	void render() const override;
	bool update(float delta) override;
	bool hit(const Rectangle&) override;
	int getPoints(int combo) override;

	// Rectángulo que ocupa el globo
	Rectangle getRect() const;

	void saveObject(std::ostream& out) const override;
};

