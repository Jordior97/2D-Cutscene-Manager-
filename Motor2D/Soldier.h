#pragma once
#ifndef __SOLDIER_H_
#define __SOLDIER_H_

#include "NPC.h"
#include <vector>

enum SoldierType { AGGRESSIVE, PASSIVE };
enum SoldierState { S_IDLE = 0, S_WALKING, S_DYING, S_ATTACKING, S_CHASING, S_HIT };


class Soldier : public NPC
{
public:
	Soldier();

	~Soldier();

	// Called before render is available
	bool Awake(pugi::xml_node&, uint);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	void OnCollision(Collider* c1, Collider* c2);

	void Draw();

	// Called before quitting
	bool CleanUp();

	//STATE MACHINE -----------
	bool Idle();
	bool Walking(float dt);
	bool Move(float dt);
	bool Die();
	//-------------------------

private:
	//ANIMATION VARIABLES ---------
	SoldierType soldier_type;
	uint id = 0;
	SoldierState state = S_IDLE;
	SoldierState anim_state = S_IDLE;
	AnimationStruct animation;
	SDL_Rect anim_rect;
	iPoint pivot;
	bool stunned = false;
	SDL_Texture* texture = nullptr;

	//MOVEMENT VARIABLES -------
	int timetorun = 0;
	bool reset_run = false;
	int distance = 0;
	int dis_moved = 0;
	bool reset_distance = false;
	//----------------------
};

#endif //__SOLDIER_H_