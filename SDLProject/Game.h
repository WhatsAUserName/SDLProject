#pragma once
class Game
{
public:
	Game();
	virtual ~Game();
	void Run();

private:
	bool Startup();
	bool LoadMedia();
	bool GameLoop();
	void Shutdown();


};

