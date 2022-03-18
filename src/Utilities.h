#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
//#include "Program.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
struct Mesh;
struct Model;

std::string LoadFileAsString(std::string filename);
void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes);
Model* LoadModel(std::string filename);
