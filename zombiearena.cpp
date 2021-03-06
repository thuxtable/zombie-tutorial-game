#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <fstream>
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
	Texture textureBackground = TextureHolder::GetTexture("graphics/grass_textures.png");

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

	// For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20, 980);

	// Load the font
	Font font;
	font.loadFromFile("fonts/zombiecontrol.ttf");

	// Paused
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter \nto continue");

	// Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press Enter to play");

	// Levelling up
	Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);
	levelUpText.setFillColor(Color::White);
	levelUpText.setPosition(150, 250);
	std::stringstream levelUpStream;
	levelUpStream <<
		"1- Increased rate of fire" <<
		"\n2- Increased clip size(next reload)" <<
		"\n3- Increased max health" <<
		"\n4- Increased run speed" <<
		"\n5- More and better health pickups" <<
		"\n6- More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());

	// Ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55);
	ammoText.setFillColor(Color::White);
	ammoText.setPosition(200, 980);

	// Score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(20, 0);

	//Load the high score from the gamedata scores text file
	std::ifstream inputFile("gamedata/scores.txt");
	if (inputFile.is_open()) {
		inputFile >> highScore;
		inputFile.close();
	}

	// High Score
	Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(55);
	hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(1400, 0);
	std::stringstream s;
	s << "High Score:" << highScore;
	hiScoreText.setString(s.str());

	// Zombies remaining
	Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(55);
	zombiesRemainingText.setFillColor(Color::White);
	zombiesRemainingText.setPosition(1500, 980);
	zombiesRemainingText.setString("Zombies: 100");

	// Wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave: 0");

	// Health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);

	// When did we last update the HUD?
	int framesSinceLastHUDUpdate = 0;

	// How often (in frames) should we update the HUD
	int fpsMeasurementFrameInterval = 1000;

	// Prepare the hit sound
	SoundBuffer hitBuffer;
	hitBuffer.loadFromFile("sound/hit.wav");
	Sound hit;
	hit.setBuffer(hitBuffer);

	// Prepare the splat sound
	SoundBuffer splatBuffer;
	splatBuffer.loadFromFile("sound/splat.wav");
	sf::Sound splat;
	splat.setBuffer(splatBuffer);

	// Prepare the shoot sound
	SoundBuffer shootBuffer;
	shootBuffer.loadFromFile("sound/shoot.wav");
	Sound shoot;
	shoot.setBuffer(shootBuffer);

	// Prepare the reload sound
	SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile("sound/reload.wav");
	Sound reload;
	reload.setBuffer(reloadBuffer);

	// Prepare the failed sound
	SoundBuffer reloadFailedBuffer;
	reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
	Sound reloadFailed;
	reloadFailed.setBuffer(reloadFailedBuffer);

	// Prepare the powerup sound
	SoundBuffer powerupBuffer;
	powerupBuffer.loadFromFile("sound/powerup.wav");
	Sound powerup;
	powerup.setBuffer(powerupBuffer);

	// Prepare the pickup sound
	SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile("sound/pickup.wav");
	Sound pickup;
	pickup.setBuffer(pickupBuffer);

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
					wave = 0;
					score = 0;

					//Prepare the ammo for the next game
					currentBullet = 0;
					bulletsSpare = 24;
					bulletsInClip = 6;
					clipSize = 6;
					fireRate = 1;

					//Reset player's stats
					player.resetPlayerStats();
				}

				if (state == State::PLAYING) {
					//Reloading
					if (event.key.code == Keyboard::R) {
						if (bulletsSpare >= clipSize) {
							//Plenty of bullets. Reload.
							bulletsInClip = clipSize;
							bulletsSpare -= clipSize;
							reload.play();
						}
						else if (bulletsSpare > 0) {
							//Only a few bullets left
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
							reload.play();
						}
						else {
							//Reload failed
							reloadFailed.play();
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

					shoot.play();

					bulletsInClip--;
				}
			} //End fire bullet

		} //End of WASD input handling if-statement

		//Handle the levelling up state
		if (state == State::LEVELING_UP) {
			if (event.key.code == Keyboard::Num1) {
				// Increase fire rate
				fireRate++;
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num2) {
				// Increase clip size
				clipSize += clipSize;
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num3) {
				// Increase health
				player.upgradeHealth();
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num4) {
				// Increase speed
				player.upgradeSpeed();
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num5) {
				healthPickup.upgrade();
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num6) {
				ammoPickup.upgrade();
				state = State::PLAYING;
			}

			if (state == State::PLAYING) {
				
				wave++;

				//Prepare the level
				arena.width = 500 * wave;
				arena.height = 500 * wave;
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
				numZombies = 5 * wave;

				//Delete any previously allocated memory
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;

				//Play powerup sound
				powerup.play();

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
						splat.play();
					}
				}
			} //End of zombie being shot

			//Has any zombie touched the player?
			for (int i = 0; i < numZombies; i++) {
				if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive()) {
					if (player.hit(gameTimeTotal)) {
						//Player was hit
						hit.play();
					}

					if (player.getHealth() <= 0) {
						state = State::GAME_OVER;
						std::ofstream outputFile("gamedata/scores.txt");
						outputFile << highScore;
						outputFile.close();
					}
				}
			} //End of player touched

			//Has the player touched the health pickup
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned()) {
				player.increaseHealthLevel(healthPickup.gotIt());
				pickup.play();
			}

			//Has the player touched the ammo pickup
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned()) {
				bulletsSpare += ammoPickup.gotIt();
				reload.play();
			}

			// Size up the health bar
			healthBar.setSize(Vector2f(player.getHealth() * 3, 70));

			// Increment the number of frames since the last HUD calculation
			framesSinceLastHUDUpdate++;

			// Calculate FPS every fpsMeasurementFrameInterval frames
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
			{

				// Update game HUD text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssZombiesAlive;

				// Update the ammo text
				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ssAmmo.str());

				// Update the score text
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore << "Hi Score:" << highScore;
				hiScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the high score text
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());

				framesSinceLastHUDUpdate = 0;
			}// End HUD update

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

			//Switch to the HUD view
			window.setView(hudView);

			//Draw all the HUD elements
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}

		if (state == State::LEVELING_UP) {
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}

		if (state == State::PAUSED) {
			window.draw(pausedText);
		}

		if (state == State::GAME_OVER) {
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}

		window.display();

	} //End of the game loop

	delete[] zombies;
    return 0;
}

