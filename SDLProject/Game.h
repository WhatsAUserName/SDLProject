#pragma once
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


class Dot
{
public:
	

	

	//Initializes the variables
	Dot();

	
	//Centers the camera over the dot
	void setCamera(SDL_Rect& camera);


private:
	//Collision box of the dot
	SDL_Rect mBox;

	//The velocity of the dot
	int mVelX, mVelY;
};

bool init();


