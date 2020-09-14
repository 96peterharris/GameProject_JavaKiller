#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.h"

int createBackground(sf::VertexArray& rVA, sf::IntRect arena);
Enemy* createHorde(int numZombies, sf::IntRect arena);