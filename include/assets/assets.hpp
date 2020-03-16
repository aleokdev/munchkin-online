#ifndef MUNCHKIN_ONLINE_ASSET_MANAGER_HPP_
#define MUNCHKIN_ONLINE_ASSET_MANAGER_HPP_

#include "assets/asset_loaders.hpp"

#include <unordered_map>
#include <string>

namespace munchkin {

namespace assets {

template<typename T>
struct Handle {
    size_t id;
};

template<typename A>
class Manager {
public:
    // Clear all assets
    ~Manager() {
        for (auto&[id, asset] : assets) {
            loaders::free(asset);
        }
    }

    Handle<A> load_asset(std::string const& str_id, loaders::LoadParams<A> const& params) {
        // check if the asset is already loaded
        if (auto it = str_id_map.find(str_id); it != str_id_map.end()) {
            return { it->second };
        }

        size_t id = id_generator.next();
        // Create new value
        A& a = assets[id];
        str_id_map[str_id] = id;
        loaders::load(a, params);
        return { id };
    }

    Handle<A> get_asset_handle(std::string const& str_id) {
        return { str_id_map[str_id] };
    }

    void delete_asset(Handle<A> handle) {
        loaders::free(assets[handle.id]);
        assets.erase(handle.id);
    }

    A& get_asset(Handle<A> handle) {
        return assets[handle.id];
    }

    A const& get_const_asset(Handle<A> handle) const {
        return assets.at(handle.id);
    }

private:
    struct IDGen {
        size_t cur = 0;

        size_t next() {
            return cur++;
        }

    } id_generator;


    std::unordered_map<size_t, A> assets;
    std::unordered_map<std::string, size_t> str_id_map;
};

template<typename T>
Manager<T>& get_manager() {
    static Manager<T> manager;
    return manager;
}

}

} // namespace munchkin

#endif