#pragma once

#include "mobileObject.h"

class Rectangle;

class Arrow : public MobileObject
{
public:
	static constexpr float WIDTH = 80;
	static constexpr float SPEED = 150;
	static constexpr float TIP_WIDTH = 15;

	int combo;

	Arrow(Game* game, Texture* texture, Point2D<> start, Vector2D<> velocity);
	~Arrow() override;

	void render() const override;

	//devuelve false cuando ha salido por la derecha
	bool update(float delta) override;

	// Rectángulo de la punta, el único trozo que explota globos
	Rectangle getTip() const;

	void saveObject(std::ostream& out) const override;
};

