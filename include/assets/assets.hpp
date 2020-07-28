#ifndef MUNCHKIN_ONLINE_ASSET_MANAGER_HPP_
#define MUNCHKIN_ONLINE_ASSET_MANAGER_HPP_

#include "assets/asset_loaders.hpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace munchkin {

namespace assets {

constexpr std::string_view assets_relative_path = "data";
class AssetRef;
using AssetReferenceContainer = std::vector<AssetRef>;
template<typename T> struct Handle;

class AssetManager {
public:
    // Returns an enumerator that allows loading the assets present in a JSON file.
    static inline AssetReferenceContainer enumerate_assets(fs::path const& json_file);

    // Gets an already loaded asset from the database, or loads it by manually inputting its
    // parameters and ID if it doesn't exist.
    template<typename AssetT>
    static inline Handle<AssetT> load_asset(std::string const& str_id,
                                            loaders::LoadParams<AssetT> const& params);

    // Gets an already loaded asset from the database, otherwise fails and throws an exception.
    template<typename AssetT> static inline Handle<AssetT> load_asset(std::string const& str_id);

    template<typename AssetT> static inline void delete_asset(Handle<AssetT> handle);

    // The file that is used to index an asset if it doesn't exist when calling load_asset(str_id).
    static inline const fs::path backup_asset_database = "data/assets.json";

private:
    template<typename AssetT> using AssetContainer = std::unordered_map<std::size_t, AssetT>;

    template<typename AssetT> static inline AssetContainer<AssetT>& container() {
        static AssetContainer<AssetT> instance;
        return instance;
    }

    static inline std::unordered_map<std::string, std::size_t> id_map;

    static inline std::size_t generate_id() {
        static std::size_t last_id = 1;
        return last_id++;
    }

    template<typename T> friend struct Handle;
};

namespace detail {

class TypelessAssetLoader {
public:
    template<typename AssetT> static inline TypelessAssetLoader create() {
        TypelessAssetLoader result;
        result.loader = std::make_unique<TypedAssetLoader<AssetT>>();
        return result;
    }

    inline void load(std::string const& str_id, nlohmann::json const& asset_obj) const {
        if (!loader)
            throw std::runtime_error("what!!!!!!!!");
        loader->load(str_id, asset_obj);
    }

    static TypelessAssetLoader from_category_name(std::string const& json_category_name) {
        // TODO: Codegen this
        if (json_category_name == loaders::MetaInfo<renderer::Texture>::json_category_name) {
            return TypelessAssetLoader::create<renderer::Texture>();
        } else if (json_category_name == loaders::MetaInfo<renderer::Shader>::json_category_name) {
            return TypelessAssetLoader::create<renderer::Shader>();
        } else if (json_category_name == loaders::MetaInfo<renderer::Font>::json_category_name) {
            return TypelessAssetLoader::create<renderer::Font>();
        } else if (json_category_name ==
                   loaders::MetaInfo<sound::SoundEffect>::json_category_name) {
            return TypelessAssetLoader::create<sound::SoundEffect>();
        } else if (json_category_name == loaders::MetaInfo<sound::Music>::json_category_name) {
            return TypelessAssetLoader::create<sound::Music>();
        } else {
            throw std::runtime_error("Unknown asset category found");
        }
    }

    inline TypelessAssetLoader() : loader(nullptr) {}

private:
    class BaseAssetLoader {
    public:
        inline virtual ~BaseAssetLoader() = default;
        inline virtual void load(std::string const& str_id, nlohmann::json const& asset_obj) const {
            throw std::runtime_error("bruh");
        };
    };
    template<typename AssetT> class TypedAssetLoader : public BaseAssetLoader {
    public:
        inline void load(std::string const& str_id,
                         nlohmann::json const& asset_obj) const override {
            AssetManager::load_asset(
                str_id, loaders::LoadParams<AssetT>{loaders::LoadParams<AssetT>::from_json(
                            asset_obj, assets_relative_path)});
        }
    };
    std::unique_ptr<BaseAssetLoader> loader;
};

} // namespace detail

class AssetRef {
public:
    inline AssetRef(std::string const& str_id,
                    std::string const& json_category_name,
                    nlohmann::json const& asset_object) :
        str_id(str_id),
        category_name(json_category_name), asset_object(asset_object),
        loader(detail::TypelessAssetLoader::from_category_name(json_category_name)) {}

    inline AssetRef(AssetRef const& other) :
        str_id(other.str_id), category_name(other.category_name), asset_object(other.asset_object),
        loader(detail::TypelessAssetLoader::from_category_name(category_name)) {}

    inline void load() const { loader.load(str_id, asset_object); }

private:
    AssetRef() = default;
    std::string str_id;
    std::string category_name;
    nlohmann::json asset_object;
    detail::TypelessAssetLoader loader;
};

AssetReferenceContainer AssetManager::enumerate_assets(fs::path const& json_file) {
    std::ifstream json_file_handle(json_file);
    auto json = nlohmann::json::parse(json_file_handle);
    AssetReferenceContainer asset_container;
    for (const auto& [category_name, category_obj] : json.items()) {
        for (const auto& [asset_str_id, asset_obj] : category_obj.items()) {
            asset_container.emplace_back(asset_str_id, (std::string)category_name, asset_obj);
        }
    }
    return asset_container;
}

template<typename AssetT>
Handle<AssetT> AssetManager::load_asset(std::string const& str_id,
                                        loaders::LoadParams<AssetT> const& params) {
    if (auto it = id_map.find(str_id); it != id_map.end()) {
        // Return the asset if it is already loaded
        return {it->second};
    } else {
        // Otherwise, create new value
        auto id = generate_id();
        id_map[str_id] = id;
        AssetT& a = container<AssetT>()[id];
        loaders::load(a, params);
        return {id};
    }
}

template<typename AssetT> Handle<AssetT> AssetManager::load_asset(std::string const& str_id) {
    if (auto it = id_map.find(str_id); it != id_map.end()) {
        // Return the asset if it is already loaded
        return {it->second};
    } else {
        // Otherwise return a placeholder if it doesn't exist
        std::cout << "Warning: Loading asset \"" << str_id << "\" from backup database.\n";
        auto id = generate_id();
        id_map[str_id] = id;

        std::ifstream json_file_handle(backup_asset_database);
        auto json = nlohmann::json::parse(json_file_handle);
        AssetT& a = container<AssetT>()[id];
        loaders::load(a,
                      loaders::LoadParams<AssetT>{loaders::LoadParams<AssetT>::from_json(
                          json[(std::string)loaders::MetaInfo<AssetT>::json_category_name][str_id],
                          assets_relative_path)});
        return {id};
    }
}

template<typename AssetT> void AssetManager::delete_asset(Handle<AssetT> handle) {
    loaders::free(container<AssetT>()[handle.id]);
    container<AssetT>().erase(handle.id);

    // Erase string IDs that point to this asset
    id_map.erase(std::remove_if(id_map.begin(), id_map.end(),
                                [handle](const auto& pair) { return pair->second == handle.id; }),
                 id_map.end());
}

template<typename AssetT> struct Handle {
    std::size_t id = 0;

    Handle() = default;
    Handle(Handle const&) = default;
    Handle& operator=(Handle const&) = default;
    Handle(std::size_t id) : id(id) {}
    Handle& operator=(std::nullptr_t) {
        id = 0;
        return *this;
    }

    [[nodiscard]] AssetT& get() const { return AssetManager::container<AssetT>()[id]; }

    operator bool() const noexcept { return id; }
};

} // namespace assets

} // namespace munchkin

#endif