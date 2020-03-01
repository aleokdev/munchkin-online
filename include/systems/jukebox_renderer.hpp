#ifndef MUNCHKIN_ONLINE_JUKEBOX_RENDERER_HPP
#define MUNCHKIN_ONLINE_JUKEBOX_RENDERER_HPP
#include <audeo/audeo.hpp>
#include <renderer/assets.hpp>

namespace munchkin {
class RenderWrapper;

namespace systems {

class JukeboxRenderer {
public:
    JukeboxRenderer(RenderWrapper& wrapper);
    void render();

private:
    audeo::Sound last_music_being_played{-1};
    RenderWrapper* wrapper;

    assets::Handle<renderer::Shader> solid_shader;
    assets::Handle<renderer::Font> song_title_font;
    assets::Handle<renderer::Font> song_artist_font;

    float current_y_pos = 0;
    std::size_t ticks_to_close = 0;
    constexpr static std::size_t ticks_to_show_panel = 60 * 3;
    constexpr static float panel_animation_slowness = 8.f;
};

} // namespace systems
} // namespace munchkin
#endif // MUNCHKIN_ONLINE_JUKEBOX_RENDERER_HPP
