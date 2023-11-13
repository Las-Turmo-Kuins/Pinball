#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

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

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
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
	map = App->physics->CreateChain(0, 0, map_data, 102);
	map->body->SetType(b2_staticBody);
	map->body->GetFixtureList()->SetRestitution(0.5f);
	//flippers
	//right flippers
	right = App->physics->CreateRectangle(235, 706, 32, 12, b2_dynamicBody);
	right_circle = App->physics->CreateCircleStatic(235, 706, 6);

	b2RevoluteJointDef rightRevJoint;
	rightRevJoint.bodyA = right->body;
	rightRevJoint.bodyB = right_circle->body;
	rightRevJoint.localAnchorA.Set(PIXEL_TO_METERS(25), 0);
	rightRevJoint.localAnchorB.Set(0, 0);
	rightRevJoint.referenceAngle = 0 * DEGTORAD;
	rightRevJoint.enableLimit = true;
	rightRevJoint.lowerAngle = -30 * DEGTORAD;
	rightRevJoint.upperAngle = 30 * DEGTORAD;

	b2RevoluteJoint* joint_right = (b2RevoluteJoint*)App->physics->world->CreateJoint(&rightRevJoint);

	//left flippers
	left = App->physics->CreateRectangle(140, 706, 32, 12, b2_dynamicBody);
	left_circle = App->physics->CreateCircleStatic(140, 706, 6);

	b2RevoluteJointDef leftRevJoint;
	leftRevJoint.bodyA = left->body;
	leftRevJoint.bodyB = left_circle->body;
	leftRevJoint.localAnchorA.Set(PIXEL_TO_METERS(-25), 0);
	leftRevJoint.localAnchorB.Set(0, 0);
	leftRevJoint.referenceAngle = 0 * DEGTORAD;
	leftRevJoint.enableLimit = true;
	leftRevJoint.lowerAngle = -30 * DEGTORAD;
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
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		Create();
	}
	//
	spring->body->ApplyForce(b2Vec2(0, -20), b2Vec2(0, 0), true);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		spring->body->ApplyForce(b2Vec2(0, 21), b2Vec2(0, 0), true);
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
		right->body->ApplyForceToCenter(b2Vec2(0, -300), 1);
	}

	if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)/* && (lives != 0)*/)
	{
		left->body->ApplyForceToCenter(b2Vec2(0, -300), 1);
	}

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

	return UPDATE_CONTINUE;
}
void ModuleSceneIntro::Create()
{

	circles.add(App->physics->CreateCircle(345, 350, 8, b2_dynamicBody));
	circles.getLast()->data->listener = this;
	circles.getLast()->data->body->SetBullet(true);
}
void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

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
