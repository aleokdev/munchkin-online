#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <cctype>

namespace munchkin::codegen {

std::string_view consume_spaces(std::string_view view) {
    std::size_t index = 0;
    for(;std::isspace(view[index]); ++index);
    return view.substr(index);
}

std::vector<std::string_view> split(std::string_view view, char separator) {
    std::vector<std::string_view> items;
    std::string_view items_left = view;
    while (!items_left.empty()) {
        items.emplace_back(items_left.substr(0, items_left.find_first_of(separator)));

        if (auto separator_index = items_left.find_first_of(separator);
            separator_index != std::string_view::npos) {
            items_left = items_left.substr(items_left.find_first_of(separator) + 1);
        } else
            break;
    }

    return items;
}

Attribute parse_attribute(std::string_view& view) {
    Attribute attribute;
    // Consume [[
    view = view.substr(2);
    view = consume_spaces(view);

    std::string_view full_attribute = view.substr(0, view.find_first_of("]]"));

    std::size_t scope_operator_location = full_attribute.find_last_of("::");
    if (scope_operator_location != std::string_view::npos) {
        attribute.scope = std::string(full_attribute.substr(0, scope_operator_location - 1));
    }

    std::size_t parenthesis_open_location = full_attribute.find_first_of('(');
    if (parenthesis_open_location != std::string_view::npos) {
        if (scope_operator_location == std::string_view::npos) {
            attribute.name = full_attribute.substr(0, parenthesis_open_location);
        } else {
            attribute.name =
                full_attribute.substr(scope_operator_location + 1,
                                      parenthesis_open_location - scope_operator_location - 1);
        }
        std::string_view arguments_view =
            full_attribute.substr(parenthesis_open_location + 1, full_attribute.find_first_of(')') -
                                                                 parenthesis_open_location - 1);
        std::vector<std::string_view> args = split(arguments_view, ',');
        for (const auto& arg : args) {
            attribute.arguments.emplace_back(consume_spaces(arg));
        }
    } else {
        if(scope_operator_location == std::string_view::npos) {
            attribute.name = full_attribute.substr(0, view.find_first_of("]]"));
        } else {
            attribute.name = full_attribute.substr(scope_operator_location + 1, view.find_first_of("]]") - 1);
        }
    }

    view = view.substr(view.find_first_of("]]")+2);

    return attribute;
}

bool next_token_is_attribute(std::string_view view) {
    return view.substr(0, 2) == "[[";
}

bool next_token_is_struct(std::string_view view) {
    return view.substr(0, 6) == "struct";
}

bool next_token_is_class(std::string_view view) {
    return view.substr(0, 5) == "class";
}

std::vector<AttributedEntity> parse_cpp_file(fs::path const& path) {
    std::vector<AttributedEntity> result;

    std::ifstream f(path);
    std::stringstream buffer;
    buffer << f.rdbuf();
    std::string file_str = buffer.str();
    std::string_view file_view = file_str;
    std::vector<Attribute> attribute_queue;
    EntityType type_read = EntityType::none;

    while (!file_view.empty()) {
        if(next_token_is_struct(file_view)) {
            type_read = EntityType::struct_t;
            file_view = file_view.substr(6);
        } else if(next_token_is_class(file_view)) {
            type_read = EntityType::class_t;
            file_view = file_view.substr(5);
        } else if ((type_read == EntityType::struct_t || type_read == EntityType::class_t) && next_token_is_attribute(file_view)) {
            attribute_queue.emplace_back(parse_attribute(file_view));
        } else if(!std::isspace(file_view[0]) && (type_read == EntityType::struct_t || type_read == EntityType::class_t)) {
            std::size_t entity_name_start_index = 0;
            for(;std::isspace(file_view[entity_name_start_index]); ++entity_name_start_index);
            std::size_t entity_name_length = 1;
            for(;!std::isspace(file_view[entity_name_start_index+entity_name_length]); ++entity_name_length);

            if(!attribute_queue.empty()) {
                std::string_view entity_name_view = file_view.substr(entity_name_start_index, entity_name_length);

                result.emplace_back(AttributedEntity{attribute_queue, std::string(entity_name_view), type_read});
                attribute_queue.clear();
            }
            file_view = file_view.substr(entity_name_start_index + entity_name_length);
            type_read = EntityType::none;
        }
        else {
            // Consume one character
            file_view = file_view.substr(1);
        }
    }

    return result;
}

} // namespace arpiyi::codegen