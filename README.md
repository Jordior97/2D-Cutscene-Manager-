# 2D Cutscene Manager
This is my Personal Research about creating and managing 2D cutscenes

## INTRODUCTION
Before starting to program, let's talk about some introduction concepts.

### WHAT IS A CUTSCENE?

It’s a video sequence where the player doesn’t have the control (or it’s limited), and it has multiple functions:

-	Makes advance the plot.
- 	Presents new characters.
-	Provides crucial information (dialogues, clues, background information…).
-	And more.

These scenes are made by scheduled events, using often game graphics. 
They can also be animated, real images or previously rendered images (and transmitted from a video file). 
These pre-made videos used in video games are called “Full Motion Videos” (FMVs).

### TYPES OF CUTSCENES

**1) Live-Action Cutscenes:**

Similar to films, they use actors, real scenarios, cameras... to create scenographies that  will be introduced into the game.
Due to this, Interactive Films were created (hours of real action footage, without the characteristic interactivity and complexity of the games).

![](https://media.giphy.com/media/3og0ItU6KZEeXNAC9G/giphy.gif)

**2) Animated Cutscenes:**
- **Pre-rendered scenes**

The developers animate and render these scenes, and they take advantages of the compter generated image, 
traditional animation or graphic novel techniques.

This type of scenes has two notable problems:
1.	Quality difference can create difficulties for the recognition of the game (the player used to play with low-quality graphics) and this can break the flow of the game.
2.	They can’t be adapted to the game state (clothes, etc.).

![](https://s-media-cache-ak0.pinimg.com/originals/02/ea/7c/02ea7c6497c18c1c43efb9c5c85294c6.jpg)

- **Real Time Cutscenes**

Rendered using the same game engine as the graphics during the gameplay. It’s known as *Machinima*.

*Comparison with Live–Action Cutscenes:*
- Lower detail.
- Adaptation to the game state.
- Player can control the camera movement.

![](http://rs1370.pbsrc.com/albums/ag248/Sente_Black/My%20Gifs/ratchetandclank3dancinggif_zpsfc5006a2.gif~c200)  ![](https://24.media.tumblr.com/c6b077223e18725a014afef59691f71b/tumblr_mmtg15zpf51rlm74no1_400.gif)

**3) Interactive Cutscenes:**

Player can take the control of the character doing some indicated actions ( pressing  a buttons sequence, for exaple) 
shown on the screen. 
He must follow these instructions in order to continue the game.
This mechanic is commonly called *quick time events*.

![](http://static1.gamespot.com/uploads/original/1457/14573540/2834776-0956136431-But%2Bh.gif)


## HOW MY CUTSCENE MANAGER WORKS

This project has been built in C++ language using Visual Studio. It's divided into different MODULES: each one works in a specific area (audio, textures, file system...). So, the Cutscene Manager is developed as one of them (works on the specific area of Cutscenes).
This manager works with Real Time Cutscenes, it's implemented using SDL2.0 libraries and read the cutscenes from XML files.

The functionality of the Cutscene Manager is to **TAKE CONTROL OF THE GAME** when an event triggers a specific cutscene (starting the game, when we access into a specific zone, etc.). 

### CUTSCENE MANAGER ANATOMY
```cpp
class j1CutSceneManager : public j1Module
{
	bool StartCutscene(uint id); 	//Active a cutscene when an event triggers it
	bool FinishCutscene(); 	//Deactive the active cutscene

	list<std::string> paths;  //container with names of all .xml paths of the cutscenes
	Cutscene* active_cutscene = nullptr;  //To know wich cutscene is reproduced at the moment
};
```
The Cutscene Manager works as a container that stores all the cutscenes Paths. This paths are used to acces the correct file when a specific cutscene is triggered.


### CUTSCENE ANATOMY
```cpp
class Cutscene
{
	//LOAD ELEMENTS FUNCTIONS ---------
	bool LoadElement(pugi::xml_node&);
	// ---------------------------------

	//STEPS FUNCTIONS -----------------------------------
	bool LoadStep(pugi::xml_node&, Cutscene* cutscene);
	void StepDone();
	//----------------------------------------------------

	uint id = 0;  //ID to locate when triggered
	j1Timer	timer;  //To control reproducing time of the cutscene
	std::list<CS_Element*> elements; //Elements controlled by the cutscene
	std::list<CS_Step*> steps; //Steps to follow in order when reproduced
};
```
CUTSCENES CAN BE DIVIDED INTO 2 NOTABLE PARTS:

**ELEMENTS:** game objects that will be controlled by the cutscene during its reproduction.

They can be of different types, for example:
 - IMAGES
 - TEXTS
 - MUSICS
 - SOUND FX
 - NPCs
 - MAPS
 - ...
 
These elements store different variables (depending on its type) that the cutscene can modify through a STEP SYSTEM.

**STEPS:** ordered actions the cutscene will call through its reproduction time. Each step is **linked to an element** of the specific cutscene. It has got also defined a **specified action** that will be applied to the linked object.

Different action types can be defined:
 - ENABLE -> Activate the element (it will be able to be updated & drawn).
 - DISABLE -> Deactivate the element (to not be drawn or updated).
 - MOVE -> change it's coordinates to a specified destination.
 - PLAY -> reproduce the linked music or sound fx.
 - ...
  

### ELEMENT ANATOMY
```.cpp
class CS_Element
{
	CS_Element(CS_Type type, int n, const char* name, bool active, const char* path);
	virtual ~CS_Element();

	std::string name;
	bool active = false;
	CS_Type type = CS_NONE;	  //Cutscene element type
	int n = -1;		  //identifier
	std::string path;	  //auxiliar path (texture file name, animation, sound/music file...)
};
```
This is the base class for a cutscene element. It has a child for every different type of element, so it can store multiple and different variables (TEXTURES for images, GAME ENTITIES for NPC's, STRINGS for texts... and more).

### STEP ANATOMY
```.cpp
class CS_Step
{
	//STEP FUNCTIONS -------------
	void SetElement(pugi::xml_node&);
	void SetAction(pugi::xml_node&);
	//------------------------------

	//ACTION FUNCTIONS ----------
	bool DoAction(float dt);  //Perform the correct action according to the action type assigned
	bool DoMovement(float dt);
	void Play();
	void StopMusic();
	void ActiveElement();
	void DisableElement();
	//---------------------------

	Cutscene* cutscene = nullptr;		//Pointer to the cutscene that it is integrated
	Action_Type act_type= ACT_NONE;		//Type of action that will be executed in this step
	CS_Element*	element = nullptr;	//LINKED ELEMENT to apply the action
};
```
Steps always are linked to an ELEMENT and ACTION.
 
### CUTSCENE XML STRUCTURE

Every cutscene is stored in a different XML file to keep the project organized. The Cutscene Manager contains all the names of these files in a list, so it can access them to load the Cutscene and store its respective data (Elements & Steps).

Let's see an EXAMPLE:
```cpp
<Cutscene name="HelloWorld" id="0">

  <!--LOAD INFO OF EACH ELEMENT THAT ACT IN THE CUTSCENE-->
  <elements>
    <MAP id="1" file="TiledLinkHouse.tmx"/>   
    <NPCs>
      <npc n="0" name="Link" x="100" y="100" dir="down" state="idle" active="true"/>
      <npc n="1" name="soldier" x="100" y="400" dir="up" state="idle" active="true"/>
    </NPCs>
    <TEXTS>
      <text n="5" name="text1" text="Hello, what are you doing here?" x="100" y="100" active="false"/>
      <text n="6" name="text2" text="I'm doing pancakes" x="100" y="100" active ="false"/>
    </TEXTS>
    <MUSIC>
      <music n="7" name="KakarikoVillage" path="audio/music/ZELDA/Zeldakakariko_village.ogg"/>
    </MUSIC>
  </elements>

  <!--EXECUTE ALL ACTIONS IN ORDER-->
  <steps>
    <step n="0" start="0" duration="0">
      <element name="KakarikoVillage" action="play"/>
    </step>
    <step n="1" start="0" duration="-1">
      <element name="Link" action="move" dir="down">
        <movement dest_x ="100" dest_y="380" speed="30"/>     
      </element>
    </step>
    <step n="2" start="1" duration="0">
      <element name="text1" action="enable"/>
    </step>
    <step n="3" start="4" duration="0">
      <element name="text1" action="disable"/>
    </step>
    <step n="4" start="5" duration="0">
      <element name="text2" action="enable"/>
    </step>
    <step n="5" start="8" duration="0">
      <element name="text2" action="disable"/>
    </step>
    <step n="6" start="10" duration="0">
      <element name="KakarikoVillage" action="stop"/>
    </step>
  </steps>
  
</Cutscene>
```
**1) LOAD ALL ELEMENTS INVOLVED IN THE CUTSCENE TO ITS OWN LIST:** each element, depending on its type, will store different type of variables.

**2) LOAD ALL STEPS IN THE CUTSCENE:** each step contains a time variable (*start*) that determines when it starts, the element that it's linked to it, and the duration time of the step (*note that movement actions has a duration of -1 because they are finished when the movement is completed*, not when the duration time is reached).

### HOW CAN WE ACCESS TO THE CORRECT XML FILE?
Well, in the config.xml file we have to write every path of the cutscene in a order, to enable the cutscene manager storing these paths.

```.cpp
<config>
 <cutscenemanager>
    <!--id= 0--> <file file="cutscenes/Cutscene1.xml"/>
    <!--id= 1--> <file file="cutscenes/Cutscene2.xml"/>
    <!--id= 2--> <file file="cutscenes/Cutscene3.xml"/>
  </cutscenemanager>
 </config>
```
So, in the Awake() function, we load every path in the list, IN ORDER: 
```.cpp
paths[0] = "cutscenes/Cutscene1.xml"
paths[1] = "cutscenes/Cutscene2.xml"
paths[2] = "cutscenes/Cutscene3.xml"
```

### CUTSCENE REPRODUCTION

A Cutscene can be activated through multiple ways:
- By clicking a button.
- Killing a monster.
- Accessing to a specific zone (with the tiled map structure, we can make a tile of the map contain an ID property that activates a specific cutscene when the player is above it).
- etc.

## IMPROVING THE SYSTEM

We can extend this Cutscene Manager as much as we want by implementing new ELEMENTS, ACTIONS, EFFECTS...
Here are few examples to implement:
- Link Game Entities with the NPC Cutscene Elements to control their STATE (moving, attacking, idle...) and POSITION while a cutscene is being reproduced.
- Try to take the control of the CAMERA to make it move (implement bezier curves for great results).
- Cutscenes can also trigger other cutscenes when they finish.
- Load a specific map of the game while the cutscene is being reproduced.

## RESUMING THIS MANAGEMENT
![](https://raw.githubusercontent.com/Jordior97/2D-Cutscene-Manager-/master/CutsceneManager.png)

# FURTHER READING & WATCHING
https://en.wikipedia.org/wiki/Cutscene
https://www.youtube.com/watch?v=oOhXBcmLq2M
https://www.reddit.com/r/gamedev/comments/1x3o9o/how_should_i_go_about_adding_unique_events/
https://www.reddit.com/r/gamedev/comments/1jjxrk/how_do_you_handle_cut_scenes_and_scripting_systems/













