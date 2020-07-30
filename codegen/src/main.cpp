#include "parser.hpp"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "lua_bindings.hpp"

namespace fs = std::filesystem;
using namespace munchkin::codegen;

struct AssetWithDirName {
    AttributedEntity asset_entity;
    Attribute dir_name_attribute;
};

struct SerializableAsset {
    fs::path header_path;
    AttributedEntity asset_entity;
    /// Names of asset types that must be loaded before this one.
    std::vector<std::string> asset_load_dependencies;
};

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Syntax: munchkin-codegen <script> <project path> <build path>" << std::endl;
        return -1;
    }
    const fs::path script_path = fs::absolute(fs::path(argv[1]));
    const fs::path project_path = fs::absolute(fs::path(argv[2]));
    const fs::path build_path = fs::absolute(fs::path(argv[3]));

    sol::state state;
    state.open_libraries(sol::lib::table, sol::lib::base, sol::lib::math, sol::lib::string);
    lua_bindings::inject_types(state);
    state["codegen"] = lua_bindings::LuaCodegenObject(project_path, build_path);
    state.script_file(script_path);
}