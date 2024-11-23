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
  interactableManager->setFurnitureVisibility(CHAIN_CUT2, false);
  interactableManager->setFurnitureVisibility(CHAIN_CUT1, false);
  interactableManager->setFurnitureVisibility(CHAIN, false);
  interactableManager->setFurnitureVisibility(DOORBLOCK, false);

  setUpPhase(); // set up phase 0
}

// true for advancing to the next phase
bool StoryManager::advanceStory() {
  bool set_up_next_phase = false;

  switch (current_phase) {
  case 0:
    // if player open the bedroom door -> need to be in OPEN
    if (interactableManager->interactStatusCheck(BEDROOM_DOOR) == 2) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 1:
    // if player read the file
    if (interactableManager->interactStatusCheck(FILE1) == 1) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 2:
    // if player find the key and open the bedroom door
    if (interactableManager->interactStatusCheck(BEDROOM_DOOR) == 1) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 3:
    if (interactableManager->interactStatusCheck(DOOR1) == 1) {
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

    interactableManager->setFurniturePhaseAvailability(BEDROOM_DOOR, true);
    interactableManager->setFurniturePhaseAvailability(DOOR1, true);
    interactableManager->setItemPhaseAvailability(FILE1, false);


    break;
  case 1:
    // bed no longer interactable, key interactable
    interactableManager->setFurniturePhaseAvailability(BED, false);
    interactableManager->setItemPhaseAvailability(BEDROOM_KEY, false);
    interactableManager->setItemPhaseAvailability(FILE1, true);

    gameplayUI->addDialogueText("This must be their bedroom.");
    gameplayUI->addDialogueText("That thing must be here somewhere...");

    gameplayUI->setMissionText("Find Secret File");


    break;

  case 2:
    interactableManager->setItemPhaseAvailability(FILE1, false);
    interactableManager->setItemPhaseAvailability(BEDROOM_KEY, true);
    interactableManager->setFurniturePhaseAvailability(BEDROOM_DOOR, true);

    gameplayUI->addDialogueText(
        "[XXXXXXXXXXXXXXX   Something Secret  XXXXXXXXXXXXXXX]");
    gameplayUI->addDialogueText("...");
    gameplayUI->addDialogueText(
        "Wait, the door is closed? I need to find a way out.");

    gameplayUI->setMissionText("Leave Bedroom");

    break;

  case 3:
    interactableManager->setFurniturePhaseAvailability(BED, true);
    enableGhost("ghost1");

    break;

  case 4:
    break;

  case 10:
      // player is currently hiding, need to push sofa away
      // sofa can now be pushed away
      interactableManager->setFurniturePhaseAvailability(SOFA, true);
      //we set ladder to usable once we interact with the sofa
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
