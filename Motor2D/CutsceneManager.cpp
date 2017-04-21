#include "CutsceneManager.h"
#include "j1FileSystem.h"
#include "SceneElements.h"
#include "j1Scene.h"
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
	pugi::xml_node		steps_node;
	pugi::xml_node		temp;
	std::list<std::string>::iterator file = paths.begin();

	for (; file != paths.end(); file++)
	{
		//Load XML
		cutscene_node = LoadXML(cutscene_file, file._Ptr->_Myval);

		//Create temp pointers 
		Cutscene* temp_cutscene = new Cutscene();

		temp_cutscene->id = cutscene_node.attribute("id").as_uint(0);			//Sets its identifier.
		temp_cutscene->name = cutscene_node.attribute("name").as_string("");	//Sets its name.

		//LOAD ELEMENTS INVOLVED IN THE CUTSCENE --------------------------------------------------------------
		elements_node = cutscene_node.child("elements");

		//Load Map
		temp_cutscene->SetMap(elements_node);

		//Load NPCs
		for (temp = elements_node.child("NPCs").child("npc"); temp != NULL; temp = temp.next_sibling("npc"))
		{
			temp_cutscene->LoadNPC(temp);
		}

		//Load DynObjects
		for (temp = elements_node.child("DYNOBJECTS").child("dynobject"); temp != NULL; temp = temp.next_sibling("dynobject"))
		{
			temp_cutscene->LoadDynObject(temp);
		}

		//Load Items
		for (temp = elements_node.child("ITEMS").child("item"); temp != NULL; temp = temp.next_sibling("item"))
		{
			temp_cutscene->LoadItem(temp);
		}

		//Load Items
		for (temp = elements_node.child("IMAGES").child("image"); temp != NULL; temp = temp.next_sibling("image"))
		{
			temp_cutscene->LoadImg(temp);
		}

		//Load Items
		for (temp = elements_node.child("TEXTS").child("text"); temp != NULL; temp = temp.next_sibling("text"))
		{
			temp_cutscene->LoadText(temp);
		}

		//Load Music
		for (temp = elements_node.child("MUSIC").child("music"); temp != NULL; temp = temp.next_sibling("music"))
		{
			temp_cutscene->LoadMusic(temp);
		}

		//Load Fx
		for (temp = elements_node.child("FX").child("fx"); temp != NULL; temp = temp.next_sibling("fx"))
		{
			temp_cutscene->LoadFx(temp);
		}
		// -----------------------------------------------------------------------------------

		// LOAD STEPS --------------
		steps_node = cutscene_node.child("steps");
		for (temp = steps_node.child("step"); temp != NULL; temp = temp.next_sibling("step"))
		{
			temp_cutscene->LoadStep(temp, temp_cutscene);
		}


		// ---------------------

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
			App->scene->ChangeState(CUTSCENE);
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
			App->scene->ChangeState(INGAME);
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

CS_Element* Cutscene::GetElement(const char* name)
{
	CS_Element* temp = nullptr;

	//Iterates elements list of the cutscene to find the correct element
	for (std::list<CS_Element*>::iterator it = elements.begin(); it != elements.end(); it++)
	{
		if (it._Ptr->_Myval->name == name)
		{
			temp = it._Ptr->_Myval;
			break;
		}
	}

	return temp;
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
	bool active = false;
	//Iterate the steps of the cutscene to update the active ones
	std::list<CS_Step*>::iterator temp = steps.begin();
	while (temp != steps.end())
	{
		CS_Step* step = *temp;

		//Init the step (only once)
		if (timer.ReadSec() >= step->GetStartTime() && step->isActive() == false && step->isFinished() == false)
		{
			step->StartStep();
		}

		//Update the state to perform its action
		if (step->isActive() == true)
		{
			step->DoAction(dt);
		}

		temp++;
	}

	//If all stpes have been reproduced, finish the cutscene
	if (steps_done >= num_steps)
	{
		finished = true;
	}

	return ret;
}

bool Cutscene::LoadNPC(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		this->elements.push_back(new CS_Element(CS_NPC, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("update").as_bool(false), nullptr));
		ret = true;
	}
	return ret;
}

bool Cutscene::LoadDynObject(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		elements.push_back(new CS_Element(CS_DYNOBJECT, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("update").as_bool(false), nullptr));
		ret = true;
	}
	return false;
}

bool Cutscene::LoadItem(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		elements.push_back(new CS_Element(CS_ITEM, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("update").as_bool(false), nullptr));
		ret = true;
	}
	return false;
}

bool Cutscene::LoadImg(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		iPoint pos(node.attribute("x").as_int(0), node.attribute("y").as_int(0));
		SDL_Rect rect = { node.attribute("tex_x").as_int(0), node.attribute("tex_y").as_int(0), node.attribute("tex_w").as_int(0), node.attribute("tex_h").as_int(0) };
		elements.push_back(new CS_Image(CS_IMAGE, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("update").as_bool(false), node.attribute("file").as_string(""), rect, pos));
		ret = true;
	}
	return false;
}

bool Cutscene::LoadText(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		elements.push_back(new CS_Element(CS_TEXT, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("update").as_bool(false), nullptr));
		ret = true;
	}
	return false;
}

bool Cutscene::LoadMusic(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		elements.push_back(new CS_Element(CS_MUSIC, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("update").as_bool(false), nullptr));
		ret = true;
	}
	return false;
}

bool Cutscene::LoadFx(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		elements.push_back(new CS_Element(CS_FX, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("update").as_bool(false), nullptr));
		ret = true;
	}
	return false;
}


bool Cutscene::LoadStep(pugi::xml_node& node, Cutscene* cutscene) //Pass the cutscene that it's involved to link
{
	bool ret = false;
	if (node != NULL && cutscene != nullptr)
	{
		CS_Step* temp_step = new CS_Step(node.attribute("n").as_int(-1), node.attribute("start").as_int(-1), node.attribute("duration").as_int(-1), cutscene);

		temp_step->SetAction(node);
		temp_step->SetElement(node);

		this->steps.push_back(temp_step);
		num_steps++;
		ret = true;
	}
	return ret;
}

void Cutscene::StepDone()
{
	steps_done++;
}

bool Cutscene::SetMap(pugi::xml_node& node)
{
	map_id = node.child("MAP").attribute("id").as_int(-1);
	//tmx = node.attribute("file").as_string("");
	return false;
}

//--------------------------------------


//CS ELEMENTS ------------------------------
CS_Element::CS_Element(CS_Type type, int n, const char* name, bool active, const char* path):type(type), n(n), active(active),name(name)
{
	if (path != nullptr)
	{
		this->path = path;
	}
}

CS_Element::~CS_Element()
{
}


//CS STEPS ----------------------------------
CS_Step::CS_Step(int n, int start, int duration, Cutscene* cutscene):n(n), start(start), duration(duration), cutscene(cutscene)
{
}

CS_Step::~CS_Step()
{
}

bool CS_Step::DoAction(float dt)
{
	if (cutscene->timer.ReadSec() <= start + duration) // TODO MED -> every action will trigger the finished step when completed
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
		case ACT_PLAY:
			action_name = "play";
			break;
		default:
			action_name = "none";
			break;
		}

		LOG("Step '%i' performing '%s' on '%s'", n, action_name.c_str(), element->name.c_str());
	}

	else
	{
		FinishStep();
	}

	return true;
}

uint CS_Step::GetStartTime() const
{
	return start;
}

void CS_Step::StartStep()
{
	active = true;
	LOG("Step %i started at %.3fs", n, cutscene->timer.ReadSec());
}

void CS_Step::FinishStep()
{
	active = false;
	finished = true;
	cutscene->StepDone(); 
	LOG("Step %i finished at %.3fs", n, cutscene->timer.ReadSec());
}

//Set the action programmed to that step that will execute the linked element
void CS_Step::SetAction(pugi::xml_node& node)
{
	std::string action_type = node.child("element").attribute("action").as_string("");

	if (action_type == "enable")
	{
		action = ACT_ENABLE;
	}
	else if (action_type == "disable")
	{
		action = ACT_DISABLE;
	}
	else if (action_type == "move")
	{
		action = ACT_MOVE;
	}
	else if (action_type == "play")
	{
		action = ACT_PLAY;
	}
	else
	{
		action = ACT_NONE;
	}
}

//Link the element of the cutscene with the step
void CS_Step::SetElement(pugi::xml_node& node)
{
	std::string element_name = node.child("element").attribute("name").as_string("");
	element = cutscene->GetElement(element_name.c_str()); 
}

bool CS_Step::isActive() const
{
	return active;
}

bool CS_Step::isFinished() const
{
	return finished;
}

// CS IMAGE -----------------
CS_Image::CS_Image(CS_Type type, int n, const char* name, bool active, const char* path, SDL_Rect rect, iPoint pos):
	CS_Element(type, n, name, active, path),rect(rect),pos(pos){}

CS_Image::~CS_Image()
{
}

//-----------------------------
