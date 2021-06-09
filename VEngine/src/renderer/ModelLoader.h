#pragma once

#include "RenderCommand.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/scene.h>           // Output data structure

#include "Mesh.h"


namespace vengine
{
	class ModelLoader
	{
	public:
		[[nodiscard]] static Mesh& get_mesh(const std::string& filepath);


	private:
		static void load_model(const std::string& filepath);
		static void process_scene(const aiScene* scene, const std::string& filepath);
		[[nodiscard]] static RenderCommand process_mesh(aiMesh* ai_mesh, const aiScene* scene, const std::string& file_dir);
		[[nodiscard]] static std::vector<TextureGL> process_material(aiMaterial* ai_material, const std::string& file_dir, aiTextureType ai_texture_type);

	
	private:
		inline static std::unordered_map<std::string, Mesh> s_meshes =
			std::unordered_map<std::string, Mesh>();

		inline static std::unordered_map<std::string, TextureGL> s_loaded_textures =
			std::unordered_map<std::string, TextureGL>();
	};

	
}

