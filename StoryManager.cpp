#include "StoryManager.hpp"

#include "util.hpp"

#include <iostream>

StoryManager::StoryManager() {}

void StoryManager::SetUpManager(GameplayUI *ui, InteractableManager* im) {
  gameplayUI = ui;
  interactableManager = im;

  setUpPhase(); // set up phase 0
}

void StoryManager::advanceStory() {
    bool set_up_next_phase = false;

    switch (current_phase) {
    case 0:
        if (interactableManager->interactStatusCheck(BED)) { // if player is in bed
            current_phase++; // advance to the next phase
            set_up_next_phase = true;
        }
        break;
    case 1:
        if (interactableManager->interactStatusCheck(BEDROOM_DOOR)) { // if player unlocks the door
            current_phase++; // advance to the next phase
            set_up_next_phase = true;
        }
        break;
    case 2:
        if (false) { // this is where the prototype ends
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

    v.push_back("I should go to bed.");
    gameplayUI->setDialogueTexts(v);
    gameplayUI->setMissionText("Go to bed.");

    interactableManager->setFurniturePhaseAvailability(BED, true);

    break;
  case 1:
      // bed no longer interactable, key interactable
      interactableManager->setFurniturePhaseAvailability(BED, false);
      interactableManager->setItemPhaseAvailability(BEDROOM_KEY, true);

    v.push_back("I should leave this room.");
    gameplayUI->setDialogueTexts(v);

    gameplayUI->setMissionText("Leave this room.");

    break;

  case 2:
      v.push_back("Prototype end!");
      gameplayUI->setDialogueTexts(v);

      break;
  default:

    wait_and_exit("Game Over - set up phase not programmed");
  }
}
