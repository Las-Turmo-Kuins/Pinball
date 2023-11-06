#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateCircle(position.x + 30, position.y + 30, 13, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	player.LoadAnimations("player");
	player_speed.LoadAnimations("player_speed");
	player_jump.LoadAnimations("player_jump");
	player_dead.LoadAnimations("player_dead");

	////Normal atack
	//player_attack.PushBack({ 0,259, 32, 32 });
	//player_attack.PushBack({ 31,259, 32, 32 });
	//player_attack.PushBack({ 63,259, 32, 32 });
	//player_attack.PushBack({ 95,259, 32, 32 });
	//player_attack.PushBack({ 126,259, 32, 32 });
	//player_attack.PushBack({ 163,259, 32, 32 });
	//player_attack.PushBack({ 192,259, 32, 32 });
	//player_attack.loop = false;
	//player_attack.speed = 0.3f;

	//Invisible
	//player_inv.PushBack({ 31, 195, 32, 32 });
	//player_inv.PushBack({ 63, 195, 32, 32 });
	//player_inv.PushBack({ 95, 195, 32, 32 });
	//player_inv.PushBack({ 1000, 100, 32, 32 });
	//player_inv.PushBack({ 95, 193, 32, 32 });
	//player_inv.PushBack({ 95, 193, 32, 32 });
	//player_inv.PushBack({ 95, 193, 32, 32 });
	//player_inv.PushBack({ 127, 128, 32, 32 });
	//player_inv.speed = 0.2f;
	//player_inv.loop = false;

	////Damage resive
	//player_hurt.PushBack({ 0,227, 32, 32 });
	//player_hurt.PushBack({ 31,227, 32, 32 });
	//player_hurt.PushBack({ 31,227, 32, 32 });
	//player_hurt.speed = 10.0f;
	//player_hurt.loop = false;

	currentAnimation = &player;

	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 veljump = pbody->body->GetLinearVelocity();


	if (!atacking && !jumping && inground && !dead && !Godmode)
	{
		currentAnimation = &player;
	}
	//Check
	if (app->render->camera.x <= -2900)
	{
		check_1 = true;
		check_2 = false;
	}
	if (app->render->camera.x <= -5540)
	{
		check_2 = true;
		check_1 = false;
	}
	//Debug inputs
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !dead)
	{
		Godmode = !Godmode;
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN && !dead)
	{
		pbody->body->SetTransform({ PIXEL_TO_METERS(-700 + 16), PIXEL_TO_METERS(700) }, 0);
		app->render->camera.x = 0;
	}
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && !dead)
	{
		pbody->body->SetTransform({ PIXEL_TO_METERS(2190 + 16), PIXEL_TO_METERS(700) }, 0);
		app->render->camera.x = -2900;
	}
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && !dead)
	{
		pbody->body->SetTransform({ PIXEL_TO_METERS(4815 + 16), PIXEL_TO_METERS(460) }, 0);
		app->render->camera.x = -5540;
	}
	//Movement inputs
	if (app->input->GetKey(SDL_SCANCODE_A)==KEY_IDLE && app->input->GetKey(SDL_SCANCODE_D)==KEY_IDLE)
	{
		veljump.x = 0;
	}

	if (Godmode && !dead)
	{
		speed = 0.5f;
		veljump = b2Vec2(0.0, -0.1675);
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			veljump.y = -5;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			veljump.y = 5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			veljump.x = -speed * dt;
			app->render->camera.x = -(position.x - 60);
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			veljump.x = speed * dt;
			app->render->camera.x = -(position.x - 60);
		}
		currentAnimation = &player;
	}

	if (canmove && !Godmode && !dead)
	{
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !atacking)
		{
			rightmode = true;
			leftmode = false;
			veljump.x = 3;
			app->render->camera.x = -(position.x - 90);
			if (inground && !jumping)
			{
				currentAnimation = &player_speed;
			}
			if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && !jumping)
			{
				veljump.x = 4;
			}
			else
			{
				veljump.x = 3;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !atacking)
		{
			leftmode = true;
			rightmode = false;
			speed = -speed;
			veljump.x = -3;
			app->render->camera.x = -(position.x - 90);
			if (inground && !jumping)
			{
				currentAnimation = &player_speed;
			}
			if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && !jumping)
			{
				veljump.x = -4;
			}
			else
			{
				veljump.x = -3;
			}
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN && respawn>0 && dead)
	{
		currentAnimation->Reset();
		if (!check_1 && !check_2)
		{
			pbody->body->SetTransform({ PIXEL_TO_METERS(-700 + 16), PIXEL_TO_METERS(700) }, 0);
			app->render->camera.x = 0;
		}
		if (check_1)
		{
			pbody->body->SetTransform({ PIXEL_TO_METERS(2190 + 16), PIXEL_TO_METERS(700) }, 0);
			app->render->camera.x = -2900;
		}
		if (check_2)
		{
			pbody->body->SetTransform({ PIXEL_TO_METERS(4815 + 16), PIXEL_TO_METERS(460) }, 0);
			app->render->camera.x = -5540;
		}

		canmove = true;
		dead = false;
		rightmode = true;
		leftmode = false;
		respawn--;
	}
	if (!jumping && inground && !dead)
	{
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN )
		{
			jumping = true;
			inground = false;
			veljump.y = -6;
			pbody->body->SetLinearVelocity(veljump);
			if (jumping)
			{
				currentAnimation = &player_jump;
				currentAnimation->loopCount = 0;
			}
		}
	}

	//Ability inputs
	//if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT && !dead && !jumping)
	//{
	//	atacking = true;
	//	if (atacking)
	//	{
	//		canmove = false;
	//		currentAnimation->Reset();
	//		currentAnimation = &player_attack;
	//		currentAnimation->loopCount = 0;
	//	}
	//}

	//Finished animations

	if (dead)
	{
		currentAnimation = &player_dead;
		currentAnimation->loopCount = 0;
	} 


	//if (currentAnimation == &player_attack && currentAnimation->HasFinished()) {
	//	atacking = false;
	//	canmove = true;
	//}

	if (currentAnimation == &player_jump && currentAnimation->HasFinished() && inground) {
		currentAnimation->Reset();
		jumping = false;
	}

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(veljump);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;


	currentAnimation->Update();

	if (rightmode == true)
	{
		app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	}
	if (leftmode == true)
	{
		app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame(), SDL_FLIP_HORIZONTAL);
	}


	return true;
}

bool Player::CleanUp()
{
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		inground = true;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;

	case ColliderType::ENEMY:
		if (!Godmode)
		{
			dead = true;
			canmove = false;
		}
		break;

	case ColliderType::WALL:
		LOG("Colission WALL");
		inground = false;
		break;
	}

}