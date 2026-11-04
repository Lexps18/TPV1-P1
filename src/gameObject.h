#pragma once
class Game;

class GameObject
{
protected:
	Game* game;

public:
	GameObject(Game* game);
	virtual ~GameObject() = default;

	virtual void render() const = 0;
	virtual bool update(float delta) = 0;
};

