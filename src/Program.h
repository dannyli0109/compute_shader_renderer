#pragma once
#include "Graphics.h"
#include "GUI.h"
#include "Utilities.h"
#include <vector>
#include <iostream>
#include <unordered_map>
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
	void LoadTexture(std::string name, std::string path);
	void UpdateTexture();
	void InitFrameBuffer(int width, int height);
	void InitModes();
	void SetPixel(int x, int y, glm::vec4 color);
	void Clear();
	GLFWwindow* window;
	GLuint shaderProgram;

	GLuint clearProgram;
	GLuint clearShader;

	GLuint rendererProgram;
	GLuint rendererShader;
	GLuint quadBuffer;
	GLuint frameBufferTexture;
	GLuint diffuseTexture;
	//GLuint depthBufferTexture;
	GLuint ssbo;
	GLuint depthBuffer;
	std::vector<glm::vec4> frameBuffer;
	std::vector<Depth> depthBufferV;
	int w, h;
	glm::vec4 clearColor{ 0, 0, 0, 1 };
	std::vector<Model*> models;
	GLuint vertexBuffer;
	GLuint indexBuffer;
	std::unordered_map<std::string, GLuint> textures;
	std::vector<Light> lights;
};