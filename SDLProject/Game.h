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

	SDL_Rect getTileBox();

private:
	SDL_Rect tileBox;

	int tType;
};


bool init();


bool loadMedia(Tile *tile[]);

bool checkCollision(SDL_Rect a, SDL_Rect b);

bool touchGoal(SDL_Rect box, Tile *tiles[]);

bool touchGround(SDL_Rect box, Tile *tiles[]);

bool setTiles(Tile* tiles[]);

