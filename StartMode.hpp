#pragma once
#include "Mode.hpp"

#include "InteractableManager.hpp"
#include "Scene.hpp"
#include "Text.hpp"
#include "UI.hpp"

#include <glm/glm.hpp>



struct StartMode : Mode {
    StartMode();
  virtual ~StartMode();

  struct Section {
	  std::string text;
	  std::string text_file;
  };

  // functions called by main loop:
  virtual bool handle_event(SDL_Event const &,
                            glm::uvec2 const &window_size) override;
  virtual void update(float elapsed) override;
  virtual void draw(glm::uvec2 const &drawable_size) override;

  //----- game state -----

  // input tracking:
  struct Button {
    uint8_t downs = 0;
    uint8_t pressed = 0;
  } space, click;

  // local copy of the game scene (so code can change it during gameplay):
  Scene scene;
  Scene::Drawable* background = nullptr;

  Scene::Camera* camera = nullptr;

  Text text;
  uint8_t current_section = 0;
  Section texts[16] = { 
      {"Six days ago, I found a letter on my windshield, tucked under the wiper like a parking ticket. ", "IMG_3075.png"},
	  {"It was from my parents. They... they've been gone for years.", "IMG_3076.png"},
	  {"The letter talked about inheriting a mansion, a huge place, in some town called Ravenshade. The postmark was from three years ago, strangely close to the third anniversary of their death.", "IMG_3077.png"},
	  {"The letter was brief, just an address at the end of Holyhill Road in Ravenshade. But every map I've looked at seems to blur that spot, like my eyes refuse to see it", "IMG_3079.png"},
	  {"I even checked with the librarian, and she confirmed... there's a house there. A big one.", "IMG_3078.png"},
	  {"I've thought about this a lot. But with the debt piling up and no way out... I didn't have much choice. So, here I am, driving across the country.", "IMG_3080.png"},
	  {"When I got here, I wanted to ask for directions...", "IMG_3081.png"},
	  {"But everyone I saw, old, young, it didn't matter, they just... smiled and pointed, like some silent guide. No words, just that knowing look and a finger pointing down the road.", "IMG_3082.png"},
	  {"I even bought this cross for a bit of protection. Call me superstitious, but something feels... off.", "pure_black.png"},
	  {"I'm recording this now. I have no idea what's waiting for me. But if you're hearing this, and I didn't make it back, don't come looking for me. Stay away from Holyhill Road.", "pure_black.png"},
	  {"This place... it's unreal. Like it's been frozen in time.", "IMG_3083.png"},
	  {"I need to record this. Tapes aren't cheap, but... something tells me I'll need this. It's like I'm being watched already, like the house itself is... waiting.", "IMG_3083.png"},
	  {"Load tape", "pure_black.png"},
      {"Load tape.", "pure_black.png"},
	  {"Load tape..", "pure_black.png"},
	  {"Load tape...", "pure_black.png"}
  };

  // In game UI
  GameplayUI *gameplayUI;

  bool gStop = false;
  bool gamePause = false;

  std::shared_ptr<Sound::PlayingSample> walking_sound;

  std::string current_dialogue = "";

  //bool finished = false;
};
