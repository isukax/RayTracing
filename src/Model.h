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
	Vector3 normal;
	Imath::V2d texcoord;
};

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
			list.Add(std::make_shared<Triangle>(
				vertices[indices[i]].position, 
				vertices[indices[i + 1]].position, 
				vertices[indices[i + 2]].position, 
				vertices[indices[i]].texcoord,
				vertices[indices[i + 1]].texcoord,
				vertices[indices[i + 2]].texcoord,
				vertices[indices[i]].normal,
				vertices[indices[i + 1]].normal,
				vertices[indices[i + 2]].normal,
				material
				)
			);
			//list.Add(std::make_shared<Triangle>(vertices[indices[i]].position, vertices[indices[i + 1]].position, vertices[indices[i + 2]].position, material));
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

	void ProcessNode(aiNode * node, const aiScene * scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.emplace_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;

			if (mesh->mTextureCoords[0])
			{
				vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
				vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
			}

			if (mesh->HasNormals())
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// 法線算出
		if(!mesh->HasNormals())
		{
			// 面法線、面積の計算
			std::vector<Vector3> faceNormals;
			for (uint32_t i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];

				Vector3 p0 = vertices[face.mIndices[0]].position;
				Vector3 p1 = vertices[face.mIndices[1]].position;
				Vector3 p2 = vertices[face.mIndices[2]].position;

				Vector3 e1 = p1 - p0;
				Vector3 e2 = p2 - p0;

				Vector3 normal = Normalize(Cross(e1, e2));
				double area = 0.5 * Cross(e1, e2).Length();
				faceNormals.push_back(area * normal);
			}

			// 頂点を含む面を検索、normalの総和をとって正規化
			std::vector<uint32_t>* vertexFace = new std::vector<uint32_t>[vertices.size()]; // 各頂点について、その頂点を含む三角形のインデックスを格納
			for (int i = 0; i < vertices.size(); ++i)
			{
				// 頂点が含まれる面のインデックスを格納
				for (uint32_t j = 0; j < mesh->mNumFaces; ++j)
				{
					auto face = mesh->mFaces[j];
					double v0 = (vertices[i].position - vertices[face.mIndices[0]].position).LengthSquared();
					double v1 = (vertices[i].position - vertices[face.mIndices[1]].position).LengthSquared();
					double v2 = (vertices[i].position - vertices[face.mIndices[2]].position).LengthSquared();
					if (v0 < kEPS || v1 < kEPS || v2 < kEPS)
					{
						vertexFace[i].push_back(j);
					}
				}
				for (int j = 0; j < vertexFace[i].size(); ++j)
				{
					vertices[i].normal += faceNormals[vertexFace[i][j]];
				}
				vertices[i].normal = Normalize(vertices[i].normal);
			}
			delete[] vertexFace;
		}

		// マテリアル
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			aiTextureType type = aiTextureType_DIFFUSE;
			for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				std::string path = std::string(MODEL_DIR) + str.C_Str();
				ImageTexturePtr texPtr = std::make_shared<ImageTexture>(path);

				// とりあえず上書き
				// 方法はあとで考える
				material = std::make_shared<LambertMaterial>(texPtr);
			}
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
		ProcessNode(pScene->mRootNode, pScene);

		return true;
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