enum chase_state {
	Seen,
	Sped_up,
	follow,
	end
} states;

states = Seen;
ghost_loc = glm::vec3(0.0f, 0.0f, 0.0f);
ghost_speed = 1.f;
player_loc = glm::vec3(0.0f, 0.0f, 0.0f);

switch (states) {
case Seen:
	if (ghostLoc == playerLoc) {
		states = Sped_up;
	}
	else {
		states = follow;
	}
	break;
case Sped_up:
	if (ghostLoc == playerLoc) {
		states = Sped_up;
	}
	else {
		states = follow;
	}
	break;
case follow:
	if (ghostLoc == playerLoc) {
		states = Sped_up;
	}
	else {
		states = follow;
	}
	break;
case end:
	break;
}
