#ifndef _CONTROLPOINT_H_
#define _CONTROLPOINT_H_

#include "defines.h"
#include <vector>

class Controlpoint {
public :
	vec3 position_;
	unsigned int VAO, VBO;
	std::vector<GLfloat> vertices;

	Controlpoint(vec3 position);
	void loadVertices(std::vector<GLfloat> & vertexData);
	void Draw();
	void Init();
	
};


#endif // !_CONTROLPOINT_H_
