#include "StoryManager.hpp"

#include "util.hpp"

#include <iostream>

StoryManager::StoryManager() {}

void StoryManager::SetUpManager(GameplayUI *ui) {
  gameplayUI = ui;
  setUpPhase(); // set up phase -1
}

void StoryManager::advanceStory() {
  current_phase++;
  current_phase = std::min(current_phase, total_phase);
  setUpPhase();
}

void StoryManager::setUpPhase() {
  std::deque<std::string> v;
  std::cout << "Setting up for phase " << current_phase << std::endl;

  switch (current_phase) {
  case 0:

    v.push_back("I should go to bed.");
    gameplayUI->setDialogueTexts(v);

    gameplayUI->setMissionText("Go to bed.");

    break;
  case 1:

    gameplayUI->addDialogueText("It's already 12.");
    gameplayUI->addDialogueText("What is that sound??");
    gameplayUI->addDialogueText(
        "Someone's here. I need to hide in the closet.");

    gameplayUI->setMissionText("Hide in the closet.");

    break;
  case 2:
    v.push_back("I need to leave this house!!");
    gameplayUI->setDialogueTexts(v);

    gameplayUI->setMissionText("Leave this room.");

    break;
  default:

    wait_and_exit("Game Over");
  }
}
