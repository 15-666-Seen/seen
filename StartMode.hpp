#include "Mode.hpp"

#include "InteractableManager.hpp"
#include "Scene.hpp"
#include "StoryManager.hpp"
#include "UI.hpp"

#include <glm/glm.hpp>

struct StartMode : Mode {
    StartMode();
  virtual ~StartMode();

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
  } left, right, down, up, F, esc;

  // local copy of the game scene (so code can change it during gameplay):
  Scene scene;

  // In game UI
  GameplayUI *gameplayUI;

  bool gStop = false;
  bool gamePause = false;

  std::shared_ptr<Sound::PlayingSample> walking_sound;

  std::string current_dialogue = "";
};
