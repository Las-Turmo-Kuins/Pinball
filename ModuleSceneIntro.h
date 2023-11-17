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
	void Destroy();
public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	PhysBody* coins[4];
	PhysBody* triangulos[2];
	PhysBody* map;
	PhysBody* sensor;
	PhysBody* right;
	PhysBody* right_circle;
	PhysBody* left;
	PhysBody* left_circle;
	PhysBody* end;
	PhysBody* toDestroy;

	PhysBody* spring;
	PhysBody* springSurface;
	PhysBody* limit;
	SDL_Texture* flippR;
	SDL_Texture* flippL;
	int flippery = 585;

	bool sensed;
	bool create = false;

	SDL_Texture* mapa;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
	int lives = 0;
};
