#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1FadeToBlack.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "j1GuiEntity.h"
#include "j1GuiElements.h"
#include "j1Scene.h"
#include "j1SceneIntro.h"
#include "Soldier.h"
#include "j1Player.h"
#include "j1FileSystem.h"
#include "j1Collision.h"
#include "CutsceneManager.h"
#include "j1AnimationManager.h"

j1SceneIntro::j1SceneIntro() : j1Module()
{
	name = "scene";
}

// Destructor
j1SceneIntro::~j1SceneIntro()
{}

// Called before render is available
bool j1SceneIntro::Awake()
{
	LOG("Loading SceneIntro");
	bool ret = true;
	return ret;
}

// Called before the first frame
bool j1SceneIntro::Start()
{
	fade = true;
	menu = true;
	bg_anim = 0;
	Menu_bg = App->tex->Load("gui/title_screen/menu_bg.png");
	TitleScreen_letters = App->tex->Load("gui/title_screen/letters_menu.png");
	LoadMainMenu();
	return true;
}

// Called each loop iteration
bool j1SceneIntro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1SceneIntro::Update(float dt)
{
	if (goHouse)
	{
		if (fade)
		{
			App->fadetoblack->FadeToBlack(3);
			App->audio->FadeMusic(3);
			fade = false;
		}
		else
		{
			if (App->fadetoblack->Checkfadetoblack())
			{
				App->scene->Start();
				main_menu->OpenClose(false);
				goHouse = false;
			}
		}
	}

	return true;
}

// Called each loop iteration
bool j1SceneIntro::PostUpdate()
{
	bool ret = true;

	if (App->scene->ingame == false)
	{
		if (menu)
		{
			if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
			{
				main_menu->Select(1);
			}
			if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
			{
				main_menu->Select(-1);
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			ret = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			if (main_menu->id_selected == 1)
			{
				//TODO 3: Start the IntroCutscene by passing its ID (call the correct function of the Cutscene Manager)

				//Uncomment this when TODO 3 is done.
				//App->scene->ingame = true;
			}
		}
	}
	
	return ret;
}

void j1SceneIntro::LoadMainMenu()
{
	main_menu = App->gui->CreateZeldaMenu();
	Button* menu_button=App->gui->CreateButton({ 1,146,110,17 }, { 172 / 2,180 / 2 }, { 0,0 }, { 112,164 }, true);
	menu_button->selected = true;
	menu_button->anim->PushBack({ 112,146,110,17 });
	menu_button->anim->PushBack({ 223,146,110,17 });
	menu_button->anim->PushBack({ 334,146,110,17 });
	menu_button->anim->PushBack({ 1,164,110,17 });
	menu_button->anim->PushBack({ 334,146,110,17 });
	menu_button->anim->PushBack({ 223,146,110,17 });
	menu_button->anim->speed = 0.25f;
	menu_button->resize = false;
	main_menu->AddElement(menu_button);
	menu_button= App->gui->CreateButton({ 1,182,125,17 }, { 172 / 2,210 / 2 }, { 0,0 }, { 127,200 }, true);
	menu_button->anim->PushBack({ 127,182,125,17 });
	menu_button->anim->PushBack({ 253,182,125,17 });
	menu_button->anim->PushBack({ 379,182,125,17 });
	menu_button->anim->PushBack({ 1,200,125,17 });
	menu_button->anim->PushBack({ 379,182,125,17 });
	menu_button->anim->PushBack({ 253,182,125,17 });
	menu_button->anim->speed = 0.25f;
	menu_button->resize = false;
	main_menu->AddElement(menu_button);
	menu_button = App->gui->CreateButton({ 1,218,110,17 }, { 172 / 2, 240/ 2 }, { 0,0 }, { 112,236 }, true);
	menu_button->anim->PushBack({ 112,218,110,17 });
	menu_button->anim->PushBack({ 223,218,110,17 });
	menu_button->anim->PushBack({ 334,218,110,17 });
	menu_button->anim->PushBack({ 1,236,110,17 });
	menu_button->anim->PushBack({ 334,218,110,17 });
	menu_button->anim->PushBack({ 223,218,110,17 });
	menu_button->anim->speed = 0.25f;
	menu_button->resize = false;
	main_menu->AddElement(menu_button);
}

void j1SceneIntro::LoadNewMap(int id)
{
	//Load the correct map accord to the id passed
	switch (id)
	{
	case 1: //ID = 1 -> Set the first map of the game (Link's house)
		goHouse = true;
		break;
	default:
		break;
	}
	
}

// Called before quitting
bool j1SceneIntro::CleanUp()
{
	LOG("Freeing sceneintro");

	return true;
}