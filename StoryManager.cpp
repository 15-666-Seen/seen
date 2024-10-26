#include "StoryManager.hpp"

void StoryManager::SetUpManager(InteractableManager* im, GameplayUI* ui) {
    interactableManager = im;
    gameplayUI = ui;
    setUpPhase(); // set up phase -1
}

void StoryManager::advanceStory() {

  bool set_up_next_phase = false;

  switch (current_phase) {

  case 0:
    if () { // if player is in bed
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 1:

    break;
  case 2:

    break;
  case 3:

    break;
  default:
    assert(false);
  }

  if (set_up_next_phase)
    setUpPhase();
}

void StoryManager::setUpPhase() {
  switch (current_phase) {
  case 0:

     std::deque<std::string> v;
     v.push_back("I should go to bed.");
     gameplayUI->setDialogueTexts(v);

     gameplayUI->setMissionText("Go to bed.");

    break;
  case 1:

        std::deque<std::string> v;
        v.push_back("It's already 12.");
        v.push_back("What is that sound??");
        v.push_back("Someone's here. I need to hide in the closet.");
        gameplayUI->setDialogueTexts(v);

        gameplayUI->setMissionText("Hide in the closet.");

    break;
  case 2:
        std::deque<std::string> v;
        v.push_back("I need to leave this house!!");
        gameplayUI->setDialogueTexts(v);

        gameplayUI->setMissionText("Leave this room.");

    break;
  case 3:
        gameplayUI->setMissionText("Leave this house.");

    break;
  default:

    assert(false);
  }


}
