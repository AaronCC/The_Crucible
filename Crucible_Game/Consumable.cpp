#include "Consumable.h"

void Consumable::draw(sf::RenderWindow& window)
{
	window.draw(this->back);
	window.draw(this->sprite);
	window.draw(this->quantText);
}
