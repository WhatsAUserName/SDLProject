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

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SPRITE_SIZE    32

const int LEVEL_WIDTH = 640;
const int LEVEL_HEIGHT = 480;

const int CHAR_WIDTH = 32;
const int CHAR_HEIGHT = 50;

const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int TOTAL_TILES = 300;
const int TOTAL_TILE_SPRITES = 3;


const int TILE_GRASS = 1;
const int TILE_BRICK = 2;

const int FPS = 60;
const Uint8 *keyState;





