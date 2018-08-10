#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Player {
private:
	//Starting movement speed and health
	const float START_SPEED = 200;
	const float START_HEALTH = 100;

	//Current position
	Vector2f m_Position;

	//Sprite and texture
	Sprite m_Sprite;
	Texture m_Texture;

	//Current screen resolution
	Vector2f m_Resolution;

	//Current arena and size of tiles in arena
	IntRect m_Arena;
	int m_TileSize;

	//Direction the player is currently moving
	bool m_UpPressed;
	bool m_DownPressed;
	bool m_LeftPressed;
	bool m_RightPressed;

	//Player's current health and maximum possible health
	int m_Health;
	int m_MaxHealth;

	//Time the player was last hit
	Time m_LastHit;

	//Speed player is moving, in pixels per second
	float m_Speed;

public:
	//Standard constructor
	Player();

	//Spawner function, to place character on map
	void spawn(IntRect arena, Vector2f resolution, int tileSize);

	//To call at end of any game
	void resetPlayerStats();

	//Handles when player gets hit by a zombie
	bool hit(Time timeHit);

	//Returns time when player was last hit
	Time getLastHitTime();

	//Returns player location
	FloatRect getPosition();

	//Returns coordinates of player sprite center
	Vector2f getCenter();

	//Returns angle player sprite is facing
	float getRotation();

	//Returns a copy of the sprite to the main function
	Sprite getSprite();

	//To move the player sprite in the four key-press directions
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	//...then, stop the player sprite moving in the directions after key is released
	void stopLeft();
	void stopRight();
	void stopUp();
	void stopDown();

	//Call this function once per frame to update
	void update(float elapsedTime, Vector2i mousePosition);

	//To change the player's move speed
	void upgradeSpeed();

	//To change the player's health
	void upgradeHealth();

	//To increase the player's max health
	void increaseHealthLevel(int amount);

	//Returns player's current health
	int getHealth();
};