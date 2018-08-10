#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Zombie {
private:
	//Zombie speed by type
	const float BLOATER_SPEED = 40;
	const float CHASER_SPEED = 80;
	const float CRAWLER_SPEED = 20;

	//Zombie health by type
	const float BLOATER_HEALTH = 5;
	const float CHASER_HEALTH = 1;
	const float CRAWLER_HEALTH = 3;

	//Natural variance in stats
	const int MAX_VARIANCE = 30;
	const int OFFSET = 101 - MAX_VARIANCE;

	//Position, speed, health, etc. of this zombie
	Vector2f m_Position;
	Sprite m_Sprite;
	float m_Speed;
	float m_Health;
	bool m_Alive;

public:
	//Handles when a bullet his the zombie
	bool hit();

	//Check if alive
	bool isAlive();

	//Spawn new instance of Zombie
	void spawn(float startX, float startY, int type, int seed);

	//Return position
	FloatRect getPosition();

	//Return copy of the sprite to draw
	Sprite getSprite();

	//Update the zombie each frame
	void update(float elapsedTime, Vector2f playerLocation);
};