// SDLProject.cpp : Defines the entry point for the console application.
//TODO Add gravity, and jumping state.

#include "stdafx.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "definitions.h"

SDL_Rect gTileClips[TOTAL_TILE_SPRITES];

SDL_Window *window = nullptr;
SDL_Texture *currentImage = nullptr;
SDL_Renderer *renderTarget = nullptr;




bool checkCollision(SDL_Rect a, SDL_Rect b);

bool touchGround(SDL_Rect box, Tile* tiles[]);

WTexture gTileTexture;


WTexture::WTexture() {

	texture = NULL;
	mWidth = 0;
	mHeight = 0;

}

WTexture::~WTexture() {

	free();
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
		gTileTexture.render(tBox.x - camera.x, tBox.y - camera.y, &gTileClips[mType]);
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

	std::ifstream map("mapdata/map.txt");

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
			gTileClips[TILE_GRASS].x = 0;
			gTileClips[TILE_GRASS].y = 0;
			gTileClips[TILE_GRASS].w = TILE_WIDTH;
			gTileClips[TILE_GRASS].h = TILE_HEIGHT;
		}
	}
	map.close();
	return tilesLoaded;
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
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

	//If any of the sides from A are outside of B
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

	//If none of the sides from A are outside B
	return true;
}


/*bool init() {
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
		window = SDL_CreateWindow("Try", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		currentImage = LoadTexture("images/sprite.png", renderTarget);

		SDL_QueryTexture(currentImage, NULL, NULL, &textureWidth, &textureHeight);
		success = true;
	}
	return success;
}*/




int main(int argc, char* args[]) {

	
	const int FPS = 60;
	float frameTime = 0;
	int prevTime = 0;
	int currentTime = 0;
	float deltaTime = 0;
	float movSpeed = 100.0f;
	const Uint8 *keyState;
	bool jumping = false;
	float jumpvel = 5;
	float gravity = 0.2f;
	int groundlevel = 430;
	SDL_Rect playerRect;
	SDL_Rect playerPos;
	playerPos.x = 0;
	playerPos.y = 430;
	playerPos.w = 32;
	playerPos.h = 50;
	int frameWidth, frameHeight;
	int textureWidth, textureHeight;

	
	

	SDL_Init(SDL_INIT_VIDEO);

	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags)&imgFlags))
	{
		std::cout << "Error 3" << IMG_GetError() << std::endl;
	}
	window = SDL_CreateWindow("Try", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	currentImage = LoadTexture("images/sprite.png", renderTarget);

	SDL_QueryTexture(currentImage, NULL, NULL, &textureWidth, &textureHeight);

	frameWidth = textureWidth / 4;
	frameHeight = textureHeight / 4;

	playerRect.x = playerRect.y = 0;
	playerRect.w = frameWidth;
	playerRect.h = frameHeight;
	//Set Background color
	SDL_SetRenderDrawColor(renderTarget, 0xFD, 0, 0, 0xFD);

	Tile* tileSet[TOTAL_TILES];

	bool isRunning = true;
	SDL_Event e;

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
		keyState = SDL_GetKeyboardState(NULL);

		if (keyState[SDL_SCANCODE_RIGHT]) {
			playerRect.y = 100;
			playerPos.x += movSpeed *deltaTime;
		}
		else if (keyState[SDL_SCANCODE_LEFT])
		{
			playerRect.y = 50;
			playerPos.x -= movSpeed *deltaTime;
		}
		/*else if (keyState[SDL_SCANCODE_UP]) {
			playerRect.y = 150;
			playerPos.y -= movSpeed *deltaTime;
		}
		else if (keyState[SDL_SCANCODE_DOWN]) {
			playerRect.y = 0;
			playerPos.y += movSpeed *deltaTime;*/
		else if (keyState[SDL_SCANCODE_SPACE]&&!jumping) {
			jumping = true;
		}
		if (jumping) {
			playerPos.y -= jumpvel;
			jumpvel -= gravity;
		}
		if (playerPos.y == groundlevel) {
			jumpvel = 5;
			jumping = false;
		}
		frameTime += deltaTime;
		if (frameTime >= 0.25f)
		{
			frameTime = 0;
			playerRect.x += frameWidth;
			if (playerRect.x >= textureWidth)
			{
				playerRect.x = 0;
			}
		}


		SDL_RenderClear(renderTarget);
		SDL_RenderCopy(renderTarget, currentImage, &playerRect, &playerPos);
		SDL_RenderPresent(renderTarget);
	}
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(currentImage);
	SDL_DestroyRenderer(renderTarget);
	window = nullptr;
	currentImage = nullptr;
	renderTarget = nullptr;

	SDL_Quit();

	return 0;

	
}





/*void Char::setCamera(SDL_Rect& camera) {
 	camera.x = (cPos.x + CHAR_WIDTH / 2) - SCREEN_WIDTH / 2;
 	camera.y = (cPos.y + CHAR_HEIGHT / 2) - SCREEN_HEIGHT / 2;
 
 	//Keep the camera in bounds
 	if (camera.x < 0)
 	{
 		camera.x = 0;
 	}
 	if (camera.y < 0)
 	{
 		camera.y = 0;
 	}
 	if (camera.x > LEVEL_WIDTH - camera.w)
 	{
 		camera.x = LEVEL_WIDTH - camera.w;
 	}
 	if (camera.y > LEVEL_HEIGHT - camera.h)
 	{
 		camera.y = LEVEL_HEIGHT - camera.h;
 	}
 }*/