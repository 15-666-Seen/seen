#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "UI.hpp"
#include "InteractableManager.hpp"

#include <cassert>

struct StoryManager {

  StoryManager();

  const int total_phase = 4;
  int current_phase = 0;

  GameplayUI *gameplayUI;
  InteractableManager *interactableManager;

  /* Initial set up of the story manager */
  void SetUpManager(GameplayUI *ui, InteractableManager* im);

  /* called per frame to test if we need to go to the next stage */
  void advanceStory();

  /* set up phase[current_phase] */
  void setUpPhase();

  int getCurrentPhase() { return current_phase; }
};