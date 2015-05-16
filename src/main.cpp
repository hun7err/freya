#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

#include "../include/Model.hpp"

#include <string>
#include <vector>
#include <fstream>
using namespace glm;

// ^ to-do: tekstury, uv-ki

#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	600
#define HALF_PI 1.57f

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
 
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
 
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
 
    GLint Result = GL_FALSE;
    int InfoLogLength;
 
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
 
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}

/*
		
to-do:
=======
- sprawdzic jak wygladalo main.cpp gdy dzialala kamera
- tekstury
- crytek sponza
- ewentualne poprawki kamery (kwaterniony?)

*/

/*
	to-do:
	0) naprawiæ build dla Win32
	2) kamera
	3) cegui -> zbudowaæ + wgraæ
	5) pomys³ na pamiêæ (z placement new)
	6) papery o GI -> g³ównie jakieœ state-of-the-art, ale te¿ photon mapping (o photon splashingu), deep g-buffer radiosity itd.
	7) kolor s³oñca (atmospheric attenuation itp.)
	8) tiled deferred rendering
*/

int main(int argc, char *argv[]) {
	if( !glfwInit() ) {
		fprintf( stderr, "Failed to initialize GLFW.\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
	GLFWwindow* window;
	window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Freya :: Global Illumination Playground", NULL, NULL); 

	if( window == NULL ) {
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;	// dla 3.3+
	
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW.\n");
		return -1;
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
 
	Model model;

	printf("loading model...\n");
	model.load("../res/models/stanford-bunny/bunny.obj");
	printf("model loaded\n");

	GLuint programID = LoadShaders( "../res/shaders/triangle_vertex.glsl", "../res/shaders/triangle_fragment.glsl" );

	glm::vec3 position = glm::vec3( 0, 0, 5 );
	// horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;
	// Initial Field of View
	float FoV = 45.0f;
 
	float speed = 0.005f,
		mouseSpeed = 0.0005f,
		deltaTime;
	
	double xpos, ypos, currentTime, lastTime = glfwGetTime();
	glm::vec3 direction, right, up;
	glm::mat4	ProjectionMatrix,
				ViewMatrix,
				ModelMatrix = glm::mat4(1.0f);  // Changes for each model !

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	direction.x = cos(verticalAngle) * sin(horizontalAngle);
	direction.y = sin(verticalAngle);
	direction.z = cos(verticalAngle) * cos(horizontalAngle);

	right.x = sin(horizontalAngle - HALF_PI);
	right.y = 0;
	right.z = cos(horizontalAngle - HALF_PI);

	up = glm::cross( right, direction );

	do{
		currentTime = glfwGetTime();
		deltaTime = float(currentTime - lastTime);
		
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		
		if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		{
			horizontalAngle += mouseSpeed * deltaTime * float( WINDOW_WIDTH/2 - xpos );
			verticalAngle   += mouseSpeed * deltaTime * float( WINDOW_HEIGHT/2 - ypos );

			direction.x = cos(verticalAngle) * sin(horizontalAngle);
			direction.y = sin(verticalAngle);
			direction.z = cos(verticalAngle) * cos(horizontalAngle);

			right.x = sin(horizontalAngle - HALF_PI);
			right.y = 0;
			right.z = cos(horizontalAngle - HALF_PI);

			up = glm::cross( right, direction );
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		
		ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
		// Camera matrix
		ViewMatrix       = glm::lookAt(
			position,           // Camera is here
			position+direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
		);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP        = ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around

		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
 
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		model.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if(glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
			position += direction * deltaTime * speed;
		}
		if(glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
			position -= direction * deltaTime * speed;
		}
		// Strafe right
		if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
			position += right * deltaTime * speed;
		}
		// Strafe left
		if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
			position -= right * deltaTime * speed;
		}
	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

	return 0;
}