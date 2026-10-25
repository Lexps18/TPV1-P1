#include "InfoBar.h"
#include "game.h"
#include "texture.h"

#include <string>

using namespace std;

// Constantes
constexpr float MARGIN = 10;		// separación con los bordes de la ventana
constexpr float ICON_WIDTH = 30;	// ancho de cada flechita
constexpr float ICON_SPACING = 5;	// hueco entre dos flechitas
constexpr float TEXT_SCALE = 2;		
constexpr SDL_Color TEXT_COLOR = { 255, 255, 255, 255 };	// blanco

InfoBar::InfoBar(const Game* game, Texture* arrowTexture, SDL_Renderer* renderer)
	: game(game)
	, arrowTexture(arrowTexture)
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
	string text = "Puntos: " + to_string(game->getScore());
	float textWidth = float(text.size()) * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * TEXT_SCALE;
	float textX = Game::WINDOW_WIDTH - MARGIN - textWidth;

	SDL_SetRenderDrawColor(renderer, TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b, TEXT_COLOR.a);
	SDL_SetRenderScale(renderer, TEXT_SCALE, TEXT_SCALE);	// la escala también multiplica la posición
	SDL_RenderDebugText(renderer, textX / TEXT_SCALE, MARGIN / TEXT_SCALE, text.c_str());
	SDL_SetRenderScale(renderer, 1, 1);	// la dejamos como estaba para el resto del juego
}
	
