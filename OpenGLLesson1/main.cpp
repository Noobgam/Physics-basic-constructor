#include "glut.h"
#include <Windows.h>
#include <iostream>
#include <algorithm>

using namespace std;

#define COLOR_WHITE 1.0, 1.0, 1.0
#define COLOR_GRAY 0.3, 0.3, 0.3
#define COLOR_RED 0.7, 0.2, 0.2

const int N = 20;
const int windowX = 800, windowY = 600;
const int GAPY = windowY / N;
const int GAPX = windowX / N;
const double RGAPX = (.0 + GAPX) / windowX;
const double RGAPY = (.0 + GAPY) / windowY;

struct element {
	const int TYPES = 2;
	int type;
	int color;
	element() {
		type = 0;
		color = -1;
	}
	element& nextType() {
		++type;
		if (type == TYPES) 
			type = 0;
		return *this;
	}
};

element* horizontal_elements[N][N];
element* vertical_elements[N][N];

void setColor(int color) {
	if (color == 1)
		glColor3f(COLOR_RED);
	else
		glColor3f(COLOR_WHITE);
}

void DrawLine(double x1, double y1, double x2, double y2) {
	glBegin(GL_LINES);
	glVertex3f(x1, y1, 0.0);
	glVertex3f(x2, y2, 0.0);
	glEnd();
}

void DrawRect(double x1, double y1, double x2, double y2) {
	DrawLine(x1, y1, x2, y1);
	DrawLine(x2, y1, x2, y2);
	DrawLine(x2, y2, x1, y2);
	DrawLine(x1, y2, x1, y1);
}

void DrawHorizontal(int i, int j, element* e) {
	if (e == NULL) {
		glColor3f(COLOR_GRAY);
		double y  = (.0 +  i) / N;
		double xl = (.0 +  j) / N;
		double xr = (1.0 + j) / N;
		DrawLine(xl, y, xr, y);
	} else if (e->type == 0) {
		setColor(e->color);
		double y = (.0 + i) / N;
		double xl = (.0 + j) / N;
		double xr = (1.0 + j) / N;
		DrawLine(xl, y, xr, y);
	} else if (e->type == 1) {
		setColor(e->color);
		double y = (.0 + i) / N;
		double xl = (.0 + j) / N;
		double xr = (1.0 + j) / N;
		DrawLine(xl, y, (3 * xl + 1 * xr) / 4, y);
		DrawLine(xr, y, (1 * xl + 3 * xr) / 4, y);
		DrawRect((3 * xl + 1 * xr) / 4, y - RGAPY / 5, (1 * xl + 3 * xr) / 4, y + RGAPY / 5);
	}
}

void DrawVertical(int i, int j, element* e) {
	if (e == NULL) {
		glColor3f(COLOR_GRAY);
		double x = (.0 + i) / N;
		double yl = (.0 + j) / N;
		double yr = (1.0 + j) / N;
		DrawLine(x, yl, x, yr);
	}
	else if (e->type == 0) {
		setColor(e->color);
		double x = (.0 + i) / N;
		double yl = (.0 + j) / N;
		double yr = (1.0 + j) / N;
		DrawLine(x, yl, x, yr);
	} else if (e->type == 1) {
		setColor(e->color);
		double x = (.0 + i) / N;
		double yl = (.0 + j) / N;
		double yr = (1.0 + j) / N;
		DrawLine(x, (3 * yl + 1 * yr) / 4, x, yl);
		DrawLine(x, (1 * yl + 3 * yr) / 4, x, yr);
		DrawRect(x - RGAPX / 5, (3 * yl + 1 * yr) / 4, x + RGAPX / 5, (1 * yl + 3 * yr) / 4);
	}
}

void Draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(COLOR_GRAY);
	for (int i = 1; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			DrawHorizontal(i, j, horizontal_elements[i][j]);
			DrawVertical  (i, j, vertical_elements[i][j]);
		}
	}
	glFlush();
}

void Initialize() {
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			horizontal_elements[i][j] = vertical_elements[i][j] = NULL;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void processClickOnVertical(int idx, int idy, int type) {
	if (type == 1) {
		if (vertical_elements[idx][idy] == NULL) {
			vertical_elements[idx][idy] = new element();
		} else {
			vertical_elements[idx][idy]->nextType();
		}
		cerr << "Vertical.\n";
	} else if (type == 2) { //this should keep being this way, right click deletes element
		cerr << "Vertical.\n";
		if (vertical_elements[idx][idy] == NULL) 
			return;
		else {
			delete vertical_elements[idx][idy];
			vertical_elements[idx][idy] = NULL;
		}
	}
}

void processClickOnHorizontal(int idy, int idx, int type) {
	if (type == 1) {
		if (horizontal_elements[idy][idx] == NULL) {
			horizontal_elements[idy][idx] = new element();
		} else {
			horizontal_elements[idy][idx]->nextType();
		}
		cerr << "Horizontal.\n";
	}
	else if (type == 2) { //this should keep being this way, right click deletes element
		cerr << "Horizontal.\n";
		if (horizontal_elements[idy][idx] == NULL)
			return;
		else {
			delete horizontal_elements[idy][idx];
			horizontal_elements[idy][idx] = NULL;
		}
	}
}

void processMouse(int button, int state, int x, int y) {
	cerr << "Simple: " << button << " " << state << " " << x << " " << y << endl;
	y = windowY - y;
	int closestX;
	if (x <= GAPX) 
		closestX = GAPX;
	else if (x >= windowX - GAPX)
		closestX = windowX - GAPX;
	else {
		int l = x / GAPX * GAPX;
		int r = (x + GAPX - 1) / GAPX * GAPX;
		if (x - l <= r - x)
			closestX = l;
		else
			closestX = r;
	} 
	int distX = abs(x - closestX);

	int closestY;
	if (y <= GAPY)
		closestY = GAPY;
	else if (y >= windowY - GAPY)
		closestY = windowY - GAPY;
	else {
		int l = y / GAPY * GAPY;
		int r = (y + GAPY - 1) / GAPY * GAPY;
		if (y - l <= r - y)
			closestY = l;
		else
			closestY = r;
	}
	int distY = abs(y - closestY);
	cerr << distX << " : " << distX << endl;
	if (distX <= GAPX / 4 && distY <= GAPY / 4) {
		cerr << "Intersection.\n";
	} else if (distX <= GAPX / 4) {
		int idx = closestX / GAPX;
		int idy = y / GAPY;
		if (button == 0 && state == 1)
			processClickOnVertical(idx, idy, 1);
		if (button == 2 && state == 1)
			processClickOnVertical(idx, idy, 2);
	} else if (distY <= GAPY / 4) {
		int idx = x / GAPX;
		int idy = closestY / GAPY;
		if (button == 0 && state == 1)
			processClickOnHorizontal(idy, idx, 1);
		if (button == 2 && state == 1)
			processClickOnHorizontal(idy, idx, 2);
		cerr << "Horizontal.\n";	
	} else {
		cerr << "Nothing\n";
	}
	//searching for vertical thing
	Draw();
	//ReDraw();
}

void processMouseActiveMotion(int x, int y) {
	//cerr << "Active: " << x << " " << y << endl;
}

void processMousePassiveMotion(int x, int y) {
	//cerr << "Passive: " << x << " " << y << endl;
}

void processMouseEntry(int state) {
	//cerr << "Entry: " << state << endl;
}

int main(int iArgc, char** cppArgv) {
	//FreeConsole();
	glutInit(&iArgc, cppArgv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(windowX, windowY);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("XoaX.net");
	Initialize();
	glutDisplayFunc(Draw);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutEntryFunc(processMouseEntry);
	glutMainLoop();
	return 0;
}