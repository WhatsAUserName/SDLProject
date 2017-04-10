// SDLProject.cpp : Defines the entry point for the console application.
//TODO Add gravity, and jumping state.

#include "stdafx.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "definitions.h"

class WTexture {
public:
	
	WTexture();

	~WTexture();

	bool loadFromFile(std::string path);

	void free();

	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	int getWidth();
	int getHeight();

private:
	SDL_Texture* texture;

	int mWidth;
	int mHeight;
};

SDL_Rect cSpriteClips[4];

class Tile {
public:
	Tile(int x, int y, int tileType);

	void render(SDL_Rect& camera);

	int getType();

	SDL_Rect getBox();

private:
	SDL_Rect tBox;

	int mType;
};

class Char {
public:
	static const int CHAR_WIDTH = 32;
	static const int CHAR_HEIGHT = 50;
	const float CHAR_VEL = 1;

	Char();

	void handleEvent(SDL_Event& e);

	void move(Tile *tiles[]);

	void setCamera(SDL_Rect& camera);

	void render(SDL_Rect& camera);

private:
	SDL_Rect cBox;
	SDL_Rect cPos;

	float charPosx, charPosy;

};

bool init();

bool loadMedia(Tile* tiles[]);

void close(Tile* tiles[]);

bool checkCollision(SDL_Rect a, SDL_Rect b);

bool touchGround(SDL_Rect box, Tile* tiles[]);

bool setTiles(Tile* tiles[]);


float frameTime = 0;
int prevTime = 0;
int currentTime = 0;
float deltaTime = 0;
float movSpeed = 101.0f;


SDL_Window* gWindow = nullptr;
SDL_Renderer* gRender = nullptr;

WTexture gCharTexture;
WTexture gTileTexture;

SDL_Rect gTileClips[TOTAL_TILE_SPRITES];

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
		newTexture = SDL_CreateTextureFromSurface(gRender, loadedSurface);
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
	SDL_RenderCopyEx(gRender, texture, clip, &renderQuad, angle, center, flip);
}

int WTexture::getWidth() {
	return mWidth;
}

int WTexture::getHeight() {

	return mHeight;
}

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


Char::Char() {

	cPos.x = 0;
	cPos.y = 430;
	cPos.w = CHAR_WIDTH;
	cPos.h = CHAR_HEIGHT;
	

	cBox.x = cBox.y = 0;
	

	charPosx = 0;
	charPosy = 0;
}

void Char::handleEvent(SDL_Event& e) {
	
	   //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: charPosy -= CHAR_VEL; break;
            case SDLK_DOWN: charPosy += CHAR_VEL; break;
            case SDLK_LEFT: charPosx -= CHAR_VEL; break;
            case SDLK_RIGHT: charPosx += CHAR_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: charPosy += CHAR_VEL; break;
            case SDLK_DOWN: charPosy -= CHAR_VEL; break;
            case SDLK_LEFT: charPosx += CHAR_VEL; break;
            case SDLK_RIGHT: charPosx -= CHAR_VEL; break;
        }
    }
}

void Char::move(Tile *tiles[]) {
	cPos.x += charPosx;
	//cBox.y += charPosy;
}

void Char::setCamera(SDL_Rect& camera) {
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
}

void Char::render(SDL_Rect& camera) {
	gCharTexture.render(cPos.x - camera.x, cPos.y - camera.y);
}

bool init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Sdl initialisation failed, aborting");
		success = false;
	}
	else
	{
		gWindow = SDL_CreateWindow("Aventures of Queueue", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window creation failure, aborting");
			success = false;
		}
		gRender = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		if (gRender == NULL) {
			printf("Failed to create renderer, aborting");
			success = false;
		}
		else
		{
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
		}

	}
	return success;
}

bool loadMedia(Tile* tiles[]) {
	bool success = true;
	
	if (!gCharTexture.loadFromFile("images/sprite.bmp")) {
		printf("failed to load sprite sheet");
		success = false;
	}
	if (!gTileTexture.loadFromFile("images/grass.bmp")) {
		printf("failed to load tile");
			success = false;
	}
	if (!setTiles(tiles)) {
		printf("fail");
		success = false;
	}
	return success;
}

void close(Tile* tiles[])
{
	//Deallocate tiles
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		if (tiles[i] == NULL)
		{
			delete tiles[i];
			tiles[i] = NULL;
		}
	}

	//Free loaded images
	gCharTexture.free();
	gTileTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRender);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRender = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
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

bool setTiles(Tile* tiles[])
{
	//Success flag
	bool tilesLoaded = true;

	//The tile offsets
	int x = 0, y = 0;

	//Open the map
	std::ifstream map("mapdata/map.txt");

	//If the map couldn't be loaded
	if (!map.is_open())
	{
		printf("Unable to load map file!\n");
		tilesLoaded = false;
	}
	else
	{
		//Initialize the tiles
		for (int i = 0; i < TOTAL_TILES; ++i)
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if (map.fail())
			{
				//Stop loading map
				printf("Error loading map: Unexpected end of file!\n");
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if ((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES))
			{
				tiles[i] = new Tile(x, y, tileType);
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf("Error loading map: Invalid tile type at %d!\n", i);
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if (x >= LEVEL_WIDTH)
			{
				//Move back
				x = 0;

				//Move to the next row
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




int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//The level tiles
		Tile* tileSet[TOTAL_TILES];

		//Load media
		if (!loadMedia(tileSet))
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The character that will be moving around on the screen
			Char character;

			//Level camera
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					//Handle input for the character
					character.handleEvent(e);
				}

				//Move the character
				character.move(tileSet);
				character.setCamera(camera);

				//Clear screen
				SDL_SetRenderDrawColor(gRender, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRender);

				//Render level
				for (int i = 0; i < TOTAL_TILES; ++i)
				{
					tileSet[i]->render(camera);
				}

				//Render character
				character.render(camera);

				//Update screen
				SDL_RenderPresent(gRender);
				
			}
			
		}
	

		//Free resources and close SDL
		close(tileSet);
	}

	return 0;


//int main(int argc, char* args[]) {

	//Game theGame;
	//theGame.Run();


	//return 0;
/*
	
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


	SDL_Window *window = nullptr;
	SDL_Texture *currentImage = nullptr;
	SDL_Renderer *renderTarget = nullptr;
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
			playerPos.y += movSpeed *deltaTime;
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

	*/
}