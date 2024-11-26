#pragma once
#include "Mode.hpp"

#include "InteractableManager.hpp"
#include "Scene.hpp"
#include "Text.hpp"
#include "UI.hpp"

#include <glm/glm.hpp>

struct EndMode : Mode {
	EndMode();
  virtual ~EndMode();

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
  //Scene::Drawable* background = nullptr;
  Scene::Camera* camera = nullptr;

  Text text;
  uint8_t current_section = 0;
  Section texts[4] = { 
      {"So, what do you think? Ravenshade Manor,charming, isn't it? It's not every day you find a place with so much character.", "pure_black.png"},
	  {"You really should come back sometime. The fireplace will be crackling, the rooms full of stories, and there's always tea on the stove. It's everything a home should be,warm, cozy, alive.", "pure_black.png"},
	  {"Come on, I'll walk you back inside. Trust me, it's an experience you won't forget¡Xand neither will I.", "pure_black.png"},
	  {"The END", "pure_black.png"}
  };

  // In game UI
  GameplayUI *gameplayUI;

  bool gStop = false;
  bool gamePause = false;

  std::shared_ptr<Sound::PlayingSample> walking_sound;

  float text_elapsed = 0.0f;  // time for text animation

  //std::string current_dialogue = "";

  //bool finished = false;
};
