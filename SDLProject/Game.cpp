#include "stdafx.h"
#include "Game.h"
#include <SDL.h>
#include "definitions.h"
#include <string>

//Screen dimension constants









Game::Game()
{
	//TODO Start Stuff
}


Game::~Game()
{
	//TODO Delete stuff
}

void Game::Run() {
	
	
	GameLoop();
	
	return;

}

bool Game::Startup() {
	
	bool success = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		mWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (mWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			//Get window surface
			mSurface = SDL_GetWindowSurface(mWindow);
		}
	}
	return success;
}

bool Game::LoadMedia() {

	
	//Loading success flag
	bool success = true;

	//Load default surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("C:/Users/Sam Minton/OneDrive - Nottingham Trent University/SOFT10101/SDLProject/SDLProject/images/press.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
	{
		printf("Failed to load default image!\n");
		success = false;
	}
	

	

	return success;

}

SDL_Surface* loadSurface(std::string path)
{
	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}

	return loadedSurface;
}

bool Game::GameLoop() {

	//Start up SDL and create window
	if (!Startup())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!LoadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool quit = false;
			SDL_Event e;
			//Set default current surface
			gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
			while (!quit) 
			{
				while (SDL_PollEvent(&e) != 0) 
				{
					if (e.type == SDL_QUIT) 
					{
						quit = true;
					}
					
						
					
				}
				//Apply the current image
				SDL_BlitSurface(gCurrentSurface, NULL, mSurface, NULL);

				//Update the surface
				SDL_UpdateWindowSurface(mWindow);
			}
		}
	}

	Shutdown();

	return 0;	


}

void Game::Shutdown() {
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i)
	{
		SDL_FreeSurface(gKeyPressSurfaces[i]);
		gKeyPressSurfaces[i] = NULL;
	}

	SDL_DestroyWindow(mWindow);
	mWindow = NULL;
	
	SDL_Quit();
	
}
