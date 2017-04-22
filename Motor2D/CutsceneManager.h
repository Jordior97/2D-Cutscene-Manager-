#ifndef _CUTSCENEMANAGER_H_
#define _CUTSCENEMANAGER_H_

#include "j1Module.h"
#include "Bezier.h"
#include <vector>
#include <string>

enum CS_Type { CS_IMAGE, CS_TEXT, CS_NPC, CS_DYNOBJECT, CS_ITEM, CS_MUSIC, CS_FX, CS_NONE };
enum Action_Type { ACT_ENABLE, ACT_DISABLE, ACT_MOVE, ACT_PLAY, ACT_STOP, ACT_NONE };
enum Dir_Type { CS_UP, CS_DOWN, CS_LEFT, CS_RIGHT, NO_DIR };

class j1Timer;
class Cutscene;
class Text;


class CS_Element
{
public:
	CS_Element() {}
	CS_Element(CS_Type type, int n, const char* name, bool active, const char* path);
	virtual ~CS_Element();

	//UTILITY FUNCTIONS ---------
	CS_Type GetType() const;

	// ------------------------

	std::string name;
	bool active = false;

protected:
	CS_Type type = CS_NONE;
	int n = -1;
	std::string path;
};

/*class CS_NPC: public CS_Element
{
	CS_NPC(CS_Type type, int n, const char* name, bool active, const char* path, iPoint pos);
	~CS_NPC();

	SceneElement* GetEntity() const;
	void SetNull();
	void SetEntity(SceneElement* e);

private:
	SceneElement*	entity;
};*/

class CS_Image : public CS_Element
{
public:
	CS_Image(CS_Type type, int n, const char* name, bool active, const char* path, SDL_Rect rect, iPoint pos);
	~CS_Image();
	void Move(float x, float y);

	//UTILITY FUNCTIONS -------------
	SDL_Texture* GetTexture()const;
	SDL_Rect GetRect()const;
	iPoint GetPos()const;

private:
	SDL_Texture* tex = nullptr;
	SDL_Rect rect = { 0, 0, 0, 0 };
	iPoint pos = { 0, 0 };
};

class CS_Text : public CS_Element
{
public:
	CS_Text(CS_Type type, int n, const char* name, bool active, const char* path, iPoint pos, const char* text);
	~CS_Text();

	//UTILITY FUNCTIONS ---------------
	void SetText(const char* txt);
	Text* GetText()const;
	//void Move(float x, float y);
	//---------------------------------

private:
	Text* text = nullptr;
};

class CS_Music : public CS_Element
{
public:
	CS_Music(CS_Type type, int n, const char* name, bool active, const char* path);
	~CS_Music();

	//UTILITY FUNCTIONS ------------
	void Play();
};

class CS_SoundFx : public CS_Element
{
public:
	CS_SoundFx(CS_Type type, int n, const char* name, bool active, const char* path, uint loops);
	~CS_SoundFx();
	
	//UTILITY FUNCTIONS ------------
	void LoadFx();
	void Play();
	uint GetID() const;
	uint GetLoops() const;

private:
	uint fx_id = 0;
	uint loops = 0;
};


class CS_Step
{
public:
	CS_Step(int n, int start, int duration, Cutscene* cutscene);
	virtual ~CS_Step();

	bool DoAction(float dt);

	//STEP FUNCTIONS -----
	void StartStep();
	void FinishStep();
	void SetElement(pugi::xml_node&);
	void SetAction(pugi::xml_node&);

	//ACTION FUNCTIONS ----------
	void LoadMovement(iPoint dest, int speed, const std::string& dir);
	bool DoMovement(float dt);
	bool CheckMovementCompleted(iPoint curr_pos);
	void Play();
	void StopMusic();
	void ActiveElement();
	void DisableElement();
	//---------------------------

	//UTILITY FUNCTIONS ------------
	CS_Type GetElementType() const;
	uint GetStartTime() const;
	bool isActive() const;
	bool isFinished() const;


private:
	Cutscene* cutscene = nullptr;	//Pointer to the cutscene that it is integrated
	int n = -1;						//Number to manage an order
	int start = -1;					//Time to start the step
	int duration = -1;				//Duration of the step TODO MED -> delete this
	Action_Type act_type= ACT_NONE;	
	CS_Element*	element = nullptr;	//Element to apply the action
	bool active = false;			//If step is reproducing.
	bool finished = false;

	//ACTIONS VARIABLES
	/*Movement*/
	iPoint origin = { 0, 0 };
	iPoint dest = { 0, 0 };
	int mov_speed = 0;
	Dir_Type direction = NO_DIR;

};

class Cutscene
{
public:
	Cutscene();
	~Cutscene();
	bool Start();

	bool Update(float dt);
	bool DrawElements();

	//LOAD ELEMENTS FUNCTIONS -------
	bool LoadNPC(pugi::xml_node&);
	bool LoadDynObject(pugi::xml_node&);
	bool LoadItem(pugi::xml_node&);
	bool LoadImg(pugi::xml_node&);
	bool LoadText(pugi::xml_node&);
	bool LoadMusic(pugi::xml_node&);
	bool LoadFx(pugi::xml_node&);
	// ------------------------------

	//STEPS FUNCTIONS ---
	bool LoadStep(pugi::xml_node&, Cutscene* cutscene);
	void StepDone();
	//--------------

	//MAP -------------
	bool SetMap(pugi::xml_node&);
	//---------------------

	//UTILITY FUNCTIONS ------
	uint GetID() const;
	CS_Element* GetElement(const char* name);
	bool isFinished() const;
	//---------------------

	std::string name;						//Name of the cutscenes
	uint id = 0;							//ID to locate when triggered
	j1Timer	timer;							//To control reproducing time of the cutscene
	int map_id = -1;						//Id to know wich map charge

private:
	std::list<CS_Element*> elements;		//Elements controlled by the cutscene
	std::list<CS_Step*> steps;				//Steps to follow in order when reproduced
	bool finished = false;					//To know if Cutscene has finished
	
	//TO CONTROL WHEN THE CUTSCENE IS FINISHED
	uint num_steps = 0;
	uint steps_done = 0;
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
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	

	//void Clear();

private:

	pugi::xml_node LoadXML(pugi::xml_document& config_file, std::string file) const;

	std::list<Cutscene*> cutscenes;			//container with all cutscenes
	std::list<std::string> paths;			//container with names of all paths of the cutscenes
	Cutscene* active_cutscene = nullptr;	//To know wich cutscene is active
};



#endif