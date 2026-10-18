#include "balloon.h"
#include "texture.h"

// Ancho de un globo en pantalla (el alto se calcula para mantener la proporción)
constexpr float BALLOON_WIDTH = 100.0f;

Balloon::Balloon(Texture* texture, Point2D<> position, Vector2D<> velocity, int color) 
	: position(position),
	width(BALLOON_WIDTH),
	height(BALLOON_WIDTH* texture->getFrameHeight() / texture->getFrameWidth()),
	color(color),
	velocity(velocity),
	texture(texture)
	
{
}

void Balloon::render() const {
	//rectangulo de destino, donde y de que tamaño se dibuja
	SDL_FRect dest{ position.getX(), position.getY(), width, height };

	//fila es color, y columna 0 es el globo entero porque los demas son la explosion
	texture->renderFrame(dest, color, 0);

}

bool Balloon::update(float delta) {
	//desplazamiento = velocidad x tiempo
	position += velocity * delta;

	//sigue vivo mientras su borde inferir este por debajo del boprde superior de la ventana
	return position.getY() + height > 0;
}
