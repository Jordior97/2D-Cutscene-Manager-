#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Gui.h"
#include "j1GuiEntity.h"
#include "j1GuiElements.h"
#include "j1Player.h"
#include "j1FileSystem.h"
#include "j1AnimationManager.h"
#include "j1Collision.h"
#include "Soldier.h"
#include "j1Creature.h"
#include "j1FadeToBlack.h"
#include "Animation.h"

//Constructor
Player::Player() : Creature()
{
	type = CREATURE;
	name = "Link";
}

// Destructor
Player::~Player()
{}

// Called before render is available
bool Player::Awake(pugi::xml_node& conf)
{
	LOG("Loading Texture Player");
	bool ret = true;
	hp = conf.child("stats").attribute("hp").as_int(0);
	attack = conf.child("stats").attribute("attack").as_int(0);
	speed = conf.child("stats").attribute("speed").as_int(0);
	position.x = conf.child("stats").attribute("pos_x").as_int(0);
	position.y = conf.child("stats").attribute("pos_y").as_int(0);

	return ret;
}

// Called before the first frame
bool Player::Start()
{
	bool ret = true;

	//ANIMATION ---------------
	direction = DOWN;
	state = L_IDLE;
	anim_state = L_IDLE;
	scale = App->win->GetScale();
	offset_x = 7;
	offset_y = 10;
	//--------------------------

	canSwitchMap = true;
	collision_feet = App->collision->AddCollider({ position.x - offset_x, position.y - offset_y, 14, 14 }, COLLIDER_PLAYER, this);
	return ret;
}

bool Player::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool Player::Update(float dt)
{
	bool ret = true;

	// STATE MACHINE ------------------
	if (App->scene->gamestate == INGAME) //Only you control the player if it's INGAME state
	{
		switch (state)
		{
		case L_IDLE:
		{
			Idle();
			break;
		}
		case L_WALKING:
		{
			Walking(dt);
			break;
		}
		default:
		{
			break;
		}
		}
	}
	// ----------------------------

	//Collision follow the player
	collision_feet->SetPos(position.x - offset_x, position.y - offset_y);

	return ret;
}

void Player::Draw()
{
	//DRAW LINK ANIMATION DEPENDING ON ITS STATE
	App->anim_manager->Drawing_Manager(anim_state, direction, position, LINK); 
}

bool Player::CleanUp()
{
	bool ret = true;

	return ret;
}

bool Player::Save()
{
	App->entity_elements->XML.child("config").child("Link").child("stats").attribute("hp").set_value(hp);
	App->entity_elements->XML.save_file("config.xml");
	return true;
}

void Player::OnCollision(Collider* c1, Collider* c2)
{
	if (c1 != nullptr && c2 != nullptr)
	{
		//CHANGING MAPS STRUCTURE ------------------------------------------
		if (c1 == collision_feet && c2->type == COLLIDER_SWITCH_MAP)
		{
			if (canSwitchMap == false)
			{
				canSwitchMap = true;
			}
			else
			{
				iPoint temp_meta = App->map->WorldToMap(position.x, position.y);
				MapLayer* meta_ = App->map->data.layers[1];
				int id_meta = meta_->Get(temp_meta.x, temp_meta.y);
				for (int i = 0; i < App->map->directMap.size(); i++)
				{
					if (App->map->directMap[i].id_tile == id_meta)
					{
						canSwitchMap = false;
						App->scene->switch_map = App->map->directMap[i].id_map;
						App->scene->newPosition = App->map->directMap[i].position;
					}
				}
			}
		}
		//-------------------------------------------------------------------
	}
}


bool Player::Camera_inside()
{
	//256x224
	if (camera_follow == true)
	{
		iPoint camera_pos(-App->render->camera.x / 2, -App->render->camera.y / 2);
		iPoint size_map = App->map->MapToWorld(App->map->data.width, App->map->data.height);
		if (direction == UP)
		{
			if (camera_pos.y == 0)
			{
				return false;
			}
			else
			{
				if (position.y > size_map.y - (App->win->GetHeight() / scale) / 2)
				{
					return false;
				}
			}
		}
		if (direction == DOWN)
		{
			if (camera_pos.y + (App->win->GetHeight() / scale) >= size_map.y)
			{
				return false;
			}
			else
			{
				if (position.y < (App->win->GetHeight() / scale) / 2)
				{
					return false;
				}
			}
		}
		if (direction == LEFT)
		{
			if (camera_pos.x <= 0)
			{
				camera_pos.x = 0;
				return false;
			}
			else
			{
				if (position.x > size_map.x - (App->win->GetWidth() / scale) / 2)
				{
					return false;
				}
			}
		}
		if (direction == RIGHT)
		{
			if (camera_pos.x + (App->win->GetWidth() / scale) >= size_map.x)
			{
				return false;
			}
			else
			{
				if (position.x < (App->win->GetWidth() / scale) / 2)
				{
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

//STATE MACHINE ---------------------------------------------------------------------
bool Player::Idle()
{
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || 
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		state = L_WALKING;
		anim_state = L_WALKING;
	}
	else
	{
		state = L_IDLE;
		anim_state = L_IDLE;
	}

	return true;
}

bool Player::Walking(float dt)
{
	walking = false;
	Move(dt);

	if (walking == false)
	{
		state = L_IDLE;
		anim_state = L_IDLE;
	}

	else
	{
		state = L_WALKING;
		anim_state = L_WALKING;
	}
	return false;
}

bool Player::Move(float dt)
{
	int keysuse = GetnuminputUse();
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		direction = LEFT;
		int temp = App->map->MovementCost(collision_feet->rect.x - ceil(speed*dt), collision_feet->rect.y, collision_feet->rect.w, collision_feet->rect.h, LEFT);
		if (temp == T_CONTINUE)
		{
			if (Camera_inside())
				App->render->camera.x += ceil(speed*dt) * scale;
			position.x -= ceil(speed*dt);
		}
		if (keysuse == 1)
		{
			if (temp == T_UP)
			{
				direction = UP;
				if (Camera_inside())
					App->render->camera.y += ceil(speed*dt) * scale;
				position.y -= ceil(speed*dt);
				direction = LEFT;
			}
			if (temp == T_DOWN)
			{
				direction = DOWN;
				if (Camera_inside())
					App->render->camera.y -= ceil(speed*dt) * scale;
				position.y += ceil(speed*dt);
				direction = LEFT;
			}
		}
		walking = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		direction = RIGHT;
		int temp = App->map->MovementCost(collision_feet->rect.x + collision_feet->rect.w + ceil(speed*dt), collision_feet->rect.y, collision_feet->rect.w, collision_feet->rect.h, RIGHT);
		if (temp == T_CONTINUE)
		{
			if (Camera_inside())
				App->render->camera.x -= ceil(speed*dt) * scale;
			position.x += ceil(speed*dt);
		}
		if (keysuse == 1)
		{
			if (temp == T_UP)
			{
				direction = UP;
				if (Camera_inside())
					App->render->camera.y += ceil(speed*dt) * scale;
				position.y -= ceil(speed*dt);
				direction = RIGHT;
			}
			if (temp == T_DOWN)
			{
				direction = DOWN;
				if (Camera_inside())
					App->render->camera.y -= ceil(speed*dt) * scale;
				position.y += ceil(speed*dt);
				direction = RIGHT;
			}
		}
		walking = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		direction = UP;
		int temp = App->map->MovementCost(collision_feet->rect.x, collision_feet->rect.y - ceil(speed*dt), collision_feet->rect.w, collision_feet->rect.h, UP);
		if (temp == T_CONTINUE)
		{
			if (Camera_inside())
				App->render->camera.y += ceil(speed*dt) * scale;
			position.y -= ceil(speed*dt);
		}
		if (keysuse == 1)
		{
			if (temp == T_LEFT)
			{
				direction = LEFT;
				if (Camera_inside())
					App->render->camera.x += ceil(speed*dt) * scale;
				position.x -= ceil(speed*dt);
				direction = UP;
			}
			if (temp == T_RIGHT)
			{
				direction = RIGHT;
				if (Camera_inside())
					App->render->camera.x -= ceil(speed*dt) * scale;
				position.x += ceil(speed*dt);
				direction = UP;
			}
		}
		walking = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		direction = DOWN;
		int temp = App->map->MovementCost(collision_feet->rect.x, collision_feet->rect.y + collision_feet->rect.h + ceil(speed*dt), collision_feet->rect.w, collision_feet->rect.h, DOWN);
		if (temp == T_CONTINUE)
		{
			if (Camera_inside())
				App->render->camera.y -= ceil(speed*dt) * scale;
			position.y += ceil(speed*dt);
		}
		if (keysuse == 1)
		{
			if (temp == T_LEFT)
			{
				direction = LEFT;
				if (Camera_inside())
					App->render->camera.x += ceil(speed*dt) * scale;
				position.x -= ceil(speed*dt);
				direction = DOWN;
			}
			if (temp == T_RIGHT)
			{
				direction = RIGHT;
				if (Camera_inside())
					App->render->camera.x -= ceil(speed*dt) * scale;
				position.x += ceil(speed*dt);
				direction = DOWN;
			}
		}
		walking = true;
	}

	return walking;
}


int Player::GetnuminputUse()
{
	int ret = 0;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		ret++;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		ret++;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		ret++;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		ret++;
	}
	return ret;
}

void Player::SetState(LinkState set)
{
	if (set >= L_IDLE && set <= L_INTERACTING)
	{
		state = set;
	}
}

LinkState Player::GetState() const
{
	return state;
}

void Player::SetAnimState(LinkState anim)
{
	if (anim >= L_IDLE && anim <= L_INTERACTING)
	{
		anim_state = anim;
	}
}

