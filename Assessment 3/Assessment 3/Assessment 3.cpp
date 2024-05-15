#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"
#include "shadow.h"
#include "texture.h"
#include "obj.h"
#include "modelcontainer.h"
#include "fixedcamera.h"

// light direction variable here
glm::vec3 lightDirection = glm::vec3(.3f, -0.3f, -0.3f);
glm::vec3 lightPos = glm::vec3(-25, 15, 20.);
SCamera Camera;
FixedCamera fixedCamera;
bool useFixedCamera = false; 


#define NUM_VBOs 5
#define NUM_VAOS 5
GLuint VBOs[NUM_VBOs];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1024
#define HEIGHT 768
#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048

void SizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}



/*------------------------------------------Vertex Data----------------------------------------------------*/

float rectangleVertices[] = {
	// back face
	// pos                     // uv          // normal
	-0.5f, -0.5f, -0.5f,      0.0f, 0.0f,  0.f, 0.f, -1.f,
	 0.5f, -0.5f, -0.5f,      1.0f, 0.0f,  0.f, 0.f, -1.f,
	 0.5f,  0.5f, -0.5f,      1.0f, 1.0f,  0.f, 0.f, -1.f,
	 0.5f,  0.5f, -0.5f,      1.0f, 1.0f,  0.f, 0.f, -1.f,
	-0.5f,  0.5f, -0.5f,      0.0f, 1.0f,  0.f, 0.f, -1.f,
	-0.5f, -0.5f, -0.5f,      0.0f, 0.0f,  0.f, 0.f, -1.f,

	// front face
	-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,  0.f, 0.f, 1.f,
	 0.5f, -0.5f,  0.5f,      1.0f, 0.0f,  0.f, 0.f, 1.f,
	 0.5f,  0.5f,  0.5f,      1.0f, 1.0f,  0.f, 0.f, 1.f,
	 0.5f,  0.5f,  0.5f,      1.0f, 1.0f,  0.f, 0.f, 1.f,
	-0.5f,  0.5f,  0.5f,      0.0f, 1.0f,  0.f, 0.f, 1.f,
	-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,  0.f, 0.f, 1.f,

	// left face
	-0.5f,  0.5f,  0.5f,      0.0f, 1.0f,  -1.f, 0.f, 0.f,
	-0.5f,  0.5f, -0.5f,      1.0f, 1.0f,  -1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,      1.0f, 0.0f,  -1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,      1.0f, 0.0f,  -1.f, 0.f, 0.f,
	-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,  -1.f, 0.f, 0.f,
	-0.5f,  0.5f,  0.5f,      0.0f, 1.0f,  -1.f, 0.f, 0.f,

	// right face
	 0.5f,  0.5f,  0.5f,      0.0f, 1.0f,   1.f, 0.f, 0.f,
	 0.5f,  0.5f, -0.5f,      1.0f, 1.0f,   1.f, 0.f, 0.f,
	 0.5f, -0.5f, -0.5f,      1.0f, 0.0f,   1.f, 0.f, 0.f,
	 0.5f, -0.5f, -0.5f,      1.0f, 0.0f,   1.f, 0.f, 0.f,
	 0.5f, -0.5f,  0.5f,      0.0f, 0.0f,   1.f, 0.f, 0.f,
	 0.5f,  0.5f,  0.5f,      0.0f, 1.0f,   1.f, 0.f, 0.f,

	 // bottom face
	 -0.5f, -0.5f, -0.5f,      0.0f, 1.0f,   0.f, -1.f, 0.f,
	  0.5f, -0.5f, -0.5f,      1.0f, 1.0f,   0.f, -1.f, 0.f,
	  0.5f, -0.5f,  0.5f,      1.0f, 0.0f,   0.f, -1.f, 0.f,
	  0.5f, -0.5f,  0.5f,      1.0f, 0.0f,   0.f, -1.f, 0.f,
	 -0.5f, -0.5f,  0.5f,      0.0f, 0.0f,   0.f, -1.f, 0.f,
	 -0.5f, -0.5f, -0.5f,      0.0f, 1.0f,   0.f, -1.f, 0.f,

	 // top face
	 -0.5f,  0.5f, -0.5f,      0.0f, 0.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f, -0.5f,      1.0f, 0.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f,  0.5f,      1.0f, 1.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f,  0.5f,      1.0f, 1.0f,   0.f, 1.f, 0.f,
	 -0.5f,  0.5f,  0.5f,      0.0f, 1.0f,   0.f, 1.f, 0.f,
	 -0.5f,  0.5f, -0.5f,      0.0f, 0.0f,   0.f, 1.f, 0.f
};

float floorVertices[] = {
	// pos                     // uv          // normal
	 -0.5f,  0.5f, -0.5f,      0.0f, 0.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f, -0.5f,      10.0f, 0.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f,  0.5f,      10.0f, 10.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f,  0.5f,      10.0f, 10.0f,   0.f, 1.f, 0.f,
	 -0.5f,  0.5f,  0.5f,      0.0f, 10.0f,   0.f, 1.f, 0.f,
	 -0.5f,  0.5f, -0.5f,      0.0f, 0.0f,   0.f, 1.f, 0.f
};

float glassVertices[] = {
	// back face
	-0.5f, -0.5f, -0.5f,      
	 0.5f, -0.5f, -0.5f,      
	 0.5f,  0.5f, -0.5f,      
	 0.5f,  0.5f, -0.5f,     
	-0.5f,  0.5f, -0.5f,      
	-0.5f, -0.5f, -0.5f,      

	// front face
	-0.5f, -0.5f,  0.5f,      
	 0.5f, -0.5f,  0.5f,     
	 0.5f,  0.5f,  0.5f,      
	 0.5f,  0.5f,  0.5f,     
	-0.5f,  0.5f,  0.5f,      
	-0.5f, -0.5f,  0.5f,      

	// left face
	-0.5f,  0.5f,  0.5f,      
	-0.5f,  0.5f, -0.5f,      
	-0.5f, -0.5f, -0.5f,      
	-0.5f, -0.5f, -0.5f,     
	-0.5f, -0.5f,  0.5f,     
	-0.5f,  0.5f,  0.5f,      

	// right face
	 0.5f,  0.5f,  0.5f,     
	 0.5f,  0.5f, -0.5f,      
	 0.5f, -0.5f, -0.5f,      
	 0.5f, -0.5f, -0.5f,      
	 0.5f, -0.5f,  0.5f,      
	 0.5f,  0.5f,  0.5f,     

	 // bottom face
	 -0.5f, -0.5f, -0.5f,     
	  0.5f, -0.5f, -0.5f,     
	  0.5f, -0.5f,  0.5f,      
	  0.5f, -0.5f,  0.5f,     
	 -0.5f, -0.5f,  0.5f,      
	 -0.5f, -0.5f, -0.5f,      

	 // top face
	 -0.5f,  0.5f, -0.5f,      
	  0.5f,  0.5f, -0.5f,      
	  0.5f,  0.5f,  0.5f,      
	  0.5f,  0.5f,  0.5f,     
	 -0.5f,  0.5f,  0.5f,      
	 -0.5f,  0.5f, -0.5f,      
};

float houseVertices[] = {
	// back face
	// pos                     // uv          // normal
	-0.5f, -0.5f, -0.5f,      0.0f, 0.0f,  0.f, 0.f, -1.f,
	 0.5f, -0.5f, -0.5f,      0.0f, 2.0f,  0.f, 0.f, -1.f,
	 0.5f,  0.5f, -0.5f,      2.0f, 2.0f,  0.f, 0.f, -1.f,
	 0.5f,  0.5f, -0.5f,      2.0f, 2.0f,  0.f, 0.f, -1.f,
	-0.5f,  0.5f, -0.5f,      2.0f, 0.0f,  0.f, 0.f, -1.f,
	-0.5f, -0.5f, -0.5f,      0.0f, 0.0f,  0.f, 0.f, -1.f,

	// front face
	-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,  0.f, 0.f, 1.f,
	 0.5f, -0.5f,  0.5f,      0.0f, 2.0f,  0.f, 0.f, 1.f,
	 0.5f,  0.5f,  0.5f,      2.0f, 2.0f,  0.f, 0.f, 1.f,
	 0.5f,  0.5f,  0.5f,      2.0f, 2.0f,  0.f, 0.f, 1.f,
	-0.5f,  0.5f,  0.5f,      2.0f, 0.0f,  0.f, 0.f, 1.f,
	-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,  0.f, 0.f, 1.f,

	// left face
	-0.5f,  0.5f,  0.5f,      0.0f, 2.0f,  -1.f, 0.f, 0.f,
	-0.5f,  0.5f, -0.5f,      0.0f, 0.0f,  -1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,      2.0f, 0.0f,  -1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,      2.0f, 0.0f,  -1.f, 0.f, 0.f,
	-0.5f, -0.5f,  0.5f,      2.0f, 2.0f,  -1.f, 0.f, 0.f,
	-0.5f,  0.5f,  0.5f,      0.0f, 2.0f,  -1.f, 0.f, 0.f,

	// right face
	 0.5f,  0.5f,  0.5f,      0.0f, 2.0f,   1.f, 0.f, 0.f,
	 0.5f,  0.5f, -0.5f,      0.0f, 0.0f,   1.f, 0.f, 0.f,
	 0.5f, -0.5f, -0.5f,      2.0f, 0.0f,   1.f, 0.f, 0.f,
	 0.5f, -0.5f, -0.5f,      2.0f, 0.0f,   1.f, 0.f, 0.f,
	 0.5f, -0.5f,  0.5f,      2.0f, 2.0f,   1.f, 0.f, 0.f,
	 0.5f,  0.5f,  0.5f,      0.0f, 2.0f,   1.f, 0.f, 0.f,

	 // bottom face
	 -0.5f, -0.5f, -0.5f,      0.0f, 2.0f,   0.f, -1.f, 0.f,
	  0.5f, -0.5f, -0.5f,      0.0f, 0.0f,   0.f, -1.f, 0.f,
	  0.5f, -0.5f,  0.5f,      2.0f, 0.0f,   0.f, -1.f, 0.f,
	  0.5f, -0.5f,  0.5f,      2.0f, 0.0f,   0.f, -1.f, 0.f,
	 -0.5f, -0.5f,  0.5f,      2.0f, 2.0f,   0.f, -1.f, 0.f,
	 -0.5f, -0.5f, -0.5f,      0.0f, 2.0f,   0.f, -1.f, 0.f,

	 // top face
	 -0.5f,  0.5f, -0.5f,      0.0f, 2.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f, -0.5f,      0.0f, 0.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f,  0.5f,      2.0f, 0.0f,   0.f, 1.f, 0.f,
	  0.5f,  0.5f,  0.5f,      2.0f, 0.0f,   0.f, 1.f, 0.f,
	 -0.5f,  0.5f,  0.5f,      2.0f, 2.0f,   0.f, 1.f, 0.f,
	 -0.5f,  0.5f, -0.5f,      0.0f, 2.0f,   0.f, 1.f, 0.f
};

/*------------------------------------------Draw Function---------------------------------------------------*/
void drawFloor(unsigned int shaderProgram, const std::vector<GLuint>& textures)
{
	glBindVertexArray(VAOs[0]);

	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0, -3, -10));
	model = glm::scale(model, glm::vec3(50, 0.1, 60));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 6); // only need 1 face

	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawBox(unsigned int shaderProgram, const std::vector<GLuint>& textures)
{
	glBindVertexArray(VAOs[1]); 

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0); 
	 
	// store the translations
	std::vector<glm::vec3> translations = { 
		glm::vec3(-1, -0.45, -2.6),
		glm::vec3(-1, -2.45, -4), 
		glm::vec3(-2, -2.45, -3.7), 
		glm::vec3(-3, -2.45, -3.7), 
		glm::vec3(0, -2.45, -4),
		glm::vec3(-3, -2.45, -2.7),
		glm::vec3(-2, -2.45, -2.7),
		glm::vec3(-0.5, -1.95, -3.25),
		glm::vec3(-0.05, -2.45, -2.5),
		glm::vec3(-1, -2.45, -2),
		glm::vec3(0, -1.45, -2.45),
		glm::vec3(-1, -1.45, -2.5),
		glm::vec3(-2, -1.45, -3.6),
		glm::vec3(-3, -1.45, -3.6),
		glm::vec3(-2, -1.45, -2.6),
		glm::vec3(-3, -1.45, -2.6),
		glm::vec3(-3, -2.6, -4.55),
		glm::vec3(2.8, -1.85, -10.15),
		glm::vec3(3.95, 1.5, -9.0),
		glm::vec3(3, -2.45, 7),
		glm::vec3(3, -1.45, 7),

	};

	// store the scales
	std::vector<glm::vec3> scales = {
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(2, 2, 0.5),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(0.7, 0.7, 0.7),
		glm::vec3(2, 2, 2),
		glm::vec3(0.6, 4, 4),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 1),

	};

	std::vector<glm::vec3> rotations = {
		glm::vec3(0, 1, 0)
	};

	std::vector<float> angles = {
		45.0f
	};

	for (size_t i = 0; i < translations.size(); ++i) {
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, translations[i]);
		model = glm::scale(model, scales[i]);
		// put then object that needs to be rotated in front
		if (i < angles.size()) { 
			model = glm::rotate(model, glm::radians(angles[i]), rotations[i]);
		}
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawBase(unsigned int shaderProgram, const std::vector<GLuint>& textures)
{
	glBindVertexArray(VAOs[1]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	std::vector<glm::vec3> translations = {
		glm::vec3(3.95, -1.65, -3.1),
		glm::vec3(5.90, -0.55, -3.1),
		glm::vec3(0.4, -1.75, -20.1),
		glm::vec3(-8.6, -0.75, -13.5),
		glm::vec3(-8.6, -1, -13.25),
		glm::vec3(-8.6, -1.25, -13),
		glm::vec3(-8.6, -1.5, -12.75),
		glm::vec3(-8.6, -1.75, -12.5),
		glm::vec3(-8.6, -2, -12.25),
		glm::vec3(-8.6, -2.25, -12),
		glm::vec3(-8.6, -2.5, -11.75),
		glm::vec3(-8.6, -2.75, -11.5),
		glm::vec3(-8.6, -3, -11.25),
		glm::vec3(-8.6, -1.75, -21.4),
		glm::vec3(-5.55, -0.35, -25.95),
		glm::vec3(-5.55, -0.05, -26.25),
		glm::vec3(-5.55, 0.25, -26.55),
		glm::vec3(-5.55, 0.55, -26.85),
		glm::vec3(-5.55, 0.85, -27.15),
		glm::vec3(-5.55, 1.15, -27.45),
		glm::vec3(-0.55, 0.4, -27.6),
		glm::vec3(-0.55, 0.6, -25.95),
		glm::vec3(3, 0.6, -27.45),
		glm::vec3(-5.55, 0.4, -28.35),






	};

	std::vector<glm::vec3> scales = {
		glm::vec3(0.3, 2.7, 16),
		glm::vec3(4, 0.1, 16),
		glm::vec3(15, 2.5, 18),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 2.5, 15.5),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(3, 0.3, 0.3),
		glm::vec3(7, 1.8, 3),
		glm::vec3(7, -2.2, 0.3),
		glm::vec3(0.3, -2.2, 3.3),
		glm::vec3(3, 1.8, 1.5),



	};

	for (size_t i = 0; i < translations.size(); ++i) { 
		glm::mat4 model = glm::mat4(1.f); 
		model = glm::translate(model, translations[i]); 
		model = glm::scale(model, scales[i]); 
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model)); 
		glDrawArrays(GL_TRIANGLES, 0, 36); 
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawHouse(unsigned int shaderProgram, const std::vector<GLuint>& textures)
{
	glBindVertexArray(VAOs[3]);

	std::vector<glm::vec3> translations = {
		// first texture
		glm::vec3(-2.0, 5.5, 5.7),
		glm::vec3(-2.0, 5.5, 9.7),
		glm::vec3(-2.0, 6, 7.7),
		glm::vec3(-2.0, 1, 10.15),
		glm::vec3(-8.15, 1, 7.7),
		glm::vec3(4.15, 1, 7.7),
		glm::vec3(-1.05, 2.4, 4.95),

		glm::vec3(8.05, 1, -12.15),
		glm::vec3(4.2, -1.55, 4.95),
		glm::vec3(-2.9, -1.55, 4.95),
		glm::vec3(-8.05, -1.55, 4.95),
		glm::vec3(-6.55, 2.25, -29.25),
		glm::vec3(3.4 ,2.25, -29.25),
		glm::vec3(6.9, 2.25, -28.15),
		glm::vec3(-8.65, 2.25, -27.45),
		glm::vec3(-2.1, 0.9, -29.25),
		glm::vec3(-2.1, 4.5, -29.25),

		// second texture
		glm::vec3(-1.5, 1.5, -11.25),
		glm::vec3(-6.85, 1.55, -12),
		glm::vec3(3.95, 1.55, -12.0),
		glm::vec3(-6.85, 1.55, -15.5),
		glm::vec3(3.95, 1.55, -15.5),
		glm::vec3(-6.85, 0.4, -13.75),
		glm::vec3(3.95, 0.4, -13.75),
		glm::vec3(-6.85, 3., -13.75),
		glm::vec3(3.95, 3., -13.75),
		glm::vec3(-1.5, 4.25, -12.3),	
		glm::vec3(-1.5, 4.25, -15.7),	
		glm::vec3(-1.45, 4.55, -14.0),
		glm::vec3(-1.45, 0.15, -15.0),

		glm::vec3(-15.1, -1.75, -21.55),
		glm::vec3(-15.1, 0.55, -13.9),
		glm::vec3(-15.1, 0.55, -29.05),
		glm::vec3(-20.1, 0.55, -21.55),
		glm::vec3(-15.1, 4, -21.55),
		glm::vec3(-9.85, 3.15, -19.7),
		glm::vec3(-9.85, 1, -15.17),

	};

	std::vector<glm::vec3> scales = {
		// first texture
		glm::vec3(12.5, 4, 0.2),
		glm::vec3(12.5, 4, 0.2),
		glm::vec3(0.5, 2.5, 12.5),
		glm::vec3(12, 8, 0.3),
		glm::vec3(0.3, 8, 5.2),
		glm::vec3(0.3, 8, 5.2),
		glm::vec3(18, 5.2, 0.3),

		glm::vec3(0.3, 8, 34.45),
		glm::vec3(7.5, 2.7, 0.3),
		glm::vec3(2.9, 2.7, 0.3),
		glm::vec3(4, 2.7, 0.3),
		glm::vec3(6.95, 5.5, 0.3),	
		glm::vec3(9, 5.5, 0.3),
		glm::vec3(2, 5.5, 2),
		glm::vec3(3, 5.5, 3.3),
		glm::vec3(2, 0.73, 0.3),
		glm::vec3(2, 1, 0.3),

		// second texture
		glm::vec3(10.3, 4, 0.5),
		glm::vec3(0.5, 4, 2),
		glm::vec3(0.6, 4, 2),
		glm::vec3(0.5, 4, 2),
		glm::vec3(0.6, 4, 2),
		glm::vec3(0.5, 1.7, 1.5),
		glm::vec3(0.6, 1.7, 1.5),
		glm::vec3(0.5, 1.1, 1.5),
		glm::vec3(0.6, 1.1, 1.5),
		glm::vec3(10.3, 3, 0.3),
		glm::vec3(10.3, 3, 0.3),
		glm::vec3(0.6, 1.5, 11.35),	
		glm::vec3(8, 1, 3),

		glm::vec3(10, 2.5, 15),
		glm::vec3(10, 7, 0.5),
		glm::vec3(10, 7, 0.5),
		glm::vec3(0.5, 7, 15),
		glm::vec3(12, 0.5, 17),
		glm::vec3(0.5, 1.3, 12),
		glm::vec3(0.5, 3, 3),

	};

	std::vector<glm::vec3> rotations = {
		// first texture
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),

		// second texture
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),

	};

	std::vector<float> angles = {
		// first texture
		60.f,
		-60.f,
		90.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,

		// second texture
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		-60.f,
		60.f,
		90.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,
		0.f,

	};

	for (size_t i = 0; i < translations.size(); ++i) {
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, translations[i]);
		model = glm::rotate(model, glm::radians(angles[i]), rotations[i]);
		model = glm::scale(model, scales[i]);

		// i less than the number of the first texture
		GLuint currentTexture = (i < 17) ? textures[3] : textures[4];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, currentTexture);
		glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawWindow(unsigned int shaderProgram)
{	
	std::vector<glm::vec3> positions;
	positions.push_back(glm::vec3(-6.85, 2.f, -14.f));
	positions.push_back(glm::vec3(3.96, 2.f, -14.f));

	std::map<float, glm::vec3> sorted_positions; 
	for (int i = 0; i < positions.size(); i++)
	{
		// calculate the the difference vector v between the camera position and the current position
		glm::vec3 v = glm::vec3(Camera.Position.x, 0.f, Camera.Position.z) - positions[i]; 
		float l = glm::length(v);
		sorted_positions[l] = positions[i];

	}
	glUseProgram(shaderProgram);

	glm::mat4 model = glm::mat4(1.f);
	glm::mat4 view;
	if (useFixedCamera) {
		view = glm::lookAt(fixedCamera.Position, fixedCamera.Position + fixedCamera.Front, fixedCamera.Up);
	}
	else {
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
	}
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// set the transparency value
	glUniform1f(glGetUniformLocation(shaderProgram, "transparency"), 0.3f);  

	glBindVertexArray(VAOs[2]);
	// reverse iterator
	for (auto it = sorted_positions.rbegin(); it != sorted_positions.rend(); ++it)
	{
		//test the distance
		//std::cout << "Distance: " << it->first << " Position: " << it->second.x << ", " << it->second.y << ", " << it->second.z << std::endl;

		model = glm::mat4(1.0f);
		model = glm::translate(model, it->second);
		model = glm::scale(model, glm::vec3(0.1, 1.5, 2));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

}

/*------------------------------------------Render OBJ----------------------------------------------------*/

ModelContainer::ModelContainer(const std::string& path)
// initialize translation
	: position(0.0f), scale(1.0f), rotation(1.0f) {
	if (!model.loadOBJ(path)) {
		std::cerr << "Failed to load model: " << path << std::endl;
		exit(EXIT_FAILURE);
	}
	// Obj VAO
	glGenVertexArrays(1, &modelVAO);
	glBindVertexArray(modelVAO);

	// Obj VBO
	glGenBuffers(1, &modelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), &model.vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// texture
	if (!model.texCoords.empty()) {
		glGenBuffers(1, &texCoordBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(glm::vec2), &model.texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		glEnableVertexAttribArray(1);
	}
	else {
		texCoordBuffer = 0;
	}
	// normal
	if (!model.normals.empty()) {
		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), &model.normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(2);
	}
	else {
		normalBuffer = 0;
	}

	glBindVertexArray(0);
	updateTransform();
}

// update the position of obj model
void ModelContainer::setPosition(const glm::vec3& pos) {
	position = pos;
	updateTransform();
}

// update the rotation of obj model
void ModelContainer::setRotation(float angle, const glm::vec3& axis) {
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
	updateTransform();
}

// update the sclae of obj model
void ModelContainer::setScale(const glm::vec3& scl) {
	scale = scl;
	updateTransform();
}

void ModelContainer::updateTransform() {
	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, position);
	modelTransform *= rotation;
	modelTransform = glm::scale(modelTransform, scale);
}


void ModelContainer::draw(GLuint program) {
	glBindVertexArray(modelVAO);


	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelTransform));

	// use the material
	for (const auto& material : model.materialIndices) {
		if (material.map_Kd) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material.map_Kd);
			glUniform1i(glGetUniformLocation(program, "Texture"), 0);
		}
		glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
	}

	glBindVertexArray(0);
}


std::vector<ModelContainer> modelContainers;

void initModels() {
	modelContainers.emplace_back("resources/wall.obj");
	modelContainers.emplace_back("resources/wall.obj");
	modelContainers.emplace_back("resources/tube light.obj");
	modelContainers.emplace_back("resources/tube light.obj");
	modelContainers.emplace_back("resources/Flashlight.obj");
	modelContainers.emplace_back("resources/RailSwitch.obj");
	modelContainers.emplace_back("resources/camera.obj");


}

void updateModelTransforms() {
	modelContainers[0].setPosition(glm::vec3(-10.85f, -2.9f, -8.0f));
	modelContainers[0].setRotation(90.f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelContainers[0].setScale(glm::vec3(0.9f, 1.f, 1.f));


	modelContainers[1].setPosition(glm::vec3(-10.85f, -2.9f, 1.0f));
	modelContainers[1].setRotation(90.f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelContainers[1].setScale(glm::vec3(0.9f, 1.f, 1.f));


	modelContainers[2].setPosition(glm::vec3(1, 0.5, -14.0));
	modelContainers[2].setScale(glm::vec3(0.2f, 0.1f, 0.1f));
	modelContainers[3].setPosition(glm::vec3(-4, 0.5, -14.0));
	modelContainers[3].setScale(glm::vec3(0.2f, 0.1f, 0.1f));

	modelContainers[4].setPosition(glm::vec3(3, -0.8, 7));
	modelContainers[4].setRotation(-90.f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelContainers[4].setScale(glm::vec3(0.1f, 0.1f, 0.1f));

	modelContainers[5].setPosition(glm::vec3(-1.f, 0.7f, -16.0f));
	modelContainers[5].setRotation(90.f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelContainers[5].setScale(glm::vec3(2.f, 2.f, 2.f));

	modelContainers[6].setPosition(glm::vec3(7.7f, 3.8f, -3.0f));
	modelContainers[6].setRotation(-90.f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelContainers[6].setScale(glm::vec3(0.1f, 0.1f, 0.1f));

}

void drawModels(GLuint program) {
	for (auto& container : modelContainers) {
		container.draw(program);
	}
}

/*------------------------------------------Translation and Animation----------------------------------------------------*/
glm::vec3 boxInitialPosition = glm::vec3(-2.1, 2.5, -29.5);
glm::vec3 boxTargetPosition = glm::vec3(-2.1, 6.5, -29.3);
glm::vec3 boxCurrentPosition = boxInitialPosition;
bool boxMovingDown = false;
bool boxMovingUp = false;
bool eKeyPressed = false;
float moveSpeed = 1.0f;
glm::vec3 switchPosition = glm::vec3(-1.0f, 0.7f, -16.0f);// the position of switch
float activationDistance = 4.0f; // the distance to activate the door 

void translateDoor(unsigned int shaderProgram, const std::vector<GLuint>& textures)
{
	if (boxMovingDown) {
		// use linear interpolation to calculate position, and make a effect like animation 
		boxCurrentPosition = glm::mix(boxCurrentPosition, boxTargetPosition, moveSpeed * 0.01f);
		if (glm::distance(boxCurrentPosition, boxTargetPosition) < 0.01f) {
			boxCurrentPosition = boxTargetPosition;
			boxMovingDown = false;
		}
	}
	else if (boxMovingUp) {
		boxCurrentPosition = glm::mix(boxCurrentPosition, boxInitialPosition, moveSpeed * 0.01f);
		if (glm::distance(boxCurrentPosition, boxInitialPosition) < 0.01f) {
			boxCurrentPosition = boxInitialPosition;
			boxMovingUp = false;
		}
	}

	glBindVertexArray(VAOs[1]);
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, boxCurrentPosition);
	model = glm::scale(model, glm::vec3(2.0f, 3.f, 0.2f)); 
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, 0);
}

float currentTime = 0.0f; 
float cameraRotationSpeed = 0.5f; 
float cameraMaxRotationAngle = 45.0f; 

void updateCameraModelTransform(float rotationAngle) {
	glm::vec3 cameraPosition = glm::vec3(7.7f, 3.8f, -3.0f);
	glm::vec3 cameraScale = glm::vec3(0.1f, 0.1f, 0.1f);

	modelContainers[6].setPosition(cameraPosition);
	if (!useFixedCamera) {
		// automatic rotation without camera
		modelContainers[6].setRotation(rotationAngle - 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else {
		modelContainers[6].setRotation(-fixedCamera.Yaw - 90.f, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	modelContainers[6].setScale(cameraScale);
}


/*------------------------------------------Handling Input----------------------------------------------------*/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		useFixedCamera = !useFixedCamera;
	}

}
void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (!useFixedCamera) {
		float cameraSpeed = Camera.MovementSpeed;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			Camera.Position += cameraSpeed * Camera.Front;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			Camera.Position -= cameraSpeed * Camera.Front;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			Camera.Position -= cameraSpeed * Camera.Right;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			Camera.Position += cameraSpeed * Camera.Right;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			Camera.Position += cameraSpeed * Camera.WorldUp;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			Camera.Position -= cameraSpeed * Camera.WorldUp;
	}

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!eKeyPressed) { 
			// calculate the distance from player and switch
            float distanceToSwitch = glm::distance(Camera.Position, switchPosition);
            if (distanceToSwitch < activationDistance) { 
				// check if moving
                if (!boxMovingDown && !boxMovingUp) {
                    if (boxCurrentPosition == boxInitialPosition) {
                        boxMovingDown = true;
                    } else if (boxCurrentPosition == boxTargetPosition) {
                        boxMovingUp = true;
                    }
                }
            }
            eKeyPressed = true;
        }
    } else {
        eKeyPressed = false; 
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static double lastX = WIDTH / 2.0;
	static double lastY = HEIGHT / 2.0;
	static bool firstMouse = true;

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	if (useFixedCamera) {
		fixedCamera.ProcessMouseMovement(xoffset, yoffset);
	}
	else {
		Camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

/*------------------------------------------Manager Light----------------------------------------------------*/
void initializePositionalLights(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& colors, std::vector<float>& intensities) {
	positions = {
		glm::vec3(1, 3, -14.0),
		glm::vec3(-4, 3, -14.0),
	};
	colors = {
		glm::vec3(0.9f, 0.9f, 0.98f),
		glm::vec3(0.9f, 0.9f, 0.98f),
	};
	intensities = {
		1.f,
		1.f
	};
}

void initializeSpotLights(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& directions, std::vector<glm::vec3>& colors, std::vector<float>& intensities, std::vector<float>& cutOffs) {
	positions = {
		 glm::vec3(3, -0.8, 7)
	};
	directions = {
		glm::vec3(-1.0f, 0.0f, 0.0f)
	};
	colors = {
		 glm::vec3(1.0f, 1.0f, 1.0f)
	};
	intensities = {
		 1.0f
	};
	cutOffs = {
		 10.0f
	};
}

void setPositionalLights(GLuint program, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& colors, const std::vector<float>& intensities) {
	glUniform1i(glGetUniformLocation(program, "numPositionalLights"), positions.size()); // bind the number
	// bind the attributes of each light to fragment
	for (int i = 0; i < positions.size(); ++i) {
		std::string index = std::to_string(i);
		glUniform3fv(glGetUniformLocation(program, ("positionalLights[" + index + "].position").c_str()), 1, glm::value_ptr(positions[i]));
		glUniform3fv(glGetUniformLocation(program, ("positionalLights[" + index + "].color").c_str()), 1, glm::value_ptr(colors[i]));
		glUniform1f(glGetUniformLocation(program, ("positionalLights[" + index + "].intensity").c_str()), intensities[i]);
	}
}

// same as above
void setSpotLights(GLuint program, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& directions, const std::vector<glm::vec3>& colors, const std::vector<float>& intensities, const std::vector<float>& cutOffs) {
	glUniform1i(glGetUniformLocation(program, "numSpotLights"), positions.size());
	for (int i = 0; i < positions.size(); ++i) {
		std::string index = std::to_string(i);
		glUniform3fv(glGetUniformLocation(program, ("spotLights[" + index + "].position").c_str()), 1, glm::value_ptr(positions[i]));
		glUniform3fv(glGetUniformLocation(program, ("spotLights[" + index + "].direction").c_str()), 1, glm::value_ptr(directions[i]));
		glUniform3fv(glGetUniformLocation(program, ("spotLights[" + index + "].color").c_str()), 1, glm::value_ptr(colors[i]));
		glUniform1f(glGetUniformLocation(program, ("spotLights[" + index + "].intensity").c_str()), intensities[i]);
		glUniform1f(glGetUniformLocation(program, ("spotLights[" + index + "].cutOff").c_str()), cutOffs[i]);
	}
}

/*------------------------------------------Render Shadow----------------------------------------------------*/

void generateDepthMap(unsigned int shadowShaderProgram, unsigned int transparencyProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix,
	const std::vector<GLuint>& textures)
{
	glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shadowShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	drawFloor(shadowShaderProgram, textures);
	drawBase(shadowShaderProgram, textures);
	drawBox(shadowShaderProgram, textures);
	drawHouse(shadowShaderProgram, textures);
	drawModels(shadowShaderProgram);
	translateDoor(shadowShaderProgram, textures); 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawWindow(transparencyProgram);
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void renderWithShadow(unsigned int renderShaderProgram, unsigned int transparencyProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix,
	const std::vector<GLuint>& textures)
{
	glViewport(0, 0, WIDTH, HEIGHT);

	static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
	glClearBufferfv(GL_COLOR, 0, bgd);
	glClear(GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(renderShaderProgram);

	glBindTexture(GL_TEXTURE_2D, shadow.Texture);

	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightColour"), 1.f, 1.f, 1.f);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 view;
	if (useFixedCamera) {
		view = glm::lookAt(fixedCamera.Position, fixedCamera.Position + fixedCamera.Front, fixedCamera.Up);
	}
	else {
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
	}
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// using multiple textures simultaneously
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadow.Texture);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "shadowMap"), 1);

	drawFloor(renderShaderProgram, textures);
	drawBase(renderShaderProgram, textures);
	drawBox(renderShaderProgram, textures);
	drawHouse(renderShaderProgram, textures);
	drawModels(renderShaderProgram);
	translateDoor(renderShaderProgram, textures); 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawWindow(transparencyProgram);
	glDisable(GL_BLEND);
}



void SetupBuffer() {

	// specifying VAO
	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	// specifying VBO
	glCreateBuffers(NUM_VBOs, VBOs);
	glNamedBufferStorage(VBOs[0], sizeof(floorVertices), floorVertices, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	glBindVertexArray(VAOs[1]);
	glNamedBufferStorage(VBOs[1], sizeof(rectangleVertices), rectangleVertices, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	glBindVertexArray(VAOs[2]);
	glNamedBufferStorage(VBOs[2], sizeof(glassVertices), glassVertices, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glBindVertexArray(VAOs[3]);
	glNamedBufferStorage(VBOs[3], sizeof(houseVertices), houseVertices, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

int main(int argc, char** argv)
{
	// antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4); 

	// window creation
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assessment 3", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	glfwSetKeyCallback(window, key_callback); // keyboard controller
	glfwSetCursorPosCallback(window, mouse_callback); // mouse controller
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide the cursor
	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT); 
	glDebugMessageCallback(DebugCallback, 0);

	// depth test
	glEnable(GL_DEPTH_TEST); 
	// enable the antialiasing
	glEnable(GL_MULTISAMPLE); 

	ShadowStruct shadow = setup_shadowmap(SH_MAP_WIDTH, SH_MAP_HEIGHT); 

	GLuint phong_program = CompileShader("shaders/phong.vert", "shaders/phong.frag");
	GLuint shadow_program = CompileShader("shaders/shadow.vert", "shaders/shadow.frag"); 
	GLuint transparency_program = CompileShader("shaders/transparency.vert", "shaders/transparency.frag");

	SetupBuffer();
	//initial the state of camera
	Camera.InitCamera(glm::vec3(7.0f, 2.0f, -23.0f), glm::vec3(0.0f, 1.0f, 0.0f), 135.0f, 0.0f);
	fixedCamera.InitCamera(glm::vec3(7.9f, 4.5f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 180.f, 0.0f);

	initModels();
	updateModelTransforms();
	/*------------------------------------------Load Texture----------------------------------------------------*/
	std::vector<GLuint> textures = {
		setup_texture("resources/floor.jpg"), // 0
		setup_texture("resources/box.jpg"), // 1
		setup_texture("resources/base.jpg"), // 2
		setup_texture("resources/wood1.jpg"), // 3
		setup_texture("resources/wood2.jpg"), // 4
		setup_texture("resources/door.jpg") // 5
	};

	/*------------------------------------------Define Light Sources----------------------------------------------------*/
	std::vector<glm::vec3> positionalLightPositions; 
	std::vector<glm::vec3> positionalLightColors; 
	std::vector<float> positionalLightIntensities; 
	initializePositionalLights(positionalLightPositions, positionalLightColors, positionalLightIntensities); 

	std::vector<glm::vec3> spotLightPositions; 
	std::vector<glm::vec3> spotLightDirections; 
	std::vector<glm::vec3> spotLightColors; 
	std::vector<float> spotLightIntensities; 
	std::vector<float> spotLightCutOffs; 
	initializeSpotLights(spotLightPositions, spotLightDirections, spotLightColors, spotLightIntensities, spotLightCutOffs);

	/*------------------------------------------Main Loop----------------------------------------------------*/
	while (!glfwWindowShouldClose(window))
	{

		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT); 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram(phong_program);

		glUniform3f(glGetUniformLocation(phong_program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform3f(glGetUniformLocation(phong_program, "lightColour"), 1.f, 1.f, 1.f);
		glUniform3f(glGetUniformLocation(phong_program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(phong_program, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);

		// update positional light parameters
		setPositionalLights(phong_program, positionalLightPositions, positionalLightColors, positionalLightIntensities);

		// update spot light parameters
		setSpotLights(phong_program, spotLightPositions, spotLightDirections, spotLightColors, spotLightIntensities, spotLightCutOffs);

	
		processKeyboard(window);

		// calculate the angle of camera with the time
		currentTime += glfwGetTime();
		glfwSetTime(0.0); // set the time in order to get the time
		float cameraRotationAngle = cos(currentTime * cameraRotationSpeed) * cameraMaxRotationAngle;

		// update the transform of camera
		updateCameraModelTransform(cameraRotationAngle);

		glm::mat4 model = glm::mat4(1.f);
		glUniformMatrix4fv(glGetUniformLocation(phong_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glm::mat4 view; 
		if (useFixedCamera) {
			view = glm::lookAt(fixedCamera.Position, fixedCamera.Position + fixedCamera.Front, fixedCamera.Up); 
		}
		else {
			view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up); 
		}
		glUniformMatrix4fv(glGetUniformLocation(phong_program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100.f);
		glUniformMatrix4fv(glGetUniformLocation(phong_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		
		drawModels(phong_program);
		translateDoor(phong_program, textures);
		drawFloor(phong_program, textures);
		drawBase(phong_program, textures);
		drawBox(phong_program, textures);
		drawHouse(phong_program, textures);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		drawWindow(transparency_program);
		glDisable(GL_BLEND);


		float near_plane = 1.0f, far_plane = 70.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projectedLightSpaceMatrix = lightProjection * lightView;
		generateDepthMap(shadow_program, transparency_program, shadow, projectedLightSpaceMatrix, textures);
		renderWithShadow(phong_program, transparency_program, shadow, projectedLightSpaceMatrix, textures);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}