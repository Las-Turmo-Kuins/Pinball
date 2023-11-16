#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void Create();
public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	PhysBody* coins[4];
	PhysBody* triangulos[2];
	PhysBody* barriles[4];
	PhysBody* map;
	PhysBody* sensor;
	PhysBody* BarrilSensors1;
	PhysBody* HatSensors2;
	PhysBody* CoinsSensors3;
	PhysBody* right;
	PhysBody* right_circle;
	PhysBody* left;
	PhysBody* left_circle;

	//Bonus
	bool Bonus1;


	PhysBody* spring;
	PhysBody* springSurface;
	SDL_Texture* springTex;

	PhysBody* limit;
	SDL_Texture* flippR;
	SDL_Texture* flippL;
	int flippery = 585;

	bool sensed;

	SDL_Texture* mapa;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;

	

	//Rectangulo amarillo del Score
	SDL_Texture* YellowRectangle;

	SDL_Texture* canonball;


	int lives = 4;
	int score = 0000;
	int Bbonus = 0;
	int scoreFont = -1;
	int scoreFont2 = -1;
	char scoreText[10] = { "\0" };
};
