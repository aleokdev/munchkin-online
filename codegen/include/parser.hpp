#ifndef MUNCHKIN_PARSER_HPP
#define MUNCHKIN_PARSER_HPP

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace munchkin::codegen {

enum class EntityType {
    // parser only accepts attributes assigned to structs/classes right now
    struct_t,
    class_t,
    none
};

struct Attribute {
    std::vector<std::string> arguments;
    std::string name;
    std::string scope;
};

struct AttributedEntity {
    std::vector<Attribute> attributes;
    std::string name;
    // TODO: implement scope as well
    EntityType type;
};

std::vector<AttributedEntity> parse_cpp_file(fs::path const& path);

}

#endif // MUNCHKIN_PARSER_HPP