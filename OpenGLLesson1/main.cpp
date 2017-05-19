#include "glut.h"
#include <Windows.h>
#include <iostream>
#include <ostream>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

#define COLOR_WHITE 1.0, 1.0, 1.0
#define COLOR_GRAY 0.3, 0.3, 0.3
#define COLOR_RED 0.7, 0.2, 0.2
#define COLOR_YELLOW 0.8, 0.8, 0.1

const int N = 10;
const long double EPS = 1e-6;
const int windowX = 800, windowY = 800;
const int GAPY = windowY / N;
const int GAPX = windowX / N;
const double RGAPX = (.0 + GAPX) / windowX;
const double RGAPY = (.0 + GAPY) / windowY;

struct element {
	const int TYPES = 3; //Type 0 - wire, Type 1 - Resistor, Type 2 - EMF
	int type;
	int color;
	double EMF;
	double R;
	element() {
		type = 0;
		color = -1;
	}
	element& nextType() {
		++type;
		if (type == TYPES)
			type = 0;
		if (type == 1)
			R = 0;
		if (type == 2)
			EMF = 1;
		return *this;
	}
};

element* horizontal_elements[N][N];
element* vertical_elements[N][N];

void setColor(int color) {
	switch (color) {
		case -1:
			glColor3f(COLOR_WHITE);
			break;
		case 1:
			glColor3f(COLOR_RED);
			break;
		case 2:
			glColor3f(COLOR_YELLOW);
			break;
		default:
			glColor3f(COLOR_GRAY);
			break;
	}
}

void DrawLine(double x1, double y1, double x2, double y2) {
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
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
	} else if (e->type == 2) {
		setColor(e->color);
		double y = (.0 + i) / N;
		double xl = (.0 + j) / N;
		double xr = (1.0 + j) / N;
		DrawLine(xl, y, (3 * xl + 1 * xr) / 4, y);
		DrawLine(xr, y, (1 * xl + 3 * xr) / 4, y);
		if (abs(e->EMF) < EPS) {
			DrawLine(xl, y, xr, y);		
			e->type = 0; //this is not EMF, this is a wire
		} else if (e->EMF > 0) {
			DrawLine((3 * xl + 1 * xr) / 4, y - RGAPY / 3, (3 * xl + 1 * xr) / 4, y + RGAPY / 3);
			DrawLine((1 * xl + 3 * xr) / 4, y - RGAPY / 6, (1 * xl + 3 * xr) / 4, y + RGAPY / 6);
		} else if (e->EMF < 0) {
			DrawLine((3 * xl + 1 * xr) / 4, y - RGAPY / 6, (3 * xl + 1 * xr) / 4, y + RGAPY / 6);
			DrawLine((1 * xl + 3 * xr) / 4, y - RGAPY / 3, (1 * xl + 3 * xr) / 4, y + RGAPY / 3);
		}
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
	} else if (e->type == 2) {
		setColor(e->color);
		double x = (.0 + i) / N;
		double yl = (.0 + j) / N;
		double yr = (1.0 + j) / N;
		DrawLine(x, (3 * yl + 1 * yr) / 4, x, yl);
		DrawLine(x, (1 * yl + 3 * yr) / 4, x, yr);
		//TODO: check for EMF, reverse object direction.
		if (abs(e->EMF) < EPS) {
			DrawLine(x, yl, x, yr);
			e->type = 0; //this is not EMF, this is a wire
		}
		else if (e->EMF > 0) {
			DrawLine(x - RGAPX / 3, (1 * yl + 3 * yr) / 4, x + RGAPX / 3, (1 * yl + 3 * yr) / 4);
			DrawLine(x - RGAPX / 6, (3 * yl + 1 * yr) / 4, x + RGAPX / 6, (3 * yl + 1 * yr) / 4);
		}
		else if (e->EMF < 0) {
			DrawLine(x - RGAPX / 6, (1 * yl + 3 * yr) / 4, x + RGAPX / 6, (1 * yl + 3 * yr) / 4);
			DrawLine(x - RGAPX / 3, (3 * yl + 1 * yr) / 4, x + RGAPX / 3, (3 * yl + 1 * yr) / 4);
		}
	}
}



bool editing = false;
double keepx, keepy;
string prefix, suffix;
string keep;
element* current;

string doubleToStr(double dbl) {
	string thing = to_string(dbl);
	if (thing.find('.') != string::npos) 
		while (thing.back() == '0' && thing[thing.size() - 2] != '.')
			thing.pop_back();
	return thing;
}

void editVertical(int idx, int idy) {
	cerr << "Want to edit vertical " << idx << " " << idy << endl;
	if (vertical_elements[idx][idy] == NULL || vertical_elements[idx][idy]->type == 0)
		return;
	editing = true;
	vertical_elements[idx][idy]->color = 2;
	keepx = idx;
	keepy = idy;
	current = vertical_elements[idx][idy];
	int type = current->type;
	if (type == 1) {
		prefix = "R =";
		keep = doubleToStr(current->R);
		suffix = " ohm";
	}
	else if (type == 2) {
		prefix = "EMF =";
		keep = doubleToStr(current->EMF);
		suffix = " v.";
	}
}

void editHorizontal(int idy, int idx) {
	cerr << "Want to edit horizontal " << idy << " " << idx << endl;
	if (horizontal_elements[idy][idx] == NULL)
		return;
	editing = true;
	horizontal_elements[idy][idx]->color = 2;
	keepx = idx;
	keepy = idy - .5;

	current = horizontal_elements[idy][idx];
	int type = current->type;
	if (type == 1) {
		prefix = "R =";
		keep = doubleToStr(current->R);
		suffix = " ohm";
	}
	else if (type == 2) {
		prefix = "EMF =";
		keep = doubleToStr(current->EMF);
		suffix = " v.";
	}
}

void stopEditing() {
	editing = false;
	int type = current->type;
	try {
		double thing = stod(keep);
		if (type == 1) {
			current->R = thing;
		} else if (type == 2) {
			current->EMF = thing;
		}
		current->color = -1;
	} catch (exception e) {

	}
}

void Draw() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	if (editing) {
		glColor3f(COLOR_YELLOW); 
		glRasterPos2f((.0 + keepx) / N, (.0 + keepy) / N);
		for (auto x : prefix)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, x);
		for (auto x : keep)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, x);
		for (auto x : suffix)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, x);
		//glutPostRedisplay();
	}
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
	if (editing)
		return;
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
	Draw();
}

void processKeyboard(unsigned char key,	int x, int y) {
	cerr << (int)key << " " << x << " " << y << endl;
	if (editing) {
		if (key == 'e' || key == 13) {
			stopEditing();			
		} else if (key == 8) {
			keep.pop_back();
		} else if (key >= '0' && key <= '9') {
			keep.push_back(key);
		} else if (key == '.') {
			keep.push_back(key);
		} else if (key == '-') {
			keep.push_back(key);
		}
	} else if (key == 'e') {
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
		}
		else if (distX <= GAPX / 4) {
			int idx = closestX / GAPX;
			int idy = y / GAPY;
			editVertical(idx, idy);
		}
		else if (distY <= GAPY / 4) {
			int idx = x / GAPX;
			int idy = closestY / GAPY;
			editHorizontal(idy, idx);
			cerr << "Horizontal.\n";
		}
		else {
			cerr << "Nothing\n";
		}
	} 
	Draw();
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
	//glutInitWindowPosition(200, 200);
	glutCreateWindow("Конструктор схем");
	Initialize();
	glutDisplayFunc(Draw);
	glutMouseFunc(processMouse);
	glutKeyboardFunc(processKeyboard);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutEntryFunc(processMouseEntry);
	glutMainLoop();
	return 0;
}