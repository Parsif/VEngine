#pragma once

#ifdef V_PLATFORM_WINDOWS

	#include "../platform/windows/WindowsWindow.h"

#endif

#include "../renderer/Renderer.h"
#include "renderer/MaterialLibrary.h"

#include "Vtypes.h"

#include "../ui/ImGuiUI.h"

#include "events/WindowEvents.h"
#include "events/Input.h"

#include "scene/Scene.h"
#include "scene/SceneSerializer.h"


#include "utils/Timer.h"

