#pragma once
#include <SFML/Graphics.hpp>

class Bullet
{
private:
	sf::Vector2f m_Position;

	sf::RectangleShape m_BulletShape;

	bool m_InFlight = false;

	float m_BulletSpeed = 1000;

	float m_BulletDistanceX;
	float m_BulletDistanceY;

	//Bundaries for bullet
	float m_MaxX; 
	float m_MinX;
	float m_MaxY;
	float m_MinY;

public:
	Bullet();

	void stop();

	bool isInFlight();

	void shoot(float startX, float startY, float targetX, float targetY);

	//Inform where the bullet is in the world
	sf::FloatRect getPosition();

	sf::RectangleShape getShape();

	void update(float elapsedTime);

};