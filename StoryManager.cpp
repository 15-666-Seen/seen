#include "StoryManager.hpp"

StoryManager::StoryManager() {
}

void StoryManager::SetUpManager(InteractableManager* im, GameplayUI* ui) {
    interactableManager = im;
    gameplayUI = ui;
    setUpPhase(); // set up phase -1
}

void StoryManager::advanceStory() {

  bool set_up_next_phase = false;

  switch (current_phase) {
  case 0:
    if (interactableManager->cur_furniture == BED) { // if player is in bed
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 1:
      if (interactableManager->cur_furniture == CLOSET) { // if player is in closet
          current_phase++; // advance to the next phase
          set_up_next_phase = true;
      }
    break;
  case 2:
      if (interactableManager->cur_furniture == BEDROOM_DOOR) { // if player unlocks the door
          current_phase++; // advance to the next phase
          set_up_next_phase = true;
      }
    break;
  case 3:
      if (false) { // this is where the prototype ends
          current_phase++; // advance to the next phase
          set_up_next_phase = true;
      }
    break;
  default:
    assert(false);
  }

  if (set_up_next_phase)
    setUpPhase();
}

void StoryManager::setUpPhase() {
  std::deque<std::string> v;
  printf("Setting up for phase %d", current_phase);

  switch (current_phase) {
  case 0:

     
     v.push_back("I should go to bed.");
     gameplayUI->setDialogueTexts(v);

     gameplayUI->setMissionText("Go to bed.");

    break;
  case 1:

        v.push_back("It's already 12.");
        v.push_back("What is that sound??");
        v.push_back("Someone's here. I need to hide in the closet.");
        gameplayUI->setDialogueTexts(v);

        gameplayUI->setMissionText("Hide in the closet.");

    break;
  case 2:
        v.push_back("I need to leave this house!!");
        gameplayUI->setDialogueTexts(v);

        gameplayUI->setMissionText("Leave this room.");

    break;
  default:

    assert(false);
  }


}
