#include "stdafx.h"
#include "Zombie.h"
#include "TextureHolder.h"
#include <cstdlib>
#include <ctime>

using namespace std;

void Zombie::spawn(float startX, float startY, int type, int seed) {
	switch (type) {
		case 0:
			//Bloater
			m_Sprite = Sprite(TextureHolder::GetTexture("graphics/bloater.png"));
			m_Speed = 40;
			m_Health = 5;
			break;
		case 1:
			//Chaser
			m_Sprite = Sprite(TextureHolder::GetTexture("graphics/chaser.png"));
			m_Speed = 70;
			m_Health = 1;
			break;
		case 2:
			//Crawler
			m_Sprite = Sprite(TextureHolder::GetTexture("graphics/crawler.png"));
			m_Speed = 20;
			m_Health = 3;
			break;
	}

	//Modify the speed to make each zombie vary uniquely
	srand((int)time(0) * seed);
	float modifier = (rand() % MAX_VARIANCE) + OFFSET;

	//Express as a fraction of 1
	modifier /= 100; //Now the mofiier equals between 0.7 and 1
	m_Speed *= modifier;

	//Initialize the location
	m_Position.x = startX;
	m_Position.y = startY;

	//Set the origin to the sprite's center
	m_Sprite.setOrigin(25, 25);

	//Set its position
	m_Sprite.setPosition(m_Position);
}

//Handle when zombie is hit
bool Zombie::hit() {
	m_Health--;
	
	if (m_Health < 0) {
		m_Alive = false;
		m_Sprite.setTexture(TextureHolder::GetTexture("graphics/blood.png"));
		return true; //Dead
	}
	// Injured but not dead
	return false;
}

//Check if alive
bool Zombie::isAlive() {
	return m_Alive;
}

//Return position
FloatRect Zombie::getPosition() {
	return m_Sprite.getGlobalBounds();
}

//Return copy of the sprite to draw
Sprite Zombie::getSprite() {
	return m_Sprite;
}

//Update the zombie each frame
void Zombie::update(float elapsedTime, Vector2f playerLocation) {
	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	//Update the zombie position variables
	if (playerX > m_Position.x) {
		m_Position.x = m_Position.x + m_Speed * elapsedTime;
	}

	if (playerY > m_Position.y) {
		m_Position.y = m_Position.y + m_Speed * elapsedTime;
	}

	if (playerX < m_Position.x) {
		m_Position.x = m_Position.x - m_Speed * elapsedTime;
	}

	if (playerY < m_Position.y) {
		m_Position.y = m_Position.y - m_Speed * elapsedTime;
	}

	//Move the sprite
	m_Sprite.setPosition(m_Position);

	//Face the sprite in the correct direction
	float angle = (atan2(playerY - m_Position.y, playerX - m_Position.x) * 180) / 3.141;
	m_Sprite.setRotation(angle);
}