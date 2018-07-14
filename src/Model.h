#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "Vector3.h"
#include "Ray.h"
#include "Constant.h"
#include "HitPoint.h"
#include "Material.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h> // Post processing flags

//#include <ImathVec.h>
//#include <ImathLine.h>
//#include <ImathLineAlgo.h>
#pragma comment(lib, "assimp-vc140-mt.lib")
//#pragma comment(lib, "assimp-vc140-mt.dll")

struct Vertex {
	Vector3 position;
	//Imath::V2d texcoord;
};

//// 三角形.
//struct Triangle {
//	Imath::V3d p0;
//	Imath::V3d p1;
//	Imath::V3d p2;
//};
//
//struct Texture {
//	std::string type;
//	std::string path;
//	//ID3D11ShaderResourceView *texture;
//};


class Mesh
{

public:
	Mesh(const std::vector<Vertex>&& vertices, const std::vector<uint32_t>&& indices, MaterialPtr material)
		//Mesh(const std::vector<Vertex>&& vertices, const std::vector<uint32_t>&& indices, const std::vector<Texture>&& textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		//this->textures = textures;
		for (int i = 0; i < indices.size(); i += 3)
		{
			//triangles.emplace_back(Triangle(vertices[indices[i]].position, vertices[indices[i+1]].position, vertices[indices[i+2]].position, material));
			//list.Add(std::make_shared<Triangle>(vertices[indices[i+2]].position, vertices[indices[i + 1]].position, vertices[indices[i]].position, material));
			list.Add(std::make_shared<Triangle>(vertices[indices[i]].position, vertices[indices[i + 1]].position, vertices[indices[i + 2]].position, material));
		}
	}

public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	ShapeList list;
	std::vector<Triangle> triangles;
	//std::vector<Texture> textures;
};

class Model : public Hitable
{
public:
	std::vector<Mesh> meshes;
	MaterialPtr material;

public:
	Model(std::string path, MaterialPtr material)
		: meshes()
		, material(material)
	{
		Load(path);
	}

	void processNode(aiNode * node, const aiScene * scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.emplace_back(processMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh * mesh, const aiScene * scene)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

			//if (textype.empty()) textype = determineTextureType(scene, mat);
		}

		// Walk through each of the mesh's vertices
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			double scale = 100;
			vertex.position.x = mesh->mVertices[i].x * scale;
			vertex.position.y = mesh->mVertices[i].y * scale;
			vertex.position.z = mesh->mVertices[i].z * scale;

			if (mesh->mTextureCoords[0])
			{
				//vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
				//vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
			}

			vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			//vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
			//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		}

		return Mesh(std::move(vertices), std::move(indices), material);
		//return Mesh(std::move(vertices), std::move(indices), std::move(textures));
	}

	bool Load(std::string path)
	{
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(path,	aiProcess_Triangulate |	aiProcess_ConvertToLeftHanded);

		if (pScene == NULL)
		{
			return false;
		}

		//Triangle triangle;
		//triangle.p0 = Imath::V3d(128, 50, -10);
		//triangle.p1 = Imath::V3d(50, 200, -10);
		//triangle.p2 = Imath::V3d(200, 200, -10);
		//// 画像真ん中の後ろのほうをposとした適当なレイを作る.
		//Imath::Line3d ray;
		//ray.pos = Imath::V3d(1 / 2.0, 1 / 2.0, 100);
		//// レイの方向.
		//ray.dir = Imath::V3d(1, 0, 0) - ray.pos;

		//// 三角形とレイの交差判定.
		//Imath::V3d hit_point;
		//Imath::V3d barycentric;
		//bool is_front = false;
		//if (Imath::intersect(ray, triangle.p0, triangle.p1, triangle.p2, hit_point, barycentric, is_front))
		//{
		//}

		if (pScene->HasMaterials()) {
			//ci::app::console() << "Materials:" << num << std::endl;
			processNode(pScene->mRootNode, pScene);

			uint32_t num = pScene->mNumMaterials;
			aiMaterial** mat = pScene->mMaterials;
			for (uint32_t i = 0; i < num; ++i) {
//				model.material.push_back(createMaterial(mat[i]));
//
//				// テクスチャ読み込み
//				const auto& m = model.material.back();
//				if (!m.has_texture) continue;
//
//#if defined (USE_FULL_PATH)
//				std::string path = model.directory + "/" + PATH_WORKAROUND(m.texture_name);
//				auto texture = loadTexrture(path);
//#else
//				auto texture = loadTexrture(PATH_WORKAROUND(m.texture_name));
//#endif
//
//				model.textures.insert(std::make_pair(m.texture_name, texture));
			}
		}
		return true;
		//model.node = createNode(scene->mRootNode, scene->mMeshes);
	}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
		for (auto& mesh : meshes)
		{
			return mesh.list.intersect(ray, hitpoint);
			//for (auto& triangle : mesh.triangles)
			//{
			//	triangle.intersect(ray, hitpoint);
			//}
		}
		return false;
	}

	virtual const MaterialPtr GetMaterial() const override
	{
		return material;
	}

private:

};