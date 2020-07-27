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
class AssetCategoryEnumerator;
template<typename T> struct Handle;

class AssetManager {
public:
    // Returns an enumerator that allows loading the assets present in a JSON file.
    static inline AssetCategoryEnumerator enumerate_assets(fs::path const& json_file);

    // Gets an already loaded asset from the database, or loads it by manually inputting its
    // parameters and ID if it doesn't exist.
    template<typename AssetT>
    static inline Handle<AssetT> load_asset(std::string const& str_id,
                                            loaders::LoadParams<AssetT> const& params);

    // Gets an already loaded asset from the database, otherwise fails and throws an exception.
    template<typename AssetT> static inline Handle<AssetT> load_asset(std::string const& str_id);

    template<typename AssetT> static inline void delete_asset(Handle<AssetT> handle);

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
    inline TypelessAssetLoader() = default;
    template<typename AssetT> static inline TypelessAssetLoader create() {
        TypelessAssetLoader result;
        result.loader = std::make_unique<TypedAssetLoader<AssetT>>();
        return result;
    }

    inline void load(nlohmann::json::iterator const& asset_it) const { loader->load(asset_it); }

private:
    class BaseAssetLoader {
    public:
        inline virtual ~BaseAssetLoader() = default;
        inline virtual void load(nlohmann::json::iterator const& asset_it) const = 0;
    };
    template<typename AssetT> class TypedAssetLoader : public BaseAssetLoader {
    public:
        inline ~TypedAssetLoader() override = default;
        inline void load(nlohmann::json::iterator const& asset_it) const override {
            AssetManager::load_asset(
                asset_it.key(), loaders::LoadParams<AssetT>{loaders::LoadParams<AssetT>::from_json(
                                    asset_it.value(), assets_relative_path)});
        }
    };
    std::unique_ptr<BaseAssetLoader> loader;
};

}

class AssetEnumerator {
public:
    class AssetIterator;
    class Asset {
    public:
        inline void load() const { loader->load(asset_iterator); }

    private:
        inline Asset(nlohmann::json::iterator const& asset_iterator,
                     detail::TypelessAssetLoader const& loader) :
            asset_iterator(asset_iterator),
            loader(&loader) {}

        nlohmann::json::iterator asset_iterator;
        detail::TypelessAssetLoader const* loader;
        friend class AssetIterator;
    };

    class AssetIterator {
    public:
        AssetIterator(nlohmann::json::iterator assets_begin, nlohmann::json::iterator assets_end, std::string const& json_category_name) :
            json_iterator(assets_begin), json_end_iterator(assets_end), type_loader(get_type_loader(json_category_name)) {}

        Asset operator*() const {
            if (json_iterator == json_end_iterator)
                throw std::runtime_error("Tried to dereference out-of-bounds asset iterator");
            return Asset(json_iterator, type_loader);
        }

        AssetIterator& operator++() {
            if (json_iterator == json_end_iterator) {
                throw std::runtime_error("Tried to overflow asset iteration");
            } else {
                json_iterator++;
            }
            return *this;
        }

        bool operator==(AssetIterator const& other) const noexcept {
            return other.json_iterator == json_iterator;
        }
        bool operator!=(AssetIterator const& other) const noexcept { return other.json_iterator != json_iterator; }

    private:
        static detail::TypelessAssetLoader get_type_loader(std::string const& json_category_name) {
            // TODO: Codegen this
            detail::TypelessAssetLoader result;
            if (json_category_name ==
                loaders::MetaInfo<renderer::Texture>::json_category_name) {
                result = detail::TypelessAssetLoader::create<renderer::Texture>();
            } else if (json_category_name ==
                       loaders::MetaInfo<renderer::Shader>::json_category_name) {
                result = detail::TypelessAssetLoader::create<renderer::Shader>();
            } else if (json_category_name ==
                       loaders::MetaInfo<renderer::Font>::json_category_name) {
                result = detail::TypelessAssetLoader::create<renderer::Font>();
            } else if (json_category_name ==
                       loaders::MetaInfo<sound::SoundEffect>::json_category_name) {
                result = detail::TypelessAssetLoader::create<sound::SoundEffect>();
            } else if (json_category_name ==
                       loaders::MetaInfo<sound::Music>::json_category_name) {
                result = detail::TypelessAssetLoader::create<sound::Music>();
            } else {
                throw std::runtime_error("Unknown asset category found");
            }
            return result;
        }
        nlohmann::json::iterator json_iterator;
        nlohmann::json::iterator json_end_iterator;
        detail::TypelessAssetLoader type_loader;
    };

    AssetEnumerator(nlohmann::json category_object, std::string const& category_name) : json(category_object), _category_name(category_name) {}

    inline AssetIterator begin() { return AssetIterator(json.begin(), json.end(), _category_name); }
    inline AssetIterator end() { return AssetIterator(json.end(), json.end(), _category_name); }

    inline std::string category_name() const noexcept { return _category_name; }

private:
    nlohmann::json json;
    std::string const _category_name;
};

class AssetCategoryEnumerator {
public:
    class AssetCategoryIterator;

    AssetCategoryEnumerator(nlohmann::json json_root) : json(json_root) {}

    inline AssetCategoryIterator begin() { return AssetCategoryIterator(json.begin(), json.end()); }
    inline AssetCategoryIterator end() { return AssetCategoryIterator(json.end(), json.end()); }

    class AssetCategoryIterator {
    public:
        AssetCategoryIterator(nlohmann::json::iterator const& categories_begin,
                 nlohmann::json::iterator const& categories_end) :
            category_iterator(categories_begin),
        categories_end(categories_end) {
        }

        AssetEnumerator operator*() const {
            if (category_iterator == categories_end)
                throw std::runtime_error("Tried to dereference out-of-bounds asset category iterator");
            return AssetEnumerator(category_iterator.value(), category_iterator.key());
        }

        AssetCategoryIterator& operator++() {
            if (category_iterator == categories_end) {
                    throw std::runtime_error("Tried to overflow asset iteration");
            } else {
                category_iterator++;
            }
            return *this;
        }

        bool operator==(AssetCategoryIterator const& other) const noexcept {
            return other.category_iterator == category_iterator;
        }
        bool operator!=(AssetCategoryIterator const& other) const noexcept { return other.category_iterator != category_iterator; }

    private:
        nlohmann::json::iterator category_iterator;
        nlohmann::json::iterator categories_end;
    };

private:
    nlohmann::json json;
};

AssetCategoryEnumerator AssetManager::enumerate_assets(fs::path const& json_file) {
    std::ifstream json_file_handle(json_file);
    auto json = nlohmann::json::parse(json_file_handle);
    return AssetCategoryEnumerator(json);
}

template<typename AssetT>
Handle<AssetT> AssetManager::load_asset(std::string const& str_id,
                                        loaders::LoadParams<AssetT> const& params) {
    // Return the asset if it is already loaded
    if (auto it = id_map.find(str_id); it != id_map.end()) {
        return {it->second};
    }

    auto& assets = container<AssetT>();

    // Otherwise, create new value
    auto id = generate_id();
    id_map[str_id] = id;
    AssetT& a = assets[id];
    loaders::load(a, params);
    return {id};
}

template<typename AssetT> Handle<AssetT> AssetManager::load_asset(std::string const& str_id) {
    // Return the asset if it is already loaded
    if (auto it = id_map.find(str_id); it != id_map.end()) {
        return {it->second};
    }

    throw std::runtime_error("Could not find asset");
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
    std::size_t id;

    AssetT& get() const { return AssetManager::container<AssetT>()[id]; }

    operator bool() const noexcept { return id; }
};

} // namespace assets

} // namespace munchkin

#endif