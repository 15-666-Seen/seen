#include "UI.hpp"
#include "DrawLines.hpp"

GameplayUI::GameplayUI() { mission.init("BungeeHairline-Regular.ttf");
     mission.set_color(glm::vec3(1.0f, 1.0f, 1.0f));
}

void GameplayUI::setInteractionText(const std::string &text) {
  if (!text.empty()) {
      interactionText = "Press [F] to " + text;
  } else {
    interactionText = "";
  }
  interaction.set_text(interactionText);
}

void GameplayUI::setMissionText(std::string s) { 
    missionText = s; 
	mission.set_text(missionText);
}

void GameplayUI::setDialogueTexts(const std::deque<std::string> &v) {
  dialogueText = v;
  dialogue.set_text(dialogueText[0]);
}

void GameplayUI::insertDialogueText(const std::string &s) {
  if (dialogueText.empty() || dialogueText.front() != s) {
    dialogueText.push_front(s);
    dialogue.set_text(dialogueText[0]);
  }
}

void GameplayUI::addDialogueText(const std::string &s) {
  if (dialogueText.empty() || dialogueText.back() != s) {
    dialogueText.push_back(s);
    dialogue.set_text(dialogueText[0]);
  }
}

void GameplayUI::DrawUI(glm::uvec2 const &drawable_size) {
  // use DrawLines to overlay some text:
  float aspect = float(drawable_size.x) / float(drawable_size.y);

  glm::mat4 world_to_clip =
      glm::mat4(1.0f / aspect, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); // world to clip

  float x = drawable_size.x * 0.f;
  float y = drawable_size.y * 0.95f;
  float width = drawable_size.x * 1.f;
  interaction.set_bound(drawable_size.x * 0.9f);
  interaction.draw(100.f, drawable_size, width, glm::vec2(x, y), 1.f);

  x = drawable_size.x * 0.f;
  y = drawable_size.y * 0.01f;
  //width = drawable_size.x * 0.75f;
  mission.set_bound(drawable_size.x * 0.9f);
  mission.draw(100.f, drawable_size, width, glm::vec2(x, y), 1.f);
    
  if (dialogueText.size() > 0) {
      x = drawable_size.x * 0.1f;
      y = drawable_size.y * 0.3f;
      //width = drawable_size.x * 0.75f;

    // dialogue box

    UIShader sprites(world_to_clip);

    sprites.draw_dialogue_box(glm::mat4(0.9f * aspect, 0.0f, 0.0f, 0.0f,
                                        0.0f, 0.3f, 0.0f, 0.f, 
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, -0.45f, 0.0f, 1.0f));

    sprites.~UIShader(); // explicitly draws any sprite

    dialogue.set_bound(drawable_size.x * 0.9f);
	dialogue.draw(100.f, drawable_size, width, glm::vec2(x, y), 1.3f);
  }
}

void GameplayUI::InteractOnClick(int const x, int const y) {
  // advance dialogue

  // if no more dialogue, hide the dialogue box and clicking can no longer
  // interact with this
  if (dialogueText.size() > 0) {
    dialogueText.pop_front();
  }
}