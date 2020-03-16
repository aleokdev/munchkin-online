#ifndef MUNCHKIN_ONLINE_SOUND_PLAYER_HPP
#define MUNCHKIN_ONLINE_SOUND_PLAYER_HPP
#include "assets/assets.hpp"
#include "sound_assets.hpp"

namespace munchkin {
namespace sound {

void play_music(assets::Handle<Music>);
void stop_music();
audeo::Sound get_current_music_being_played();
assets::Handle<Music> get_current_music_handle_being_played();

void play_sfx(assets::Handle<SoundEffect>);

constexpr float music_crossfade_seconds = .5f;

} // namespace sound
} // namespace munchkin

#endif // MUNCHKIN_ONLINE_SOUND_PLAYER_HPP