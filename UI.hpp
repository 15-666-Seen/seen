#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <deque>
#include <string>

/* On screen UI during game play */
struct GameplayUI {
  GameplayUI();

  // upper left corner: interaction (press [F] to interact)
  std::string interactionText = "Interaction Text Holder";
  void setInteractionText(const std::string &text);

  // lower left corner: tutorial or mission (find the key without being killed)
  std::string missionText = "";
  void setMissionText(std::string s);

  // lower half of screen: dialogue ("The owner of this house is not human!!")
  std::deque<std::string> dialogueText;
  // TODO: add background image for the dialogue
  void setDialogueTexts(const std::deque<std::string> &v);
  void addDialogueText(const std::string &s);

  /* Draw UI for each frame */
  void DrawUI(glm::uvec2 const &drawable_size);

  /* Each time the mouse is clicked, figure out if its interacting with some UI
   * controlled by this struct */
  void InteractOnClick(int const x, int const y);
};

/* We'll get to this after the prototype

struct MainMenu
{
};

*/

struct Button {};