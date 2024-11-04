#include "UI.hpp"
#include "DrawLines.hpp"


GameplayUI::GameplayUI() {}

void GameplayUI::setInteractionText(const std::string &text) {
  if (!text.empty()) {
    interactionText = "Press [F] to " + text;
  } else {
    interactionText = "";
  }
}

void GameplayUI::setMissionText(std::string s) { missionText = s; }

void GameplayUI::setDialogueTexts(const std::deque<std::string> &v) {
  dialogueText = v;
}

void GameplayUI::insertDialogueText(const std::string& s) {
    if (dialogueText.empty() || dialogueText.front() != s) {
        dialogueText.push_front(s);
    }
}

void GameplayUI::addDialogueText(const std::string &s) {
  if (dialogueText.empty() || dialogueText.back() != s) {
    dialogueText.push_back(s);
  }
}

void GameplayUI::DrawUI(glm::uvec2 const &drawable_size) {
  // use DrawLines to overlay some text:
  float aspect = float(drawable_size.x) / float(drawable_size.y);
  const float H = 0.1f;

  DrawLines lines(glm::mat4(1.0f / aspect, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f));

  lines.draw_text(interactionText.c_str(),
                  glm::vec3(-aspect + 0.1f * H, 1.0 - 2 * H, 0.0),
                  glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                  glm::u8vec4(0x00, 0x00, 0x00, 0x00));

  lines.draw_text(missionText.c_str(),
                  glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
                  glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                  glm::u8vec4(0x00, 0x00, 0x00, 0x00));

  if (dialogueText.size() > 0) {
    lines.draw_box(glm::mat4(0.9f * aspect, 0.0f, 0.0f, 0.0f, 0.0f, 0.3f, 0.0f,
                             0.f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -0.45f, 0.0f,
                             1.0f));
    // make a temperary dialogue box
    lines.draw_text(dialogueText[0].c_str(),
                    glm::vec3(-aspect + 2 * H, -0.3 + 0.1f * H, 0.0),
                    glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                    glm::u8vec4(0x00, 0x00, 0x00, 0x00));
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