#include "engine/core/Logger.h"

#include "engine/lua/usertypes/LAllUsertypes.h"

#include "engine/lua/usertypes/components/LCamera.h"
#include "engine/lua/usertypes/components/LLight.h"
#include "engine/lua/usertypes/components/LModel.h"
#include "engine/lua/usertypes/components/LTransform.h"

#include "engine/lua/usertypes/glcore/LShader.h"
#include "engine/lua/usertypes/glcore/LTexture.h"
#include "engine/lua/usertypes/glcore/LTexture2D.h"
#include "engine/lua/usertypes/glcore/LTextureCubemap.h"

#include "engine/lua/usertypes/math/LQuat.h"
#include "engine/lua/usertypes/math/LVec2.h"
#include "engine/lua/usertypes/math/LVec3.h"

#include "engine/lua/usertypes/model/LMaterial.h"
#include "engine/lua/usertypes/model/LMesh.h"
#include "engine/lua/usertypes/model/LTextureType.h"
#include "engine/lua/usertypes/model/LTextureTypeInfo.h"
#include "engine/lua/usertypes/model/LVertex.h"

namespace engine::lua::usertypes {
    void InitializeAllUsertypes(sol::state& L) {
        InitializeCamera(L);
        InitializeLight(L);
        InitializeModel(L);
        InitializeTransform(L);

        InitializeShader(L);

        InitializeQuat(L);
        InitializeVec2(L);
        InitializeVec3(L);

        InitializeMaterial(L);
        InitializeMesh(L);
        InitializeTexture(L);
        InitializeTextureCubemap(L);
        InitializeTextureType(L);
        InitializeTextureTypeInfo(L);
        InitializeVertex(L);

        LOG_ENGINE_TRACE("Initialized all lua usertypes");
    }
}
