#pragma once

#include "RenderCommand.h"
#include "RenderQueue.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/scene.h>           // Output data structure



namespace vengine
{
	class ModelLoader
	{
	public:
		[[nodiscard]] static Drawable& get_drawable(const std::string& filepath);


	private:
		static void load_model(const std::string& filepath);
		static void process_scene(const aiScene* scene, const std::string& filepath);
		[[nodiscard]] static RenderCommand process_mesh(aiMesh* ai_mesh, const aiScene* scene, const std::string& file_dir);
		[[nodiscard]] static std::vector<TextureGL> process_material(aiMaterial* ai_material, const std::string& file_dir, aiTextureType ai_texture_type);

	
	private:
		inline static std::unordered_map<std::string, Drawable> s_drawables =
			std::unordered_map<std::string, Drawable>();

		inline static std::unordered_map<std::string, TextureGL> s_loaded_textures =
			std::unordered_map<std::string, TextureGL>();
	};

	
}

