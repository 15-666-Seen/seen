#include "StoryManager.hpp"

#include "Ghost.hpp"
#include "Interactable.hpp"
#include "util.hpp"

#include <iostream>

StoryManager::StoryManager() {}

void StoryManager::SetUpManager(GameplayUI *ui, InteractableManager *im) {
  gameplayUI = ui;
  interactableManager = im;

  // hide some things
  interactableManager->setFurniturePhaseVisibility(CHAIN_CUT2, false);
  interactableManager->setFurniturePhaseVisibility(CHAIN_CUT1, false);
  interactableManager->setFurniturePhaseVisibility(CHAIN, false);
  interactableManager->setFurniturePhaseVisibility(DOORBLOCK, false);

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
  case 10:
    // player uses the ladder
    if (interactableManager->interactStatusCheck(LADDER) == 1) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 11:
    // player is in the basement searching for clip
    // if player pushes sofa away
    if (interactableManager->interactStatusCheck(LADDER) == 2) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 12:
    // if the player finds the clip
    // the player climbs up
    if (false) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 13:
    // ghost jumpscare
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

  // update phrase in interactable manager
  interactableManager->current_phase = current_phase;
  std::deque<std::string> v;
  std::cout << "Setting up for phase " << current_phase << std::endl;

  switch (current_phase) {
  case 0:

    v.push_back("I should check bedroom first.");
    gameplayUI->setDialogueTexts(v);
    gameplayUI->setMissionText("Find Bedroom");

    // door block is invisible for phase 0
    interactableManager->setItemPhaseVisibility(REDROOM_KEY, false);
    interactableManager->setFurniturePhaseVisibility(DOORBLOCK, false);
    interactableManager->setFurniturePhaseAvailability(BEDROOM_DOOR, true);
    interactableManager->setFurniturePhaseAvailability(DOOR1, true);

    break;

  case 1:

    // user get in blue room, tiny sculpture is available
    interactableManager->setFurniturePhaseVisibility(SCULPTURE_EYE_R, false);
    interactableManager->setFurniturePhaseAvailability(TINY_SCULPTURE, true);
    interactableManager->setItemPhaseAvailability(EYEBALL, true);

    break;

  case 2:

    // blue door is closed, shelf is avaible
    interactableManager->closeDoor(DOOR1);
    interactableManager->setItemPhaseAvailability(REDROOM_KEY, true);
    interactableManager->setFurniturePhaseAvailability(TINY_SCULPTURE, false);
    interactableManager->setFurniturePhaseAvailability(BOOKSHELF, true);
    interactableManager->setFurniturePhaseVisibility(DOORBLOCK, true);

    break;

  case 3:
    interactableManager->setItemPhaseAvailability(CLIP_L, true);
    interactableManager->setItemPhaseAvailability(BEDROOM_KEY, true);
    interactableManager->setFurniturePhaseAvailability(BED, true);

    break;

  case 4:
    enableGhost("ghost1");
    break;

  case 10:
    // player is currently hiding, need to push sofa away
    // sofa can now be pushed away
    interactableManager->setFurniturePhaseAvailability(SOFA, true);
    // we set ladder to usable once we interact with the sofa
    break;
  case 11:
    // move player to the basement

    interactableManager->setFurniturePhaseAvailability(FRIDGE1, true);
    interactableManager->setFurniturePhaseAvailability(FRIDGE2, true);
    interactableManager->setFurniturePhaseAvailability(FRIDGE3, true);
    interactableManager->setFurniturePhaseAvailability(FRIDGE4, true);
    interactableManager->setFurniturePhaseAvailability(FRIDGE5, true);

    break;
  case 12:
    // the player climbs up

    break;
  case 13:
    // ghost jumpscare

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
