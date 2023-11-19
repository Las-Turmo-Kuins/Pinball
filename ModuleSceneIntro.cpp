#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"

using namespace std;

#include <fstream>
#include <iostream>
#include <string>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;
	flippR = App->textures->Load("pinball/flipperR.png");
	flippL = App->textures->Load("pinball/flipperL.png");
	mapa = App->textures->Load("pinball/Fondo_pinball.png");
	bola = App->textures->Load("pinball/Canonball.png");
	App->audio->PlayMusic("pinball/Bonus.ogg");

	YellowRectangle = App->textures->Load("pinball/YellowRectangle.png");
	numsscore = App->textures->Load("pinball/nums.png");

	//Score
	char lookupTable[] = { "0123456789 0123456789" };
	scoreFont = App->fonts->Load("pinball/fonts/font1.png", lookupTable, 2);

	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	int map_data[102] = {
	92, 637,
	92, 617,
	0, 540,
	1, 442,
	12, 430,
	1, 417,
	0, 355,
	72, 285,
	7, 217,
	0, 187,
	17, 122,
	2, 102,
	0, 87,
	11, 67,
	27, 57,
	45, 59,
	58, 69,
	64, 82,
	96, 76,
	85, 50,
	114, 35,
	149, 24,
	178, 22,
	211, 25,
	239, 32,
	265, 45,
	297, 70,
	320, 94,
	342, 128,
	360, 174,
	358, 233,
	358, 587,
	331, 586,
	332, 199,
	326, 165,
	313, 135,
	269, 88,
	262, 92,
	286, 116,
	310, 157,
	296, 173,
	309, 184,
	319, 200,
	325, 213,
	327, 227,
	327, 340,
	312, 352,
	327, 365,
	325, 537,
	232, 619,
	232, 637
	};

	
	int coin2[16] = {
	156, 73,
	149, 78,
	149, 87,
	155, 93,
	162, 93,
	169, 87,
	169, 79,
	162, 72
	};
	int coin3[16] = {
	199, 73,
	194, 78,
	194, 85,
	198, 93,
	206, 93,
	212, 87,
	213, 78,
	208, 72
	};
	
	int coin5[16] = {
	50, 17,
	44, 23,
	44, 31,
	50, 37,
	58, 37,
	64, 31,
	64, 23,
	58, 17
	};
	int coin6[16] = {
	-3, -8,
	-9, -2,
	-9, 6,
	-3, 12,
	5, 12,
	11, 6,
	11, -2,
	5, -8
	};
	int trianguloiz[22] = {
	33, 453,
	31, 465,
	35, 501,
	31, 525,
	56, 539,
	73, 560,
	81, 559,
	83, 543,
	62, 504,
	53, 477,
	41, 456
	};
	int triangulode[22] = {
	294, 453,
	283, 456,
	265, 503,
	244, 547,
	247, 559,
	256, 558,
	270, 538,
	294, 525,
	297, 514,
	292, 470,
	298, 460
	};
	int bola1[16] = {//primera grande
	124, 143,
	107, 155,
	107, 170,
	124, 188,
	135, 187,
	150, 174,
	151, 159,
	135, 143
	};
	// Pivot 0, 0
	int bola2[16] = {//segunda grande
		188, 198,
		173, 213,
		173, 226,
		188, 244,
		200, 244,
		217, 228,
		216, 215,
		201, 197
	};
	// Pivot 0, 0
	int bola3[16] = {// tercera bola
		223, 130,
		209, 145,
		209, 158,
		224, 175,
		238, 174,
		253, 159,
		252, 144,
		238, 131
	};
	int bola4[16] = {//bola peque�a
		64, 120,
		53, 130,
		53, 141,
		63, 152,
		71, 153,
		85, 142,
		85, 132,
		72, 120
	};

	int mapa2[18] = {
	271, 154,
	272, 145,
	279, 136,
	287, 126,
	306, 148,
	295, 160,
	295, 150,
	286, 140,
	277, 144
	};
	//mapa
	map = App->physics->CreateChain(0, 0, map_data, 102);
	map->body->SetType(b2_staticBody);
	map->body->GetFixtureList()->SetRestitution(0.5f);


	map2 = App->physics->CreateChain(0, 0, mapa2, 18);
	map2->body->SetType(b2_staticBody);
	map2->body->GetFixtureList()->SetRestitution(0.5f);

	end = App->physics->CreateRectangleSensor(83, 639, 400, 10);
	//coins
	
	coins[0] = App->physics->CreateBumper(158, 83, 9 );
	coins[1] = App->physics->CreateBumper(202, 83, 9);
	coins[0]->listener = this;
	coins[1]->listener = this;
	coins[0]->collidertype = ColliderType::MONEDAS;
	coins[1]->collidertype = ColliderType::MONEDAS;
	coins[2] = App->physics->CreateBumper(17, 393, 9);
	
	coins[5] = App->physics->CreateBumper(311, 315, 9);
	coins[6] = App->physics->CreateBumper(311, 392, 9);
	
	coins[2]->listener = this;
	coins[5]->listener = this;
	coins[6]->listener = this;

	coins[2]->collidertype = ColliderType::MONEDAS;
	coins[5]->collidertype = ColliderType::MONEDAS;
	coins[6]->collidertype = ColliderType::MONEDAS;	
	;

	coins[3] = App->physics->CreateBigBumper(32, 88, 10);
	coins[4] = App->physics->CreateBigBumper(283, 152, 10);
	coins[3]->listener = this;
	coins[4]->listener = this;
	coins[3]->collidertype = ColliderType::MONEDAS2;
	coins[4]->collidertype = ColliderType::MONEDAS2;
	
	

	//triangulos laterales
	triangulos[0] = App->physics->CreateReboundChain(0, 0, trianguloiz, 22);
	triangulos[1] = App->physics->CreateReboundChain(0, 0, triangulode, 22);
	triangulos[0]->listener = this;
	triangulos[1]->listener = this;
	triangulos[0]->collidertype = ColliderType::SOMBREROS;
	triangulos[1]->collidertype = ColliderType::SOMBREROS;

	barriles[0] = App->physics->CreateBumper(128, 166, 23);
	barriles[1] = App->physics->CreateBumper(194, 223, 23);
	barriles[2] = App->physics->CreateBumper(230, 154, 23);
	barriles[3] = App->physics->CreateBumper(68, 137, 15 );
	barriles[0]->listener = this;
	barriles[1]->listener = this;
	barriles[2]->listener = this;
	barriles[3]->listener = this;
	barriles[0]->collidertype = ColliderType::BARRIL;
	barriles[1]->collidertype = ColliderType::BARRIL;
	barriles[2]->collidertype = ColliderType::BARRIL;
	barriles[3]->collidertype = ColliderType::BARRIL;
	//flippers
	
	//right flippers
	right = App->physics->CreateRectangle(225, flippery, 64, 12, b2_dynamicBody);
	right_circle = App->physics->CreateCircleStatic(225, flippery, 6);

	//left flippers

	left = App->physics->CreateRectangle(100, flippery, 64, 12, b2_dynamicBody);
	left_circle = App->physics->CreateCircleStatic(100, flippery, 6);


	b2RevoluteJointDef rightRevJoint;
	rightRevJoint.bodyA = right->body;
	rightRevJoint.bodyB = right_circle->body;
	rightRevJoint.localAnchorA.Set(PIXEL_TO_METERS(25), 0);
	rightRevJoint.localAnchorB.Set(0, 0);
	rightRevJoint.referenceAngle = 0 * DEGTORAD;
	rightRevJoint.enableLimit = true;
	rightRevJoint.lowerAngle = -30 * DEGTORAD;
	rightRevJoint.upperAngle = 50 * DEGTORAD;

	b2RevoluteJoint* joint_right = (b2RevoluteJoint*)App->physics->world->CreateJoint(&rightRevJoint);


	b2RevoluteJointDef leftRevJoint;
	leftRevJoint.bodyA = left->body;
	leftRevJoint.bodyB = left_circle->body;
	leftRevJoint.localAnchorA.Set(PIXEL_TO_METERS(-25), 0);
	leftRevJoint.localAnchorB.Set(0, 0);
	leftRevJoint.referenceAngle = 0 * DEGTORAD;
	leftRevJoint.enableLimit = true;
	leftRevJoint.lowerAngle = -50 * DEGTORAD;
	leftRevJoint.upperAngle = 30 * DEGTORAD;

	b2RevoluteJoint* joint_left = (b2RevoluteJoint*)App->physics->world->CreateJoint(&leftRevJoint);
	
	spring = App->physics->CreateRectangle(345, 500, 15, 4, b2_dynamicBody);
	springSurface = App->physics->CreateRectangle(345, 575, 15, 10, b2_staticBody);
	spring->body->GetFixtureList()->SetDensity(10.0f);

	b2PrismaticJointDef springJoint;
	springJoint.collideConnected = true;
	springJoint.bodyA = spring->body;
	springJoint.bodyB = springSurface->body;

	springJoint.localAnchorA.Set(0, 0);
	springJoint.localAnchorB.Set(0, -0.65f);
	springJoint.localAxisA.Set(0, -1);
	springJoint.enableLimit = true;
	springJoint.lowerTranslation = -0.02;
	springJoint.upperTranslation = 1;
	(b2PrismaticJoint*)App->physics->world->CreateJoint(&springJoint);

	fstream file;
	file.open("score.txt", ios::in);
	file >> highScore;
	cout << highScore;
	file.close();

	for (int i = 0; i < 10; i++)
	{
		scorerect[i] = {20 * i, 91, 16, 21 };
	}



	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
  App->renderer->Blit(mapa, 0, 0);
  
  
  if (create == true && lives > 0)
  {
	  Create();
  }
  Destroy();
	//
	spring->body->ApplyForce(b2Vec2(0, -20), b2Vec2(0, 0), true);

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		spring->body->ApplyForce(b2Vec2(0, 50), b2Vec2(0, 0), true);
	}
	/*if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)

	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}*/

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 25, b2_dynamicBody));
		circles.getLast()->data->listener = this;
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50, b2_dynamicBody));
	}

	if ((App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) /*&& (lives != 0)*/)
	{
		right->body->ApplyForceToCenter(b2Vec2(0, -150), 1);
	}

	if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)/* && (lives != 0)*/)
	{
		left->body->ApplyForceToCenter(b2Vec2(0, -150), 1);
	}

	App->renderer->Blit(flippR, 168, 565, NULL, 1.0f,  right->GetRotation());

	App->renderer->Blit(flippL, 50, 565, NULL, 1.0f, left->GetRotation());

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	}

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}
	c = circles.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(bola, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	//Yellow Rectangle
	App->renderer->Blit(YellowRectangle, 360, 570, NULL, 0.5f);

	int springX, springY;
	spring->GetPosition(springX, springY);
	App->renderer->Blit(springTex, springX, springY, NULL, 1.0f, spring->GetRotation());


	LOG( "%8d", score);
	App->fonts->BlitText(20, 50, scoreFont, scoreText);

	if (score > highScore)
	{
		highScore = score;
		fstream file;
		file.open("score.txt", ios::out);
		file.seekp(0);
		file << highScore;
		file.close();
	}

	string sc = to_string(score);
	int xpos = 460 - (sc.size() * 16);
	int digit = 0;
	for (unsigned int i = 0; i < sc.size(); i++)
	{
		digit = sc[i] - '0';

		App->renderer->Blit(numsscore, xpos + (i * 17), 580, &scorerect[digit], 0.0f);

	}
	App->renderer->DrawQuad({ 340,50,40,20 }, 0, 0, 0);
	sprintf_s(scoreText, 10, "%2d", lives);
	App->fonts->BlitText(355, 50, scoreFont, scoreText);
	//App->renderer->Blit(Heart, 340, 53, NULL);

	string sl = to_string(lives);
	
	for (unsigned int i = 0; i < sl.size(); i++)
	{
		int digitl = sl[i] - '0';

		App->renderer->Blit(numsscore, 420 , 520, &scorerect[digitl], 0.0f);

	}
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		LOG("add 100 score");
		score += 100;
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{

		lives = 0;
	}


	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Destroy() {

	if (toDestroy != nullptr) {
		App->physics->world->DestroyBody(toDestroy->body);
		toDestroy = nullptr;
	}
}

void ModuleSceneIntro::Create()
{

	circles.add(App->physics->CreateCircle(345, 500, 8, b2_dynamicBody));
	circles.getLast()->data->listener = this;
	circles.getLast()->data->collidertype = ColliderType::BALL;
	circles.getLast()->data->body->SetBullet(true);
	create = false;
}



void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	if (bodyB == end) {
		toDestroy = circles.getLast()->data;
		circles.clear();
		lives--;
		create = true;
	}
	App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}
