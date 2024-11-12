#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Ghost.hpp"
#include "InteractableManager.hpp"
#include "UI.hpp"

#include <cassert>
#include <map>

struct StoryManager {

  StoryManager();

  const int total_phase = 4;
  int current_phase = 0;

  GameplayUI *gameplayUI;
  InteractableManager *interactableManager;

  std::map<std::string, Ghost *> GhostMap;

  /* Initial set up of the story manager */
  void SetUpManager(GameplayUI *ui, InteractableManager *im);

  /* called per frame to test if we need to go to the next stage */
  bool advanceStory();

  /* set up phase[current_phase] */
  void setUpPhase();

  int getCurrentPhase() { return current_phase; }

  void enableGhost(const std::string &name);
};