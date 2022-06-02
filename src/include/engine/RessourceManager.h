#pragma once

#include "engine/Base.h"
#include "engine/ILoadable.h"
#include "engine/SpecificRessourceManager.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>

namespace engine {
    class RessourceManager {
    private:

    public:
        static inline std::unordered_map<std::string, sptr<ISpecificRessourceManager>> RessourceManagers{};

        template<class T>
        static void RegisterRessourceType() {
            std::string TypeName = typeid(T).name();

            if (RessourceManagers.find(TypeName) != RessourceManagers.end()) {
                LOG_ENGINE_ERROR("Failed to register already registered ressource type {0}!", TypeName);
                throw std::invalid_argument("Tried to register already registered ressource type!");
            }

            RessourceManagers.insert({TypeName, std::make_shared<SpecificRessourceManager<T>>()});
	    LOG_ENGINE_TRACE("Registered ressource type {0}", TypeName);
        }

        template<class T>
        static T Get(std::string Path) {
            std::string TypeName = typeid(T).name();

            if (!RessourceManagers.count(TypeName)) {
                LOG_ENGINE_ERROR("Failed to get ressource of non-registered ressource type {0}!", TypeName);
                throw std::invalid_argument("Tried to get ressource of non-registered ressource type!");
            }

            return std::static_pointer_cast<SpecificRessourceManager<T>>(RessourceManagers.at(TypeName))->Get(Path);
        }

        template<class T>
        static void Set(std::string Path, T Ressource) {
            std::string TypeName = typeid(T).name();

            if (!RessourceManagers.count(TypeName)) {
                LOG_ENGINE_ERROR("Failed to set ressource of non-registered ressource type {0}!", TypeName);
                throw std::invalid_argument("Tried to set ressource of non-registered ressource type!");
            }

            return std::static_pointer_cast<SpecificRessourceManager<T>>(RessourceManagers.at(TypeName))->Set(Path, Ressource);
        }
    };
}// namespace engine
