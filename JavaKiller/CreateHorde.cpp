#include "pch.h"
#include "EnemyArena.h"
#include "Enemy.h"

//This function is used for creating horde of enemies and creathing them randomly
Enemy* createHorde(int numEnemies, sf::IntRect arena)
{
	Enemy* enemies = new Enemy[numEnemies];

	int maxY = arena.height - 20;
	int minY = arena.top + 20;
	int maxX = arena.width - 20;
	int minX = arena.left + 20;

	for (int i = 0; i < numEnemies; i++)
	{
		srand((int)time(0)*i);
		int side = (rand() % 4);
		float x, y;

		switch (side)
		{
		case 0:
			x = minX;
			y = (rand() % maxY) + minY;
			break;

		case 1:
			x = maxX;
			y = (rand() % maxY) + minY;
			break;

		case 2:
			x = (rand() % maxX) + minX;
			y = minY;
			break;

		case 3:
			x = (rand() % maxX) + minX;
			y = maxY;
			break;
		}

		srand((int)time(0)*i * 2);
		int type = (rand() % 3);

		enemies[i].spawn(x, y, type, i);
	}

	return enemies;
}