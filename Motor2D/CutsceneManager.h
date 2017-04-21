#ifndef _CUTSCENEMANAGER_H_
#define _CUTSCENEMANAGER_H_

#include "j1Module.h"
#include "Bezier.h"
#include <vector>
#include <string>

enum CS_Type { CS_IMAGE, CS_TEXT, CS_NPC, CS_MUSIC, CS_FX, CS_NONE };
enum CS_Action { ACT_ENABLE, ACT_DISABLE, ACT_MOVE, ACT_NONE };

class j1Timer;


class CS_Image
{
public:
	CS_Image();
	~CS_Image();

private:
	SDL_Texture* tex = nullptr;
	SDL_Rect rect = { 0, 0, 0, 0 };
};


class CS_Element
{
public:
	CS_Element(CS_Type type, bool active, uint id, const char* path = nullptr);
	virtual ~CS_Element();


	std::string name;
private:
	CS_Type type = CS_NONE;
	bool active = false;
	uint id = 0;
	std::string path;

};

class CS_Step
{
public:
	CS_Step(uint start, uint id, CS_Action action, CS_Element* element);
	virtual ~CS_Step();

	bool PerformAction(float dt);

	//UTILITY FUNCTIONS ------------
	uint GetStartTime() const;

	//STEP FUNCTIONS -----
	void StartStep();
	void FinishStep();
	bool isActive() const;

private:
	uint id = 0;
	uint start = 0;					//Time to start the step
	CS_Action action = ACT_NONE;	//Action to perform
	CS_Element*	element = nullptr;	//Element to apply the action
	bool active = false;
};

class Cutscene
{
public:
	Cutscene();
	~Cutscene();
	bool Start();

	bool Update(float dt);

	//LOAD ELEMENTS FUNCTIONS -------
	bool LoadNPC(pugi::xml_node&);
	bool LoadMap(pugi::xml_node&);
	bool LoadDynObject(pugi::xml_node&);
	bool LoadItem(pugi::xml_node&);
	bool LoadImg(pugi::xml_node&);
	bool LoadText(pugi::xml_node&);
	bool LoadMusic(pugi::xml_node&);
	bool LoadFx(pugi::xml_node&);
	// ------------------------------

	//MAP -------------
	//bool SetMap(uint id);
	//---------------------

	//UTILITY FUNCTIONS ------
	uint GetID() const;
	bool isFinished() const;
	//---------------------

	std::string name;						//Name of the cutscenes
	uint time = 0;							//Max time of the cutscene
	uint id = 0;							//ID to locate when triggered

private:
	std::list<CS_Element*> elements;		//Elements controlled by the cutscene
	std::list<CS_Step*> steps;			//Steps to follow in order when reproduced
	CS_Step* current_step = nullptr;		//Current step that is reproducing in the cutscene
	uint map_id = 0;;						//Id to know wich map charge
	bool finished = false;					//To know if Cutscene has finished
	j1Timer	timer;							//To control reproducing time of the cutscene
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

	//Active a cutscene when an event triggers it
	bool StartCutscene(uint id);
	bool FinishCutscene();

	// Called before all Updates
	//bool PostUpdate();

	// Called before quitting
	//bool CleanUp();
	

	//void Clear();

private:

	pugi::xml_node LoadXML(pugi::xml_document& config_file, std::string file) const;

	std::list<Cutscene*> cutscenes;			//container with all cutscenes
	std::list<std::string> paths;			//container with names of all paths of the cutscenes
	Cutscene* active_cutscene = nullptr;	//To know wich cutscene is active
};



#endif