#include "StoryManager.hpp"

#include "Ghost.hpp"
#include "Interactable.hpp"
#include "util.hpp"

#include <iostream>

StoryManager::StoryManager() {}

void StoryManager::SetUpManager(GameplayUI *ui, InteractableManager *im) {
  gameplayUI = ui;
  interactableManager = im;

  setUpPhase(); // set up phase 0
}

// true for advancing to the next phase
bool StoryManager::advanceStory() {
  bool set_up_next_phase = false;

  switch (current_phase) {
  case 0:
    // forward if player open the blue door -> need to be in OPEN
    if (interactableManager->interactStatusCheck(DOOR1) == 2) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 1:
    // forward if tiny sculpture is interacted, red key is shown
    if (interactableManager->interactStatusCheck(TINY_SCULPTURE) == 1) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 2:
    // forward if bookshelf is moved
    if (interactableManager->interactStatusCheck(BOOKSHELF) == 1) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 3:
    if (interactableManager->interactStatusCheck(BEDROOM_DOOR) == 1) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 4:
    // if player move the shelf
    if (false) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  default:
    wait_and_exit("Game Over - advanced past programmed phase");
  }

  if (set_up_next_phase) {
    setUpPhase();
  }

  return set_up_next_phase;
}

void StoryManager::setUpPhase() {

  // update phrase in interactable manaer
  interactableManager->current_phase = current_phase;
  std::deque<std::string> v;
  std::cout << "Setting up for phase " << current_phase << std::endl;

  switch (current_phase) {
  case 0:

    v.push_back("I should check bedroom first.");
    gameplayUI->setDialogueTexts(v);
    gameplayUI->setMissionText("Find Bedroom");

    // door block is invisible for phase 0
    interactableManager->setFurniturePhaseVisability(DOORBLOCK, false);
    interactableManager->setFurniturePhaseAvailability(BEDROOM_DOOR, true);
    interactableManager->setFurniturePhaseAvailability(DOOR1, true);

    break;

  case 1:

    // user get in blue room, tiny sculpture is available
    interactableManager->setFurniturePhaseVisability(SCULPTURE_EYE_R, false);
    interactableManager->setFurniturePhaseAvailability(TINY_SCULPTURE, true);
    interactableManager->setItemPhaseAvailability(EYEBALL, true);

    break;

  case 2:

    // blue door is closed, shelf is avaible
    interactableManager->closeDoor(DOOR1);
    interactableManager->setItemPhaseAvailability(REDROOM_KEY, true);
    interactableManager->setFurniturePhaseAvailability(TINY_SCULPTURE, false);
    interactableManager->setFurniturePhaseAvailability(BOOKSHELF, true);
    interactableManager->setFurniturePhaseVisability(DOORBLOCK, true);

    break;

  case 3:
    interactableManager->setFurniturePhaseAvailability(BED, true);
    enableGhost("ghost1");

    break;

  case 4:
    break;

  default:

    wait_and_exit("Game Over - set up phase not programmed");
  }
}

void StoryManager::enableGhost(const std::string &name) {
  Ghost *ghost = GhostMap[name];
  ghost->active = true;
  ghost->drawable->visible = true;
}
