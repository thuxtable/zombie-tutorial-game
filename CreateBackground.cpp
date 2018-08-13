#include "stdafx.h"
#include "SFML/Graphics.hpp"
#include "ZombieArena.h"

int createBackground(VertexArray& rVA, IntRect arena) {
	//Anything we do to rVA we're actually doing to "background" in the main function

	//Size of each tile/texture
	const int TILE_SIZE = 50;
	const int TILE_TYPES = 3;
	const int VERTS_IN_QUAD = 4;

	int worldWidth = (arena.width / TILE_SIZE);
	int worldHeight = (arena.height / TILE_SIZE);

	//Set primitive type
	rVA.setPrimitiveType(Quads);

	//Set size of the vertex array
	rVA.resize(worldWidth * worldHeight * VERTS_IN_QUAD);

	//Start at the beginning of the vertex array
	int currentVertex = 0;

	//For-loop to intialize vertex array
	for (int w = 0; w < worldWidth; w++) {
		for (int h = 0; h < worldHeight; h++) {
			//Position each vertex in the current quad
			rVA[currentVertex + 0].position = Vector2f(w * TILE_SIZE, h * TILE_SIZE);
			rVA[currentVertex + 1].position = Vector2f((w * TILE_SIZE) + TILE_SIZE, h * TILE_SIZE);
			rVA[currentVertex + 2].position = Vector2f((w * TILE_SIZE) + TILE_SIZE, (h * TILE_SIZE) + TILE_SIZE);
			rVA[currentVertex + 3].position = Vector2f((w * TILE_SIZE), (h * TILE_SIZE) + TILE_SIZE);
		
			//Define the position in the Text for the current quad
			//Either grass, stone, bush, or wall
			if ((h == 0) || h == worldHeight - 1 || w == 0 || w == worldWidth - 1) {
				//Use the wall texture if at the edges of the map
				rVA[currentVertex + 0].texCoords = Vector2f(0, 0 + TILE_TYPES * TILE_SIZE);
				rVA[currentVertex + 1].texCoords = Vector2f(TILE_SIZE, 0 + TILE_TYPES * TILE_SIZE);
				rVA[currentVertex + 2].texCoords = Vector2f(TILE_SIZE, TILE_SIZE + TILE_TYPES * TILE_SIZE);
				rVA[currentVertex + 3].texCoords = Vector2f(0, TILE_SIZE + TILE_TYPES * TILE_SIZE);
			}
			else {
				//Otherwise, use a random floor texture to fill the middle
				srand((int)time(0) + h * w); // Guide advocates using term "h * w - h" but this resulted in first column always being same tile type
				int mOrG = (rand() % TILE_TYPES);
				int verticalOffset = mOrG * TILE_SIZE;

				rVA[currentVertex + 0].texCoords = Vector2f(0, 0 + verticalOffset);
				rVA[currentVertex + 1].texCoords = Vector2f(TILE_SIZE, 0 + verticalOffset);
				rVA[currentVertex + 2].texCoords = Vector2f(TILE_SIZE, TILE_SIZE + verticalOffset);
				rVA[currentVertex + 3].texCoords = Vector2f(0, TILE_SIZE + verticalOffset);
			}

			//Position ready for the next vertices
			currentVertex = currentVertex + VERTS_IN_QUAD;
		}
	}

	return TILE_SIZE;
}