#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Timer.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:
	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	int pickCoinFxId;

	int respawn = 2;


	Animation* currentAnimation;

	Animation player;

	Animation player_walk;

	Animation player_speed;

	Animation player_jump;
	Animation player_inair;

	Animation player_inv;

	Animation player_hurt;

	Animation player_dead;

	Animation player_attack;


	pugi::xml_node config;

	bool leftmode = false;
	bool rightmode = true;

	bool moving = false;

	bool atacking = false;

	bool jumping = false;

	bool inground = false;

	bool Godmode=false;

	bool canmove = true;

	bool dead = false;

	bool check_1 = false;
	bool check_2 = false;

	uint32 startTime;
};

#endif // __PLAYER_H__