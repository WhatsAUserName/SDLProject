// SDLProject.cpp : Defines the entry point for the console application.
//TODO Add gravity, and jumping state.

#include "stdafx.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "definitions.h"


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

int main(int argc, char* args[]) {

	//Game theGame;
	//theGame.Run();


	//return 0;

	
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