#pragma once
#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "SceneElements.h"
#include "j1Creature.h"
#include "p2Defs.h"
#include "j1Timer.h"
enum LinkState { L_IDLE = 0, L_WALKING, L_ATTACKING, L_DYING, L_HOOKTHROWN, L_HIT, L_INTERACTING };

class Creature;
class Animation;
class Text;
class Dialogue;
class Image;
class j1Timer;


class Player : public Creature
{
public:
	Player();

	// Destructor
	~Player();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	void Draw();

	// Called before quitting
	bool CleanUp();

	bool Save();

	void OnCollision(Collider*, Collider*);

	bool Camera_inside();

	//STATE MACHINE -------
	bool Idle();
	bool Walking(float dt);
	bool Move(float dt);
	//----------------------

	//UTILITY FUNCTIONS ----------
	int GetnuminputUse();
	void SetState(LinkState state);
	LinkState GetState() const;
	void SetAnimState(LinkState anim);
	//-----------------------------


public:
	//Control camera movement
	bool camera_follow = false;

private:
	LinkState state = L_IDLE; //Logical state
	LinkState anim_state = L_IDLE; //Animation state
	Animation* current_animation = nullptr;

	//SWITCH MAP CONTROLLER
	bool canSwitchMap = false;

	//Timers
	j1Timer timer;
	
	std::string stats_temp;
};



#endif