#ifndef MUNCHKIN_ONLINE_LUA_BINDINGS_H
#define MUNCHKIN_ONLINE_LUA_BINDINGS_H

#include <sol/sol.hpp>

namespace munchkin::codegen::lua_bindings {

class LuaCodegenObject {
public:
    LuaCodegenObject(std::string const& project_dir, std::string const& build_dir);

    /// Calls a function for each attributed entity found in a path searched recursively
    void scan_dir(std::string const& path, sol::function const& for_each_entity);

    class LuaOutputFile {
    public:
        LuaOutputFile(std::string const& path);
        /// Contents are flushed to file on GC (Destructor)
        ~LuaOutputFile();

        /// Appends some contents to the file (overwrites previous file)
        void write(std::string const& contents);

    private:
        std::string path;
        std::string contents;
    };

    LuaOutputFile new_output_file(std::string const& path_relative_to_build_dir);

private:
    std::string project_dir;
    std::string build_dir;
};

void inject_types(sol::state& state);

}

#endif // MUNCHKIN_ONLINE_LUA_BINDINGS_H
