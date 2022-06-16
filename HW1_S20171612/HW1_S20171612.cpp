#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <tuple>
#include <vector>
#include <cmath>

#define PI 3.1415926535

using namespace std;

class Point {
private:
	float x;
	float y;
public:
	Point(float x, float y) {
		this->x = x;
		this->y = y;
	}

	float getX() {
		return x;
	}
	
	float getY() {
		return y;
	}

	float getX(int windowWidth) {
		return (float)(x) / (float)(windowWidth / 2) - 1.0f;
	}

	float getY(int windowHeight) {
		return -((float)(y) / (float)(windowHeight / 2) - 1.0f);
	}
};

class PointManager {
private:
	vector<Point> points;

public:
	PointManager() {

	}

	int getSize() {
		return points.size();
	}

	void moveLeft() {
		vector<Point> newPoints;
		for (Point point : points) {
			newPoints.push_back(Point(point.getX() - 5.0f, point.getY()));
		}
		points = newPoints;
	}

	void moveRight() {
		vector<Point> newPoints;
		for (Point point : points) {
			newPoints.push_back(Point(point.getX() + 5.0f, point.getY()));
		}
		points = newPoints;
	}

	void moveUp() {
		vector<Point> newPoints;
		for (Point point : points) {
			newPoints.push_back(Point(point.getX(), point.getY() - 5.0f));
		}
		points = newPoints;
	}

	void moveDown() {
		vector<Point> newPoints;
		for (Point point : points) {
			newPoints.push_back(Point(point.getX(), point.getY() + 5.0f));
		}
		points = newPoints;
	}

	void updateRatio(float ratioX, float ratioY) {
		vector<Point> newPoints;
		for (Point point : points) {
			newPoints.push_back(Point(point.getX()*ratioX, point.getY()*ratioY));
		}
		points = newPoints;
	}

	void move(int delX, int delY) {
		vector<Point> newPoints;
		for (Point point : points) {
			newPoints.push_back(Point(point.getX() + delX, point.getY() + delY));
		}
		points = newPoints;
	}

	vector<Point> getAllPoints() {
		return points;
	}

	Point getPoint(int offset) {
		return points.at(offset);
	}

	void addPoint(Point point) {
		this->points.push_back(point);
	}

	Point getCenterPoint() {
		int n = points.size();
		double area = 0.0f;
		double centerX = 0.0f;
		double centerY = 0.0f;

		for (int i = 0; i < n; ++i) {
			int j = (i + 1) % n;
			Point p1 = points.at(i);
			Point p2 = points.at(j);
			float p1X = p1.getX();
			float p1Y = p1.getY();
			float p2X = p2.getX();
			float p2Y = p2.getY();
			area += p1X * p2Y;
			area -= p2X * p1Y;
			centerX += (p1X + p2X) * (p1X * p2Y - p2X * p1Y);
			centerY += (p1Y + p2Y) * (p1X * p2Y - p2X * p1Y);
		}
		area *= 0.5;
		centerX /= 6.0 * area;
		centerY /= 6.0 * area;

		return Point(centerX, centerY);
	}
};

bool isComplete;
bool isRotating;
int windowWidth = 500;
int windowHeight = 500;
bool rightButtonPressed;
PointManager pointManager;
int angle;
int prevX;
int prevY;

void display();

void initVariable() {
	isRotating = false;
	isComplete = false;
	rightButtonPressed = false;
	angle = 0;
	pointManager = PointManager();
}

void timer(int value) {
	angle += 1;
	display();
	if (isRotating) {
		glutTimerFunc(10, timer, 0);
	}
}

void drawPoint(Point point, bool isCenter, Point centerPoint) {
	float relativeX = point.getX(windowWidth) - centerPoint.getX(windowWidth);
	float relativeY = point.getY(windowHeight) - centerPoint.getY(windowHeight);
	float x = relativeX * cos(angle * PI / 180) - relativeY * sin(angle * PI / 180) + centerPoint.getX(windowWidth);
	float y = relativeX * sin(angle * PI / 180) + relativeY * cos(angle * PI / 180) + centerPoint.getY(windowHeight);

	glPointSize(5.0f);
	glBegin(GL_POINTS);
	if(isCenter){
		glColor3f(1.0f, 1.0f, 0.0f);
	}
	else {
		glColor3f(0.0f, 0.0f, 0.0f);
	}
	glVertex2f(x, y);
	glEnd();
}

void drawLine(Point p1, Point p2, Point centerPoint) {
	float relativeP1x = p1.getX(windowWidth) - centerPoint.getX(windowWidth);
	float relativeP1y = p1.getY(windowHeight) - centerPoint.getY(windowHeight);
	float relativeP2x = p2.getX(windowWidth) - centerPoint.getX(windowWidth);
	float relativeP2y = p2.getY(windowHeight) - centerPoint.getY(windowHeight);

	float p1X = relativeP1x * cos(angle * PI / 180) - relativeP1y * sin(angle * PI / 180) + centerPoint.getX(windowWidth);
	float p1Y = relativeP1x * sin(angle * PI / 180) + relativeP1y * cos(angle * PI / 180) + centerPoint.getY(windowHeight);	
	float p2X = relativeP2x * cos(angle * PI / 180) - relativeP2y * sin(angle * PI / 180) + centerPoint.getX(windowWidth);
	float p2Y = relativeP2x * sin(angle * PI / 180) + relativeP2y * cos(angle * PI / 180) + centerPoint.getY(windowHeight);


	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(p1X, p1Y);
	glVertex2f(p2X, p2Y);
	glEnd();
}

void display(void) {
	glBegin(GL_CLEAR);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnd();

	Point prev = Point(-1,-1);
	int cnt = 0;

	vector<Point> points = pointManager.getAllPoints();

	Point centerPoint = Point(0, 0);
	if (points.size() >= 3) {
		centerPoint = pointManager.getCenterPoint();
	}
	for (Point point : points) {
		drawPoint(point, false, centerPoint);

		if (cnt > 0) {
			drawLine(point, prev, centerPoint);
		}

		prev = point;
		cnt++;
	}

	if (isComplete) {
		drawLine(points.front(), points.back(), centerPoint);
	}

	if (isRotating) {
		drawPoint(centerPoint, true, centerPoint);
	}

	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		if (isRotating) {
			break;
		}
		initVariable();
		break;
	case 'p':
		if (isRotating) {
			break;
		}
		if (pointManager.getSize() < 3) {
			fprintf(stdout, "You draw less than 3 points! Cannot make polygon");
		}
		else {
			isComplete = true;
		}
		break;
	case 'r':
		if (!isComplete) {
			break;
		}
		isRotating = !isRotating;
		if (isRotating)
			glutTimerFunc(100, timer, 0);
		break;
	case 'f':
		glutLeaveMainLoop();
		return;
	}
	display();
}

void special(int key, int x, int y) {
	if (!isComplete || isRotating) {
		return;
	}
	switch (key) {
		case GLUT_KEY_LEFT:
			pointManager.moveLeft();
			break;
		case GLUT_KEY_RIGHT:
			pointManager.moveRight();
			break;
		case GLUT_KEY_DOWN:
			pointManager.moveDown();
			break;
		case GLUT_KEY_UP:
			pointManager.moveUp();
			break;
	}
	display();
}


void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN) && (glutGetModifiers() == GLUT_ACTIVE_SHIFT)) {
		if (isComplete) {
			display();
			return;
		}
		pointManager.addPoint(Point(x, y));
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		prevX = x;
		prevY = y;
		rightButtonPressed = true;
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {
		rightButtonPressed = false;
	}

	display();
}

void mousemove(int x, int y) {
	if (rightButtonPressed && !isRotating && isComplete) {
		int del_x = x - prevX, del_y = y - prevY;
		pointManager.move(del_x, del_y);
		prevX = x;
		prevY = y;
	}
	display();
}


void reshape(int width, int height) {
	if (height == 0) {
		height = 1;
	}
	pointManager.updateRatio(1.0f * width / windowWidth, 1.0f * height / windowHeight);
	windowWidth = width, windowHeight = height;
	glViewport(0, 0, width, height);
}

void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
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

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

void init(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 HW1-S20171612";
	char messages[4][256] = {
		"    - Keys used: 'r', 'p', 'c', 's', 'q'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: L-click, R-click and move",
		"    - Other operations: window size change"
	};

	initVariable();

	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(500, 200);
	glutCreateWindow(program_name);

	greetings(program_name, messages, 4);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutMainLoop();


	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
}

int main(int argc, char* argv[]) {
	init(argc, argv);
	return 0;
}