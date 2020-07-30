#include "lua_bindings.hpp"
#include "parser.hpp"
#include <fstream>

namespace munchkin::codegen::lua_bindings {

LuaCodegenObject::LuaCodegenObject(const std::string& project_dir, const std::string& build_dir) :
    project_dir(project_dir), build_dir(build_dir) {}

void LuaCodegenObject::scan_dir(std::string const& path, sol::function const& for_each_entity) {
    for (const auto& entry : std::filesystem::directory_iterator(
             fs::path(project_dir) / path,
             std::filesystem::directory_options::follow_directory_symlink)) {
        if (entry.is_regular_file()) {
            for (const auto& entity : parse_cpp_file(entry)) { for_each_entity(entity, entry.path().generic_string()); }
        }
    }
}

LuaCodegenObject::LuaOutputFile
LuaCodegenObject::new_output_file(std::string const& path_relative_to_build_dir) {
    return LuaOutputFile(fs::path(build_dir) / path_relative_to_build_dir);
}

LuaCodegenObject::LuaOutputFile::LuaOutputFile(std::string const& path) : path(path) {}

LuaCodegenObject::LuaOutputFile::~LuaOutputFile() {
    fs::create_directories(fs::path(path).parent_path());
    std::ofstream file(path);

    bool use_header_guards = path.find(".h") != std::string::npos;
    if (use_header_guards) {
        std::string header_filename = fs::path(path).filename().generic_string();
        // Make the header filename uppercase
        std::transform(header_filename.begin(), header_filename.end(), header_filename.begin(),
                       [](unsigned char c) -> unsigned char {
                           if (c == '.')
                               return '_';
                           return std::toupper(c);
                       });
        const std::string header_guard_def = "CODEGEN_" + header_filename + "__";
        const std::string header_guard = "#ifndef " + header_guard_def + "\n#define " + header_guard_def + "\n\n";
        file.write(header_guard.c_str(), header_guard.size());
    }

    file.write(contents.c_str(), contents.size());

    if (use_header_guards) {
        file.write("\n\n#endif", 8);
    }
}

void LuaCodegenObject::LuaOutputFile::write(std::string const& str) { contents += str; }

void inject_types(sol::state& state) {
    /* clang-format off */
    state.new_usertype<LuaCodegenObject>("code_generator",
        sol::constructors<LuaCodegenObject(std::string const&, std::string const&)>(),
        "scan_dir", &LuaCodegenObject::scan_dir,
        "new_output_file", &LuaCodegenObject::new_output_file);

    state.new_usertype<LuaCodegenObject::LuaOutputFile>("code_generator_output_file",
        sol::no_constructor,
        "write", &LuaCodegenObject::LuaOutputFile::write);

    state.new_usertype<AttributedEntity>("code_generator_attr_entity",
        "attributes", &AttributedEntity::attributes,
        "name", &AttributedEntity::name,
        "type", &AttributedEntity::type);

    state.new_usertype<Attribute>("code_generator_attr",
        "arguments", &Attribute::arguments,
        "name", &Attribute::name,
        "scope", &Attribute::scope);
    /* clang-format on */
}

} // namespace munchkin::codegen::lua_bindings