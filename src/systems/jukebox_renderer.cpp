#include "systems/jukebox_renderer.hpp"
#include "game_wrapper.hpp"
#include "render_wrapper.hpp"
#include "renderer/font_renderer.hpp"
#include "renderer/sprite_renderer.hpp"
#include "sound/sound_player.hpp"
#include <glm/glm/gtc/type_ptr.hpp>

namespace munchkin {
namespace systems {

JukeboxRenderer::JukeboxRenderer(RenderWrapper& w) : wrapper(&w) {
    solid_shader = assets::get_manager<renderer::Shader>().load_asset(
        "solid_shader", {"data/shaders/solid.vert", "data/shaders/solid.frag"});
    song_title_font = assets::get_manager<renderer::Font>().load_asset(
        "default_font", {"data/generic/quasimodo_regular.ttf"});
    song_artist_font = assets::get_manager<renderer::Font>().load_asset(
        "subtitle_font", {"data/generic/Raleway-Light.ttf"});
}

void JukeboxRenderer::render() {
    if (audeo::is_playing_music() &&
        last_music_being_played != sound::get_current_music_being_played()) {
        last_music_being_played = sound::get_current_music_being_played();
        ticks_to_close = ticks_to_show_panel;
    }

    renderer::SpriteRenderer spr;

    auto& shader = assets::get_manager<renderer::Shader>().get_asset(solid_shader);
    glUseProgram(shader.handle);
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(wrapper->projection));

    const auto& window_w = wrapper->wrapper->game.window_w;
    const auto& window_h = wrapper->wrapper->game.window_h;
    // Set draw data
    spr.set_camera_drag(false);
    spr.set_color(0, 0, 0, .8f);
    spr.set_position({window_w - 250, -50 + current_y_pos});
    spr.set_scale(glm::vec2(300, 100));
    // Execute drawcall
    spr.do_draw();
    current_y_pos +=
        ((ticks_to_close == 0 ? 0.f : 100.f) - current_y_pos) / panel_animation_slowness;
    if (ticks_to_close != 0)
        ticks_to_close--;

    renderer::FontRenderer fnt;
    fnt.set_window_size(window_w, window_h);
    fnt.set_position({(float)(window_w - 390) / (float)window_w,
                      (float)(window_h - current_y_pos) / (float)window_h});
    fnt.set_size({.7f, .7f});
    fnt.render_text(song_title_font, assets::get_manager<assets::Music>()
                                         .get_asset(sound::get_current_music_handle_being_played())
                                         .title);
    fnt.set_size({.5f, .5f});
    fnt.set_position({(float)(window_w - 390) / (float)window_w,
                      (float)(window_h + 35 - current_y_pos) / (float)window_h});
    fnt.render_text(song_artist_font, assets::get_manager<assets::Music>()
        .get_asset(sound::get_current_music_handle_being_played())
        .artist);
}

} // namespace systems
} // namespace munchkin