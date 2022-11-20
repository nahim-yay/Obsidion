
#pragma once




#include <unordered_map>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cfloat>
#include <functional>



#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>

#include <obs/Shader.h>
#include <obs/Vertex.h>
#include <obs/Mesh.h>
#include <obs/stb_image.h>





inline void scaledAdd(aiMatrix4x4& m1, const float scale, const aiMatrix4x4& m2)
{
	m1.a1 += scale * m2.a1;
	m1.a2 += scale * m2.a2;
	m1.a3 += scale * m2.a3;
	m1.a4 += scale * m2.a4;
	m1.b1 += scale * m2.b1;
	m1.b2 += scale * m2.b2;
	m1.b3 += scale * m2.b3;
	m1.b4 += scale * m2.b4;
	m1.c1 += scale * m2.c1;
	m1.c2 += scale * m2.c2;
	m1.c3 += scale * m2.c3;
	m1.c4 += scale * m2.c4;
	m1.d1 += scale * m2.d1;
	m1.d2 += scale * m2.d2;
	m1.d3 += scale * m2.d3;
	m1.d4 += scale * m2.d4;
}

inline std::ostream &operator<<(std::ostream &os, const aiMatrix4x4 &m)
{
	os << "{Matrix ";
	os << m.a1 << ", ";
	os << m.a2 << ", ";
	os << m.a3 << ", ";
	os << m.a4 << " || ";
	os << m.b1 << ", ";
	os << m.b2 << ", ";
	os << m.b3 << ", ";
	os << m.b4 << " || ";
	os << m.c1 << ", ";
	os << m.c2 << ", ";
	os << m.c3 << ", ";
	os << m.c4 << " || ";
	os << m.d1 << ", ";
	os << m.d2 << ", ";
	os << m.d3 << ", ";
	os << m.d4;
	os << "}";

	return os;
}





namespace
{
	// Previous animation weights
	float lastAnimTime = 0.0f;
	float prev_idleWeight = 0.0f;
	float prev_rightWeight = 0.0f;
	float prev_forwardWeight = 0.0f;
	float prev_backWeight = 0.0f;
	float prev_leftWeight = 0.0f;
	
	const float animTransitionTime = 0.2f;

	aiMatrix4x4 zeroAiMat()
	{
		aiMatrix4x4 m;
		for (int i = 0; i < 4; i++)
		{
			m[i][i] = 0.0f;
		}

		return m;
	}

	void logTimeSince(const std::string& label, std::chrono::time_point<std::chrono::high_resolution_clock> start)
	{
		const auto x = std::chrono::high_resolution_clock::now();
		const auto y = std::chrono::duration_cast<std::chrono::milliseconds>(x - start).count();

		std::cout << label << y << "ms" << std::endl;
	}



	void appendNodePoints(
		float targetAnimTicks, aiAnimation *anim, aiMatrix4x4 transform,
	    aiNode *node,
	    std::unordered_map<std::string, aiMatrix4x4> *nodeTransformMap)
	{
		aiMatrix4x4 animTransform;
		bool animFound = false;
		for (int channelIndex = 0; channelIndex < anim->mNumChannels; channelIndex++)
		{
	    	const aiNodeAnim *const nodeAnim = anim->mChannels[channelIndex];
		    if (nodeAnim->mNodeName != node->mName)
				continue;
	    
	    	animFound = true;
		    for (int x = 0; x < nodeAnim->mNumRotationKeys; ++x)
		    {
				aiQuatKey k = nodeAnim->mRotationKeys[x];
	      		if (k.mTime >= targetAnimTicks)
	      		{
	        		animTransform = aiMatrix4x4(k.mValue.GetMatrix()) * animTransform;
	        		break;
			      }
		    }
	    	
	    	for (int x = 0; x < nodeAnim->mNumPositionKeys; ++x)
	    	{
	      		aiVectorKey k = nodeAnim->mPositionKeys[x];
	      		if (k.mTime >= targetAnimTicks)
	      		{
	        		if (std::abs(k.mTime - targetAnimTicks) > 2.0f) {
	        		}
	        
	        		animTransform.a4 += k.mValue.x;
			        animTransform.b4 += k.mValue.y;
			        animTransform.c4 += k.mValue.z;
			        break;
				}
		    }
		}

		transform *= animFound ? animTransform : node->mTransformation;
		const std::string name = node->mName.C_Str();
		nodeTransformMap->emplace(name, transform);
		
		for (int i = 0; i < node->mNumChildren; ++i)
		{
	    	appendNodePoints(targetAnimTicks, anim, transform, node->mChildren[i], nodeTransformMap);
		}
	}
}




class PlayerModel
{
public:

	PlayerModel(char *path)
	{
		loadModel(path);
	}

	void Draw(Shader shader, bool applyTextures = true) const
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			if (applyTextures)
			{
				shader.setInt("texture_diffuse", i);
				shader.setInt("texture_spec", i == 0 ? 19 : 20);
			}

			meshes[i].Draw(shader);
		}
	}




unsigned int GetNodeVAO() const
{
	return nodeVAO;
}







void setPlayerDead(float time)
{
	if (deathTime < 0.0f)
	{
		deathTime = time;
	}
}








void UpdatePointsForAnim(
  bool isMeasuredFrame,
  const glm::vec2 movementDir,
  const float aimTheta,
  float time)
{
	const auto start = std::chrono::high_resolution_clock::now();

	aiAnimation *const anim = scene->mAnimations[0];
	aiNode *const rootNode = scene->mRootNode;
	std::unordered_map<std::string, aiMatrix4x4> mergedNodeTransformMap;
	const auto processAnim = [&mergedNodeTransformMap, rootNode, anim, time](
		const float weight,
		const float minTicks,
		const float maxTicks,
		const float tickOffset,
		// For non-looped
		const float* opt_animStart = nullptr) {
		if (weight == 0.0f)
			return;
		
	    const float tickRange = maxTicks - minTicks;
    	float targetAnimTicks = opt_animStart
	        ? std::min((float)((time - *opt_animStart) * anim->mTicksPerSecond + tickOffset), tickRange)
	        : fmod(time * anim->mTicksPerSecond + tickOffset, tickRange);
    	targetAnimTicks += minTicks;
		if (targetAnimTicks < (minTicks - 0.01f) || targetAnimTicks > (maxTicks + 0.01f))
		{
			std::cerr << targetAnimTicks << std::endl;
			exit(1);
	    }
    
		std::unordered_map<std::string, aiMatrix4x4> localNodeTransformMap;
    	appendNodePoints(targetAnimTicks, anim, aiMatrix4x4(), rootNode, &localNodeTransformMap);
		
		for (const auto &localNodeTransform : localNodeTransformMap)
		{
      		if (mergedNodeTransformMap.find(localNodeTransform.first) == mergedNodeTransformMap.end())
      		{
        		mergedNodeTransformMap[localNodeTransform.first] = zeroAiMat();
			}
			
			// TODO +=???
			// scaledAdd(mergedNodeTransformMap[localNodeTransform.first], weight, localNodeTransformMap.second);
	      	mergedNodeTransformMap[localNodeTransform.first] = 
	      		mergedNodeTransformMap[localNodeTransform.first] + localNodeTransform.second * weight;
    	}
	};

	const bool isPlayerMoving = glm::length(movementDir) > 0.1f;
	const float movementTheta = atan(movementDir.x / movementDir.y) + (movementDir.y < 0.0f ? 3.14f : 0.0f);
	const float thetaDelta = movementTheta - aimTheta;
	const glm::vec2 movementAnim(sin(thetaDelta), cos(thetaDelta));

	const float deltaTime = time - lastAnimTime;
	lastAnimTime = time;
	prev_idleWeight = std::max(0.0f, prev_idleWeight - deltaTime / animTransitionTime);
	prev_rightWeight = std::max(0.0f, prev_rightWeight - deltaTime / animTransitionTime);
	prev_leftWeight = std::max(0.0f, prev_leftWeight - deltaTime / animTransitionTime);
	prev_forwardWeight = std::max(0.0f, prev_forwardWeight - deltaTime / animTransitionTime);
	prev_backWeight = std::max(0.0f, prev_backWeight - deltaTime / animTransitionTime);

	const bool isDead = deathTime >= 0.0f;
	float deathWeight = isDead ? 1.0f : 0.0f;
	float idleWeight = prev_idleWeight + ((isDead || isPlayerMoving) ? 0.0f : 1.0f);
	float rightWeight = prev_rightWeight + (isPlayerMoving ? std::max(0.0f, -movementAnim.x) : 0.0f);
	float forwardWeight = prev_forwardWeight + (isPlayerMoving ? std::max(0.0f, movementAnim.y) : 0.0f);
	float backWeight = prev_backWeight + (isPlayerMoving ? std::max(0.0f, -movementAnim.y) : 0.0f);
	float leftWeight = prev_leftWeight + (isPlayerMoving ? std::max(0.0f, movementAnim.x) : 0.0f);
	const float weightSum = deathWeight + idleWeight + rightWeight + forwardWeight + backWeight + leftWeight;
	deathWeight /= weightSum;
	idleWeight /= weightSum;
	rightWeight /= weightSum;
	forwardWeight /= weightSum;
	backWeight /= weightSum;
	leftWeight /= weightSum;
	prev_idleWeight = std::max(prev_idleWeight, idleWeight);
	prev_rightWeight = std::max(rightWeight, prev_rightWeight);
	prev_leftWeight = std::max(prev_leftWeight, leftWeight);
	prev_forwardWeight = std::max(prev_forwardWeight, forwardWeight);
	prev_backWeight = std::max(prev_backWeight, backWeight);
  
	if (abs(rightWeight + forwardWeight + backWeight + leftWeight + idleWeight + deathWeight - 1.0f) > 0.001f)
	{
		std::cerr << "anims did not add to 1.0f" << std::endl;
		std::cerr << "aimTheta: " << aimTheta << std::endl;
		std::cerr << "idleWeight: " << idleWeight << std::endl;
		std::cerr << "deathWeight: " << deathWeight << std::endl;
		std::cerr << "rightWeight: " << rightWeight << std::endl;
		std::cerr << "forwardWeight: " << forwardWeight << std::endl;
		std::cerr << "backWeight: " << backWeight << std::endl;
		std::cerr << "leftWeight: " << leftWeight << std::endl;

		exit(1);
	}

	processAnim(deathWeight, 234.0f, 293.0f, 0.0f, &deathTime);
	processAnim(idleWeight, 55.0f, 130.0f, 0.0f);
	const float movementAnimDur = 20.0f;
	processAnim(forwardWeight, 134.0f, 134.0f + movementAnimDur, 0.0f);
	processAnim(rightWeight, 184.0f, 184.0f + movementAnimDur, 10.0f);
	processAnim(backWeight, 159.0f, 159.0f + movementAnimDur, 10.0f);
	processAnim(leftWeight, 209.0f, 209.0f + movementAnimDur, 0.0f);

	if (isMeasuredFrame)
    	logTimeSince("  anims processed: ", start);


	gunTransform = glm::mat4(0.0f);
	const aiMatrix4x4& gunAiMat = mergedNodeTransformMap["Gun"];

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			gunTransform[j][i] = gunAiMat[i][j];
		}
	}

	int meshesProcessed = 0;
	processNode(&meshesProcessed, isMeasuredFrame, scene->mRootNode, scene, 0, mergedNodeTransformMap);
	
	if (isMeasuredFrame)
		logTimeSince("  nodes processed: ", start);
}







glm::mat4 getAnimatedGunTransform() const
{
	return gunTransform;
}

std::vector<Mesh> meshes;




private:
	
	float deathTime = -1.0f;
	glm::mat4 gunTransform;
	unsigned int nodeVAO;
	unsigned int nodeVBO;
	int numNodes;
	Assimp::Importer importer;
	const aiScene *scene;
	aiMatrix4x4 globalInv;
	
	/*  Model Data  */
	std::string directory;


	/*  Functions   */
	void loadModel(std::string path)
	{
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Failed to load scene: " << importer.GetErrorString() << std::endl;
			exit(1);
		}

		globalInv = scene->mRootNode->mTransformation;
		globalInv = globalInv.Inverse();
		directory = path.substr(0, path.find_last_of('/'));
		
		// Make node vao
		glGenVertexArrays(1, &nodeVAO);
		glGenBuffers(1, &nodeVBO);
	}







	void processNode(
		int* meshesProcessed,
		bool isMeasuredFrame,
		aiNode *node,
		const aiScene *scene,
		int depth,
		std::unordered_map<std::string,
		aiMatrix4x4> &transformMap)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			const auto start = std::chrono::high_resolution_clock::now();
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
	
			if (meshes.size() > *meshesProcessed)
		  		meshes[*meshesProcessed].updateVertices(getMeshVertices(isMeasuredFrame, mesh, scene, transformMap));
			else
				meshes.push_back(std::move(processMesh(isMeasuredFrame, mesh, scene, transformMap)));

			(*meshesProcessed)++;

			if (isMeasuredFrame)
				logTimeSince("    mesh processed in: ", start);
		}
	
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			aiNode* childNode = node->mChildren[i];
			processNode(meshesProcessed, isMeasuredFrame, childNode, scene, depth + 1, transformMap);
		}
	}
	



	Mesh processMesh(
		bool isMeasuredFrame,
		aiMesh *mesh,
		const aiScene *scene,
		std::unordered_map<std::string, aiMatrix4x4> &nodeTransformMap)
	{
		const auto start = std::chrono::high_resolution_clock::now();
		const std::vector<Vertex> vertices = getMeshVertices(isMeasuredFrame, mesh, scene, nodeTransformMap);

		std::vector<unsigned int> indices;

		// process indices
		indices.reserve(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
		  		const unsigned int vertexIndex = face.mIndices[j];
		  		if (vertexIndex >= vertices.size())
		  		{
		    		std::cerr << "Unexpected element index, " << vertexIndex << " vs vertex count: " << vertices.size() << std::endl;
				    exit(1);
				}
		  		indices.push_back(vertexIndex);
			}
		}

		if (isMeasuredFrame)
			logTimeSince("      indices processed: ", start);
		
		// Player textures handled externally
		std::vector<Texture> textures;

		return std::move(Mesh(std::move(vertices), std::move(indices), std::move(textures)));
	}






	std::vector<Vertex> getMeshVertices(
		const bool isMeasuredFrame,
		aiMesh *mesh,
		const aiScene *scene,
		std::unordered_map<std::string, aiMatrix4x4> &nodeTransformMap)
	{
		const auto start = std::chrono::high_resolution_clock::now();
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);

		const aiAnimation *const anim = scene->mAnimations[0];
		const float targetAnimTicks = 2.0f * anim->mTicksPerSecond;

		std::vector<aiMatrix4x4> boneAnimTransform(mesh->mNumVertices, zeroAiMat());

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
		{
			aiBone *bone = mesh->mBones[boneIndex];
			const std::string boneName = bone->mName.C_Str();
			const aiMatrix4x4 nodeTransform = globalInv * nodeTransformMap[boneName] * bone->mOffsetMatrix;

			for (int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++)
			{
				aiVertexWeight w = bone->mWeights[weightIndex];
		  		scaledAdd(boneAnimTransform[w.mVertexId], w.mWeight, nodeTransform);
			}
		}
	
		if (isMeasuredFrame)
			logTimeSince("      bones processed: ", start);
		
		aiMatrix4x4 nodeAnimTransform;
		if (mesh->mNumBones == 0)
		{
			std::vector<aiNode *> meshNodes;
			std::function<void(aiNode *)> r;
			r = [mesh, scene, &meshNodes, &r](aiNode *n)
			{
		  		for (int xx = 0; xx < n->mNumMeshes; xx++)
		  		{
		    		aiMesh *tmpMesh = scene->mMeshes[n->mMeshes[xx]];

		    		if (tmpMesh == mesh)
		    		{
			      		meshNodes.push_back(n);
				    }
				}
			
				for (int xx = 0; xx < n->mNumChildren; xx++)
				{
			    	r(n->mChildren[xx]);
				}
			};
	
			r(scene->mRootNode);
			for (auto *meshNode : meshNodes)
			{
			  nodeAnimTransform *= nodeTransformMap[std::string(meshNode->mName.data)];
			}
		}
	
		if (isMeasuredFrame)
			logTimeSince("      boneless nodes processed: ", start);
	
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
		
			{ // Position
				aiVector3D v = (mesh->mNumBones > 0 ? boneAnimTransform[i] : nodeAnimTransform) * mesh->mVertices[i];

				vertex.position.x = v.x;
				vertex.position.y = v.y;
				vertex.position.z = v.z;
			}


			{ // Normal
				// TODO apply animation
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}


			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vec;
			}
			else
			{
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
			
			vertices.push_back(vertex);
		}

		if (isMeasuredFrame)
			logTimeSince("      vertices processed: ", start);

		return vertices;
	}
};








