#include "includesBranchCut.h"

using namespace std;


void printdata() {
	cout << endl;

	for (int j = 0; j < mm; j++) {
		for (int i = 0; i < nn; i++) cout << c[j][i] << " ";
		cout << endl;
	}
	cout << endl;

	for (int j = 0; j < mm; j++) {
		for (int i = 0; i < nn; i++) cout << a[j][i] << " ";
		cout << endl;
	}
	cout << endl;

	for (int j = 0; j < mm; j++) cout << b[j] << " ";
	cout << endl << endl;
	cout << nn << " " << mm << endl;
	cout << endl << endl;
}


//parser les instances
void parse(string s) {
	//reinitialiser la memoire
	for (int i = 0; i < 81; i++) {
		memset(a[i], 0, sizeof(a[i]));
		memset(c[i], 0, sizeof(c[i]));
	}
	memset(b, 0, sizeof(b));

	//obtenir tous les caract�res de l'instance
	ifstream f;
	f.open(s.c_str());
	stringstream ss;
	char ch;
	while (f.get(ch)) {
		if ((ch - '0' >= 0 && ch - '0' <= 9)) ss << ch;
		else ss << ' ';
	}

	//parse n & m
	ss >> nn >> mm;

	//parse c
	for (int j = 0; j < mm; j++)
		for (int i = 0; i < nn; i++) ss >> c[j][i];
	for (int i = 0; i < nn; i++) c[mm][i] = 100000;

	//parse a
	for (int j = 0; j < mm; j++)
		for (int i = 0; i < nn; i++) ss >> a[j][i];
	for (int i = 0; i < nn; i++) a[mm][i] = 1;

	//parse b
	for (int j = 0; j < mm; j++) ss >> b[j];
	b[mm] = INT_MAX;
}


int mostFracVar(IloNumArray & values) {
	int result = -1;
	double gap = RC_EPS;
	for (int k = 0; k < mm * nn; k++) {
		if (abs(values[k] - floor(values[k] + 0.5)) > gap) {
			gap = abs(values[k] - floor(values[k] + 0.5));
			result = k;
		}
	}
	return result;
}
