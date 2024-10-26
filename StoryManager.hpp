#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "UI.hpp"
#include "Interactable.hpp"

#include <cassert>
#include <string>
#include <vector>


struct StoryManager {

  StoryManager();
  

  const int total_phase = 4;
  int current_phase = -1;

  GameplayUI * gameplayUI;
  InteractableManager * interactableManager;

  /* Initial set up of the story manager */
  virtual void SetUpManager(InteractableManager *im, GameplayUI *ui);

  /* called per frame to test if we need to go to the next stage */
  virtual void advanceStory();

  /* set up phase[current_phase] */
  virtual void setUpPhase();
};