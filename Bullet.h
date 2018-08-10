#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Bullet {
private:
	//Bullet position
	Vector2f m_Position;

	//Bullet shape
	RectangleShape m_BulletShape;

	//Bullet in flight
	bool m_InFlight = false;

	//Bullet speed
	float m_BulletSpeed = 1000;

	//Bullet distance travelled
	float m_BulletDistanceX;
	float m_BulletDistanceY;

	//Some boundaries so the bullet stays within the arena
	float m_MaxX;
	float m_MaxY;
	float m_MinX;
	float m_MinY;

public:
	//Constructor
	Bullet();

	//Stop the bullet
	void stop();

	//Return whether in flight
	bool isInFlight();

	//Launch a new bullet
	void shoot(float startX, float startY, float xTarget, float yTarget);

	//Return the position
	FloatRect getPosition();

	//Return the actual shape (for drawing)
	RectangleShape getShape();

	//Update the bullet each frame
	void update(float elapsedTime);
};