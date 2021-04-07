#pragma once

#include "RenderCommand.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/scene.h>           // Output data structure


namespace vengine
{
	class ModelLoader
	{
	public:
		[[nodiscard]] static std::vector<RenderCommand> get_model_commands(const std::string& filepath);
		

	private:
		static void load_model(const std::string& filepath);
		static void process_scene(const aiScene* scene, const std::string& filepath);
		[[nodiscard]] static RenderCommand process_mesh(aiMesh* ai_mesh, const aiScene* scene, const std::string& file_dir);
		[[nodiscard]] static std::vector<Texture2dGL> process_material(aiMaterial* ai_material, const std::string& file_dir, aiTextureType ai_texture_type);

	
	private:
		inline static std::unordered_map<std::string, std::vector<RenderCommand>> s_commands =
			std::unordered_map<std::string, std::vector<RenderCommand>>();

		inline static std::unordered_map<std::string, Texture2dGL> s_loaded_textures =
			std::unordered_map<std::string, Texture2dGL>();
	};

	
}

