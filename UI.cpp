#include "UI.hpp"
#include "DrawLines.hpp"



GameplayUI::GameplayUI() {}

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
  const float H = 0.1f;

  glm::mat4 world_to_clip =
      glm::mat4(1.0f / aspect, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); // world to clip

  DrawLines lines(world_to_clip);

  float x = drawable_size.x * 0.f;
  float y = drawable_size.y * 0.9f;
  float width = drawable_size.x * 1.f;
  interaction.set_bound(drawable_size.x * 0.9f);
  interaction.draw(100.f, drawable_size, width, glm::vec2(x, y), 1.f, glm::vec3(0.0f, 0.2f, 0.0f));

  x = drawable_size.x * 0.f;
  y = drawable_size.y * 0.1f;
  //width = drawable_size.x * 0.75f;
  mission.set_bound(drawable_size.x * 0.9f);
  mission.draw(100.f, drawable_size, width, glm::vec2(x, y), 1.f, glm::vec3(0.0f, 0.2f, 0.0f));
  //std::cout << width << std::endl;

  //lines.draw_text(interactionText.c_str(),
  //                glm::vec3(-aspect + 0.1f * H, 1.0 - 2 * H, 0.0),
  //                glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
  //                glm::u8vec4(0xFF, 0xFF, 0xFF, 0x00));

  //lines.draw_text(missionText.c_str(),
  //                glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
  //                glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
  //                glm::u8vec4(0xFF, 0xFF, 0xFF, 0x00));
    
  if (dialogueText.size() > 0) {
      x = drawable_size.x * 0.2f;
      y = drawable_size.y * 0.3f;
      //width = drawable_size.x * 0.75f;
      
    lines.draw_text(dialogueText[0].c_str(),
                    glm::vec3(-aspect + 5 * H, -0.4 + 0.1f * H, 0.0),
                    glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                    glm::u8vec4(0x00, 0x00, 0x00, 0x00));

    // dialogue box

    /*

    dialogue.set_bound(drawable_size.x * 0.9f);
	dialogue.draw(100.f, drawable_size, width, glm::vec2(x, y), 1.f, glm::vec3(0.0f, 0.2f, 0.0f));*/

    UIShader sprites(world_to_clip);

    sprites.draw_dialogue_box(glm::mat4(0.9f * aspect, 0.0f, 0.0f, 0.0f, 0.0f,
        0.3f, 0.0f, 0.f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, -0.45f, 0.0f, 1.0f));
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