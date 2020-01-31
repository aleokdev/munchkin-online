#include "systems/title_screen_renderer.hpp"

#include "renderer/assets.hpp"
#include "renderer/font_renderer.hpp"
#include "input/input.hpp"

namespace munchkin::systems {

TitleScreenRenderer::TitleScreenRenderer() {
    
    // Load assets

    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& shader_manager = assets::get_manager<renderer::Shader>();
    auto& font_manager = assets::get_manager<renderer::Font>();

    assets::loaders::LoadParams<renderer::Texture> bg_params;
    bg_params.path = "data/generic/bg.png";
    background = renderer::create_background(texture_manager.load_asset("bg", bg_params));

    assets::loaders::LoadParams<renderer::Shader> sprite_shader_params {
        "data/shaders/sprite.vert", "data/shaders/sprite.frag"
    };
    sprite_shader = shader_manager.load_asset("sprite_shader", sprite_shader_params);

    assets::loaders::LoadParams<renderer::Font> font_params;
    font_params.path = "data/generic/quasimodo_regular.ttf";
    font = font_manager.load_asset("main_font", font_params);
}

void TitleScreenRenderer::set_render_target(renderer::RenderTarget* tg) {
    target = tg;
}

TitleScreenRenderer::Status TitleScreenRenderer::frame(float delta_time) {
    renderer::render_background(background);

    glm::vec2 pos = glm::vec2(0.5, 0.5);
    glm::vec2 size = glm::vec2(1, 1);
    
    renderer::FontRenderer renderer;
    renderer.set_window_size(target->get_width(), target->get_height());
    renderer.set_position(pos);
    renderer.set_size(size);
    renderer.set_color(glm::vec3(1, 0, 0));
    renderer.render_text(font, "Pengu is cool");


    if (input::is_key_pressed(SDLK_SPACE)) {
        return Status::EnterGamePlaying;
    }

    return Status::None;
}

}