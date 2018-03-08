#include "includesColGen.h"

using namespace std;


// move task from machine[task] to newMachine
void moveTask(int task, int newMachine) {
	int curMachine = machine[task];
	br[curMachine] += a[curMachine][task];
	br[newMachine] -= a[newMachine][task];
	cost += c[newMachine][task] - c[curMachine][task];
	machine[task] = newMachine;
}


//heuristique pour obtenir une solution initiale
int heuristique() {
	printf("Heuristique... ");
	cout << flush;
	clock_t start = clock();
	cost = 0;

	// sorting the tasks: a task is to be dealt with early if it has a great potential resource cost
	vector<pair<int, int> > sorted(nn);
	for (int i = 0; i < nn; i++) {
		int worst_a = 0;
		for (int j = 0; j < mm; j++) {
			if (worst_a < a[j][i]) worst_a = a[j][i];
		}
		sorted[i] = make_pair(worst_a, i);
	}
	sort(sorted.begin(), sorted.end(), greater<pair<int, int> >());


	//init
	int nbOnVirtualMachine = 0;
	for (int i = 0; i < nn; i++) {
		int taskIndex = sorted[i].second;
		int min_a = 100000;
		int minIndex = mm;
		for (int j = 0; j < mm; j++) {
			if (min_a > a[j][taskIndex] && a[j][taskIndex] <= br[j]) {
				min_a = a[j][taskIndex];
				minIndex = j;
			}
		}
		machine[taskIndex] = minIndex;
		if (minIndex == mm) {
			min_a = 1;
			nbOnVirtualMachine++;
		}
		br[minIndex] -= min_a;
		cost += c[minIndex][taskIndex];
	}

	// algo: simulated annealing
	int curTask, curMachine, bestMachine, bestTask, costDiff, bestDiff;
	double T = 1.;
	while ((clock() - start) / (double)CLOCKS_PER_SEC < 1) {
		for (int r = 0; r < mm; r++) {			// (r < m) arbitrary
			curTask = rand() % nn;
			curMachine = machine[curTask];
			bestDiff = INT_MAX;

			// try and move task from a machine to another (probability 0.5)
			if (rand() % 2 == 0) {
				bestMachine = curMachine;
				for (int j = 0; j < mm; j++) {
					if (j == curMachine) continue;
					if (a[j][curTask] <= br[j] && c[j][curTask] - c[curMachine][curTask] < bestDiff) {
						bestMachine = j;
						bestDiff = c[j][curTask] - c[curMachine][curTask];
					}
				}
				// move curTask from curMachine to bestMachine if it reduces the cost, or with a certain probability depending on T
				if (bestDiff < 0 || (bestDiff < INT_MAX && ((double)rand() / RAND_MAX) < exp(-cost / T)))
					moveTask(curTask, bestMachine);
			}

			// or try and swap two tasks (probability 0.5)
			else {
				for (int i = 0; i < nn; i++) {
					// curTaks and i can be swapped
					if (machine[i] != curMachine && a[curMachine][i] - a[curMachine][curTask] <= br[curMachine]
						&& a[machine[i]][curTask] - a[machine[i]][i] <= br[machine[i]]) {
						costDiff = c[curMachine][i] + c[machine[i]][curTask] - c[machine[i]][i] - c[curMachine][curTask];
						if (costDiff < bestDiff) {
							bestDiff = costDiff;
							bestTask = i;
						}
					}
				}
				// swap curTask and bestTask if it reduces the cost, or with a certain probability depending on T
				if (bestDiff < 0 || (bestDiff < INT_MAX && ((double)rand() / RAND_MAX) < exp(-cost / T))) {
					moveTask(curTask, machine[bestTask]);
					moveTask(bestTask, curMachine);
				}
			}
		}
		T *= 0.9; // arbitrary
	}
	printf("Terminee\n");
	cout << flush;
	return cost;
}


// afficher la solution courante
void printcursol() {
	for (int j = 0; j < mm; j++) {
		for (int i = 0; i < nn; i++) {
			cout << s_init[j][i];
		}
		cout << endl;
	}
	cout << cost << endl;
}


void printdata() {
	cout << endl;

	//parse c
	for (int j = 0; j < mm; j++) {
		for (int i = 0; i < nn; i++) cout << c[j][i] << " ";
		cout << endl;
	}
	cout << endl;
	//parse a
	for (int j = 0; j < mm; j++) {
		for (int i = 0; i < nn; i++) cout << a[j][i] << " ";
		cout << endl;
	}
	cout << endl;
	//parse b
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
		memset(s_init[i], 0, sizeof(s_init[i]));
	}
	memset(machine, 0, sizeof(machine));
	memset(br, 0, sizeof(br));
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

	for (int j = 0; j <= mm; j++) br[j] = b[j];
}
