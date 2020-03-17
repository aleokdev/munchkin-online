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

#include "game.hpp"
#include "game_wrapper.hpp"
#include "systems/ai_manager.hpp"
#include "systems/game_renderer.hpp"
#include "systems/input_binder.hpp"
#include "systems/state_debugger.hpp"
#include "systems/ai_manager.hpp"

#include "input/input.hpp"

#include "renderer/font_renderer.hpp"
#include "renderer/sprite_renderer.hpp"

#include "sound/sound_player.hpp"

#include <audeo/audeo.hpp>

#include <sstream>
#include <string_view>

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

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

int main(int argc, char* argv[]) try {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL Init error: " << SDL_GetError();
        return -1;
    }

    // GL 4.3 + GLSL 430
    const char* glsl_version = "#version 430";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window =
        SDL_CreateWindow("Munchkin Online", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    bool err = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0;
    std::cout << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_MULTISAMPLE);

    if (err) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    audeo::InitInfo audeo_init_info;
    audeo_init_info.frequency = 44100;
#ifdef __linux__
    // Some linux systems have delayed audio when the chunk size is set too high. Doesn't happen on
    // windows systems though.
    audeo_init_info.chunk_size = 2048;
#endif
    if (!audeo::init(audeo_init_info)) {
        std::cerr << "Failed to initialize audeo!" << std::endl;
        return -1;
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

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
