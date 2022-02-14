#include <mesh.h>
#include <utils.h>
#include <fstream>
#include <sstream>
#include <vector>

std::vector<std::string>
StringSplit(std::string str, const char spliter)
{
  std::vector<std::string> buf;
  std::istringstream iss(str);
  std::string token;
  while (getline(iss, token, spliter))
  {
    if (token != "")
      buf.push_back(token);
  }
  return buf;
}

Mesh::Mesh()
{

}


Mesh::Mesh(const std::string &path) { loadDataFromFile(getPath(path)); }

/**
 * TODO: implement load mesh data from file here
 * you need to open file with [path]
 *
 * File format
 * First line contains 3 integers: vertex count, normal count, face count
 *
 * For the next [vertex count] lines, each line contains 3 floats, which
 * represents the position of a vertex
 *
 * For the next [normal count] lines, each line contains 3 floats, which
 * represents the normal of a vertex
 *
 * For the next [face count] lines, each line contains 6 integers
 * [v0, n0, v1, n1, v2, n2], which represents the triangle face
 * v0, n0 means the vertex index and normal index of the first vertex
 * v1, n1 means the vertex index and normal index of the second vertex
 * v2, n2 means the vertex index and normal index of the third vertex
 */
void Mesh::loadDataFromFile(const std::string &path) {
  
  std::ifstream fin(path.c_str());
  std::vector<std::string> lines;
  std::string readline;
  ///load vertex count, normal count and face count
  getline(fin, readline);
  std::stringstream ss(readline);
  int data;
  ss >> data;
  int vertexCount = data;
  ss >> data;
  int normalCount = data;
  ss >> data;
  int faceCount = data;

  for (int i = 0; i < vertexCount; i++)
  {
    getline(fin,readline);
    if (readline == "")
    {
      i--;
      continue;
    }
    std::vector<float> vertexData;
    float fdata;
    ss = std::stringstream(readline);
    while(ss >> fdata)
    {
      vertexData.push_back(fdata);
    }
    Vertex temp;
    temp.position = vec3(vertexData[0], vertexData[1], vertexData[2]);
    vertices.push_back(temp);
  }

  std::vector<vec3> normals;
  for (int i = 0; i < normalCount; i++)
  {
    getline(fin,readline);
    if (readline == "")
    {
      i--;
      continue;
    }
    std::vector<float> normalData;
    float fdata;
    ss = std::stringstream(readline);
    while(ss >> fdata)
    {
      normalData.push_back(fdata);
    }
    normals.push_back(vec3(normalData[0], normalData[1], normalData[2]));
  }

  for (int i = 0; i < faceCount; i++)
  {
    getline(fin,readline);
    if (readline == "")
    {
      i--;
      continue;
    }
    std::vector<int> intData;
    int idata;    
    ss = std::stringstream(readline);
    
    while(ss >> idata)
    {
      intData.push_back(idata);
    }
    for (int j = 0; j < 6; j += 2)
    {
      int vertex = intData[j];
      indices.push_back(vertex);
      vertices[vertex].normal = normals[intData[j+1]];
    }
  }

/*
  while(getline(fin,readline))
  {
    if (readline != "")
      lines.push_back(readline);
  }
  std::vector<std::string> stringData = StringSplit(lines[0],' ');
  

  //.load vertex count, normal count and face count
  int vertexCount = atoi(stringData[0].c_str());
  int normalCount = atoi(stringData[1].c_str());
  int faceCount = atoi(stringData[2].c_str());

  //load vertex position
  for (int i = 1; i < vertexCount + 1;i++)
  {
    stringData = StringSplit(lines[i],' ');
    Vertex temp;
    temp.position = vec3(atof(stringData[0].c_str()), atof(stringData[1].c_str()), atof(stringData[2].c_str()));
    vertices.push_back(temp);
  }
  std::vector<vec3> normals;
  for (int i = vertexCount + 1; i < normalCount + vertexCount + 1; i++)
  {
    stringData = StringSplit(lines[i],' ');
    normals.push_back(vec3(atof(stringData[0].c_str()), atof(stringData[1].c_str()), atof(stringData[2].c_str())));
  }
  for (int i = normalCount + vertexCount + 1; i < faceCount + normalCount + vertexCount + 1; i++)
  {
    stringData = StringSplit(lines[i],' ');
    for(int j = 0; j < 6; j += 2)
    {
      int vertex = atoi(stringData[j].c_str());
      indices.push_back(vertex);
      vertices[vertex].normal = normals[atoi(stringData[j+1].c_str())];
    }
  }
  */
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION

}


void 
Mesh::InitMesh()
{
  ///VAO VBO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  GLuint element_buffer_object;//EBO
  glGenBuffers(1, &element_buffer_object);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);
  ///position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  ///normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glBindVertexArray(0);
}

/**
 * TODO: implement your draw code here
 */
void Mesh::draw() const {
  
  ///Bind VAO
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
}

void
Mesh::Terminate()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}