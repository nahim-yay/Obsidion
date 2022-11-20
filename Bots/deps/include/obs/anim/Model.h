

#pragma once





struct Primitive
{
	uint32_t firstIndex;
	uint32_t indexCount;
	int32_t  materialIndex;
};

struct Mesh
{
	std::vector<Primitive> primitives;
};




struct Material
{
	glm::vec4 baseColorFactor = glm::vec4(1.0f);
	uint32_t  baseColorTextureIndex;
};

struct Image
{
	vks::Texture2D  texture;
	VkDescriptorSet descriptorSet;
};

struct Texture
{
	int32_t imageIndex;
};



struct Vertex
{
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 color;
  glm::vec4 jointIndices;
  glm::vec4 jointWeights;
};


struct Node
{
  Node *              parent;
  uint32_t            index;
  std::vector<Node *> children;
  Mesh                mesh;
  glm::vec3           translation{};
  glm::vec3           scale{1.0f};
  glm::quat           rotation{};
  int32_t             skin = -1;
  glm::mat4           matrix;
  glm::mat4           getLocalMatrix();
};


struct Skin
{
  std::string            name;
  Node *                 skeletonRoot = nullptr;
  std::vector<glm::mat4> inverseBindMatrices;
  std::vector<Node *>    joints;
  // vks::Buffer            ssbo;
  // VkDescriptorSet        descriptorSet;
};



struct AnimationSampler
{
  std::string            interpolation;
  std::vector<float>     inputs;
  std::vector<glm::vec4> outputsVec4;
};


struct AnimationChannel
{
  std::string path;
  Node *      node;
  uint32_t    samplerIndex;
};


struct Animation
{
  std::string                   name;
  std::vector<AnimationSampler> samplers;
  std::vector<AnimationChannel> channels;
  float                         start       = std::numeric_limits<float>::max();
  float                         end         = std::numeric_limits<float>::min();
  float                         currentTime = 0.0f;
};



class Model
{
private:


	std::vector<Animation> animations;
	std::vector<Image>     images;
	std::vector<Texture>   textures;
	std::vector<Material>  materials;
	std::vector<Node *>    nodes;
	std::vector<Skin>      skins;
	std::vector<Animation> animations;

	uint32_t activeAnimation = 0;


public:



void loadSkins(tinygltf::Model &input)
{
	skins.resize(input.skins.size());

	for (size_t i = 0; i < input.skins.size(); i++)
	{
		tinygltf::Skin glTFSkin = input.skins[i];

		skins[i].name = glTFSkin.name;
		// Find the root node of the skeleton
		skins[i].skeletonRoot = nodeFromIndex(glTFSkin.skeleton);

		// Find joint nodes
		for (int jointIndex : glTFSkin.joints)
		{
			Node *node = nodeFromIndex(jointIndex);
			if (node)
			{
				skins[i].joints.push_back(node);
			}
		}

		// Get the inverse bind matrices from the buffer associated to this skin
		if (glTFSkin.inverseBindMatrices > -1)
		{
			const tinygltf::Accessor &  accessor   = input.accessors[glTFSkin.inverseBindMatrices];
			const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &    buffer     = input.buffers[bufferView.buffer];
			skins[i].inverseBindMatrices.resize(accessor.count);
			memcpy(skins[i].inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));



			// Store inverse bind matrices for this skin in a shader storage buffer object
			// To keep this sample simple, we create a host visible shader storage buffer

			// VK_CHECK_RESULT(vulkanDevice->createBuffer(
			//     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			//     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			//     &skins[i].ssbo,
			//     sizeof(glm::mat4) * skins[i].inverseBindMatrices.size(),
			//     skins[i].inverseBindMatrices.data()));
			// VK_CHECK_RESULT(skins[i].ssbo.map());
		}
	}
}






void loadAnimations(tinygltf::Model &input)
{
  animations.resize(input.animations.size());

    for (size_t i = 0; i < input.animations.size(); i++)
    {
        tinygltf::Animation glTFAnimation = input.animations[i];
        animations[i].name                = glTFAnimation.name;

        // Samplers
        animations[i].samplers.resize(glTFAnimation.samplers.size());
        for (size_t j = 0; j < glTFAnimation.samplers.size(); j++)
        {
            tinygltf::AnimationSampler glTFSampler = glTFAnimation.samplers[j];
            AnimationSampler &         dstSampler  = animations[i].samplers[j];
            dstSampler.interpolation               = glTFSampler.interpolation;

            // Read sampler keyframe input time values
            {
                const tinygltf::Accessor &  accessor   = input.accessors[glTFSampler.input];
                const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &    buffer     = input.buffers[bufferView.buffer];
                const void *                dataPtr    = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                const float *               buf        = static_cast<const float *>(dataPtr);
                for (size_t index = 0; index < accessor.count; index++)
                {
                    dstSampler.inputs.push_back(buf[index]);
                }
                // Adjust animation's start and end times
                for (auto input : animations[i].samplers[j].inputs)
                {
                    if (input < animations[i].start)
                    {
                        animations[i].start = input;
                    };
                    if (input > animations[i].end)
                    {
                        animations[i].end = input;
                    }
                }
            }

            // Read sampler keyframe output translate/rotate/scale values
            {
                const tinygltf::Accessor &  accessor   = input.accessors[glTFSampler.output];
                const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &    buffer     = input.buffers[bufferView.buffer];
                const void *                dataPtr    = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                switch (accessor.type)
                {
                    case TINYGLTF_TYPE_VEC3:
                    {
                        const glm::vec3 *buf = static_cast<const glm::vec3 *>(dataPtr);
                        for (size_t index = 0; index < accessor.count; index++)
                        {
                            dstSampler.outputsVec4.push_back(glm::vec4(buf[index], 0.0f));
                        }
                        break;
                    }
                    case TINYGLTF_TYPE_VEC4:
                    {
                        const glm::vec4 *buf = static_cast<const glm::vec4 *>(dataPtr);
                        for (size_t index = 0; index < accessor.count; index++)
                        {
                            dstSampler.outputsVec4.push_back(buf[index]);
                        }
                        break;
                    }
                    default:
                    {
                        std::cout << "unknown type" << std::endl;
                        break;
                    }
                }
            }
        }

        // Channels
        animations[i].channels.resize(glTFAnimation.channels.size());
        for (size_t j = 0; j < glTFAnimation.channels.size(); j++)
        {
            tinygltf::AnimationChannel glTFChannel = glTFAnimation.channels[j];
            AnimationChannel &         dstChannel  = animations[i].channels[j];
            dstChannel.path                        = glTFChannel.target_path;
            dstChannel.samplerIndex                = glTFChannel.sampler;
            dstChannel.node                        = nodeFromIndex(glTFChannel.target_node);
        }
    }
}






Node *nodeFromIndex(uint32_t index)
{
	Node *nodeFound = nullptr;
	for (auto &node : nodes)
	{
		nodeFound = findNode(node, index);
		if (nodeFound)
		{
			break;
		}
	}
	return nodeFound;
}




Node *findNode(Node *parent, uint32_t index)
{
	Node *nodeFound = nullptr;
	if (parent->index == index)
	{
		return parent;
	}
	for (auto &child : parent->children)
	{
		nodeFound = findNode(child, index);
		if (nodeFound)
		{
			break;
		}
	}
	return nodeFound;
}


























}