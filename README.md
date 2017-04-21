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
They can also be animated, real images or previosly rendered images (and transmitted from a video file). 
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

	list<Cutscene*> cutscenes;  //container with all cutscenes
	list<std::string> paths;  //container with names of all .xml paths of the cutscenes
	Cutscene* active_cutscene = nullptr;  //To know wich cutscene is reproduced at the moment
};
```
The Cutscene Manager works as a container that stores all Cutscenes that will be reproduced at a specific moment of the game.
We load the cutscenes when the game starts by reading from XML files that store all the information needed.

When an event TRIGGERS a cutscene, we iterate the *cutscenes* list to find the correct cutscene and start it.

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

**ELEMENTS:** game objects that will be controlled by the cutscene during its reproduction.

In my case, they can be of different types:
 - IMAGES
 - TEXTS
 - MUSICS
 - SOUND FX
 - NPCs
 
These elements store different, variables depending on its type, that the cutscene can modify through a STEP SYSTEM.

**STEPS:** ordered actions the cutscene will call through its reproduction time. Each step is **linked to an element** of the specific cutscene. It has got also defined a **specified action** that will be applied to the linked object.

Different action types can be defined:
 - ENABLE
 - DISABLE
 - MOVE
 - PLAY
 
### CUTSCENE XML STRUCTURE

Every cutscene is stored in a different XML file to keep the project organized. The Cutscene Manager contains all the names of these files in a list, so it can access them to create every Cutscene and store its respective data (Elements & Steps). 

Let's see an EXAMPLE:
```cpp
<Cutscene name="HelloWorld" id="0">

  <!--LOAD INFO OF EACH ELEMENT THAT ACT IN THE CUTSCENE-->
  <elements>
    <MAP id="1" file="TiledLinkHouse.tmx"/>   
    <NPCs>
      <npc n="0" name="Link" x="100" y="100" dir="down" state="idle" update="true"/>
      <npc n="1" name="soldier" x="100" y="120" dir="up" state="idle" update="true"/>
    </NPCs>
    <TEXTS>
      <text n="5" name="text1" text="Hello, what are you doing here?" x="100" y="100" update="false"/>
      <text n="6" name="text2" text="I'm doing pancakes" x="100" y="100" update ="false"/>
    </TEXTS>
    <MUSIC>
      <music n="7" name="KakarikoVillage" path="audio/music/ZELDA/Zeldakakariko_village.ogg"/>
    </MUSIC>
  </elements>

  <!--EXECUTE ALL ACTIONS IN ORDER-->
  <steps>
    <step n="0" start="1">
      <element name="Link" action="move">
        <movement dest_x ="300" dest_y="300"/>     
      </element>
    </step>
    <step n="1" start="2">
      <element name="text1" action="enable"/>
    </step>
    <step n="2" start="3">
      <element name="text1" action="disable"/>
    </step>
    <step n="3" start="3">
      <element name="text2" action="enable"/>
    </step>
    <step n="4" start="4">
      <element name="text2" action="disable"/>
    </step>
  </steps>
  
</Cutscene>
```
**1) LOAD ALL ELEMENTS INVOLVED IN THE CUTSCENE TO ITS OWN LIST:** each element, depending on its type, will store different type of variables.

**2) LOAD ALL STEPS IN THE CUTSCENE:** each step time variable (*start*) that determines when it starts. Once activated, it will perform the specified action on the linked element. When the action is completed, the step is done and it won't be updated again.
 
### CUTSCENE REPRODUCTION

When an event triggers a cutscene, the state of the game changes to *CUTSCENE MODE* so the player "loses the control of the game" that will be passed to the Cutscene Manager.











