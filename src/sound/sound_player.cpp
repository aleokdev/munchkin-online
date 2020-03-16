#include "sound/sound_player.hpp"
#include "sound/sound_assets.hpp"

#include <audeo/audeo.hpp>

namespace munchkin {

audeo::Sound current_music_playing(-1);
assets::Handle<sound::Music> current_music_handle_playing;

namespace sound {

void play_music(assets::Handle<Music> m) {
    current_music_handle_playing = m;
    current_music_playing =
        audeo::play_sound(assets::get_manager<Music>().get_asset(m).source,
                          audeo::loop_forever, music_crossfade_seconds);
}
void stop_music() {
    current_music_handle_playing.id = -1;
    audeo::stop_sound(current_music_playing, music_crossfade_seconds);
    current_music_playing = audeo::Sound(-1);
}

void play_sfx(assets::Handle<SoundEffect> s) {
    audeo::play_sound(assets::get_manager<SoundEffect>().get_asset(s).source);
}

audeo::Sound get_current_music_being_played() { return current_music_playing; }
assets::Handle<Music> get_current_music_handle_being_played() {
    return current_music_handle_playing;
}

} // namespace sound
} // namespace munchkin