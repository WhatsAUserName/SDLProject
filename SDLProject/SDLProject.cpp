// SDLProject.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "definitions.h"

SDL_Rect TileClips[TOTAL_TILE_SPRITES];

SDL_Window *window = nullptr;
SDL_Texture *currentImage = nullptr;
SDL_Renderer *renderTarget = nullptr;

WTexture gTileTexture;
SDL_Rect CharSpriteClip;
WTexture gCharTexture;
WTexture gBGTexture;
WTexture gTitleBG;
WTexture gGameOver;



WTexture::WTexture() {

	texture = NULL;
	mWidth = 0;
	mHeight = 0;

}

WTexture::~WTexture() {

	free();
}



bool WTexture::loadFromFile(std::string filepath) {
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(filepath.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", filepath.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderTarget, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", filepath.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	texture = newTexture;
	return texture != NULL;

}

void WTexture::free() {
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void WTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(renderTarget, texture, clip, &renderQuad, angle, center, flip);
}

int WTexture::getWidth() {
	return mWidth;
}

int WTexture::getHeight() {

	return mHeight;
}

bool setTiles(Tile* tiles[]);

Tile::Tile(int x, int y, int tileType)
{
	//Get the offsets
	tBox.x = x;
	tBox.y = y;

	//Set the collision box
	tBox.w = TILE_WIDTH;
	tBox.h = TILE_HEIGHT;

	//Get the tile type
	mType = tileType;
}

void Tile::render(SDL_Rect& camera)
{
	//If the tile is on screen
	if (checkCollision(camera, tBox))
	{
		//Show the tile
		gTileTexture.render(tBox.x - camera.x, tBox.y - camera.y, &TileClips[mType]);
	}
}

int Tile::getType()
{
	return mType;
}

SDL_Rect Tile::getBox()
{
	return tBox;
}



bool touchGround(SDL_Rect box, Tile* tiles[])
{
	//Go through the tiles
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		//If the tile is a ground type tile
		if ((tiles[i]->getType() >= TILE_GRASS) && (tiles[i]->getType() <= TILE_BRICK))
		{
			//If the collision box touches the ground tile
			if (checkCollision(box, tiles[i]->getBox()))
			{
				return true;
			}
		}
	}

	//If no wall tiles were touched
	return false;
}


SDL_Texture *LoadTexture(std::string filepath, SDL_Renderer *renderTarget)
{
	SDL_Texture *texture = nullptr;
	SDL_Surface *surface = IMG_Load(filepath.c_str());
	if (surface == NULL)
		std::cout << "Error1" << std::endl;
	else {
		texture = SDL_CreateTextureFromSurface(renderTarget, surface);
		if (texture == NULL)
			std::cout << "Error2" << std::endl;
	}
	SDL_FreeSurface(surface);
	return texture;
}



bool setTiles(Tile *tiles[]) {
	bool tilesLoaded = true;

	int x = 0, y = 0;

	//Open the map

	std::ifstream map("mapdata/mapbig.txt");

	if (!map.is_open()) {
		printf("Unable to load map");
		tilesLoaded = false;
	}
	else
	{
		
		for (int i = 0; i < TOTAL_TILES; i++)
		{
			int tileType = -1;

			map >> tileType;

			if (map.fail())
			{
				printf("Error loading map, end of file error");
				tilesLoaded = false;
				break;
			}
			if ((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES))
			{
				tiles[i] = new Tile(x, y, tileType);
			}
			else
			{
				printf("Error loading map");
				tilesLoaded = false;
				break;
			}
			x += TILE_WIDTH;

			if (x >= LEVEL_WIDTH) {
				x = 0;

				y += TILE_HEIGHT;
			}
		}
		if (tilesLoaded)
		{
			TileClips[TILE_GRASS].x = 0;
			TileClips[TILE_GRASS].y = 0;
			TileClips[TILE_GRASS].w = TILE_WIDTH;
			TileClips[TILE_GRASS].h = TILE_HEIGHT;
			TileClips[TILE_BRICK].x = 0;
			TileClips[TILE_BRICK].y = 80;
			TileClips[TILE_BRICK].w = TILE_WIDTH;
			TileClips[TILE_BRICK].h = TILE_HEIGHT;

		}
	}
	map.close();
	return tilesLoaded;
}

bool loadMedia(Tile* tiles[]) {
	bool success = true;
	if (!gBGTexture.loadFromFile("images/BG.jpg")) {
		printf("failed to load background image");
		success = false;
	}
	if (!gTitleBG.loadFromFile("images/title1.png")) {
		printf("failed to load title image");
		success = false;
	}
	if (!gGameOver.loadFromFile("images/gameover.png")) {
		printf("failed to load title image");
		success = false;
	}
	if (!gTileTexture.loadFromFile("images/tile.bmp")) {
		printf("failed to load tile");
		success = false;
	}
	if (!setTiles(tiles)) {
		printf("fail");
		success = false;
	}
	return success;
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//initialise sides of rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B return false, no collision
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B then there was a collision
	return true;
}



void render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {

	SDL_Rect renderer = { x, y, CHAR_WIDTH, CHAR_HEIGHT };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderer.w = clip->w;
		renderer.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(renderTarget, currentImage, clip, &renderer, angle, center, flip);
}



bool init() {
	bool success = true;
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("SDL initialisation failed, aborting");
		success = false;
	}
	else {
		int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(imgFlags)&imgFlags))
		{
			std::cout << "Error 3" << IMG_GetError() << std::endl;
		}
		//create window and rendering environment
		window = SDL_CreateWindow("Al's Adventure", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		currentImage = LoadTexture("images/sprite.png", renderTarget);
		success = true;
	}
	return success;
}




int main(int argc, char* args[]) {
	
	//initialise local variables

	const int FPS = 60;
	float frameTime = 0;
	int prevTime = 0;
	int currentTime = 0;
	float deltaTime = 0;
	float movSpeed = 150;
	const Uint8 *keyState;
	bool jumping = false;
	bool falling = false;
	float jumpvel = 15;
	float gravity = 0.6f;
	float air = 5;
	const int groundlevel = 910;
	SDL_Rect playerRect;
	SDL_Rect playerPos;
	playerPos.x = 0;
	playerPos.y = groundlevel - TILE_HEIGHT;
	playerPos.w = CHAR_WIDTH;
	playerPos.h = CHAR_HEIGHT;
	int frameWidth, frameHeight;
	int textureWidth, textureHeight;
	int lives = 1;
	keyState = SDL_GetKeyboardState(NULL);

	//set default state
	stateID = STATE_TITLE;


	if (!init()) {
		printf("Failed initilising");
	}
	else {

		SDL_QueryTexture(currentImage, NULL, NULL, &textureWidth, &textureHeight);

		frameWidth = textureWidth / 4;
		frameHeight = textureHeight / 4;

		playerRect.x = playerRect.y = 0;
		playerRect.w = frameWidth;
		playerRect.h = frameHeight;
		
		SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

		Tile* tileSet[TOTAL_TILES];
		bool isRunning = true;
		SDL_Event e;
		if (!loadMedia(tileSet))
		{
			printf("Failed to load media!\n");
		}
		else
		{
		

		while (isRunning)
		{
				prevTime = currentTime;
				currentTime = SDL_GetTicks();
				deltaTime = (currentTime - prevTime) / 1000.0f;
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						isRunning = false;
					}

				}

				if (stateID == STATE_ROOM1) {
					if (keyState[SDL_SCANCODE_RIGHT]) {
						playerRect.y = 100;
						playerPos.x += movSpeed * deltaTime;
						std::cout << playerPos.x << std::endl;
					}
					if (keyState[SDL_SCANCODE_LEFT])
					{
						playerRect.y = 50;
						playerPos.x -= movSpeed *deltaTime;
						std::cout << playerPos.x << std::endl;
					}
					if (keyState[SDL_SCANCODE_SPACE] && !jumping) {
						jumping = true;
					}
					if (keyState[SDL_SCANCODE_LSHIFT]) {
						movSpeed = 250;
					}
					else if (!keyState[SDL_SCANCODE_LSHIFT]) {
						movSpeed = 150;
					}

					if ((jumping == true)) {
						playerPos.y -= jumpvel;
						jumpvel -= gravity;
					}
					if (touchGround(playerPos, tileSet)) {
						jumpvel = 15;
						jumping = false;
					}

					while (touchGround(playerPos, tileSet))
					{
						jumping = false;
						playerPos.y -= 1;

					}


					if (playerPos.y <= groundlevel) {
						//printf("gravity");
						playerPos.y += 5;
					}
					if (playerPos.y > groundlevel) {
						stateID = STATE_NULL;
					}


					frameTime += deltaTime;
					if (frameTime >= 0.25f)
					{
						if ((keyState[SDL_SCANCODE_RIGHT]) || (keyState[SDL_SCANCODE_LEFT])) {
							frameTime = 0;
							playerRect.x += frameWidth;
							if (playerRect.x >= textureWidth)
							{
								playerRect.x = 0;
							}
						}
					}
				}
				switch (stateID) {
				case STATE_TITLE: 
					SDL_SetRenderDrawColor(renderTarget, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(renderTarget);
					gTitleBG.render(0, 0);
					SDL_RenderPresent(renderTarget);
					if (keyState[SDL_SCANCODE_SPACE]) {
						stateID = STATE_ROOM1;
					}
					break;
				case STATE_ROOM1:
					SDL_SetRenderDrawColor(renderTarget, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(renderTarget);
					gBGTexture.render(-200, 0);

					for (int i = 0; i < TOTAL_TILES; ++i)
					{
						tileSet[i]->render(camera);
					}
					SDL_RenderCopy(renderTarget, currentImage, &playerRect, &playerPos);
					SDL_RenderPresent(renderTarget);
					break;
				case STATE_NULL:
					SDL_SetRenderDrawColor(renderTarget, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(renderTarget);
					gGameOver.render(0, 0);
					SDL_RenderPresent(renderTarget);
					playerPos.x = 0;
					playerPos.y = 878;
					if (keyState[SDL_SCANCODE_ESCAPE]) {
						stateID = STATE_TITLE;
					}
					break;

				}
		}
	}
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(currentImage);
	SDL_DestroyRenderer(renderTarget);
	gBGTexture.free();
	gCharTexture.free();
	gGameOver.free();
	gTileTexture.free();
	gTitleBG.free();
	window = nullptr;
	currentImage = nullptr;
	renderTarget = nullptr;

	SDL_Quit();

		
	}
	return 0;
}

