#include "includesHeuristique.h"
using namespace std;


bool isnumber(char ch) {
	return ((ch - '0' >= 0 && ch - '0' <= 9) || ch == ' ');
}


void parse(string s) {
	ifstream f;
	f.open(s.c_str());
	stringstream ss;
	char ch;
	while (f.get(ch)) {
		if (isnumber(ch)) ss << ch;
		else ss << ' ';
	}

	//parse n & m
	ss >> n >> m;

	//parse c
	for (int j = 0; j < m; j++)
		for (int i = 0; i < n; i++) ss >> c[i][j];
	for (int i = 0; i < n; i++) c[i][m] = 100000;

	//parse a
	for (int j = 0; j < m; j++)
		for (int i = 0; i < n; i++) ss >> a[i][j];
	for (int i = 0; i < n; i++) a[i][m] = 1;

	//parse b
	for (int j = 0; j < m; j++) ss >> b[j];
	b[m] = INT_MAX;

	for (int j = 0; j <= m; j++) br[j] = b[j];
}

