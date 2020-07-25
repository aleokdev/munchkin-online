#include <iostream>
#define SDL_MAIN_HANDLED

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#ifdef _WIN32
#    include <sdl/SDL.h>
#else
#    include <SDL2/SDL.h>
#endif

#include <glad/glad.h>

#include "game_wrapper.hpp"
#include "systems/ai_manager.hpp"

#include "assets/assets.hpp"
#include "input/input.hpp"

#include "sound/sound_player.hpp"

#include <audeo/audeo.hpp>

#include <string_view>

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720
constexpr const char* glsl_version = "#version 430";
struct GLVersion {
    int major;
    int minor;
};
constexpr struct GLVersion gl_version { 4, 3 };

void parse_args(munchkin::GameWrapper& game_wrapper, std::vector<std::string_view> args) {
    for (int argn = 0; argn < args.size(); argn++) {
        const auto& arg = args[argn];
        if (arg == "--no-title") {
            game_wrapper.renderer.set_state(munchkin::RenderWrapper::State::GamePlaying);
            game_wrapper.do_tick = true;
        }
        if (arg == "--cp") {
            game_wrapper.game.state.add_cardpack(args[++argn]);

            std::cout << "Cards loaded: " << game_wrapper.game.state.all_cards.size() << std::endl;
            game_wrapper.renderer.game_renderer.update_sprite_vector();
        }
        if (arg == "--players") {
            auto& game = game_wrapper.game;
            int total_players = std::atoi(args[++argn].data());

            game.state.players.clear();

            for (int i = 0; i < total_players; i++) game.state.players.emplace_back(game.state, i);

            for (auto& player : game.state.players) {
                player.hand_max_cards = game.state.default_hand_max_cards;
            }

            std::vector<munchkin::PlayerPtr> players_to_control;

            for (int i = 1; i < total_players; i++)
                players_to_control.emplace_back(game.state.players[i]);

            game_wrapper.ai_manager =
                munchkin::AIManager(game.state, players_to_control, std::string(args[++argn]));
        }
        if (arg == "--name") {
            auto& game = game_wrapper.game;
            // Set local player name
            game.state.players[game.local_player_id].name = args[++argn];
        }
        if (arg == "--bgm-volume") {
            munchkin::sound::set_music_volume((float)std::atoi(args[++argn].data()) / 100.f);
        }
    }
}

static SDL_Window* init_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return nullptr;
    }

    // GL 4.3 + GLSL 430
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_version.major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_version.minor);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    // Multisampling might not work on some devices (i.e. Laptops such as mine), so I commented it
    // out.
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    auto window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window =
        SDL_CreateWindow("Munchkin Online", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, window_flags);
    return window;
}

static SDL_GLContext init_opengl(SDL_Window* window) {
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    bool err = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0;
    if (err) {
        return nullptr;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_MULTISAMPLE);
    return gl_context;
}

static bool init_sound() {
    audeo::InitInfo audeo_init_info;
    audeo_init_info.frequency = 44100;
#ifdef __linux__
    // Some linux systems have delayed audio when the chunk size is set too high. Doesn't happen on
    // windows systems though.
    audeo_init_info.chunk_size = 2048;
#endif
    return audeo::init(audeo_init_info);
}

static bool init_imgui(SDL_Window* window, SDL_GLContext context) {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    if (!ImGui_ImplSDL2_InitForOpenGL(window, context))
        return false;
    if (!ImGui_ImplOpenGL3_Init(glsl_version))
        return false;
    return true;
}

int main(int argc, char* argv[]) try {
    const char* error_help_msg = "If the problem persists, please report this error "
                                 "along with debug information to the project creators.";

    SDL_Window* window;
    SDL_GLContext ogl_context;
    if (window = init_window(); !window) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << ". " << error_help_msg << std::endl;
        return -1;
    }

    if (ogl_context = init_opengl(window); !ogl_context) {
        std::cerr << "OpenGL Init Error. Please update your drivers. " << error_help_msg
                  << std::endl;
        return -1;
    }

    if (!init_sound()) {
        std::cerr << "Sound Init Error. " << error_help_msg << std::endl;
        return -1;
    }

    if (!init_imgui(window, ogl_context)) {
        std::cerr << "ImGUI Init Error. " << error_help_msg << std::endl;
        return -1;
    }

    munchkin::assets::PathDatabase::load_paths_from_json_file("data/assets.json");
    munchkin::GameWrapper wrapper(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 3, 2);

    std::vector<std::string_view> args;

    args.resize(argc);
    for (int argn = 0; argn < argc; argn++) args[argn] = argv[argn];
    parse_args(wrapper, args);

    wrapper.main_loop(window);

    return 0;
} catch (std::exception const& e) {
    std::cout << "FATAL ERROR: " << e.what() << std::endl;
    return -1;
}
