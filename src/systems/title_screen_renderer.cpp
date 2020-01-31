#include "systems/title_screen_renderer.hpp"

#include "input/input.hpp"
#include "renderer/assets.hpp"
#include "renderer/font_renderer.hpp"
#include "util/util.hpp"


namespace munchkin::systems {

namespace option_callbacks {

TitleScreenRenderer::Status quit(TitleScreenRenderer&) {
    return TitleScreenRenderer::Status::QuitApp;
}

TitleScreenRenderer::Status local_game(TitleScreenRenderer&) {
    return TitleScreenRenderer::Status::EnterGamePlaying;
}

TitleScreenRenderer::Status credits(TitleScreenRenderer&) {
    return TitleScreenRenderer::Status::None;
}

}

TitleScreenRenderer::TitleScreenRenderer() {

    // Load assets

    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& shader_manager = assets::get_manager<renderer::Shader>();
    auto& font_manager = assets::get_manager<renderer::Font>();

    assets::loaders::LoadParams<renderer::Texture> bg_params;
    bg_params.path = "data/generic/bg.png";
    background = renderer::create_background(texture_manager.load_asset("bg", bg_params));

    assets::loaders::LoadParams<renderer::Shader> sprite_shader_params{"data/shaders/sprite.vert",
                                                                       "data/shaders/sprite.frag"};
    sprite_shader = shader_manager.load_asset("sprite_shader", sprite_shader_params);

    assets::loaders::LoadParams<renderer::Font> font_params;
    font_params.path = "data/generic/quasimodo_regular.ttf";
    font = font_manager.load_asset("main_font", font_params);

    options = {"Local game", "Credits", "Exit"};
    option_callbacks = {
        option_callbacks::local_game, 
        option_callbacks::credits,
        option_callbacks::quit
    };

    option_colors = std::vector<glm::vec3>(options.size(), default_option_color);

    text_scale = glm::vec2(1, 1);
    text_base_position = glm::vec2(0.05f, 0.2f);
}

void TitleScreenRenderer::set_render_target(renderer::RenderTarget* tg) {
    target = tg;
    text_spacing = (text_scale.y * base_point_size + spacing) / target->get_height();
}

TitleScreenRenderer::Status TitleScreenRenderer::frame(float delta_time) {
    renderer::render_background(background);

    render_menu_options();

    return update_status();
}

float TitleScreenRenderer::get_menu_option_y_offset(size_t opt_index) {
    return opt_index * text_spacing;
}

float TitleScreenRenderer::calculate_text_width(std::string const& text) {
    return (text.size() * text_scale.x * base_point_size) / target->get_width();
}

void TitleScreenRenderer::render_menu_options() {
    renderer::FontRenderer renderer;
    float yoffset = 0.0f;

    renderer.set_size(text_scale);
    renderer.set_window_size(target->get_width(), target->get_height());

    auto render_option = [this, &renderer, &yoffset](size_t i) {
        std::string const& text = options[i];
        renderer.set_color(option_colors[i]);
        renderer.set_position(glm::vec2(text_base_position.x, text_base_position.y + yoffset));
        renderer.render_text(font, text);
        yoffset += text_spacing;
    };

    for (size_t i = 0; i < options.size(); ++i) { render_option(i); }
}

TitleScreenRenderer::Status TitleScreenRenderer::update_status() {
    for (size_t opt_index = 0; opt_index < options.size(); ++opt_index) {
        // Compute bounding box
        glm::vec2 top_left = text_base_position + glm::vec2(0, get_menu_option_y_offset(opt_index));
        glm::vec2 bottom_right = top_left + glm::vec2(calculate_text_width(options[opt_index]),
                                                      base_point_size / target->get_height());
        BoundingBox bbox{top_left, bottom_right};
        auto mouse_pos = input::get_mouse_pos();
        // Normalize mouse position
        mouse_pos.x /= target->get_width();
        mouse_pos.y /= target->get_height();
        if (inside_bounding_box(bbox, glm::vec2(mouse_pos.x, mouse_pos.y))) {
            option_colors[opt_index] = glm::vec3(1, 1, 1);
            if (input::has_mousebutton_been_clicked(input::MouseButton::left)) {
                status = option_callbacks[opt_index](*this);
                return status;
            }
        } else {
            option_colors[opt_index] = default_option_color;
        }
    }

    return Status::None;
}

} // namespace munchkin::systems