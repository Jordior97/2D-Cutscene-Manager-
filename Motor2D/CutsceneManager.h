#ifndef _CUTSCENEMANAGER_H_
#define _CUTSCENEMANAGER_H_

#include "j1Module.h"
#include "Bezier.h"
#include <vector>

class SceneElement;

class Cutscene
{
public:
	Cutscene();
	~Cutscene();

private:
	std::vector<SceneElement*> elements;	//Elements in the controlled by the Cutscene
	uint map_id;							//Id to know wich map charge
	bool finished = false;					//To know if Cutscene is finished
};


class j1CutSceneManager : public j1Module
{
public:
	j1CutSceneManager();
	~j1CutSceneManager();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	//bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	//bool PostUpdate();

	// Called before quitting
	bool CleanUp();
		
	void StartCutscene(uint id);	//Through a trigger, starts the adequate cutscene
	void FinishCutscene();			//Sets to nullptr the active_cutscene pointer

	void Clear();

private:

	std::list<Cutscene*> cutscenes;			//container with all cutscenes
	Cutscene* active_cutscene = nullptr;	//To know wich cutscene is active
};



#endif