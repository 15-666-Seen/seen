#include "sound_prep.hpp"

#include "data_path.hpp"

Load<Sound::Sample>
default_bgm_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/default-bgm.wav"));
});
Load<Sound::Sample>
chase_bgm_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/chase.wav"));
});

Load<Sound::Sample>
door_open_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/door-open.wav"));
});

Load<Sound::Sample>
door_close_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/door-close.wav"));
});

Load<Sound::Sample>
footstep_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/footstep.wav"));
});

Load<Sound::Sample>
eyeball_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/eyeball.wav"));
});

Load<Sound::Sample>
breath_trigger_sample(LoadTagDefault, []() -> Sound::Sample const * {
  return new Sound::Sample(data_path("sounds/breath-trigger.wav"));
});

Load<Sound::Sample>
chain_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("sounds/chain.wav"));
	});

Load<Sound::Sample>
cut_chain_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("sounds/chain.wav"));
	});
