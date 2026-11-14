#include "InfoBar.h"
#include "game.h"
#include "texture.h"

#include <string>

using namespace std;

// Constantes
constexpr float MARGIN = 10;		// separación con los bordes de la ventana
constexpr float ICON_WIDTH = 30;	// ancho de cada flechita
constexpr float ICON_SPACING = 5;	// hueco entre dos flechitas

constexpr float DIGIT_WIDTH = 30;	// ancho de cada cifra en pantalla

constexpr float TEXT_SCALE = 2;		
constexpr SDL_Color TEXT_COLOR = { 255, 255, 255, 255 };	// blanco

InfoBar::InfoBar(Game* game, Texture* arrowTexture, Texture* numberTexture, SDL_Renderer* renderer)
	: GameObject(game)
	, arrowTexture(arrowTexture)
	, numberTexture(numberTexture)
	, renderer(renderer)
{
}

void
InfoBar::render() const {
	// Una flechita por cada flecha que le queda al arco, en fila arriba a la izquierda
	float iconHeight = ICON_WIDTH / arrowTexture->getAspectRatio();
	for (int i = 0; i < game->getArrowsLeft(); i++) {
		SDL_FRect dest{ MARGIN + i * (ICON_WIDTH + ICON_SPACING), MARGIN, ICON_WIDTH, iconHeight };
		arrowTexture->render(dest);
	}

	// Puntuación arriba a la derecha
	float digitHeight = DIGIT_WIDTH / numberTexture->getAspectRatio();
	int remaining = game->getScore();
	int i = 0;
	while (remaining > 0 || i == 0) {
		int digit = remaining % 10;	// la última cifra
		float x = Game::WINDOW_WIDTH - MARGIN - float(i + 1) * DIGIT_WIDTH;
		SDL_FRect dest{ x, MARGIN, DIGIT_WIDTH, digitHeight };
		numberTexture->renderFrame(dest, 0, digit);
		remaining /= 10;		// quita la última cifra
		i++;
	}
}

bool
InfoBar::update(float)
{
	return true;	// no cambia por sí sola
}
	
