#include "StoryManager.hpp"

StoryManager::StoryManager() {

	setUpPhase(); // set up phase -1
}

void StoryManager::advanceStory() {

	bool set_up_next_phase = false;
	
	switch (current_phase) {

	case -1:
		/* For example, if player is in bed */
		if (false) {
			current_phase++; // advance to the next phase
			set_up_next_phase = true;
		}
		break;
	case 0:

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

	if (set_up_next_phase) setUpPhase();

}

void StoryManager::setUpPhase() {
	switch (current_phase) {
	case -1:
		/*  
			Put player downstairs
			Display the first tutorial dialogue prompting user to go to bed
		*/
		break;
	case 0:
		/*
			Play clock sound 12 times
			Display dialogue/animation of player almost sleeping but hearing someone come in
		*/
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
}
