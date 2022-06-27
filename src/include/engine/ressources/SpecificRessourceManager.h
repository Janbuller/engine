#pragma once

#include <string>
#include <unordered_map>
#include "engine/Base.h"

namespace engine {
    class ISpecificRessourceManager {};

    template<class T>
    class SpecificRessourceManager : public ISpecificRessourceManager {
    private:
        std::unordered_map<std::string, T> LoadedRessources{};

    public:
        T Get(std::string Path) {
            if (LoadedRessources.count(Path)) {
                return LoadedRessources[Path];
            }

            T Loaded = T::Load(Path);
            LoadedRessources.insert({Path, Loaded});

            return Loaded;
        }

        void Set(std::string Path, T Ressource) {
            if (LoadedRessources.count(Path)) {
                LoadedRessources.at(Path).Free();
            }
            LoadedRessources.insert_or_assign({Path, Ressource});
        }
    };
}// namespace engine
