#include "j1EntityElementsScene.h"
#include "Soldier.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "j1App.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1FileSystem.h"


j1EntityElementScene::j1EntityElementScene()
{
	name = "entityelement";
}

j1EntityElementScene::~j1EntityElementScene()
{
}

bool j1EntityElementScene::Awake(pugi::xml_node &config)
{
	return true;
}

bool j1EntityElementScene::Start()
{
	bool ret = true;
	char* buf;
	int size = App->fs->Load("config.xml", &buf);
	XML.load_buffer(buf, size);
	return ret;
}

bool j1EntityElementScene::PreUpdate()
{
	return true;
}

bool j1EntityElementScene::Update(float dt)
{
	bool ret = true;

	std::list<SceneElement*>::iterator item3 = elementscene.begin();
	while (item3 != elementscene.end())
	{
		item3._Ptr->_Myval->Update(dt);
		item3++;
	}

	return ret;
}

bool j1EntityElementScene::PostUpdate()
{
	std::list<SceneElement*>::iterator item = elementscene.end();
	item--;
	while (item != elementscene.begin())
	{
		item._Ptr->_Myval->Draw();
		item--;
	}

	//Draw PLAYER
	if (elementscene.size() > 0)
	{
		item._Ptr->_Myval->Draw();
	}

	return true;
}

bool j1EntityElementScene::CleanUp()
{
	bool ret = true;
	std::list<SceneElement*>::iterator item = elementscene.begin();
	while (item != elementscene.end())
	{
		item._Ptr->_Myval->CleanUp();
		item++;
	}
	return ret;
}

bool j1EntityElementScene::DelteElements()
{
	App->collision->waittodelete = true;
	std::list<SceneElement*>::iterator item = elementscene.end();
	item--;
	if (elementscene.begin()._Ptr->_Myval->name != "Link")
	{
		std::list<SceneElement*>::iterator temp = elementscene.begin();
		while (temp != elementscene.end())
		{
			if (temp._Ptr->_Myval->name == "Link")
			{
				std::swap(temp._Ptr->_Myval, elementscene.begin()._Ptr->_Myval);
			}
			temp++;
		}
	}
	return true;
}

Soldier* j1EntityElementScene::CreateSoldier(uint id, pugi::xml_node& config)
{

	Soldier* element = new Soldier();
	element->Awake(config, id);
	element->Start();
	elementscene.push_back(element);

	return element;
}

bool j1EntityElementScene::DeleteEnemy(Soldier* enemy)
{
	if (enemy != nullptr)
	{
		elementscene.remove(enemy);
		App->scene->enemy.remove(enemy);
		enemy->collision_feet->to_delete = true;
		enemy = nullptr;
		delete enemy;
	}

	return true;
}

Player* j1EntityElementScene::CreatePlayer()
{
	Player* element = new Player();
	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = LoadConfig(config_file);
	element->Awake(config.child(element->name.c_str()));
	elementscene.push_back(element);
	element->Start();

	return element;
}

// ---------------------------------------------
pugi::xml_node j1EntityElementScene::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("Levels.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE(buf);

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("levels");

	return ret;
}