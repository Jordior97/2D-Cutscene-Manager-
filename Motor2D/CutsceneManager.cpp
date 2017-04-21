#include "CutsceneManager.h"
#include "j1FileSystem.h"
#include "SceneElements.h"
#include "p2Log.h"


// CUTSCEME MANAGER -----------------------------------
j1CutSceneManager::j1CutSceneManager()
{
	name = "cutscenemanager";
}

j1CutSceneManager::~j1CutSceneManager()
{
}

bool j1CutSceneManager::Awake(pugi::xml_node& config)
{
	bool ret = false;
	if (config != NULL)
	{
		ret = true;

		//Load all path names in a list to access the concrete cutscene in the future.
		for (pugi::xml_node temp = config.child("file"); temp != NULL; temp = temp.next_sibling())
		{
			paths.push_back(temp.attribute("file").as_string(""));
		}
	}

	return ret;
}

bool j1CutSceneManager::Start()
{
	pugi::xml_document	cutscene_file;
	pugi::xml_node		cutscene_node;
	pugi::xml_node		elements_node;
	pugi::xml_node		temp;
	std::list<std::string>::iterator file = paths.begin();

	for (; file != paths.end(); file++)
	{
		//Load XML
		cutscene_node = LoadXML(cutscene_file, file._Ptr->_Myval);

		//Create temp Cutscene 
		Cutscene* temp_cutscene = new Cutscene();

		temp_cutscene->id = cutscene_node.attribute("id").as_uint(0);			//Sets its identifier.
		temp_cutscene->name = cutscene_node.attribute("name").as_string("");	//Sets its name.
		temp_cutscene->time = cutscene_node.attribute("time").as_uint(0);		//Sets its max time.


		//Add all Elements involved in its list
		elements_node = cutscene_node.child("elements");

		//Load Maps 
		for (temp = elements_node.child("MAP").child("map"); temp != NULL; temp = temp.next_sibling("map"))
		{
			//temp_cutscene->LoadMap(temp);
		}

		//Load NPCs
		for (temp = elements_node.child("NPCs").child("npc"); temp != NULL; temp = temp.next_sibling("npc"))
		{
			//temp_cutscene->LoadNPC(temp);
		}

		//Load DynObjects
		for (temp = elements_node.child("DYNOBJECTS").child("dynobject"); temp != NULL; temp = temp.next_sibling("dynobject"))
		{
			//temp_cutscene->LoadDynObject(temp);
		}

		//Load Items
		for (temp = elements_node.child("ITEMS").child("item"); temp != NULL; temp = temp.next_sibling("item"))
		{
			//temp_cutscene->LoadItem(temp);
		}

		//Load Items
		for (temp = elements_node.child("IMAGES").child("image"); temp != NULL; temp = temp.next_sibling("image"))
		{
			//temp_cutscene->LoadImg(temp);
		}

		//Load Items
		for (temp = elements_node.child("TEXTS").child("text"); temp != NULL; temp = temp.next_sibling("text"))
		{
			//temp_cutscene->LoadText(temp);
		}

		//Load Music
		for (temp = elements_node.child("MUSIC").child("music"); temp != NULL; temp = temp.next_sibling("music"))
		{
			//temp_cutscene->LoadMusic(temp);
		}

		//Load Fx
		for (temp = elements_node.child("FX").child("fx"); temp != NULL; temp = temp.next_sibling("fx"))
		{
			//temp_cutscene->LoadFx(temp);
		}

		//Add the Cutscene in the list -------------------------
		cutscenes.push_back(temp_cutscene);
		LOG("Cutscene '%s' loaded", temp_cutscene->name.c_str());
	}
	return true;
}

bool j1CutSceneManager::Update(float dt)
{
	bool ret = true;

	if (active_cutscene != nullptr)
	{
		if (active_cutscene->isFinished() == false)
		{
			active_cutscene->Update(dt); //Update elements of the cutscene
		}
		else
		{
			FinishCutscene();
		}
	}

	return ret;
}

//Set to active the correct cutscene
bool j1CutSceneManager::StartCutscene(uint id)
{
	for (std::list<Cutscene*>::iterator it = cutscenes.begin(); it != cutscenes.end(); it++)
	{
		if (id == it._Ptr->_Myval->GetID())
		{
			active_cutscene = *it;
			active_cutscene->Start();
			LOG("%s cutscene activated", active_cutscene->name.c_str());
			return true;
		}
	}
	return false;
}

//Set active_cutscene pointer to nullptr
bool j1CutSceneManager::FinishCutscene()
{
	bool ret = false;
	if (active_cutscene != nullptr)
	{
		if (active_cutscene->isFinished() == true)
		{
			LOG("%s cutscene deactivated", active_cutscene->name.c_str());
			active_cutscene = nullptr;
			ret = true;
		}
	}
	return ret;
}


//Read from .xml cutscene file
pugi::xml_node j1CutSceneManager::LoadXML(pugi::xml_document & config_file, std::string file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load(file.c_str(), &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE(buf);

	if (result == NULL)
	{
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	}
	else
	{
		ret = config_file.child("Cutscene");
	}

	return ret;
}

// --------------------------------------------------------



// CUTSCENE ----------------------------
uint Cutscene::GetID() const
{
	return id;
}

bool Cutscene::isFinished() const
{
	return finished;
}

Cutscene::Cutscene()
{
}

Cutscene::~Cutscene()
{
}

bool Cutscene::Start()
{
	finished = false;
	timer.Start();
	return true;
}

bool Cutscene::Update(float dt)
{
	bool ret = true;

	//Iterate the steps of the cutscene to update the active ones
	std::list<CS_Step*>::iterator temp = steps.begin();
	while (temp != steps.end())
	{
		CS_Step* step = *temp;

		//Init the step
		if (timer.ReadSec() >= step->GetStartTime())
		{
			step->StartStep();
		}

		//Update the state to perform its action
		if (step->isActive() == true)
		{
			step->PerformAction(dt);
		}

		temp++;
	}

	if (timer.ReadSec() >= time)
	{
		finished = true;
	}

	float sec = timer.ReadSec();
	LOG("Reproducing %s cutscene, %.4fs", name.c_str(), sec);

	return ret;
}

bool Cutscene::LoadNPC(pugi::xml_node &)
{
	return false;
}

bool Cutscene::LoadMap(pugi::xml_node &)
{
	return false;
}

bool Cutscene::LoadDynObject(pugi::xml_node &)
{
	return false;
}

bool Cutscene::LoadItem(pugi::xml_node &)
{
	return false;
}

bool Cutscene::LoadImg(pugi::xml_node &)
{
	return false;
}

bool Cutscene::LoadText(pugi::xml_node &)
{
	return false;
}

bool Cutscene::LoadMusic(pugi::xml_node &)
{
	return false;
}

bool Cutscene::LoadFx(pugi::xml_node &)
{
	return false;
}

//--------------------------------------


//CS ELEMENTS ------------------------------
CS_Element::CS_Element(CS_Type type, bool active, uint id, const char* path):type(type),active(active),id(id),path(path)
{
}

CS_Element::~CS_Element()
{
}


//CS STEPS ----------------------------------
CS_Step::CS_Step(uint start, uint id, CS_Action action, CS_Element* cs_element):start(start), id(id), action(action)
{
	element = cs_element;
}

CS_Step::~CS_Step()
{
}

bool CS_Step::PerformAction(float dt)
{
	std::string action_name;
	switch (action)
	{
	case ACT_DISABLE:
		action_name = "disable";
		break;
	case ACT_ENABLE:
		action_name = "enable";
		break;
	case ACT_MOVE:
		action_name = "move";
		break;
	default:
		action_name = "NONE";
		break;

	}

	LOG("Step '%i' started at '%.3f' performing '%s' on '%s'", id, start, action);
	return true;
}

uint CS_Step::GetStartTime() const
{
	return start;
}

void CS_Step::StartStep()
{
	active = true;
}

void CS_Step::FinishStep()
{
	active = false;
}

bool CS_Step::isActive() const
{
	return active;
}
