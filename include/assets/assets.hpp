#ifndef MUNCHKIN_ONLINE_ASSET_MANAGER_HPP_
#define MUNCHKIN_ONLINE_ASSET_MANAGER_HPP_

#include "assets/asset_loaders.hpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <string>
#include <unordered_map>

namespace munchkin {

namespace assets {

constexpr std::string_view assets_relative_path = "data";
template<typename T> class Manager;

template<typename T> Manager<T>& get_manager();

class PathDatabase {
public:
    inline static void load_paths_from_json_file(fs::path const& path) {
        std::ifstream database_file(path);
        database_json = nlohmann::json::parse(database_file);
    }

    inline static nlohmann::json get_asset_node(std::string const& str_id) {
        return database_json[str_id];
    }

private:
    inline static nlohmann::json database_json;
};

template<typename T> struct Handle {
    size_t id;

    T& get() const { return get_manager<T>().assets[id]; }
};

template<typename A> class Manager {
public:
    // Clear all assets
    ~Manager() {
        for (auto& [id, asset] : assets) { loaders::free(asset); }
    }

    Handle<A> load_asset(std::string const& str_id, loaders::LoadParams<A> const& params) {
        // check if the asset is already loaded
        if (auto it = str_id_map.find(str_id); it != str_id_map.end()) {
            return {it->second};
        }

        size_t id = id_generator.next();
        // Create new value
        A& a = assets[id];
        str_id_map[str_id] = id;
        loaders::load(a, params);
        return {id};
    }

    // Loads an asset extracting the path of it from the assets database.
    Handle<A> load_asset(std::string const& str_id) {
        // Check if the asset is already loaded
        if (auto it = str_id_map.find(str_id); it != str_id_map.end()) {
            return {it->second};
        }

        size_t id = id_generator.next();
        // Create new value
        A& a = assets[id];
        str_id_map[str_id] = id;
        auto params = loaders::LoadParams<A>::from_json(PathDatabase::get_asset_node(str_id),
                                                        assets_relative_path);
        if (!params) {
            std::string err =
                "Could not find a corresponding path for the asset ID \"" + str_id + "\"!";
            throw std::runtime_error(err.c_str());
        }
        loaders::load(a, {params});
        return {id};
    }

    Handle<A> get_asset_handle(std::string const& str_id) { return {str_id_map[str_id]}; }

    void delete_asset(Handle<A> handle) {
        loaders::free(assets[handle.id]);
        assets.erase(handle.id);
    }

    [[deprecated("Use handle.get() instead.")]] A& get_asset(Handle<A> handle) {
        return assets[handle.id];
    }

    [[deprecated("Use handle.get() instead.")]] A const& get_const_asset(Handle<A> handle) const {
        return assets.at(handle.id);
    }

private:
    struct IDGen {
        size_t cur = 0;

        size_t next() { return cur++; }

    } id_generator;

    std::unordered_map<size_t, A> assets;
    std::unordered_map<std::string, size_t> str_id_map;
};

template<typename T> Manager<T>& get_manager() {
    static Manager<T> manager;
    return manager;
}

} // namespace assets

} // namespace munchkin

#endif