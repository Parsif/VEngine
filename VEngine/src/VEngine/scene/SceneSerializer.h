#pragma once
#include "Scene.h"

#include <yaml-cpp/emitter.h>

namespace vengine
{
	class SceneSerializer
	{
	public:
		void static serialize(const std::string& filepath, Ref<Scene> scene);
		void static deserialize(const std::string& filepath, Ref<Scene> scene);
	
	private:
		void static serialize_entity(YAML::Emitter& out, entt::entity entity, Ref<Scene> scene);

	};
}


