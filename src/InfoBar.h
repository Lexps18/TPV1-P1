#pragma once

#include <SDL3/SDL.h>
#include "gameObject.h"

class Texture;

class InfoBar : public GameObject
{
private:
	Texture* arrowTexture;	// para dibujar las flechas
	Texture* numberTexture;	// para dibujar los numeros

public:
	InfoBar(Game* game, Texture* arrowTexture, Texture* numberTexture);

	void render() const override;
	bool update(float delta) override;
};

