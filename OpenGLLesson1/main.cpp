#include "glut.h"
#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <functional>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

#define COLOR_WHITE 1.0, 1.0, 1.0
#define COLOR_GRAY 0.3, 0.3, 0.3
#define COLOR_RED 0.7, 0.2, 0.2
#define COLOR_YELLOW 0.8, 0.8, 0.1
#define COLOR_BLUE 0.1, 0.1, 0.8
#define COLOR_ORANGE 1.0, 0.64, 0.0

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
	double I;
	element() {
		type = 0;
		color = -1;
		I = 0;
	}
	element& nextType() {
		++type;
		I = 0;
		if (type == TYPES)
			type = 0;
		if (type == 1)
			R = 1.0;
		if (type == 2)
			EMF = 1;
		return *this;
	}
};

element* horizontal_elements[N][N];
element* vertical_elements[N][N];
bool calculating = false;

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
		case 3:
			glColor3f(COLOR_BLUE);
			break;
		case 4:
			glColor3f(COLOR_ORANGE);
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

void DrawHorizontal(int i, int j, element* e, int color =-2) {
	int keep = -2;
	if (e != NULL && color != -2) {
		keep = e->color;
		e->color = color;
	}		
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
		if (calculating) {
			glColor3f(COLOR_ORANGE);
			double current = e->I;
			if (current > 0) {
				DrawLine(xl, y, xl + 0.2 / N, y + 0.1 / N);
				DrawLine(xl, y, xl + 0.2 / N, y - 0.1 / N);
			}
			else if (current < 0) {
				DrawLine(xr, y, xr - 0.2 / N, y + 0.1 / N);
				DrawLine(xr, y, xr - 0.2 / N, y - 0.1 / N);
			}
		}
	} else if (e->type == 1) {
		setColor(e->color);
		double y = (.0 + i) / N;
		double xl = (.0 + j) / N;
		double xr = (1.0 + j) / N;
		DrawLine(xl, y, (3 * xl + 1 * xr) / 4, y);
		DrawLine(xr, y, (1 * xl + 3 * xr) / 4, y);
		DrawRect((3 * xl + 1 * xr) / 4, y - RGAPY / 5, (1 * xl + 3 * xr) / 4, y + RGAPY / 5);
		if (calculating) {
			glColor3f(COLOR_ORANGE);
			double current = e->I;
			if (current > 0) {
				DrawLine(xl, y, xl + 0.2 / N, y + 0.1 / N);
				DrawLine(xl, y, xl + 0.2 / N, y - 0.1 / N);
			}
			else if (current < 0) {
				DrawLine(xr, y, xr - 0.2 / N, y + 0.1 / N);
				DrawLine(xr, y, xr - 0.2 / N, y - 0.1 / N);
			}
		}
	} else if (e->type == 2) {
		setColor(e->color);
		double y = (.0 + i) / N;
		double xl = (.0 + j) / N;
		double xr = (1.0 + j) / N;
		DrawLine(xl, y, (3 * xl + 1 * xr) / 4, y);
		DrawLine(xr, y, (1 * xl + 3 * xr) / 4, y);
		if (abs(e->EMF) < EPS) {
			e->type = 0; //this is not EMF, this is a wire
			DrawHorizontal(i, j, e);
		} else if (e->EMF > 0) {
			DrawLine((3 * xl + 1 * xr) / 4, y - RGAPY / 3, (3 * xl + 1 * xr) / 4, y + RGAPY / 3);
			DrawLine((1 * xl + 3 * xr) / 4, y - RGAPY / 6, (1 * xl + 3 * xr) / 4, y + RGAPY / 6);
		} else if (e->EMF < 0) {
			DrawLine((3 * xl + 1 * xr) / 4, y - RGAPY / 6, (3 * xl + 1 * xr) / 4, y + RGAPY / 6);
			DrawLine((1 * xl + 3 * xr) / 4, y - RGAPY / 3, (1 * xl + 3 * xr) / 4, y + RGAPY / 3);
		}
		if (calculating) {
			glColor3f(COLOR_ORANGE);
			double current = e->I;
			if (current > 0) {
				DrawLine(xl, y, xl + 0.2 / N, y + 0.1 / N);
				DrawLine(xl, y, xl + 0.2 / N, y - 0.1 / N);
			}
			else if (current < 0) {
				DrawLine(xr, y, xr - 0.2 / N, y + 0.1 / N);
				DrawLine(xr, y, xr - 0.2 / N, y - 0.1 / N);
			}
		}
	}
	if (e != NULL && color != -2) {
		e->color = keep;
	}
}

void DrawVertical(int i, int j, element* e, int color = -2) {
	int keep = -2;
	if (e != NULL && color != -2) {
		keep = e->color;
		e->color = color;
	}
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
		if (calculating) {
			glColor3f(COLOR_ORANGE);
			double current = e->I;
			if (current > 0) {
				DrawLine(x, yl, x + 0.1 / N, yl + 0.2 / N);
				DrawLine(x, yl, x - 0.1 / N, yl + 0.2 / N);
			}
			else if (current < 0) {
				DrawLine(x, yr, x + 0.1 / N, yr - 0.2 / N);
				DrawLine(x, yr, x - 0.1 / N, yr - 0.2 / N);
			}
		}
	} else if (e->type == 1) {
		setColor(e->color);
		double x = (.0 + i) / N;
		double yl = (.0 + j) / N;
		double yr = (1.0 + j) / N;
		DrawLine(x, (3 * yl + 1 * yr) / 4, x, yl);
		DrawLine(x, (1 * yl + 3 * yr) / 4, x, yr);
		DrawRect(x - RGAPX / 5, (3 * yl + 1 * yr) / 4, x + RGAPX / 5, (1 * yl + 3 * yr) / 4);
		if (calculating) {
			glColor3f(COLOR_ORANGE);
			double current = e->I;
			if (current > 0) {
				DrawLine(x, yl, x + 0.1 / N, yl + 0.2 / N);
				DrawLine(x, yl, x - 0.1 / N, yl + 0.2 / N);
			}
			else if (current < 0) {
				DrawLine(x, yr, x + 0.1 / N, yr - 0.2 / N);
				DrawLine(x, yr, x - 0.1 / N, yr - 0.2 / N);
			}
		}
	} else if (e->type == 2) {
		setColor(e->color);
		double x = (.0 + i) / N;
		double yl = (.0 + j) / N;
		double yr = (1.0 + j) / N;
		DrawLine(x, (3 * yl + 1 * yr) / 4, x, yl);
		DrawLine(x, (1 * yl + 3 * yr) / 4, x, yr);
		//TODO: check for EMF, reverse object direction.
		if (abs(e->EMF) < EPS) {
			e->type = 0; //this is not EMF, this is a wire
			DrawVertical(i, j, e);
		}
		else if (e->EMF > 0) {
			DrawLine(x - RGAPX / 6, (1 * yl + 3 * yr) / 4, x + RGAPX / 6, (1 * yl + 3 * yr) / 4);
			DrawLine(x - RGAPX / 3, (3 * yl + 1 * yr) / 4, x + RGAPX / 3, (3 * yl + 1 * yr) / 4);
		}
		else if (e->EMF < 0) {
			DrawLine(x - RGAPX / 3, (1 * yl + 3 * yr) / 4, x + RGAPX / 3, (1 * yl + 3 * yr) / 4);
			DrawLine(x - RGAPX / 6, (3 * yl + 1 * yr) / 4, x + RGAPX / 6, (3 * yl + 1 * yr) / 4);
		}
		if (calculating) {
			glColor3f(COLOR_ORANGE);
			double current = e->I;
			if (current > 0) {
				DrawLine(x, yl, x + 0.1 / N, yl + 0.2 / N);
				DrawLine(x, yl, x - 0.1 / N, yl + 0.2 / N);
			}
			else if (current < 0) {
				DrawLine(x, yr, x + 0.1 / N, yr - 0.2 / N);
				DrawLine(x, yr, x - 0.1 / N, yr - 0.2 / N);
			}
		}
	}
	if (e != NULL && color != -2) {
		e->color = keep;
	}
}

bool showing = false;
bool editing = false;
bool canCalc = true;
element* last = NULL;
double keepx, keepy;
string prefix, suffix;
string keep;
element* currentEdit;

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
	currentEdit = vertical_elements[idx][idy];
	int type = currentEdit->type;
	if (type == 1) {
		prefix = "R =";
		keep = doubleToStr(currentEdit->R);
		suffix = " ohm";
	}
	else if (type == 2) {
		prefix = "EMF =";
		keep = doubleToStr(currentEdit->EMF);
		suffix = " v.";
	}
}

void editHorizontal(int idy, int idx) {
	cerr << "Want to edit horizontal " << idy << " " << idx << endl;
	if (horizontal_elements[idy][idx] == NULL || horizontal_elements[idy][idx]->type == 0)
		return;
	editing = true;
	horizontal_elements[idy][idx]->color = 2;
	keepx = idx;
	keepy = idy - .5;

	currentEdit = horizontal_elements[idy][idx];
	int type = currentEdit->type;
	if (type == 1) {
		prefix = "R =";
		keep = doubleToStr(currentEdit->R);
		suffix = " ohm";
	}
	else if (type == 2) {
		prefix = "EMF =";
		keep = doubleToStr(currentEdit->EMF);
		suffix = " v.";
	}
}

struct edge {
	int vertex;
	element* el;
	edge(int vertex, element* element) :
		vertex(vertex),
		el(element)
	{}
};

bool reCalculate(vector <vector <edge> > &g, map <pair <int, int>, int> &mp) {
	vector <vector <edge> > v = g;
	vector <vector <pair <element*, int> > > cycles; //<element, direction>
	map <element*, int> id;
	{
		int n = g.size();
		for (int i = 0; i < n; ) {
			vector <char> used(n);
			vector <pair <element*, int>> cycle;
			vector <pair <element*, int> > pr(n);
			vector <int> pv(n);
			int c_start, c_end;
			std::function<bool(int, int)> dfs;
			dfs = [&dfs, &used, &v, &cycle, &pr, &c_start, &c_end, &pv] (int cur, int p) {
				used[cur] = true;
				for (auto it = v[cur].begin(); it != v[cur].end(); ++it) {
					auto e = *it;
					int to = e.vertex;
					if (to == p)
						continue;
					if (to < cur) {
						pr[to] = { e.el, -1 };
						pv[to] = cur;
					} else {
						pr[to] = { e.el, 1 };
						pv[to] = cur;
					}
					if (used[to]) {
						c_start = to;
						c_end =   cur;
						cycle.push_back(pr[to]);
						v[cur].erase(it);
						for (auto it2 = v[to].begin(); it2 != v[to].end(); ++it2)
							if (it2->vertex == cur) {
								v[to].erase(it2);
								return true;
							}
						assert(false, "Backward edge doesn't exist");
					}
					if (dfs(to, cur))
						return true;
				}
				return false;
			};
			if (dfs(i, -1)) {
				for (; c_end != c_start; c_end = pv[c_end]) 
					cycle.push_back(pr[c_end]);
				for (auto &x: cycle) {
					if (id.count(x.first) == 0)
						id[x.first] = id.size();
				}
				cycles.push_back(cycle);
			} else {
				++i;
			}
		}
	}
	int m = (int)id.size();
	vector < vector<long double> > a; //for gauss
	for (auto& x : cycles) {
		a.push_back(vector<long double>(m + 1, 0));
		for (auto& el : x) {
			if (el.first->type == 0)
				continue;
			int index = id[el.first];
			if (el.first->type == 1) {
				a.back()[index] = el.second * el.first->R;
			} else if (el.first->type == 2) {
				a.back()[m] += el.second * el.first->EMF;			
			}
		}
	}
	for (int i = 0; i < g.size(); ++i) {
		auto& x = g[i];
		bool need = false;
		for (auto &y : x) {
			if (id.count(y.el)) {
				need = true;
				break;
			}
		}
		if (need) {
			a.push_back(vector<long double>(m + 1, 0));
			for (auto &y : x) {
				if (!id.count(y.el))
					continue;
				int index = id[y.el];
				int dir = (i < y.vertex) * 2 - 1;
				a.back()[index] = dir;
			}
		}
	}
	vector <long double> ans;
	int n = (int)a.size();

	vector<int> where(m, -1);
	for (int col = 0, row = 0; col<m && row<n; ++col) {
		int sel = row;
		for (int i = row; i<n; ++i)
			if (abs(a[i][col]) > abs(a[sel][col]))
				sel = i;
		if (abs(a[sel][col]) < EPS)
			continue;
		for (int i = col; i <= m; ++i)
			swap(a[sel][i], a[row][i]);
		where[col] = row;

		for (int i = 0; i < n; ++i)
			if (i != row) {
				long double c = a[i][col] / a[row][col];
				for (int j = col; j <= m; ++j)
					a[i][j] -= a[row][j] * c;
			}
		++row;
	}

	ans.assign(m, 0);
	for (int i = 0; i < m; ++i)
		if (where[i] != -1)
			ans[i] = a[where[i]][m] / a[where[i]][i];

	for (int i = 0; i < n; ++i) {
		double sum = 0;
		for (int j = 0; j < m; ++j)
			sum += ans[j] * a[i][j];
		if (abs(sum - a[i][m]) > EPS)
			return false;
	}

	for (int i = 0; i < m; ++i)
		if (where[i] == -1)
			return false; //?can this happen?

	for (auto &x : id) {
		x.first->I = ans[x.second];
	}
	return true;
}

void stopEditing() {
	editing = false;
	int type = currentEdit->type;
	try {
		double thing = stod(keep);
		if (type == 1) {
			currentEdit->R = thing;
		} else if (type == 2) {
			currentEdit->EMF = thing;
		}
		currentEdit->color = -1;
	} catch (exception e) {

	}
}

void Draw(string writeOnTop = "", element* selected = NULL) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	if (writeOnTop != "") {
		setColor(4);
		glRasterPos2f(0, 1.0 - 24.0 / windowY);
		for (auto x : writeOnTop) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, x);
		}
	}
	if (editing) {
		glColor3f(COLOR_YELLOW); 
		glRasterPos2f((.0 + keepx) / N, (.0 + keepy) / N);
		for (auto x : prefix)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, x);
		for (auto x : keep)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, x);
		for (auto x : suffix)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, x);
	}
	if (calculating) {
		map <pair <int, int>, int> mp;
		for (int i = 1; i < N; ++i) {
			for (int j = 1; j < N; ++j) {
				mp[{i, j}] = mp.size();			
			}
		}
		vector <vector <edge> > v(mp.size());
		for (int i = 1; i < N; ++i)
			for (int j = 1; j < N - 1; ++j) {
				//literally {i, j} -> {i, j + 1}
				element* tmp = horizontal_elements[i][j];
				if (tmp != NULL) {
					//horizontal_elements[i][j]->color = 3;
					tmp->I = 0;
					int l = mp[{i, j}];
					int r = mp[{i, j + 1}];
					v[l].push_back(edge(r, tmp));
					v[r].push_back(edge(l, tmp));
				}
			}
		for (int j = 1; j < N; ++j)
			for (int i = 1; i < N - 1; ++i) {
				//literally {i, j} -> {i + 1, j}
				element* tmp = vertical_elements[j][i];
				if (tmp != NULL) {					
					//tmp->color = 3;
					tmp->I = 0;
					int l = mp[{i, j}];
					int r = mp[{i + 1, j}];
					v[l].push_back(edge(r, tmp));
					v[r].push_back(edge(l, tmp));
				}
			}
		if (!reCalculate(v, mp)) {
			setColor(1);
			glRasterPos2f(0, 1.0 - 24.0 / windowY );
			string tmp = "Unable to solve the circuit for currents.";
			for (auto x : tmp) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, x);
			}				
			canCalc = false;
			last = NULL;
		} else {
			canCalc = true;
		}
	}
	for (int i = 1; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			if (horizontal_elements[i][j] == selected)
				DrawHorizontal(i, j, horizontal_elements[i][j], 1);
			else
				DrawHorizontal(i, j, horizontal_elements[i][j]);
			if (vertical_elements[i][j] == selected)
				DrawVertical  (i, j, vertical_elements[i][j], 1);
			else
				DrawVertical(i, j, vertical_elements[i][j]);
		}
	}
	glFlush();
}

void Draw2() {
	Draw();
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
		calculating = 0;
		canCalc = false;
		last = NULL;
		if (button == 0 && state == 1)
			processClickOnVertical(idx, idy, 1);
		if (button == 2 && state == 1)
			processClickOnVertical(idx, idy, 2);
	} else if (distY <= GAPY / 4) {
		int idx = x / GAPX;
		int idy = closestY / GAPY;
		calculating = 0;
		canCalc = false;
		last = NULL;
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
		calculating = 0;
		canCalc = false;
		last = NULL;
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
	} else if (key == 'q') {
		calculating ^= 1;
		canCalc = false;
	} else if (key == 'r') {
		if (canCalc) {
			showing ^= 1;
		}
	}
	Draw();
}



void processMouseActiveMotion(int x, int y) {
	//cerr << "Active: " << x << " " << y << endl;
}


void processMousePassiveMotion(int x, int y) {
	cerr << "Passive: " << x << " " << y << endl;
	if (!canCalc)
		return;
	element* e = NULL;
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
		e = vertical_elements[idx][idy];
	}
	else if (distY <= GAPY / 4) {
		int idx = x / GAPX;
		int idy = closestY / GAPY;
		e = horizontal_elements[idy][idx];
	}
	else {
		cerr << "Nothing\n";
	}
	if (e != last) {
		if (e != NULL) {
			string prefix;
			if (e->type == 0) {
				prefix = "Current through wire: ";
			} else if (e->type == 1) {
				prefix = "Current through resistor: ";
			} else if (e->type == 2) {
				prefix = "Current through EMF: ";		
			}
			double val = abs(e->I);
			string ext = "A";
			if (val < 1 - EPS) {
				val *= 1000;
				ext = "mA";
			} else if (val >= 1000 + EPS) {
				val /= 1000;
				ext = "MA";
			}
			string suffix = doubleToStr(val);
			Draw(prefix + suffix + ext, e);
		}
		last = e;
	}
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
	glutDisplayFunc(Draw2);
	glutMouseFunc(processMouse);
	glutKeyboardFunc(processKeyboard);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutEntryFunc(processMouseEntry);
	glutMainLoop();
	return 0;
}