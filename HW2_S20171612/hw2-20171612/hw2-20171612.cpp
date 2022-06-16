#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0; 
float centerx = 0.0f, centery = 0.0f;

// 2D 물체 정의 부분은 objects.h 파일로 분리
// 새로운 물체 추가 시 prepare_scene() 함수에서 해당 물체에 대한 prepare_***() 함수를 수행함.
// (필수는 아니나 올바른 코딩을 위하여) cleanup() 함수에서 해당 resource를 free 시킴.
#include "objects.h"

int leftbuttonpressed = 0;
int animation_mode = 1;
unsigned int timestamp = 0;

void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	glutPostRedisplay();
	if (animation_mode)
		glutTimerFunc(10, timer, 0);
}

void display(void) {
	glm::mat4 ModelMatrix;
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	ModelMatrix = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();

	int clock_car = timestamp % 1440;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((clock_car / 10.0f) * cosf(clock_car * TO_RADIAN), (clock_car / 10.0f) * sinf(clock_car * TO_RADIAN), 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(clock_car / 720.0f + 1.0f, clock_car / 720.0f + 1.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	int house_clock = timestamp % 360;
	ModelMatrix = glm::rotate(glm::mat4(1.0f), house_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.5f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_house();
    
	house_clock = (timestamp % 1442) / 2 - 360; // -360 <= house_clock <= 360 
	float rotation_angle_house = atanf(100.0f*TO_RADIAN*sinf(house_clock * TO_RADIAN)); 
	float scaling_factor_house = 5.0f*(1.0f - fabs(cosf(house_clock * TO_RADIAN)));

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3((float)house_clock, 
		                                                100.0f * cosf(house_clock * TO_RADIAN), 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_house, scaling_factor_house, 1.0f));
	ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_house, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car();
	
	int airplane_clock = timestamp % 400 + 1;

	if (airplane_clock < 100) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, airplane_clock, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_clock*0.01f + 2.0f, -1.0f *(airplane_clock*0.01f + 2.0f), 0.0f));
	}
	else if (airplane_clock < 300) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -airplane_clock+100.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(200.0f, 100.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f - (airplane_clock-100)/2.0f*0.01f, 3.0f - (airplane_clock-100)/2.0f*0.01f, 0.0f));
	}
	else {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, airplane_clock-300.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(200.0f, -100.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3((airplane_clock-300)*0.01f + 1.0f, -1.0f * ((airplane_clock-300) * 0.01f + 1.0f), 0.0f));
	}
	
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
 	draw_airplane();  
	

	int n2_clock = timestamp % 300;

	if (n2_clock < 100) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f-n2_clock/2.0f, n2_clock/2.0f, 0.0f));
	}
	else if (n2_clock < 200) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-n2_clock/2.0f+ 50.0f, 100.0f - n2_clock/2.0f, 0.0f));
	}
	else if (n2_clock < 300) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(n2_clock-250.0f, 0.0f,0.0f));
	}
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(n2_clock/150.0f + 1.0f, n2_clock/150.0f + 1.0f, 1.0f));
	
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cake();

	int n3_clock = timestamp % 1080;
	if (n3_clock < 360) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(360.0f-2.0f*n3_clock,360.0f-2.0f*n3_clock ,0.0f));
	}
	else if (n3_clock < 540) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-360.0f, -360.0f + (n3_clock - 360.0f)*4.0f, 0.0f));
	}
	else if(n3_clock < 900) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-360.0f + (n3_clock - 540.0f) * 2.0f, 360.0f - 2.0f * (n3_clock - 540.0f), 0.0f));
	}
	else {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(360.0f, -360.0f + (n3_clock - 900.0f) * 4.0f, 0.0f));
	}
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(sinf(n3_clock * TO_RADIAN)*1.5f + 1.0f, sinf(n3_clock * TO_RADIAN)*1.5f + 1.0f, 1.0f));

	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_vs();
	
	glFlush();	
}   

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}
	
void reshape(int width, int height) {
	win_width = width, win_height = height;
	
  	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0, 
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

	glDeleteVertexArrays(1, &VAO_house);
	glDeleteBuffers(1, &VBO_house);

	glDeleteVertexArrays(1, &VAO_cake);
	glDeleteBuffers(1, &VBO_cake);

	glDeleteVertexArrays(1, &VAO_car);
	glDeleteBuffers(1, &VBO_car);


	glDeleteVertexArrays(1, &VAO_car2);
	glDeleteBuffers(1, &VBO_car2);


	glDeleteVertexArrays(1, &VAO_vs);
	glDeleteBuffers(1, &VBO_vs);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0); // animation_mode = 1
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(129.0f/255.0f,216.0f/255.0f,217.0f/255.0f,1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_airplane();
	prepare_house();
	prepare_car();
	prepare_cake();
	prepare_car2();
	prepare_vs();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program(); 
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

    error = glewInit();
	if (error != GLEW_OK) { 
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
int main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 HW2 20171612 김성일";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC'"
	};

	glutInit (&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


