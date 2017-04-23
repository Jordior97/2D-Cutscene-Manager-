#pragma once
#ifndef __j1ENTITYELEMENTSCENE_H__
#define __j1ENTITYELEMENTSCENE_H__

#include "j1Module.h"
#include <list>
#include "SceneElements.h"
#include "PugiXml\src\pugixml.hpp"

class Soldier;
class Item;
class Player;
class DynamicObjects;
class NPC;
class Hookshot;
class Weapon;
class Pokemon;
class BombContainer;
class PokeTrainer;

// ---------------------------------------------------
class j1EntityElementScene : public j1Module
{
public:

	j1EntityElementScene();

	~j1EntityElementScene();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	bool Update(float dt); 

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//CREATION FUNCTIONS --------------------------
	Soldier* CreateSoldier(uint id, pugi::xml_node& );
	Player* CreatePlayer();
	// -----------------------------------------

	//DELETION FUNCTIONS ----------------------------
	bool DeleteEnemy(Soldier* enemy);
	bool DelteElements();
	//-----------------------------------

	pugi::xml_node conf;
	pugi::xml_document XML;


private:
	std::list<SceneElement*> elementscene;
	pugi::xml_node LoadConfig(pugi::xml_document& config_file) const; 
	
};

#endif // __j1ENTITYELEMENTSCENE_H__