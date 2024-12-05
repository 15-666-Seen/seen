#include "StoryManager.hpp"

#include "Ghost.hpp"
#include "Interactable.hpp"
#include "sound_prep.hpp"
#include "util.hpp"
#include "Move.hpp"

#include <iostream>

StoryManager::StoryManager() {}

Move move;

void StoryManager::SetUpManager(GameplayUI *ui, InteractableManager *im) {
  gameplayUI = ui;
  interactableManager = im;

  // hide some things
  interactableManager->setFurniturePhaseVisibility(CHAIN_CUT2, false);
  interactableManager->setFurniturePhaseVisibility(CHAIN_CUT1, false);

  interactableManager->setFurniturePhaseVisibility(TINY_SCULPTURE_TENTACLES, false);
  interactableManager->setFurniturePhaseVisibility(DOORBLOCK, false);

  interactableManager->setFurniturePhaseVisibility(SCULPTURE_EYE_R, false);

  setUpPhase(); // set up phase 0
}

// true for advancing to the next phase
bool StoryManager::advanceStory(float elapsed) {
  bool set_up_next_phase = false;

  bgmCheck();

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
    if (interactableManager->interactStatusCheck(TINY_SCULPTURE_TENTACLES) == 2) {
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
    if (interactableManager->isHiding) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
    break;
  case 5:
    // TODO: break here
    if (false) {
      current_phase++; // advance to the next phase
      set_up_next_phase = true;
    }
	break;
  case 6:
	  // player is in the attic
	  if (true) {
		  current_phase++; // advance to the next phase
		  set_up_next_phase = true;
	  }
	  break;
  case 7:
	  // open the attic door
	  if (interactableManager->interactStatusCheck(ROPE) == 1) {
		  //current_phase++; // advance to the next phase
		  //set_up_next_phase = true;
		  move.update(elapsed);
		  static bool next = false;
          uint8_t step = next;
          if (step == 0) {
              glm::vec3& old_pos = interactableManager->furnituresMap[ROPE]->drawable->transform->position;
		      glm::vec3 new_pos = glm::vec3(old_pos.x, old_pos.y + 0.5f, old_pos.z - 0.5f);
              move.AddTransition(old_pos, new_pos, 0.5f, &next);
		      glm::quat& old_rotate = interactableManager->furnituresMap[ATTIC_DOOR]->drawable->transform->rotation;
		      glm::quat new_rotate = old_rotate *= glm::angleAxis(glm::radians(-90.f), glm::vec3(0.0f, 1.f, 0.f));
		      move.AddTransition(old_pos, new_pos, 0.5f, &next);
              step = step + next;
          }      
          else if (step == 1) {
              glm::vec3& old_pos = interactableManager->furnituresMap[ATTIC_LADDER]->drawable->transform->position;
              glm::vec3 new_pos = glm::vec3(old_pos.x, old_pos.y + 0.5f, old_pos.z);
              move.AddTransition(old_pos, new_pos, 0.5f, &next);
              
              step = step + next;
          }
          else if (step == 2) {
			  glm::vec3& old_pos = interactableManager->furnituresMap[ATTIC_LADDER1]->drawable->transform->position;
			  glm::vec3 new_pos = glm::vec3(old_pos.x, old_pos.y + 0.5f, old_pos.z);
			  move.AddTransition(old_pos, new_pos, 0.5f, &next);
			  step = step + next;
		  }
          else if (step == 3) {
              glm::vec3& old_pos = interactableManager->furnituresMap[ATTIC_LADDER2]->drawable->transform->position;
              glm::vec3 new_pos = glm::vec3(old_pos.x, old_pos.y + 0.5f, old_pos.z);
              move.AddTransition(old_pos, new_pos, 0.5f, &next);
              step = step + next;
		  }
          else if (step == 4) {
              current_phase++; // advance to the next phase
              set_up_next_phase = true;
          }
	  }
	  break;
  case 8:
      // ghost chase

      break;

  case 9:
      // ghost chase completed

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

    if (!bgm_sound) {
      bgm_sound = Sound::loop_3D(*default_bgm_sample, 0.8f,
                                 glm::vec3(0.0f, 0.0f, 10.0f), 1.0f);
    }
    interactableManager->setItemPhaseAvailability(DEN_KEY, true);
    interactableManager->setFurniturePhaseVisibility(DOORBLOCK, false);
    interactableManager->setFurniturePhaseAvailability(BEDROOM_DOOR, true);
    interactableManager->setFurniturePhaseAvailability(CLOSET2, true);
    interactableManager->setFurniturePhaseAvailability(DOOR1, true);
    interactableManager->setItemPhaseVisibility(REDROOM_KEY, false);

    interactableManager->setFurniturePhaseAvailability(CHAIN, true);

    break;

  case 1:
    gameplayUI->insertDialogueText("Well, that's reassuring. Definitely no death traps ahead, right?");
    gameplayUI->insertDialogueText("Is that..a corpse?");
    gameplayUI->insertDialogueText("...");

    interactableManager->setFurniturePhaseVisibility(CHAIN_CUT2, true);
    interactableManager->setFurniturePhaseVisibility(CHAIN_CUT1, true);
    interactableManager->setFurniturePhaseVisibility(CHAIN, true);

    // user get in blue room, tiny sculpture is available
    interactableManager->setFurniturePhaseAvailability(TINY_SCULPTURE, true);
    interactableManager->setItemPhaseAvailability(EYEBALL, true);

    break;

  case 2:

    // blue door is closed, shelf is avaible
    gameplayUI->insertDialogueText("!");
    interactableManager->closeDoor(DOOR1);
    interactableManager->setItemPhaseVisibility(REDROOM_KEY, true);
    interactableManager->setItemPhaseAvailability(REDROOM_KEY, true);

    //interactableManager->setFurniturePhaseAvailability(BOOKSHELF, true);

    interactableManager->setFurniturePhaseVisibility(DOORBLOCK, true);
    interactableManager->setFurniturePhaseAvailability(DOORBLOCK, true);

    break;

  case 3:
    interactableManager->setFurniturePhaseAvailability(BOOKSHELF, false);
    interactableManager->setFurniturePhaseAvailability(BED, true);
    interactableManager->setItemPhaseAvailability(BEDROOM_KEY, true);
    interactableManager->setItemPhaseAvailability(FILE1, true);
    interactableManager->setItemPhaseAvailability(CLIP_L, true);

    break;

  case 4:
    enableGhost("ghost1", true);
    break;

  case 5:
    enableGhost("ghost1", false);
    break;
  case 6:

  case 7:
	  interactableManager->setFurniturePhaseAvailability(ROPE, true);
      interactableManager->furnituresMap[ATTIC_LADDER]->drawable->transform->parent = interactableManager->furnituresMap[ATTIC_DOOR]->drawable->transform;
      interactableManager->furnituresMap[ATTIC_LADDER1]->drawable->transform->parent = interactableManager->furnituresMap[ATTIC_LADDER]->drawable->transform;
      interactableManager->furnituresMap[ATTIC_LADDER2]->drawable->transform->parent = interactableManager->furnituresMap[ATTIC_LADDER1]->drawable->transform;
  case 8:


  case 9:


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

void StoryManager::enableGhost(const std::string &name, bool enable) {
  Ghost *ghost = GhostMap[name];
  ghost->active = enable;
  ghost->drawable->visible = enable;
}

void StoryManager::bgmCheck() {
  // if any of ghost is active, play ghost bgm
  bool one_active = false;
  for (auto &ghost : GhostMap) {
    if (ghost.second->active) {
      one_active = true;
      if (bgm_status == 0) {
        bgm_sound->stop(1.0f / 60.0f);
        bgm_sound.reset();
        bgm_sound =
            Sound::loop_3D(*chase_bgm_sample, 8.0f,
                           ghost.second->drawable->transform->position, 6.0f);
        bgm_status = 1;
      }
      return;
    }
  }

  // if no ghost is active, play default bgm
  if (!one_active && bgm_status == 1) {
    bgm_sound->stop(1.0f / 60.0f);
    bgm_sound.reset();
    bgm_sound = Sound::loop_3D(*default_bgm_sample, 0.8f,
                               glm::vec3(0.0f, 0.0f, 10.0f), 1.0f);
    bgm_status = 0;
  }
}
