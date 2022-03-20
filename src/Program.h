#pragma once
#include "Graphics.h"
#include "GUI.h"
#include "Utilities.h"
#include <vector>
#include <iostream>
#include <unordered_map>


enum class Shaders
{
	Vertex,
	Fragment,
	Renderer,
	Clear
};

enum class Programs
{
	ShaderProgram,
	Renderer,
	Clear
};

enum class Textures
{
	ScreenTexture,
	DiffuseTexture,
	NormalTexture,
	SpecularTexture,
	FrameBufferTexture
};

enum class Buffers
{
	Quad,
	Vertex,
	Index,
	Depth
};

struct Vertex
{
	glm::vec4 position;
	glm::vec4 color;
	glm::vec2 uv{ 0, 0 }; float padding2[2];
	glm::vec3 normal{ 0, 0, 0 }; float padding3;
	glm::vec3 vertTangent{ 0, 0, 0 }; float padding4;
	glm::vec3 vertBitangent{ 0, 0, 0 }; float padding5;
	Vertex() = default;
	Vertex(glm::vec4 pos, glm::vec4 col) : position(pos), color(col) {};
};

struct Index
{
	Index() = default;
	Index(int data) : data(data), padding0() {};
	int data; float padding0[3];
};

struct Depth
{
	Depth() = default;
	Depth(double data) : data(data), padding0() {};
	double data; float padding0[2];
};

struct Light
{
	glm::vec3 position;
	glm::vec3 intensity;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<Index> indices;
};

struct Model
{
	std::vector<Mesh> meshes;
};

class Program
{
public:
public:
	int Init();
	void Update();
	void End();
	void Draw();
private:
	void InitGUI();
	void UpdateGUI();
	void EndGUI();
	void InitShader();
	void InitComputeShader();
	void InitQuad();
	void InitSSBO();
	void InitTexture();
	void LoadTexture(Textures texture, std::string path);
	void UpdateTexture();
	void InitFrameBuffer(int width, int height);
	void InitModes();
	void SetPixel(int x, int y, glm::vec4 color);
	void Clear();
	void Render();
	GLFWwindow* window;
	/*GLuint shaderProgram;

	GLuint clearProgram;
	GLuint clearShader;

	GLuint rendererProgram;
	GLuint rendererShader;
	GLuint quadBuffer;
	GLuint frameBufferTexture;
	GLuint diffuseTexture;

	GLuint ssbo;
	GLuint depthBuffer;

	GLuint vertexBuffer;
	GLuint indexBuffer;*/
	std::vector<glm::vec4> frameBuffer;
	std::vector<Depth> depthBufferV;
	int w, h;
	int angles = 0;
	glm::vec3 transform = glm::vec3(0.0f, 5.0f, 30.0f);
	glm::vec4 clearColor{ 0, 0, 0, 1 };
	std::vector<Model*> models;
	std::unordered_map<Textures, GLuint> textures;
	std::unordered_map<Programs, GLuint> programs;
	std::unordered_map<Shaders, GLuint> shaders;
	std::unordered_map<Buffers, GLuint> buffers;
	std::vector<Light> lights;
};