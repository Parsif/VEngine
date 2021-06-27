#include "precheader.h"
#include "ModelLoader.h"

#include "opengl/TextureGL.h"
#include <assimp/pbrmaterial.h>


namespace vengine
{
	Mesh& ModelLoader::get_mesh(const std::string& filepath)
	{
		if (s_meshes.find(filepath) == s_meshes.end())
		{
			load_model(filepath);
		}
		//TODO: add check if model was not loaded
		return s_meshes[filepath];
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
			LOG_ERROR(importer.GetErrorString());

			return;
		}

		process_scene(scene, filepath);
	}


	void ModelLoader::process_scene(const aiScene* scene, const std::string& filepath)
	{
		Mesh mesh;
		const auto file_dir = filepath.substr(0, filepath.find_last_of('/'));
		for(size_t i = 0; i < scene->mNumMeshes; ++i)
		{
			mesh.commands.push_back(process_mesh(scene->mMeshes[i], scene, file_dir));
		}
		mesh.has_albedo_texture = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) > 0;
		mesh.has_roughness_texture = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex]->GetTextureCount(aiTextureType_SHININESS) > 0;
		mesh.has_metallic_texture = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex]->GetTextureCount(aiTextureType_METALNESS) > 0;
		mesh.has_ao_texture = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex]->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0;
		mesh.has_normal_texture = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex]->GetTextureCount(aiTextureType_NORMALS) > 0;

		s_meshes[filepath] = mesh;
	}

	RenderCommand ModelLoader::process_mesh(aiMesh* ai_mesh, const aiScene* scene, const std::string& file_dir)
	{
		RenderCommand command;
		const BufferLayout buffer_layout{
			ShaderDataType::Float3, ShaderDataType::Float2,
			ShaderDataType::Float3, ShaderDataType::Float3
		};
		command.set_buffer_layout(buffer_layout);
		
		std::vector<float> vertices;
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

			if(ai_mesh->HasNormals())
			{
				vertices.push_back(ai_mesh->mNormals[i].x);
				vertices.push_back(ai_mesh->mNormals[i].y);
				vertices.push_back(ai_mesh->mNormals[i].z);
			}

			if(ai_mesh->HasTangentsAndBitangents())
			{
				vertices.push_back(ai_mesh->mTangents[i].x);
				vertices.push_back(ai_mesh->mTangents[i].y);
				vertices.push_back(ai_mesh->mTangents[i].z);
			}
		}
		
		//TODO: add error handling in case of empty vector
		command.set_vertex_buffer(vertices.data(), sizeof(vertices.back()) * vertices.size());


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
			command.set_index_buffer(indices.data(), sizeof(indices.back()) * indices.size(), indices.size());
		}

		auto albedo_textures = process_material(scene->mMaterials[ai_mesh->mMaterialIndex], file_dir, aiTextureType_DIFFUSE);
		auto metallic_textures = process_material(scene->mMaterials[ai_mesh->mMaterialIndex], file_dir, aiTextureType_METALNESS);
		auto roughness_textures = process_material(scene->mMaterials[ai_mesh->mMaterialIndex], file_dir, aiTextureType_SHININESS);
		auto ao_textures = process_material(scene->mMaterials[ai_mesh->mMaterialIndex], file_dir, aiTextureType_AMBIENT_OCCLUSION);
		auto normals_textures = process_material(scene->mMaterials[ai_mesh->mMaterialIndex], file_dir, aiTextureType_NORMALS);

		std::vector<TextureGL> texture2ds;
		texture2ds.insert(texture2ds.end(), std::make_move_iterator(albedo_textures.begin()), std::make_move_iterator(albedo_textures.end()));
		texture2ds.insert(texture2ds.end(), std::make_move_iterator(metallic_textures.begin()), std::make_move_iterator(metallic_textures.end()));
		texture2ds.insert(texture2ds.end(), std::make_move_iterator(roughness_textures.begin()), std::make_move_iterator(roughness_textures.end()));
		texture2ds.insert(texture2ds.end(), std::make_move_iterator(ao_textures.begin()), std::make_move_iterator(ao_textures.end()));
		texture2ds.insert(texture2ds.end(), std::make_move_iterator(normals_textures.begin()), std::make_move_iterator(normals_textures.end()));

		command.set_textures2d(texture2ds);
		return command;
	}

	std::vector<TextureGL> ModelLoader::process_material(aiMaterial* ai_material, const std::string& file_dir, aiTextureType ai_texture_type)
	{
		std::vector<TextureGL> textures2d;

		for (unsigned int i = 0; i < ai_material->GetTextureCount(ai_texture_type); ++i)
		{
			aiString texture_path;
			if (ai_material->GetTexture(ai_texture_type, 0, &texture_path) == AI_SUCCESS)
			{
				const auto fullpath = file_dir + '/' + texture_path.data;
				LOG_INFO(fullpath);
				if (s_loaded_textures.find(fullpath) != s_loaded_textures.end())
				{
					textures2d.push_back(s_loaded_textures[fullpath]);
				}
				
				else
				{
					const TextureGL texture2d{ fullpath, ai_texture_type };
					textures2d.push_back(texture2d);
					s_loaded_textures[fullpath] = texture2d;
				}
			
			}
		}
		return textures2d;
	}

}
