#include "precheader.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>

#include "core/Logger.h"

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}


namespace vengine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}
	
	SceneSerializer::SceneSerializer(const Ref<Scene> scene) : m_scene(scene)
	{
	}
	
	void SceneSerializer::serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_scene->m_registry.each([&](auto entity_id)
			{
				serialize_entity(out, entity_id);
			});
		
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout{ filepath };
		fout << out.c_str();
	}

	void SceneSerializer::deserialize(const std::string& filepath) const
	{
		YAML::Node data = YAML::LoadFile(filepath);
		if (!data["Scene"])
		{
			Logger::log("File is corrupted", Logger::MessageSeverity::ERROR);
		}

		auto yaml_entities = data["Entities"];
		if (yaml_entities)
		{
			m_scene->clear();
			for (auto yaml_entity : yaml_entities)
			{
				entt::entity entity;
				auto tag_component = yaml_entity["TagComponent"];
				if(tag_component)
				{
					entity = m_scene->create_empty_entity(tag_component["Tag"].as<std::string>());
				}

				auto transform_component = yaml_entity["TransformComponent"];
				if (transform_component)
				{
					const auto translation = transform_component["Translation"].as<glm::vec3>();
					const auto rotation = transform_component["Rotation"].as<glm::vec3>();
					const auto scale = transform_component["Scale"].as<glm::vec3>();
					m_scene->add_component<TransformComponent>(entity, translation, rotation, scale);
				}

				auto camera_component = yaml_entity["CameraComponent"];
				if (camera_component)
				{
					const auto fov = camera_component["PerspectiveFOV"].as<float>();
					const auto near_z = camera_component["PerspectiveNear"].as<float>();
					const auto far_z = camera_component["PerspectiveFar"].as<float>();

					m_scene->add_component<CameraComponent>(entity, Camera{fov, near_z, far_z});
					m_scene->set_camera_entity(entity);
				}

				auto model_component = yaml_entity["ModelComponent"];
				if (model_component)
				{
					m_scene->add_component<ModelComponent>(entity, model_component["ModelFilepath"].as<std::string>());
				}

				auto dir_light_component = yaml_entity["DirLightComponent"];
				if (dir_light_component)
				{
					DirLightComponent component;

					component.direction = dir_light_component["Direction"].as<glm::vec3>();
					component.color = dir_light_component["Color"].as<glm::vec3>();
					component.ambient_intensity = dir_light_component["AmbientIntensity"].as<float>();
					component.diffuse_intensity = dir_light_component["DiffuseIntensity"].as<float>();
					component.specular_intensity = dir_light_component["SpecularIntensity"].as<float>();

					m_scene->add_component<DirLightComponent>(entity, component);
				}
			}
		}
	}
	

	void SceneSerializer::serialize_entity(YAML::Emitter& out, entt::entity entity) const
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << "12837192831273"; // TODO: Entity ID goes here

		if (m_scene->m_registry.has<TagComponent>(entity))
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = m_scene->m_registry.get<TagComponent>(entity).tag;
			out << YAML::Key << "Tag" << YAML::Value << tag.c_str();

			out << YAML::EndMap; // TagComponent
		}

		if (m_scene->m_registry.has<TransformComponent>(entity))
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& transform_component = m_scene->m_registry.get<TransformComponent>(entity);
			out << YAML::Key << "Translation" << YAML::Value << transform_component.translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform_component.rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform_component.scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (m_scene->m_registry.has<CameraComponent>(entity))
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& camera = m_scene->m_registry.get<CameraComponent>(entity).camera;

			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.get_fov();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << *camera.get_near_z_pointer();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << *camera.get_far_z_pointer();

			out << YAML::EndMap; // CameraComponent
		}


		if (m_scene->m_registry.has<ModelComponent>(entity))
		{
			out << YAML::Key << "ModelComponent";
			out << YAML::BeginMap; // ModelComponent

			auto& model_filepath = m_scene->m_registry.get<ModelComponent>(entity).filepath;

			out << YAML::Key << "ModelFilepath" << YAML::Value << model_filepath;

			out << YAML::EndMap; // ModelComponent
		}


		if (m_scene->m_registry.has<DirLightComponent>(entity))
		{
			out << YAML::Key << "DirLightComponent";
			out << YAML::BeginMap; // ModelComponent

			auto& dir_light_component = m_scene->m_registry.get<DirLightComponent>(entity);

			out << YAML::Key << "Direction" << YAML::Value << dir_light_component.direction;
			out << YAML::Key << "Color" << YAML::Value << dir_light_component.color;
			out << YAML::Key << "AmbientIntensity" << YAML::Value << dir_light_component.ambient_intensity;
			out << YAML::Key << "DiffuseIntensity" << YAML::Value << dir_light_component.diffuse_intensity;
			out << YAML::Key << "SpecularIntensity" << YAML::Value << dir_light_component.specular_intensity;

			out << YAML::EndMap; // ModelComponent
		}

		out << YAML::EndMap; // Entity
	}
}
