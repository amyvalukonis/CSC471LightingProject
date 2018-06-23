/*
ZJ Wood CPE 471 Lab 3 base code - I. Dunn class re-write
*/

#include <string> 
#include <iostream>
#include <glad/glad.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include <math.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct Vertex{
	float x;
	float y;
	float z;
};

vector<unsigned int> triBuf; 
vector<float> posBuf; 
vector<float> normals; 

Vertex v1; 
Vertex v2; 
Vertex v3; 

float movement = 0; 
float r= 0;
int shader = 0; 
int material = 0;


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	std::shared_ptr<Program> prog;
	GLuint VertexArrayID;
	GLuint VertexBufferID;
	GLuint NormalBufferID;
	GLuint elementBuffer; 


	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_Q) 
		{
			movement -= 0.2; 
		}

		if (key == GLFW_KEY_E) 
		{
			movement +=0.2; 
		}

		if (key == GLFW_KEY_P && action == GLFW_PRESS)
		{
			shader+=1; 
			if(shader==4) {
				shader=0; 
			}
		}

		if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			material+=1; 
			if(material == 5) {
				material = 0; 
			}
		}

		if (key == GLFW_KEY_R) 
		{
			r +=0.1;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	void SetMaterial(int i) {
  		switch (i) {
  		case 0: 	
    		glUniform3f(prog->getUniform("MatAmb"), 1.0, 1.0, 1.0);
    		glUniform3f(prog->getUniform("MatDif"), 1.0, 1.0, 1.0);
    		glUniform3f(prog->getUniform("MatSpec"), 1.0, 1.0, 1.0);
    		glUniform1f(prog->getUniform("shine"), 10);
    		break;

  		case 1: // shiny blue plastic
    		glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
    		glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
    		glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
    		glUniform1f(prog->getUniform("shine"), 120.0);
    		break;
  		case 2: // flat grey
    		glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
    		glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
    		glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    		glUniform1f(prog->getUniform("shine"), 4.0);
    		break;
  		case 3: // brass
    		glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
    		glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
    		glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
    		glUniform1f(prog->getUniform("shine"), 27.9);
    		break;

  		case 4: // jade
    		glUniform3f(prog->getUniform("MatAmb"), 0.135, 0.2225, 0.1575);
    		glUniform3f(prog->getUniform("MatDif"), 0.54 , 0.89, 0.63);
    		glUniform3f(prog->getUniform("MatSpec"), 0.316228, 0.316228, 0.316228);
    		glUniform1f(prog->getUniform("shine"), 0.1);
    		break;
  		}
	}

	void initGeom()
	{

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		glEnableVertexAttribArray(0);
		glGenBuffers(1, &VertexBufferID); 
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0); 

		glGenBuffers(1, &elementBuffer); 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer); 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triBuf.size() * sizeof(unsigned int), &triBuf[0],GL_STATIC_DRAW);

		glEnableVertexAttribArray(1); 
		glGenBuffers(1, &NormalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBufferID); 
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_DYNAMIC_DRAW); 
		glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		glClearColor(0.12f, 0.34f, 0.56f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/vert.glsl", resourceDirectory + "/frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("M");
		prog->addUniform("V");
		prog->addUniform("shaders");
		prog->addUniform("ka");
		prog->addUniform("kd"); 
		prog->addUniform("ks");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("shine");
		prog->addUniform("mat");
		prog->addUniform("lightPosition");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
	}


	void render()
	{
		glm::vec3 lightPosition = glm::vec3(-2+movement,2,2);
		lightPosition = glm::normalize(lightPosition);
		glm::vec3 ka = glm::vec3(0.3,0.3,0.3);
		glm::vec3 kd = glm::vec3(0.7,0.7,0.7); 
		glm::vec3 ks = glm::vec3(0.5,0.5,0.5);
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		auto P = std::make_shared<MatrixStack>();
		auto M = std::make_shared<MatrixStack>();
		auto V = std::make_shared<MatrixStack>(); 

		P->pushMatrix();
		P->perspective(45.0f,aspect,0.01f, 100.0f);
		V->pushMatrix(); 

		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V->topMatrix()));
		glUniform3fv(prog->getUniform("lightPosition"), 1, &lightPosition[0]);
		glUniform3fv(prog->getUniform("ka"), 1, &ka[0]);
		glUniform3fv(prog->getUniform("kd"), 1, &kd[0]);
		glUniform3fv(prog->getUniform("ks"), 1, &ks[0]);
		glUniform1i(prog->getUniform("shaders"), shader); 
		glUniform1i(prog->getUniform("mat"), material); 

		SetMaterial(material);

		glBindVertexArray(VertexArrayID);

		M->pushMatrix(); 
		M->translate(glm::vec3(-1,0,-4));
		M->rotate(r,glm::vec3(0,1,0));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M->topMatrix()));
		glDrawElements(GL_TRIANGLES, triBuf.size(), GL_UNSIGNED_INT, (void*)0); 
		M->popMatrix(); 

		M->pushMatrix(); 
		M->translate(glm::vec3(1,0,-4));
		M->rotate(r,glm::vec3(0,1,0));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M->topMatrix()));
		glDrawElements(GL_TRIANGLES, triBuf.size(), GL_UNSIGNED_INT, (void*)0); 
		M->popMatrix();


		//glDrawElements(GL_TRIANGLES, triBuf.size(), GL_UNSIGNED_INT, (void*)0); 

		glBindVertexArray(0);

		prog->unbind();

		// Pop matrix stacks.
		V->popMatrix();
		P->popMatrix();
	}

	void setNormals(int index1,int index2,int index3,glm::vec3 cross) {
		normals[3*index1] += cross.x; 
		normals[3*index1+1] += cross.y; 
		normals[3*index1+2] += cross.z; 

		normals[3*index2] += cross.x; 
		normals[3*index2+1] += cross.y; 
		normals[3*index2+2] += cross.z; 

		normals[3*index3] += cross.x; 
		normals[3*index3+1] += cross.y; 
		normals[3*index3+2] += cross.z; 
	}

	void computeNormals() {
		for(int i=0; i<posBuf.size();i++) {
			normals.push_back(0); 
		}

		//loop over triangles
		for(int i=0; i<triBuf.size()/3;i++) {
			glm::vec3 AB; 
			glm::vec3 AC; 
			glm::vec3 cross; 

			//A
			int index1 = triBuf[3*i];
			v1.x = posBuf[3*index1];
			v1.y = posBuf[3*index1+1];
			v1.z = posBuf[3*index1+2];

			//B 
			int index2 = triBuf[3*i+1];
			v2.x = posBuf[3*index2];
			v2.y = posBuf[3*index2+1];
			v2.z = posBuf[3*index2+2];	

			//C 
			int index3 = triBuf[3*i+2];
			v3.x = posBuf[3*index3];
			v3.y = posBuf[3*index3+1];
			v3.z = posBuf[3*index3+2];	

			//edge calculations 
			//AB 
			AB.x = v2.x-v1.x; 
			AB.y = v2.y-v1.y; 
			AB.z = v2.z-v1.z; 

			//AC 
			AC.x = v3.x-v1.x; 
			AC.y = v3.y-v1.y; 
			AC.z = v3.z-v1.z; 

			cross = glm::cross(AB,AC);
			cross = glm::normalize(cross);
			
			setNormals(index1, index2, index3, cross); 
		}
		//loop over vertices and normalize 
		for(int i=0; i<posBuf.size();i+=3) {
			glm::vec3 vert; 
			vert.x = normals[i]; 
			vert.y = normals[i+1];
			vert.z = normals[i+2]; 

			vert = glm::normalize(vert);

			normals[i] = vert.x; 
			normals[i+1] = vert.y; 
			normals[i+2] = vert.z;
		}
	}
};

void resize_obj(std::vector<tinyobj::shape_t> &shapes){
   	float minX, minY, minZ;
   	float maxX, maxY, maxZ;
   	float scaleX, scaleY, scaleZ;
   	float shiftX, shiftY, shiftZ;
   	float epsilon = 0.001;

   	minX = minY = minZ = 1.1754E+38F;
   	maxX = maxY = maxZ = -1.1754E+38F;

   	//Go through all vertices to determine min and max of each dimension
   	for (size_t i = 0; i < shapes.size(); i++) {
      	for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         	if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
         	if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];

         	if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
         	if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

         	if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
         	if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
      	}
   	}

   	//From min and max compute necessary scale and shift for each dimension
   	float maxExtent, xExtent, yExtent, zExtent;
   	xExtent = maxX-minX;
   	yExtent = maxY-minY;
   	zExtent = maxZ-minZ;
   	if (xExtent >= yExtent && xExtent >= zExtent) {
      	maxExtent = xExtent;
   	}
   	if (yExtent >= xExtent && yExtent >= zExtent) {
      	maxExtent = yExtent;
   	}
   	if (zExtent >= xExtent && zExtent >= yExtent) {
      	maxExtent = zExtent;
   	}
   	scaleX = 2.0 /maxExtent;
   	shiftX = minX + (xExtent/ 2.0);
   	scaleY = 2.0 / maxExtent;
   	shiftY = minY + (yExtent / 2.0);
   	scaleZ = 2.0/ maxExtent;
   	shiftZ = minZ + (zExtent)/2.0;

   	//Go through all verticies shift and scale them
   	for (size_t i = 0; i < shapes.size(); i++) {
      	for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         	shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
         	assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
         	assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
         	shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
         	assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
         	assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
         	shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
         	assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
         	assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
      	}
   	}
}

int main(int argc, char **argv)
{
   	if (argc < 2) { 
    	cout << "Usage: ./p3 meshfile" << endl; 
      	return 0;
   	} 

   	string meshName(argv[1]); 

	std::string resourceDir = "../resources"; // Where the resources are loaded from

    vector<tinyobj::shape_t> shapes; // geometry
    vector<tinyobj::material_t> objMaterials; // material
    string errStr;

    bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str()); 

    if(!rc) {
    	cerr << errStr << endl; 
    } 
    else {
    	resize_obj(shapes);
    	posBuf = shapes[0].mesh.positions; 
    	triBuf = shapes[0].mesh.indices; 
    }

	Application *application = new Application();

	application->computeNormals();

	WindowManager * windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->init(resourceDir);
	application->initGeom();

	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		application->render();
		glfwSwapBuffers(windowManager->getHandle());
		glfwPollEvents();
	}

	windowManager->shutdown();
	return 0;
}
