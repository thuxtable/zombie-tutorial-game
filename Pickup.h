#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Pickup {
private:
	//Start value for health pickups
	const int HEALTH_START_VALUE = 50;
	const int AMMO_START_VALUE = 12;
	const int START_WAIT_TIME = 10;
	const int START_SECONDS_TO_LIVE = 5;

	//The sprite that represents this pickup
	Sprite m_Sprite;

	//The arena it exists in
	IntRect m_Arena;

	//This pickup's value
	int m_Value;

	//Pickup type
	// 1 = health; 2 = ammo
	int m_Type;

	//Handle spawing and disappearing
	float m_Spawned;
	float m_SecondsSinceSpawn;
	float m_SecondsSinceDespawn;
	float m_SecondsToLive;
	float m_SecondsToWait;

public:
	
	Pickup(int type);

	//Prepare a new pickup
	void setArena(IntRect arena);

	void spawn();

	//Return the position of the pickup
	FloatRect getPosition();

	//Return the drawn for the pickup
	Sprite getSprite();

	//Let the pickup update itself each frame
	void update(float elapsedTime);

	//Is it spawned?
	bool isSpawned();

	//Is it picked up?
	int gotIt();

	//Upgrade the value of each pickup
	void upgrade();
};