#include "CutsceneManager.h"
#include "j1FileSystem.h"
#include "SceneElements.h"
#include "p2Log.h"

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
		temp_cutscene->name = cutscene_node.attribute("name").as_string(""); //Sets its name.

		//Add all Elements involved in its list
		elements_node = cutscene_node.child("elements");

		//Load Maps 
		for (temp = elements_node.child("MAP").child("map"); temp != NULL; temp = temp.next_sibling("map"))
		{
			//temp_cutscene->LoadMap(temp);
		}

		//Load Entity
		for (temp = elements_node.child("NPCs").child("npc"); temp != NULL; temp = temp.next_sibling("npc"))
		{
			//temp_cutscene->LoadEntity(temp);
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
	}
	return true;
}

bool j1CutSceneManager::Update(float dt)
{
	bool ret = true;

	if (active_cutscene != nullptr)
	{
		active_cutscene->Update(dt); //Update elements of the cutscene
	}

	return ret;
}

//Set to active the correct cutscene
bool j1CutSceneManager::ActiveCutscene(uint id)
{
	for (std::list<Cutscene*>::iterator it = cutscenes.begin(); it != cutscenes.end(); it++)
	{
		if (id == it._Ptr->_Myval->GetID())
		{
			active_cutscene = *it;
			LOG("%s cutscene activated", active_cutscene->name.c_str());
			return true;
		}
	}
	return false;
}

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

uint Cutscene::GetID() const
{
	return id;
}

bool Cutscene::Start()
{
	timer.Start();
	return true;
}

bool Cutscene::Update(float dt)
{
	bool ret = true;
	std::list<SceneElement*>::iterator element = elements.begin();
	while (element != elements.end())
	{
		element._Ptr->_Myval->Update(dt);
		element++;
	}

	return ret;
}
