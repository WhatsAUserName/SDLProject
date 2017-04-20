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

class Char {
public:
	Char();

	void handleEvent(SDL_Event&e);

	void move();

	void render(int camX, int camY);

	int getPosX();

	int getPosY();

private:

	SDL_Rect playerPos;

	int velX, velY;

};


bool init();

class GameState
{
public:
	 
	
	
	virtual ~GameState() {};

	virtual void handleEvents(SDL_Event&e) = 0;

	virtual void logic() = 0;

	virtual void render() = 0;
	
};

class Title : public GameState {

private:

	Title();
	~Title();

	void handleEvents(SDL_Event&e);
	void logic();
	void render();
};

class Room1 : public GameState {
private:
	Room1();
	~Room1();

	void doEverything();
};

void setNextState(int setState);

void changeState();

