#include "Soldier.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1App.h"
#include "p2Defs.h"
#include "j1Scene.h"
#include "j1Input.h"
#include "j1Collision.h"
#include "j1EntityElementsScene.h"
#include "j1Audio.h"
#include "j1Player.h"

Soldier::Soldier():NPC()
{
	name = "enemies"; 
	type = CREATURE;
	srand(time(NULL));
}

Soldier::~Soldier()
{}

bool Soldier::Awake(pugi::xml_node &conf, uint id)
{
	bool stop_search = false;
	for (int s_id = conf.attribute("id").as_int(0); stop_search == false; s_id = conf.next_sibling().attribute("id").as_int(0))
	{
		if (id == s_id)
		{
			hp = conf.attribute("hp").as_int(0);
			position.x = conf.attribute("pos_x").as_int(0);
			position.y = conf.attribute("pos_y").as_int(0);
			std::string temp = conf.attribute("file").as_string("");

			temp = conf.attribute("dir").as_string("");
			if (temp == "up")
				direction = UP;
			else if (temp == "down")
				direction = DOWN;
			else if (temp == "left")
				direction = LEFT;
			else
				direction = RIGHT;

			movable = conf.attribute("canMove").as_bool(false);
			destructible = conf.attribute("destructible").as_bool(false);

			if (destructible == false)
			{
				soldier_type = PASSIVE;
			}
			else
			{
				soldier_type = AGGRESSIVE;
			}

			npc_id = id;
			stop_search = true;
		}
	}
	return true;
}

bool Soldier::CleanUp()
{
	return true;
}

void Soldier::OnCollision(Collider* c1, Collider* c2)
{
	if (c1 != nullptr && c2 != nullptr)
	{
		if (c1 == collision_feet && c2->type == COLLIDER_SWORD && c1->callback != nullptr)
		{
			if (destructible == true && state != S_HIT)
			{
				knockback_time.Start();
				hp--;
				state = S_HIT;
				anim_state = S_IDLE;
				dir_hit = c2->callback->direction;
				prev_position = position;
			}
		}
	}
}

bool Soldier::Start()
{
	if (soldier_type == AGGRESSIVE)
	{
		offset_x = 8;
		offset_y = 15;
		state = S_IDLE;
		anim_state = S_IDLE;
		speed = 40;
		radar = 75;
		chase_speed = 60;
	}

	else if (soldier_type == PASSIVE)
	{
		offset_x = 8;
		offset_y = 15;
	}

	//Get the animations
	animation = *App->anim_manager->GetAnimStruct(SOLDIER);

	//Set the collision
	collision_feet = App->collision->AddCollider({ position.x - offset_x, position.y - offset_y, 16, 15 }, COLLIDER_ENEMY, this);
	return true;
}

bool Soldier::Update(float dt)
{
	// STATE MACHINE ------------------
		if (App->scene->gamestate == INGAME) //IA performing actions only while INGAME state
		{
			if (soldier_type == AGGRESSIVE)
			{
				switch (state)
				{
				case S_IDLE:
				{
					Idle();
					break;
				}
				case S_WALKING:
				{
					Walking(dt);
					break;
				}
				case S_DYING:
				{
					Die();
					break;
				}
				default:
				{
					break;
				}
				}	
		}

		collision_feet->SetPos(position.x - offset_x, position.y - offset_y);
	}
	return true;
}

void Soldier::Draw()
{
		if (soldier_type == PASSIVE)
		{
			id = 2;
		}
		else
		{
			switch (state)
			{
			case S_IDLE:
				id = 0;
				break;
			case S_WALKING:
				id = 1;
				break;
			case S_HIT:
				id = 1;
				break;
			case S_CHASING:
			{
				id = 1;
				break;
			}
			default:
				break;
			}
		}

	if (direction == UP)
	{
		anim_rect = animation.anim[id].North_action.GetCurrentFrame();
		pivot = animation.anim[id].North_action.GetCurrentOffset();
	}
	else if (direction == DOWN)
	{
		anim_rect = animation.anim[id].South_action.GetCurrentFrame();
		pivot = animation.anim[id].South_action.GetCurrentOffset();
	}
	else if (direction == LEFT)
	{
		anim_rect = animation.anim[id].West_action.GetCurrentFrame();
		pivot = animation.anim[id].West_action.GetCurrentOffset();
	}
	else if (direction == RIGHT)
	{
		anim_rect = animation.anim[id].East_action.GetCurrentFrame();
		pivot = animation.anim[id].East_action.GetCurrentOffset();
	}

	App->render->Blit(animation.graphics, position.x - pivot.x, position.y - pivot.y, &anim_rect);
	
}


bool Soldier::Idle()
{
	if (movable)
	{
		if (reset_run)
		{
			timetorun = SDL_GetTicks();
			reset_run = false;
		}
		else
		{
			if (SDL_GetTicks() - timetorun > 2000)
			{
				int direc_select = rand() % 4 + 1;
				if (direc_select == 1)
				{
					direction = UP;
				}
				else if (direc_select == 2)
				{
					direction = DOWN;
				}
				else if (direc_select == 3)
				{
					direction = LEFT;
				}
				else if (direc_select == 4)
				{
					direction = RIGHT;
				}
				state = S_WALKING;
				anim_state = S_WALKING;
				reset_distance = true;
			}
		}
	}
	return true;
}

bool Soldier::Walking(float dt)
{
	walking = false;

	if (reset_distance)
	{
		distance = rand() % 100 + 20;
		dis_moved = 0;
		reset_distance = false;
	}

	Move(dt);

	if(dis_moved >= distance)
	{
		walking = false;
		reset_run = true;
	}

	if (walking == false)
	{
		state = S_IDLE;
		anim_state = S_IDLE;
	}
	else
	{
		state = S_WALKING;
		anim_state = S_WALKING;
	}

	return true;
}

bool Soldier::Move(float dt)
{
	if (direction == LEFT)
	{
		if (App->map->MovementCost(collision_feet->rect.x - ceil(speed*dt), collision_feet->rect.y, collision_feet->rect.w, collision_feet->rect.h, LEFT) == 0)
		{
			position.x -= ceil(speed*dt);
			dis_moved++;
		}
		else
		{
			dis_moved++;
		}
		walking = true; 
	}

	if (direction == RIGHT)
	{
		if (App->map->MovementCost(collision_feet->rect.x + collision_feet->rect.w + ceil(speed*dt), collision_feet->rect.y, collision_feet->rect.w, collision_feet->rect.h, RIGHT) == 0)
		{
			position.x += ceil(speed*dt);
			dis_moved++;
		}
		else
		{
			dis_moved++;
		}
		walking = true;
	}
	if (direction == UP)
	{
		if (App->map->MovementCost(collision_feet->rect.x, collision_feet->rect.y - ceil(speed*dt), collision_feet->rect.w, collision_feet->rect.h, UP) == 0)
		{
			position.y -= ceil(speed*dt);
			dis_moved++;
		}
		else
		{
			dis_moved++;
		}
		walking = true;
	}
	if (direction == DOWN)
	{
		if (App->map->MovementCost(collision_feet->rect.x, collision_feet->rect.y + collision_feet->rect.h + ceil(speed*dt), collision_feet->rect.w, collision_feet->rect.h, DOWN) == 0)
		{
			position.y += ceil(speed*dt);
			dis_moved++;
		}
		else
		{
			dis_moved++;
		}
		walking = true;
	}

	return true;
}

bool Soldier::Die()
{
	App->entity_elements->DeleteEnemy(this);
	return true;
}
