#include "pch.h"
#include "Enemy.h"
#include "TextureHolder.h"
#include <cstdlib>
#include <ctime>

void Enemy::spawn(float startX, float startY, int type, int seed)
{
	switch (type)
	{
	case 0:
		m_Sprite = sf::Sprite(TextureHolder::GetTexture("graphics/bloater.png"));
		m_Speed = 40;
		m_Health = 5;
		break;

	case 1:
		m_Sprite = sf::Sprite(TextureHolder::GetTexture("graphics/chaser.png"));
		m_Speed = 70;
		m_Health = 1;
		break;

	case 2:
		m_Sprite = sf::Sprite(TextureHolder::GetTexture("graphics/crawler.png"));
		m_Speed = 20;
		m_Health = 3;
		break;
	}


	srand((int)time(0)*seed);

	float modifier = (rand() % MAX_VARIANCE) + OFFSET;

	modifier /= 100;

	m_Position.x = startX;
	m_Position.y = startY;

	m_Sprite.setOrigin(25, 25);

	m_Sprite.setPosition(m_Position);
}
bool Enemy::hit()
{
	m_Health--;

	if (m_Health < 0)
	{
		m_Alive = false;
		m_Sprite.setTexture(TextureHolder::GetTexture("graphics/blood.png"));

		return true;
	}

	return false;
}
bool Enemy::isAlive()
{
	return m_Alive;
}
sf::FloatRect Enemy::getPosition()
{
	return m_Sprite.getGlobalBounds();
}
sf::Sprite Enemy::getSprite()
{
	return m_Sprite;
}
void Enemy::update(float elapsedTime, sf::Vector2f playerLocation)
{
	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	if (playerX > m_Position.x)
	{
		m_Position.x = m_Position.x + m_Speed * elapsedTime;
	}

	if (playerY > m_Position.y)
	{
		m_Position.y = m_Position.y + m_Speed * elapsedTime;
	}

	if (playerX < m_Position.x)
	{
		m_Position.x = m_Position.x - m_Speed * elapsedTime;
	}

	if (playerY < m_Position.y)
	{
		m_Position.y = m_Position.y - m_Speed * elapsedTime;
	}

	m_Sprite.setPosition(m_Position);

	float angle = (atan2(playerY - m_Position.y, playerX - m_Position.x) * 180) / 3.141;

	m_Sprite.setRotation(angle);

}



