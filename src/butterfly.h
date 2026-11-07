#pragma once
#include "mobileObject.h"
#include "vector2D.h"

class Butterfly : public MobileObject
{
public:
	static constexpr float SIZE = 64;
	static constexpr float HITBOX_WIDTH = SIZE / 2;	// más estrecha que la imagen, que tiene bordes transparentes
	static constexpr float SPEED = 100;			// rapidez al volar, en píxeles por segundo
	static constexpr float FALL_SPEED = 150;		// rapidez al caer muerta
	static constexpr float TIME_PER_FRAME = 0.08f;
	static constexpr float DIRECTION_CHANGE_TIME = 2;	// segundos entre cambios de dirección
	static constexpr int BUTTERFLY_POINTS = -5;

private:
	int frame;
	float frameTime;
	float directionTime;
	bool dead;

	void changeDirection();
	void bounce();

public:
	Butterfly(Game* game, Texture* texture, Point2D<> position, Vector2D<> velocity);

	void render() const override;
	bool update(float delta) override;
	bool hit(const Rectangle&) override;

	// Rectángulo que ocupa el globo
	Rectangle getRect() const;
	int getPoints(int combo) override;
};

