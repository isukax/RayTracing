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

#include <ImathVec.h>
#include <ImathLine.h>
#include <ImathLineAlgo.h>
//#pragma comment(lib, "assimp-vc140-mt.lib")
//#pragma comment(lib, "assimp-vc140-mt.dll")


class Model
{
public:
	struct Vertex {
		Imath::V3d position;
		Imath::V2d texcoord;
	};

	// 三角形.
	struct Triangle {
		Imath::V3d p0;
		Imath::V3d p1;
		Imath::V3d p2;
	};

	struct Texture {
		std::string type;
		std::string path;
		//ID3D11ShaderResourceView *texture;
	};


public:
	Model() {}

	static void processNode(aiNode * node, const aiScene * scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			//meshes.push_back(this->processMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	//Mesh processMesh(aiMesh * mesh, const aiScene * scene)
	//{
	//	Triangle triangle;
	//	triangle.p0 = Imath::V3d(128, 50, -10);
	//	triangle.p1 = Imath::V3d(50, 200, -10);
	//	triangle.p2 = Imath::V3d(200, 200, -10);

	//	// Data to fill
	//	std::vector<Vertex> vertices;
	//	std::vector<uint32_t> indices;
	//	std::vector<Texture> textures;

	//	if (mesh->mMaterialIndex >= 0)
	//	{
	//		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

	//		if (textype.empty()) textype = determineTextureType(scene, mat);
	//	}

	//	// Walk through each of the mesh's vertices
	//	for (UINT i = 0; i < mesh->mNumVertices; i++)
	//	{
	//		VERTEX vertex;

	//		vertex.X = mesh->mVertices[i].x;
	//		vertex.Y = mesh->mVertices[i].y;
	//		vertex.Z = mesh->mVertices[i].z;

	//		if (mesh->mTextureCoords[0])
	//		{
	//			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
	//			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
	//		}

	//		vertices.push_back(vertex);
	//	}

	//	for (UINT i = 0; i < mesh->mNumFaces; i++)
	//	{
	//		aiFace face = mesh->mFaces[i];

	//		for (UINT j = 0; j < face.mNumIndices; j++)
	//			indices.push_back(face.mIndices[j]);
	//	}

	//	if (mesh->mMaterialIndex >= 0)
	//	{
	//		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	//		vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	//		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//	}

	//	return Mesh(dev, vertices, indices, textures);
	//}



	static bool Load(std::string path)
	{
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(path,	aiProcess_Triangulate |	aiProcess_ConvertToLeftHanded);

		if (pScene == NULL)
		{
			return false;
		}

		Triangle triangle;
		triangle.p0 = Imath::V3d(128, 50, -10);
		triangle.p1 = Imath::V3d(50, 200, -10);
		triangle.p2 = Imath::V3d(200, 200, -10);
		// 画像真ん中の後ろのほうをposとした適当なレイを作る.
		Imath::Line3d ray;
		ray.pos = Imath::V3d(1 / 2.0, 1 / 2.0, 100);
		// レイの方向.
		ray.dir = Imath::V3d(1, 0, 0) - ray.pos;

		// 三角形とレイの交差判定.
		Imath::V3d hit_point;
		Imath::V3d barycentric;
		bool is_front = false;
		if (Imath::intersect(ray, triangle.p0, triangle.p1, triangle.p2, hit_point, barycentric, is_front))
		{
		}

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

		//model.node = createNode(scene->mRootNode, scene->mMeshes);
	}

private:

};