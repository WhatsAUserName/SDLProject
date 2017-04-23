// SDLProject.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "definitions.h"

//Initialise everything to be used later
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
WTexture gWinBG;



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

	//This will be the final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(filepath.c_str());
	if (loadedSurface == NULL)
	{
		std::cout << "Unable to load image %s! SDL_image Error: %s\n" << filepath.c_str() << IMG_GetError() << std::endl;
	}
	else
	{
		
		//Create texture from the surface 
		newTexture = SDL_CreateTextureFromSurface(renderTarget, loadedSurface);
		if (newTexture == NULL)
		{
			std::cout << "Unable to create texture from %s! SDL Error: %s\n" << filepath.c_str() << SDL_GetError() << std::endl;
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		//Free old surface
		SDL_FreeSurface(loadedSurface);
	}
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
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopyEx(renderTarget, texture, clip, &renderQuad, angle, center, flip);
}

int WTexture::getWidth() {
	return mWidth;
}

int WTexture::getHeight() {

	return mHeight;
}



Tile::Tile(int x, int y, int tileType)
{
	tileBox.x = x;
	tileBox.y = y;

	//Set the collision box
	tileBox.w = TILE_WIDTH;
	tileBox.h = TILE_HEIGHT;

	//Get the tile type
	tType = tileType;
}

void Tile::render(SDL_Rect& camera)
{
	if (checkCollision(camera, tileBox))
	{
		gTileTexture.render(tileBox.x - camera.x, tileBox.y - camera.y, &TileClips[tType]);
	}
}

int Tile::getType()
{
	return tType;
}

SDL_Rect Tile::getTileBox()
{
	return tileBox;
}



bool touchGround(SDL_Rect box, Tile* tiles[])
{
	//Go through the tiles
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		//If the tile is a ground tile
		if ((tiles[i]->getType() >= TILE_GRASS) && (tiles[i]->getType() <= TILE_BRICK))
		{
			//If the collision box touches the ground 
			if (checkCollision(box, tiles[i]->getTileBox()))
			{
				return true;
			}
		}
	}

	return false;
}

bool touchGoal(SDL_Rect box, Tile* tiles[])
{
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		if ((tiles[i]->getType() >= TILE_GOLD) && (tiles[i]->getType() <= TILE_GOLD))
		{
			if (checkCollision(box, tiles[i]->getTileBox()))
			{
				return true;
			}
		}
	}
	return false;
}

//Used to load player sprite texture, as it is not a WTexture
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

//Code adapted from LazyFoo's tiling tutorial
bool setTiles(Tile *tiles[]) {
	bool tilesLoaded = true;

	int x = 0;
	int y = 0;

	//Open the map

	std::ifstream map("mapdata/mapbig.txt");

	if (!map.is_open()) {
		std::cout << "Unable to load map" << std::endl;
		tilesLoaded = false;
	}
	else
	{
		
		for (int i = 0; i < TOTAL_TILES; i++)
		{
			int tileType = -1;

			//read the map to tileType
			map >> tileType;

			if (map.fail())
			{
				std::cout << "Error loading map, end of file error" << std::endl;
				tilesLoaded = false;
				break;
			}
			//If valid map then load it to new tile
			if ((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES))
			{
				tiles[i] = new Tile(x, y, tileType);
			}
			else
			{
				std::cout << "Error loading map" << std::endl;
				tilesLoaded = false;
				break;
			}
			//cycle through the level width based on tile size, to set the tiles next to eachother
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
			TileClips[TILE_GOLD].x = 0;
			TileClips[TILE_GOLD].y = 160;
			TileClips[TILE_GOLD].w = TILE_WIDTH;
			TileClips[TILE_GOLD].h = TILE_HEIGHT;
		}
	}
	map.close();
	return tilesLoaded;
}

bool loadMedia(Tile* tiles[]) {
	bool success = true;
	if (!gBGTexture.loadFromFile("images/BG.jpg")) {
		std::cout << "failed to load background image" << std::endl;
		success = false;
	}
	if (!gTitleBG.loadFromFile("images/title1.png")) {
		std::cout << "failed to load title image" << std::endl;
		success = false;
	}
	if (!gGameOver.loadFromFile("images/gameover.png")) {
		std::cout << "failed to load game over image" << std::endl;
		success = false;
	}
	if (!gWinBG.loadFromFile("images/win.png")) {
		std::cout << "failed to load win image" << std::endl;
		success = false;
	}
	if (!gTileTexture.loadFromFile("images/tiles.png")) {
		std::cout << "failed to load tile" << std::endl;
		success = false;
	}
	if (!setTiles(tiles)) {
		std::cout<<"fail" << std::endl;
		success = false;
	}
	return success;
}

//Code found in LazyFoo's collision tutorial
bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//initialise sides of SDL_Rects
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

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




bool init() {
	bool success = true;
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout<<"SDL initialisation failed, aborting" << std::endl;
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
		std::cout<<"Failed initilising" << std::endl;
	}
	else {

		
		SDL_QueryTexture(currentImage, NULL, NULL, &textureWidth, &textureHeight);
		//clip player sprite sheet, to prepare for animation
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
			std::cout<<"Failed to load media!\n" << std::endl;
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
				//Use different states to have multiple environments
				if (stateID == STATE_ROOM1) {
					//handle key input to move player
					if (keyState[SDL_SCANCODE_RIGHT]) {
						playerRect.y = 100;
						playerPos.x += movSpeed * deltaTime;
					}
					if (keyState[SDL_SCANCODE_LEFT])
					{
						playerRect.y = 50;
						playerPos.x -= movSpeed *deltaTime;
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
					if (touchGoal(playerPos, tileSet)) {
						stateID = STATE_ROOM2;
					}

					if (playerPos.y <= groundlevel) {
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
				//Title screen
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
					//Main game room
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
					//Win splash screen
				case STATE_ROOM2:
					SDL_SetRenderDrawColor(renderTarget, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(renderTarget);
					gWinBG.render(0, 0);
					SDL_RenderPresent(renderTarget);
					if (keyState[SDL_SCANCODE_ESCAPE]) {
						stateID = STATE_EXIT;
					}
					if (keyState[SDL_SCANCODE_SPACE]) {
						playerPos.x = 0;
						playerPos.y = groundlevel - TILE_HEIGHT;
						stateID = STATE_TITLE;
					}
					break;
					//Game over screen
				case STATE_NULL:
					SDL_SetRenderDrawColor(renderTarget, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(renderTarget);
					gGameOver.render(0, 0);
					SDL_RenderPresent(renderTarget);
					playerPos.x = 0;
					playerPos.y = groundlevel - TILE_HEIGHT;
					if (keyState[SDL_SCANCODE_SPACE]) {
						stateID = STATE_TITLE;
					}
					if (keyState[SDL_SCANCODE_ESCAPE]) {
						stateID = STATE_EXIT;
					}
					break;
					//Exit game state
				case STATE_EXIT:
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
					break;
				}
		}
	}
	

		
	}
	return 0;
}

