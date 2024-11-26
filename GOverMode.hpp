#pragma once
#include "Mode.hpp"

#include "InteractableManager.hpp"
#include "Scene.hpp"
#include "Text.hpp"
#include "UI.hpp"

#include <glm/glm.hpp>



struct GOverMode : Mode {
	GOverMode();
  virtual ~GOverMode();

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
  Section texts[4] = { 
      {"Ah, there you are. I told you Ravenshade always leaves its mark. Don¡¦t worry-you'll belong here soon enough.", "pure_black.png"},
  };

  // In game UI
  //GameplayUI *gameplayUI;

  bool gStop = false;
  bool gamePause = false;

  std::shared_ptr<Sound::PlayingSample> walking_sound;

  float text_elapsed = 0.0f;  // time for text animation

  //std::string current_dialogue = "";

  //bool finished = false;
};
