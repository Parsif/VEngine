#include "precheader.h"
#include "ModelLoader.h"

#include "opengl/Texture2dGL.h"

namespace vengine
{
	[[nodiscard]] std::vector<RenderCommand> ModelLoader::get_model_commands(const std::string& filepath)
	{
		if(s_commands.find(filepath) == s_commands.end())
		{
			load_model(filepath);
		}
		//TODO: add check if model was not loaded
		return s_commands[filepath];
	}

	

	void ModelLoader::load_model(const std::string& filepath)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filepath,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (!scene)
		{
			std::cerr << importer.GetErrorString() << '\n';
			return;
		}

		process_scene(scene, filepath);
	}


	void ModelLoader::process_scene(const aiScene* scene, const std::string& filepath)
	{
		std::vector<RenderCommand> commands;
		const auto file_dir = filepath.substr(0, filepath.find_last_of('/'));
		for(size_t i = 0; i < scene->mNumMeshes; ++i)
		{
			commands.push_back(process_mesh(scene->mMeshes[i], scene, file_dir));
		}
	
		s_commands[filepath] = commands;
	}

	RenderCommand ModelLoader::process_mesh(aiMesh* ai_mesh, const aiScene* scene, const std::string& file_dir)
	{
		RenderCommand triangle_command;
		std::vector<float> vertices;
		const BufferLayout buffer_layout{ ShaderDataType::Float3, ShaderDataType::Float2 };
		triangle_command.set_buffer_layout(buffer_layout);
		
		for (size_t i = 0; i < ai_mesh->mNumVertices; ++i)
		{
			const auto pos = ai_mesh->mVertices[i];
			vertices.push_back(pos.x);
			vertices.push_back(pos.y);
			vertices.push_back(pos.z);

			if (ai_mesh->HasTextureCoords(0))
			{
				vertices.push_back(ai_mesh->mTextureCoords[0][i].x);
				vertices.push_back(ai_mesh->mTextureCoords[0][i].y);
			}
			
		}
		
		//TODO: add error handling in case of empty vector
		triangle_command.set_vertex_buffer(vertices.data(), sizeof(vertices.back()) * vertices.size());


		std::vector<unsigned int> indices;
		for (size_t i = 0; i < ai_mesh->mNumFaces; ++i)
		{
			for (size_t j = 0; j < ai_mesh->mFaces[i].mNumIndices; ++j)
			{
				indices.push_back(ai_mesh->mFaces[i].mIndices[j]);
			}
		}
		
		if (!indices.empty())
		{
			triangle_command.set_index_buffer(indices.data(), sizeof(indices.back()) * indices.size(), indices.size());
		}
		
		triangle_command.set_textures2d(process_material(scene->mMaterials[ai_mesh->mMaterialIndex], file_dir));
		
		return triangle_command;
	}

	std::vector<Texture2dGL> ModelLoader::process_material(aiMaterial* ai_material, const std::string& file_dir)
	{
		std::vector<Texture2dGL> textures2d;

		for (unsigned int i = 0; i < ai_material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
		{
			aiString texture_path;
			if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) == AI_SUCCESS)
			{
				const auto fullpath = file_dir + '/' + texture_path.data;
				if (s_loaded_textures.find(fullpath) != s_loaded_textures.end())
				{
					textures2d.push_back(s_loaded_textures[fullpath]);
				}
				
				else
				{
					const Texture2dGL texture2d{ fullpath };
					textures2d.push_back(texture2d);
					s_loaded_textures[fullpath] = texture2d;
				}
			
			}
		}
		return textures2d;
	}

}
