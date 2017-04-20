#ifndef _CUTSCENEMANAGER_H_
#define _CUTSCENEMANAGER_H_

#include "j1Module.h"
#include "Bezier.h"
#include <vector>

class SceneElement;
class j1Timer;
class Image;
class Text;

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

	//UTILITY FUNCTIONS ------
	uint GetID() const;
	bool isFinished() const;
	//---------------------

	std::string name;						//Name of the cutscenes
	uint time = 0;							//Max time of the cutscene
	uint id = 0;							//ID to locate when triggered

private:
	std::list<SceneElement*> elements;		//Elements controlled by the cutscene
	std::list<Image*> images;				//Images that will be reproduced
	std::list<Text*> texts;					//Text to show during the cutscene
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
	bool ActiveCutscene(uint id);
	bool DeactivateCutscene();

	// Called before all Updates
	//bool PostUpdate();

	// Called before quitting
	//bool CleanUp();
		
	//void StartCutscene(uint id);	//Through a trigger, starts the adequate cutscene
	//void FinishCutscene();			//Sets to nullptr the active_cutscene pointer

	//void Clear();

private:

	pugi::xml_node LoadXML(pugi::xml_document& config_file, std::string file) const;

	std::list<Cutscene*> cutscenes;			//container with all cutscenes
	std::list<std::string> paths;			//container with names of all paths of the cutscenes
	Cutscene* active_cutscene = nullptr;	//To know wich cutscene is active
};



#endif