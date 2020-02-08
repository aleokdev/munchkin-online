#include <iostream>
#define SDL_MAIN_HANDLED

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <sdl/SDL.h>

#include <glad/glad.h>

#include "game.hpp"
#include "game_wrapper.hpp"
#include "systems/ai_manager.hpp"
#include "systems/game_renderer.hpp"
#include "systems/input_binder.hpp"
#include "systems/state_debugger.hpp"

#include "input/input.hpp"

#include "renderer/font_renderer.hpp"
#include "renderer/sprite_renderer.hpp"

#include <audeo/audeo.hpp>

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

int main() try {
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
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window =
        SDL_CreateWindow("Munchkin Online", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    bool err = gladLoadGL() == 0;

    if (err) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    if (!audeo::init()) {
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

    wrapper.main_loop(window);

    return 0;
} catch (int) {}
