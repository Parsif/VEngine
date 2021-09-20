#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <utility>

#include "renderer/opengl/TextureGL.h"
#include "renderer/Camera.h"


namespace vengine
{
	struct TagComponent
	{
		explicit TagComponent(std::string tag_name) : tag(std::move(tag_name))
		{
            constexpr unsigned int TAG_MAX_SIZE = 100;
            tag.resize(TAG_MAX_SIZE);
		}
		
		std::string tag;
	};

    struct TransformComponent
    {
        TransformComponent(const glm::vec3 translation_, const glm::vec3 rotation_, const glm::vec3 scale_)
            : translation(translation_), rotation(rotation_), scale(scale_) {}
        TransformComponent(glm::vec3 translation_) : translation(translation_) {}
        TransformComponent() = default;
        glm::vec3 translation{ 0.0f }, rotation{ 0.0f }, scale{ 1.0f };

        [[nodiscard]] inline auto get_transform() const 
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, translation);
            transform *= glm::toMat4(glm::quat(rotation));
            transform = glm::scale(transform, scale);
            return transform;
        }
    };

	struct ModelComponent
	{
        ModelComponent(std::string filepath_) : filepath(std::move(filepath_)){}
        std::string filepath;
	};

	struct CameraComponent
	{
		CameraComponent(const Camera& camera_) : camera(camera_) {}
        Camera camera;
	};

	struct DirLightComponent
	{   
        glm::vec3 color{1.0f};
        float intensity{ 1.0f };
	};

    struct PointLightComponent
    {
        glm::vec3 color{ 1.0f };
        float intensity{ 1.0f };
        float light_radius{1.0f};
    };

    struct SphereAreaLightComponent
    {
        glm::vec3 color{ 1.0f };
        float intensity{ 1.0f };
        float light_radius{ 1.0f };
        float source_radius{ 1.0f };
    };

    struct TubeAreaLightComponent
    {
        glm::vec3 color{ 1.0f };
        float intensity{ 1.0f };
        float light_radius{ 1.0f };
        float source_radius{ 1.0f };
        float length{ 1.0f };
    };

	
	struct MaterialsComponent
	{
        TextureGL albedo_texture;
        TextureGL metallic_texture;
        TextureGL roughness_texture;
        TextureGL ao_texture;
        TextureGL normal_texture;
	};
}
