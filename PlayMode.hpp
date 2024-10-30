#include "Mode.hpp"

#include "InteractableManager.hpp"
#include "Scene.hpp"
#include "StoryManager.hpp"
#include "UI.hpp"
#include "WalkMesh.hpp"

#include <glm/glm.hpp>

struct PlayMode : Mode {
  PlayMode();
  virtual ~PlayMode();

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
  } left, right, down, up, F;

  // local copy of the game scene (so code can change it during gameplay):
  Scene scene;

  // player info:
  struct Player {
    WalkPoint at;
    // transform is at player's feet and will be yawed by mouse left/right
    // motion:
    Scene::Transform *transform = nullptr;
    // camera is at player's head and will be pitched by mouse up/down motion:
    Scene::Camera *camera = nullptr;
    float theta = 0.0f;
  } player;
  void cameraShake(float elapsed);

  // In game UI
  GameplayUI *gameplayUI;

  // items and furniture management
  InteractableManager interactableManager;

  // story manager
  StoryManager *storyManager;
  void checkPhaseUpdates();
};
