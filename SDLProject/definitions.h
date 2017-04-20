#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stdafx.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include <fstream>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 960
#define SPRITE_SIZE    32
#define LEVEL_WIDTH 1280
#define LEVEL_HEIGHT 960

enum GameStates {
	STATE_NULL,
	STATE_TITLE,
	STATE_ROOM1,
	STATE_ROOM2,
	STATE_EXIT,
};

int stateID = STATE_NULL;
int nextState = STATE_NULL;

const int CHAR_WIDTH = 32;
const int CHAR_HEIGHT = 50;
const int CHAR_VEL = 10;

const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int TOTAL_TILES = 1200;
const int TOTAL_TILE_SPRITES = 3;


const int TILE_GRASS = 1;
const int TILE_BRICK = 2;

const int FPS = 60;
const Uint8 *keyState;





