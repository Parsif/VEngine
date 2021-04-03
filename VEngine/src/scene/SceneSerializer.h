#pragma once
#include "Scene.h"

#include <yaml-cpp/emitter.h>

namespace vengine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(Ref<Scene> scene);
		void serialize(const std::string& filepath);
		void deserialize(const std::string& filepath) const;
	
	private:
		void serialize_entity(YAML::Emitter& out, entt::entity entity) const;

	private:
		Ref<Scene> m_scene;
	};
}


