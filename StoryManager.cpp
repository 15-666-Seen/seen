#include "StoryManager.hpp"

#include "Interactable.hpp"
#include "util.hpp"

#include <iostream>

StoryManager::StoryManager() {}

void StoryManager::SetUpManager(GameplayUI *ui, InteractableManager *im) {
  gameplayUI = ui;
  interactableManager = im;

  setUpPhase(); // set up phase 0
}

void StoryManager::advanceStory() {
  bool set_up_next_phase = false;

  switch (current_phase) {
  case 0:
    // if player open the bedroom door
    if (interactableManager->interactStatusCheck(BEDROOM_DOOR) == 1) {
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
    if (false) {       // this is where the prototype ends
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  default:
    wait_and_exit("Game Over - advanced past programmed phase");
  }

  if (set_up_next_phase)
    setUpPhase();
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

    interactableManager->setFurniturePhaseAvailability(BEDROOM_DOOR, true);
    interactableManager->setFurniturePhaseAvailability(DOOR1, true);

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
    interactableManager->setFurniturePhaseAvailability(BED, false);
    interactableManager->setItemPhaseAvailability(BEDROOM_KEY, true);
    interactableManager->setFurniturePhaseAvailability(BEDROOM_DOOR, true);

    gameplayUI->addDialogueText(
        "[XXXXXXXXXXXXXXX   Something Secret  XXXXXXXXXXXXXXX]");
    gameplayUI->addDialogueText("...");
    interactableManager->closeBedroomDoor();
    gameplayUI->addDialogueText(
        "Wait, the door is closed? I need to find a way out.");

    gameplayUI->setMissionText("Leave Bedroom");

    break;
  default:

    wait_and_exit("Game Over - set up phase not programmed");
  }
}
