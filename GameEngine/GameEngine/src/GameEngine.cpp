#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct vector4d
{
	float x, y, z, w;
};

struct mat4d
{
	vector4d mat[4];
};

void AddShader(GLint program, GLenum shaderType, const char* shaderCode);
void CompileShaders();
void CreateTriangle();
mat4d getMatrix(float init);
void translateMatrix(mat4d* mat, vector4d vec);

bool direction = true;
float triMaxOffset = 0.7f;
float triOffset = 0.0f;
float triOffsetIncrement = 0.0005f;
float toDegrees = 3.141592653 / 180.0f;

// Init vertex and fragment shaders
char vShader[] = "														\n\
#version 330															\n\
																		\n\
layout (location = 0) in vec3 pos;										\n\
																		\n\
out vec4 fColor;														\n\
																		\n\
uniform mat4 u_model;													\n\
																		\n\
void main()																\n\
{																		\n\
	gl_Position = vec4(pos*0.4, 1.f);									\n\
	fColor = vec4(1.f, 0.f, 0.f , 1.f);									\n\
}";

char fShader[] = "							\n\
#version 330								\n\
											\n\
in vec4 fColor;								\n\
											\n\
out vec4 fragColor;							\n\
											\n\
void main()									\n\
{											\n\
	fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);							\n\
}";

GLuint shader, VAO, VBO, uniformModel;

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit())
	{
		printf("Glew failed to init");
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	float angle = 45.0f;
	float angleIncrement = 0.001f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		if (direction)
		{
			triOffset += triOffsetIncrement;
		}
		else
		{
			triOffset -= triOffsetIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		auto matrixTranspose = GL_FALSE;
		float* matrixPtr;
		//angle += angleIncrement;
		/*GLM Matrix*/
		glm::mat4 myMatrix = glm::mat4(1.0f);
		myMatrix = glm::translate(myMatrix, glm::vec3(2, 0.0f, 0.0f));
		//myMatrix = glm::rotate(myMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		matrixPtr = glm::value_ptr(myMatrix);
		/*-- end GLM matrix --*/

		/*MY matrix*/
		//auto myma = getMatrix(1.0f);
		//translateMatrix(&myma, {triOffset, 0.0f, 0.0f, 1.0f});
		//matrixTranspose = GL_TRUE;
		//matrixPtr = &myma.mat[0].x;
		/*-- end my matrix --*/

		glUniformMatrix4fv(uniformModel, 1, matrixTranspose, matrixPtr);
		//glUniformMatrix4fv(uniformModel, 1, GL_TRUE, & myma.mat[0].x);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void AddShader(GLint program, GLenum shaderType, const char* shaderCode)
{
	GLuint shader = glCreateShader(shaderType);

	// Attatch source code to the shader
	const GLchar* code[1];
	code[0] = shaderCode;

	GLint lens[1];
	lens[0] = strlen(shaderCode);

	glShaderSource(shader, 1, code, lens);

	// Compile the shader code
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			GLchar linkedInfo[1000] = { 0 };
			glGetShaderInfoLog(shader, infoLen, NULL, linkedInfo);
			std::cout << linkedInfo << "\n";
		}

		glDeleteShader(shader);
		return;
	}

	// Attach the shader compiled version to the program
	glAttachShader(program, shader);
}

void CompileShaders()
{
	shader = glCreateProgram();

	if (!shader)
	{
		printf("Error creating program \n");
		return;
	}

	// Add vertex shader
	AddShader(shader, GL_VERTEX_SHADER, vShader);
	// Add fragment shader
	AddShader(shader, GL_FRAGMENT_SHADER, fShader);

	glLinkProgram(shader);
	int linkedRes;
	glGetProgramiv(shader, GL_LINK_STATUS, &linkedRes);
	if (!linkedRes)
	{
		GLchar linkedInfo[1024] = { 0 };
		glGetProgramInfoLog(shader, sizeof(linkedInfo), NULL, linkedInfo);
		printf("Error when linking program shader: %s \n", linkedInfo);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &linkedRes);
	if (!linkedRes)
	{
		GLchar linkedInfo[1024] = { 0 };
		glGetProgramInfoLog(shader, sizeof(linkedInfo), NULL, linkedInfo);
		printf("Error when validating program shader: %s \n", linkedInfo);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "u_model");
}

void CreateTriangle()
{
	GLfloat vertices[] = {
		-1.0, 1.0, 0.0f,
		-1.0, -1.0, 0.0f,
		1.0, 1.0, 0.0f,
	};

	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

void translateMatrix(mat4d * mat, vector4d vec) 
{
	mat->mat[0].w = vec.x;
	mat->mat[1].w = vec.y;
	mat->mat[2].w = vec.z;
	mat->mat[3].w = vec.w;
}

mat4d getMatrix(float init) 
{
	mat4d mat = 
	{
		init, 0.0f, 0.0f, 0.0f,
		0.0f, init, 0.0f, 0.0f,
		0.0f, 0.0f, init, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return mat;
}