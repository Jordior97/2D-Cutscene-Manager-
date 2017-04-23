#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "Animation.h"
#include "j1Scene.h"
#include "j1Gui.h"
#include "j1GuiEntity.h"
#include "j1GuiElements.h"
#include "j1Player.h"
#include "j1Window.h"
#include <assert.h>

/////////////////////////////// IMAGE METHODS ///////////////////////////////

Image::Image(SDL_Rect rectangle, iPoint position, std::string identifier, uint id) : j1GuiEntity(rectangle, position, identifier, id) {

	type = IMAGE;
	start = true;
	resize = true;
}


void Image::Update()
{

	if (start == true)
	{

		for (int i = 0; i<elements.size(); i++)
		{
			elements[i]->position.x += position.x;
			elements[i]->position.y += position.y;
			elements[i]->diferential.x = (elements[i]->position.x - position.x);
			elements[i]->diferential.y = (elements[i]->position.y - position.y);
		}
		start = false;
	}

	for (int i = 0; i<elements.size(); i++)
	{
		elements[i]->position.x = position.x + elements[i]->diferential.x;
		elements[i]->position.y = position.y + elements[i]->diferential.y;
	}
}


void Image::Draw()
{

	App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), position.x, position.y, &Hitbox, 0);

}

void Image::AssignNumber(uint n)
{
	switch (n)
	{
	case 0:
		Hitbox.x = 259;
		Hitbox.y = 13;
		break;
	case 1:
		Hitbox.x = 267;
		Hitbox.y = 13;
		break;
	case 2:
		Hitbox.x = 275;
		Hitbox.y = 13;
		break;
	case 3:
		Hitbox.x = 283;
		Hitbox.y = 13;
		break;
	case 4:
		Hitbox.x = 291;
		Hitbox.y = 13;
		break;
	case 5:
		Hitbox.x = 259;
		Hitbox.y = 21;
		break;
	case 6:
		Hitbox.x = 267;
		Hitbox.y = 21;
		break;
	case 7:
		Hitbox.x = 275;
		Hitbox.y = 21;
		break;
	case 8:
		Hitbox.x = 283;
		Hitbox.y = 21;
		break;
	case 9:
		Hitbox.x = 291;
		Hitbox.y = 21;
		break;
	}

}

Image::~Image()
{
	elements.clear();
}

/////////////////////////////// TEXT METHODS ///////////////////////////////

Text::Text(FontName search, const char* write, SDL_Color color, uint length, iPoint pos, uint size, bool draw, std::string identifier, uint id) :size(size), draw(draw), length(length), j1GuiEntity({ 0,0,0,0 }, pos, identifier, id)
{
	type = TEXT;
	text = write;
	this->color = color;
	font_name = search;
	switch (search)
	{
	case GANONF:
		font = App->font->Load("fonts/zelda_fonts/ReturnofGanon.ttf", size);
		break;
	case PIXEL:
		font = App->font->Load("fonts/zelda_fonts/Pixeled.ttf", size);
		break;
	case PIXELMORE:
		font = App->font->Load("fonts/zelda_fonts/PixeledMore.ttf", size);
		break;
	case POKE1:
		font = App->font->Load("fonts/poke_fonts/poke1.ttf", size);
		break;
	default:
		font = App->font->Load("fonts/zelda_fonts/ReturnofGanon.ttf", size);
		break;
	}

	CheckString(std::string(write));
	text_texture = App->font->Print(text.c_str(), color, font);
	Hitbox.w /= 2;
	Hitbox.h /= 2;
}

void Text::Visible(bool yes)
{
	Text* item = next_line;
	if (yes) {
		visible = true;
		while (item != nullptr)
		{
			item->visible = true;
			item = item->next_line;
		}
	}
	else {
		visible = false;
		while (item != nullptr)
		{
			item->visible = false;
			item = item->next_line;
		}
	}
}

void Text::CheckString(std::string string)
{
	iPoint volum;
	/* int pixels;
	App->font->CalcSize("l", pixels, volum.x, font);*/
	App->font->CalcSize(string.c_str(), volum.x, volum.y, font);
	if (string.size() > length)
	{
		//int characters = length / pixels;
		text = string.substr(0, length + 1);
		if (text[length] != ' ') {
			text += "-";
		}
		next_line = App->gui->CreateText(font_name, string.substr(length + 1, 100).c_str(), length, { position.x,position.y + volum.y / 2 }, size, color, draw);
	}
}

void Text::Draw()
{

	App->render->Blit(text_texture, position.x, position.y, NULL, 0, false);

}

void Text::Update()
{


}

void Text::Write(const char* string)
{
	text = string;
	//font = App->font->Load("fonts/zelda_fonts/ReturnofGanon.ttf", 23);
	text_texture = App->font->Print(text.c_str(), color, font);

}

Text::~Text() {

}

/////////////////////////////// BUTTON METHODS ///////////////////////////////
Button::Button(SDL_Rect rectangle, iPoint pos, iPoint stat2, iPoint stat3, bool animated, std::string identifier, uint id, const char* textstring, uint textsize, iPoint textpos) :j1GuiEntity(rectangle, pos, identifier, id)
{
	type = BUTTON;
	state = normal;
	texture2.x = stat2.x;
	texture3.x = stat3.x;
	texture2.y = stat2.y;
	texture3.y = stat3.y;
	texture2.w = texture3.w = Hitbox.w;
	texture2.h = texture3.h = Hitbox.h;
	if (textstring != nullptr) {
		buttontext = new Text(GANONF, textstring, { 255,255,255,255 }, 20, { textpos.x,textpos.y }, textsize, true, "undefined", 0);
	}

	start = true;
	if (animated == true) {
		anim = new Animation();
	}

}

void Button::Draw()
{
	switch (state)
	{
	case normal:
		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), position.x, position.y, &Hitbox, 0, resize);
		break;
	case over:
		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), position.x, position.y, &texture2, 0, resize);
		break;
	case clicked:
		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), position.x, position.y, &texture3, 0, resize);
		break;
	}
	if (buttontext != nullptr) {
		buttontext->Draw();
	}
}

void Button::Update()
{
	if (anim != nullptr) {
		texture2 = anim->GetCurrentFrame();
	}
	if (selected == true)
	{
		if (click == true)
		{
			state = clicked;
		}
		else
		{
			state = over;
		}
	}
	else
	{
		state = normal;
	}
}

Button::~Button()
{
	delete buttontext;

}

/////////////////////////////// DIALOGUE METHODS ///////////////////////////////

Dialogue::Dialogue(const char*string) :j1GuiEntity({ 0,82,190,62 }, { 40,150 })
{
	int win_marge = (App->win->GetWidth() - App->scene->start_menu->Hitbox.w*App->win->GetScale()) / 4;
	position = { win_marge + 40,150 };
	type = DIALOGUE;
	lines = App->gui->CreateText(GANONF, string, 29, { position.x + 10, 0 }, 30, { 255,255,255,255 }, false);

}

void Dialogue::Draw()
{

	App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), position.x, position.y, &Hitbox, 0);
	SDL_Rect viewport = { 0,312,Hitbox.w * 3,Hitbox.h*1.5 + 5 };
	SDL_RenderSetViewport(App->render->renderer, &viewport);
	lines->Draw();
	Text* item = lines;
	while (item != nullptr) {
		item->Draw();
		item = item->next_line;
	}
	SDL_RenderSetViewport(App->render->renderer, NULL);
}

void Dialogue::Update()
{
	if (push == true && SDL_GetTicks() - timer > 200)
	{
		push = false;
		PushLine(false);
	}

}

void Dialogue::PushLine(bool push)
{
	if (end == false)
	{
		Text* item = lines;

		while (item != nullptr)
		{
			item->position.y -= (Hitbox.h / 8) + diferential; //+ 0.5
			item = item->next_line;
		}
		item = lines;
		while (item->next_line != nullptr)
		{
			item = item->next_line;
		}
		if (item->position.y + position.y + 8 < position.y)
		{
			options = App->gui->CreateSelector("Yes", "No", this);
			options->parent = this;
			end = true;
		}
		diferential += 0.2;
		timer = SDL_GetTicks();
		this->push = push;
	}
}

Dialogue::~Dialogue()
{
}

Selector::Selector(const char* first_option, const char* second_option, j1GuiEntity* parent)
{
	this->parent = parent;
	this->first = App->gui->CreateText(GANONF, first_option, 50, { parent->position.x + 20,parent->position.y + 5 }, 30);
	this->second = App->gui->CreateText(GANONF, second_option, 50, { parent->position.x + 20,parent->position.y + 20 }, 30);
	selector = App->gui->CreateText(GANONF, ">", 50, { first->position.x - 10,first->position.y }, 30);
	position = true;
}

void Selector::Handle_Input()
{
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
	{
		if (position)
		{
			selector->position.y = second->position.y;
			position = false;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		if (position == false)
		{
			selector->position.y = first->position.y;
			position = true;
		}
	}
}

Selector::~Selector() {}

/////////////////////////////// MENU METHODS ///////////////////////////////

ZeldaMenu::ZeldaMenu()
{
	id_selected = 0;
	visible = false;
	type = MENU;
}

void ZeldaMenu::AddElement(j1GuiEntity* element)
{
	if (menu_buttons.size() == 0)
	{
		selected = (Button*)element;
		selected->selected = true;
	}
	if (element->type == BUTTON)
	{
		element->id = menu_buttons.size() + 1;
		menu_buttons.push_back((Button*)element);
	}
	if (element->type == IMAGE)
	{
		menu_images.push_back((Image*)element);
	}
	if (element->type == TEXT)
	{
		menu_texts.push_back((Text*)element);
	}
}

void ZeldaMenu::ResetInventory()
{
	if (menu_buttons.size() > 0)
	{
		menu_buttons.clear();
	}
	selected = nullptr;
	App->scene->hud->GetImage(1)->elements[0]->Hitbox.y = 257; //setting item selected in box to nothing
	menu_texts[2]->Visible(false);
	menu_texts[3]->Visible(false);
	menu_texts[4]->Visible(false);
	menu_images[1]->Hitbox.y = 372;
	menu_images[2]->Hitbox.y = 256;
	menu_texts[0]->Write("SELECT AN ITEM");
	menu_texts[1]->Write("PICK ITEM");

}

void ZeldaMenu::Select(int value)
{
	if (id_selected + value < menu_buttons.size() && id_selected + value >= 0)
	{
		menu_buttons[id_selected]->click = false;
		menu_buttons[id_selected]->selected = false;
		id_selected += value;
		menu_buttons[id_selected]->selected = true;
		selected = menu_buttons[id_selected];
	}
}

void ZeldaMenu::Click()
{
	if (menu_buttons.size() > 0)
	{
		menu_buttons[id_selected]->click = true;
	}
}


Image* ZeldaMenu::GetImage(uint id)
{
	return menu_images[id];
}

Button* ZeldaMenu::GetSelected()
{
	return menu_buttons[id_selected];
}

void ZeldaMenu::UnClick()
{
	if(menu_buttons.size()>0)
		menu_buttons[id_selected]->click = false;
}

void ZeldaMenu::Update()
{
}

void ZeldaMenu::Handle_Input()
{
	if (App->scene->inventory == true && App->scene->ingame == true && this->identifier == "start_menu")
	{
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
		{
			Select(1);
		}
		else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
		{
			Select(-1);
		}
		else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		{
			Click();
		}
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_UP)
		{
			UnClick();
		}
	}
}


void ZeldaMenu::OpenClose(bool open)
{
	for (uint i = 0; i < menu_buttons.size(); i++) {
		menu_buttons[i]->visible = open;
	}
	for (uint i = 0; i < menu_images.size(); i++) {
		menu_images[i]->visible = open;
		if (menu_images[i]->elements.size() > 0)
		{
			for (int j = 0; j < menu_images[i]->elements.size(); j++)
			{
				menu_images[i]->elements[j]->visible = open;
			}
		}
	}
	for (uint i = 0; i < menu_texts.size(); i++) {
		if (open == false)
		{
			menu_texts[i]->visible = open;
			if (menu_texts[i]->next_line != nullptr)
			{
				Text*line = menu_texts[i]->next_line;
				while (line != nullptr)
				{
					line->visible=open;
					line = line->next_line;
				}
			}
		}
		else if (i<2){
			menu_texts[i]->visible = open;
			if (menu_texts[i]->next_line != nullptr)
			{
				Text*line = menu_texts[i]->next_line;
				while (line != nullptr)
				{
					line->visible = open;
					line = line->next_line;
				}
			}
		}
	}
	visible = open;
}

void ZeldaMenu::Move(bool x_axis, float speed) //bool x_axis is to know in wich axis do we move (x=true/y=false)
{
	if (x_axis)
	{
		for (int i = 0; i < menu_buttons.size(); i++)
		{
			menu_buttons[i]->position.x += speed;
		}
		for (int i = 0; i < menu_images.size(); i++)
		{
			menu_images[i]->position.x += speed;
			if (menu_images[i]->elements.size() > 0)
			{
				if (x_axis == false)
				{
					for (int j = 0; j < menu_images[i]->elements.size(); j++)
					{
						menu_images[i]->elements[j]->position.x += speed;
					}
				}
			}
		}
		for (int i = 0; i < menu_texts.size(); i++)
		{
			menu_texts[i]->position.x += speed;
			if (menu_texts[i]->next_line != nullptr)
			{
				Text*line = menu_texts[i]->next_line;
				while (line != nullptr)
				{
					line->position.x += speed;
					line = line->next_line;
				}
			}
		}
		position.x += speed;
	}
	else
	{
		for (int i = 0; i < menu_buttons.size(); i++)
		{
			menu_buttons[i]->position.y += speed;
		}
		for (int i = 0; i < menu_images.size(); i++)
		{
			menu_images[i]->position.y += speed;
			if (menu_images[i]->elements.size() > 0)
			{
				for (int j = 0; j < menu_images[i]->elements.size(); j++)
				{
					menu_images[i]->elements[j]->position.y += speed;
				}
			}
		}
		for (int i = 0; i < menu_texts.size(); i++)
		{
			menu_texts[i]->position.y += speed;
			if (menu_texts[i]->next_line != nullptr)
			{
				Text*line = menu_texts[i]->next_line;
				while (line != nullptr)
				{
					line->position.y += speed;
					line = line->next_line;
				}
			}
		}
		position.y += speed;
	}
}

ZeldaMenu::~ZeldaMenu()
{
}

// Entity Elements ---------------------------------------------------