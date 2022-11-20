






struct mat4
{
	float m[4][4];
};

struct vec4
{
	float f[4];
};


// glTF node
struct Node;


typedef struct
{
	GLuint vb;
} GLBufferState;



typedef struct
{
	std::vector<GLuint> diffuseTex;  // for each primitive in mesh
} GLMeshState;


typedef struct
{
	std::map<std::string, GLint> attribs;
	std::map<std::string, GLint> uniforms;
} GLProgramState;


typedef struct
{
	GLuint vb;     // vertex buffer
	size_t count;  // byte count
} GLCurvesState;


// Stores vertex position transformed by skinning
typedef struct
{
	std::vector<float> positions;  // float4
} SkinnedMesh;


typedef struct
{
	std::vector<glm::mat4> inverseBindMatrices;  // mat44
} SkinningMatrices;


struct AnimationChannel
{
	enum PathType { TRANSLATION, ROTATION, SCALE };
	PathType path;
	// Node *node;
	uint32_t samplerIndex;
};


struct AnimationSampler
{
	enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
	InterpolationType interpolation;
	std::vector<float> inputs;
	std::vector<glm::vec4> outputsVec4;
};


struct Animation
{
	std::string name;
	std::vector<AnimationSampler> samplers;
	std::vector<AnimationChannel> channels;
	float start = std::numeric_limits<float>::max();
	float end = std::numeric_limits<float>::min();
};