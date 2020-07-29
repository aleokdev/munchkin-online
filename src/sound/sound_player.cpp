#include "sound/sound_player.hpp"
#include "sound/sound_assets.hpp"

#include <audeo/audeo.hpp>

namespace munchkin {

audeo::Sound current_music_playing(-1);
assets::Handle<sound::Music> current_music_handle_playing;
float music_volume = 1;

namespace sound {

void play_music(assets::Handle<Music> m) {
    current_music_handle_playing = m;
    current_music_playing =
        audeo::play_sound(m.get().source,
                          audeo::loop_forever, music_crossfade_seconds);
    audeo::set_volume(current_music_playing, music_volume);
}
void stop_music() {
    current_music_handle_playing = nullptr;
    audeo::stop_sound(current_music_playing, music_crossfade_seconds);
    current_music_playing = audeo::Sound(-1);
}

void play_sfx(assets::Handle<SoundEffect> s) {
    audeo::play_sound(s.get().source);
}

audeo::Sound get_current_music_being_played() { return current_music_playing; }
assets::Handle<Music> get_current_music_handle_being_played() {
    return current_music_handle_playing;
}

void set_music_volume(float new_volume)
{
    audeo::set_volume(current_music_playing, new_volume);
    music_volume = new_volume;
}

} // namespace sound
} // namespace munchkin