#include "sound_prep.hpp"

#include "data_path.hpp"

Load<Sound::Sample>
door_open_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/door-open.wav"));
});

Load<Sound::Sample>
footstep_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/footstep.wav"));
});
