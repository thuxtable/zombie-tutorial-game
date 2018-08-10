#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"

using namespace sf;

int main()
{
	TextureHolder holder;
	//Enumeration of the game's states
	enum class State {
		PAUSED, LEVELING_UP, GAME_OVER, PLAYING
	};

	//Begin game in game-over state
	State state = State::GAME_OVER;

	//Store the screen resolution and create an SFML window
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

	//Create an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	//Create the clock for the game's overall timing
	Clock clock;

	//Total game time
	Time gameTimeTotal;

	//Mouse position in relation to world coordinates
	Vector2f mouseWorldPosition;

	//Mouse position in relation to screen coordinates
	Vector2i mouseScreenPosition;

	//Create an instance of the Player
	Player player;

	//And the boundaries of the arena
	IntRect arena;

	//Create the background
	VertexArray background;

	//Load the texture for the background vertex array
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

	//Initialize the zombie horde
	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;

	//100 Bullets initially
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;

	//When was the last button pressed?
	Time lastPressed;

	//Hide the mouse pointer and replace it with crosshair
	window.setMouseCursorVisible(false);
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	//Create a couple of pickups
	Pickup healthPickup(1);
	Pickup ammoPickup(2);

	//About the game
	int score = 0;
	int highScore = 0;

	////////////////////////////MAIN GAME LOOP///////////////////////////
	while (window.isOpen()) {

		/******************** HANDLE PLAYER INPUT *****************/

		//Handle events
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyPressed) {
				//Pause the game
				if (event.key.code == Keyboard::Return && state == State::PLAYING) {
					state = State::PAUSED;
				}

				//Restart while paused
				else if (event.key.code == Keyboard::Return && state == State::PAUSED) {
					state = State::PLAYING;
					//Reset the clock so there isn't a frame jump
					clock.restart();
				}

				//Start a new game while in GAME_OVER state
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER) {
					state = State::LEVELING_UP;
				}

				if (state == State::PLAYING) {
					//Reloading
					if (event.key.code == Keyboard::R) {
						if (bulletsSpare >= clipSize) {
							//Plenty of bullets. Reload.
							bulletsInClip = clipSize;
							bulletsSpare -= clipSize;
						}
						else if (bulletsSpare > 0) {
							//Only a few bullets left
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
						}
						else {
							//More here soon
						}
					}
				}
			}
		} //End Polling Event

		//Handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		//Handle player WASD controls while playing
		if (state == State::PLAYING) {
			//Four pairs of checks for movement key press or release
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				player.moveUp();
			}
			else {
				player.stopUp();
			}

			if (Keyboard::isKeyPressed(Keyboard::S)) {
				player.moveDown();
			}
			else {
				player.stopDown();
			}

			if (Keyboard::isKeyPressed(Keyboard::D)) {
				player.moveRight();
			}
			else {
				player.stopRight();
			}

			if (Keyboard::isKeyPressed(Keyboard::A)) {
				player.moveLeft();
			}
			else {
				player.stopLeft();
			}

			//Fire a bullet
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0) {
					//Pass the center of the player and the enter of the cross-hair to the shoot function
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);

					currentBullet++;
					if (currentBullet > 99) {
						currentBullet = 0;
					}
					lastPressed = gameTimeTotal;

					bulletsInClip--;
				}
			} //End fire bullet

		} //End of WASD input handling if-statement

		//Handle the levelling up state
		if (state == State::LEVELING_UP) {
			if (event.key.code == Keyboard::Num1) {
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num2) {
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num3) {
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num4) {
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num5) {
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num6) {
				state = State::PLAYING;
			}

			if (state == State::PLAYING) {
				//Prepare the level
				arena.width = 500;
				arena.height = 500;
				arena.left = 0;
				arena.top = 0;

				//Pass the vertex array by reference to the createBackground function
				int tileSize = createBackground(background, arena);

				//Spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				//Configure the pickups
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);

				//Create a zombie horde
				numZombies = 10;

				//Delete any previously allocated memory
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;

				//Reset the clock so there isn't a frame jump
				clock.restart();
			}
		} //End of leveling up handling

		/*********************** UPDATE THE FRAME **************************/
		if (state == State::PLAYING) {
			//Update the delta time
			Time dt = clock.restart();
			//Update the total game time
			gameTimeTotal += dt;
			//Make a decimal fraction of 1 from the delta time
			float dtAsSeconds = dt.asSeconds();

			//Set the mouse pointer location
			mouseScreenPosition = Mouse::getPosition();

			//Convert the mouse position to the world coordinates
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			//Set the crosshair to the mouse world location
			spriteCrosshair.setPosition(mouseWorldPosition);

			//Update the player
			player.update(dtAsSeconds, Mouse::getPosition());

			//Store the player's new position
			Vector2f playerPosition(player.getCenter());

			//Make the view center on the player
			mainView.setCenter(player.getCenter());

			//Loop through each zombie and update them
			for (int i = 0; i < numZombies; i++) {
				if (zombies[i].isAlive()) {
					zombies[i].update(dt.asSeconds(), playerPosition);
				}
			}

			//Update any bullets that are in-flight
			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					bullets[i].update(dtAsSeconds);
				}
			}

			//Update the pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			//Collision detection between zombies and bullets
			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < numZombies; j++) {
					if (bullets[i].isInFlight() && zombies[j].isAlive()) {
						if (bullets[i].getPosition().intersects(zombies[j].getPosition())) {
							
							//Register the hit and check if it will kill
							bullets[i].stop();
							
							if (zombies[j].hit()) {
								//No just a hit but a kill too
								score += 10;
								if (score >= highScore) {
									highScore = score;
								}
							}

							numZombiesAlive--;

							//When all zombies are dead
							if (numZombiesAlive == 0) {
								state = State::LEVELING_UP;
							}
						}
					}
				}
			} //End of zombie being shot

			//Has any zombie touched the player?
			for (int i = 0; i < numZombies; i++) {
				if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive()) {
					if (player.hit(gameTimeTotal)) {
						// More here later
					}

					if (player.getHealth() <= 0) {
						state = State::GAME_OVER;
					}
				}
			} //End of player touched

			//Has the player touched the health pickup
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned()) {
				player.increaseHealthLevel(healthPickup.gotIt());
			}

			//Has the player touched the ammo pickup
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned()) {
				bulletsSpare += ammoPickup.gotIt();
			}

		} //End of PLAYING state update

		/************************** DRAW THE SCENE ***************************/

		if (state == State::PLAYING) {
			window.clear();
			
			//Set the mainView to the window display
			window.setView(mainView);

			//Draw the background
			window.draw(background, &textureBackground);
			
			//Draw the zombies
			for (int i = 0; i < numZombies; i++) {
				window.draw(zombies[i].getSprite());
			}

			//Draw the bullets
			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					window.draw(bullets[i].getShape());
				}
			}

			//Draw the player
			window.draw(player.getSprite());

			//Draw the pickups, if currently spawned
			if (ammoPickup.isSpawned()) {
				window.draw(ammoPickup.getSprite());
			}
			if (healthPickup.isSpawned()) {
				window.draw(healthPickup.getSprite());
			}

			//Draw the crosshair
			window.draw(spriteCrosshair);

		}

		if (state == State::LEVELING_UP) {

		}

		if (state == State::PAUSED) {

		}

		if (state == State::GAME_OVER) {

		}

		window.display();

	} //End of the game loop

	delete[] zombies;
    return 0;
}

