#include "includesHeuristique.h"

using namespace std;
using namespace std::chrono;

#define D false

int n, m, cost, a[1600][81], b[81], br[81], c[1600][81], machine[1600];

void moveTask(int task, int newMachine);
int heuristique(int millisec);
void printSol();

int main() {
	srand(time(NULL));

	string instance;
	int millisec;
	printf("Entrez l'instance que vous voulez resoudre (par exemple 'GAP-a05100.dat') : ");
	cin >> instance;
	printf("Temps d'execution (en ms) ? ");
	cin >> millisec;

	parse("GAP/"+instance);
	printf("Solution de valeur %d\n", heuristique(millisec));
	printSol();
	return 0;
}


int heuristique(int millisec) {

	cost = 0;

	// sorting the tasks: a task is to be dealt with early if it has a great potential resource cost
	vector<pair<int, int> > sorted(n);
	for (int i = 0; i < n; i++) {
		int worst_a = 0;
		for (int j = 0; j < m; j++) {
			if (worst_a < a[i][j]) worst_a = a[i][j];
		}
		sorted[i] = make_pair(worst_a, i);
	}
	sort(sorted.begin(), sorted.end(), greater<>());


	//init
	int nbOnVirtualMachine = 0;
	for (int i = 0; i < n; i++) {
		int taskIndex = sorted[i].second;
		int min_a = INT_MAX;
		int minIndex = m;
		for (int j = 0; j < m; j++) {
			if (min_a > a[taskIndex][j] && a[taskIndex][j] <= br[j]) {
				min_a = a[taskIndex][j];
				minIndex = j;
			}
		}
		machine[taskIndex] = minIndex;
		if (minIndex == m) {
			min_a = 1;
			nbOnVirtualMachine++;
		}
		br[minIndex] -= min_a;
		cost += c[taskIndex][minIndex];
	}



	// remove tasks from virtual machine
	if (nbOnVirtualMachine > 0) {
		// all tasks that could not be put on a real machine
		list<int> virtualMachine;
		for (int i = 0; i < n; i++)
			if (machine[i] == m) virtualMachine.push_back(i);

		int curTask, curMachine;

		while (nbOnVirtualMachine > 0) {
			curTask = rand() % n;
			while (machine[curTask] == m) curTask = (curTask + 1) % n;

			// try and move a task from a real machine to another
			if (rand() % 2 == 0) {
				for (int j = 0; j < m; j++) {
					curMachine = machine[curTask];
					// current task can be moved from current machine to machine j
					if (a[curTask][j] <= br[j]) {
						moveTask(curTask, j);
						// a new task can be put on curMachine
						if (a[virtualMachine.front()][curMachine] <= br[curMachine]) {
							int newTask = virtualMachine.front();
							virtualMachine.pop_front();
							nbOnVirtualMachine--;
							moveTask(newTask, curMachine);
							break;
						}
					}
				}
			}
			// try and swap two tasks so as to free some space on a machine and put a new task on it
			else {
				for (int i = 0; i < n; i++) {
					curMachine = machine[curTask];
					// swapping curTask and i frees some space on machine[i] or machine[curTaks]
					if (a[i][curMachine] - a[curTask][curMachine] <= br[curMachine]
						&& a[curTask][machine[i]] - a[i][machine[i]] <= br[machine[i]]
						&& (a[curTask][curMachine] > a[i][curMachine] || a[curTask][machine[i]] < a[i][machine[i]])) {
						moveTask(curTask, machine[i]);
						moveTask(i, curMachine);
						// a new task can be put on a real machine
						if (a[virtualMachine.front()][machine[i]] <= br[machine[i]]
							|| a[virtualMachine.front()][machine[curTask]] <= br[machine[curTask]]) {
							int newTask = virtualMachine.front();
							virtualMachine.pop_front();
							int machineNew = curMachine;
							if (a[virtualMachine.front()][machine[i]] > br[machine[i]]) machineNew = machine[curTask];
							nbOnVirtualMachine--;
							moveTask(newTask, machineNew);
							break;
						}
					}
				}
			}
		}
	}

  // algo: simulated annealing
  int curTask, curMachine, bestMachine, bestTask, costDiff, bestDiff;
  double T = 1.;
  auto start = steady_clock::now();
  while (duration_cast<chrono::milliseconds>(steady_clock::now() - start).count() < millisec) {
      for (int r = 0; r < m; r++) {			// (r < m) arbitrary
          curTask = rand() % n;
          curMachine = machine[curTask];
          bestDiff = INT_MAX;

          // try and move task from a machine to another (probability 0.5)
          if (rand() % 2 == 0) {
              bestMachine = curMachine;
              for (int j = 0; j < m; j++) {
                  if (j == curMachine) continue;
                  if (a[curTask][j] <= br[j] && c[curTask][j] - c[curTask][curMachine] < bestDiff) {
                      bestMachine = j;
                      bestDiff = c[curTask][j] - c[curTask][curMachine];
                  }
              }
              // move curTask from curMachine to bestMachine if it reduces the cost, or with a certain probability depending on T
              if (bestDiff < 0 || (bestDiff < INT_MAX && ((double)rand() / RAND_MAX) < exp(-cost / T)))
                  moveTask(curTask, bestMachine);
          }

          // or try and swap two tasks (probability 0.5)
          else {
              for (int i = 0; i < n; i++) {
                  // curTaks and i can be swapped
                  if (machine[i] != curMachine && a[i][curMachine] - a[curTask][curMachine] <= br[curMachine]
                      && a[curTask][machine[i]] - a[i][machine[i]] <= br[machine[i]]) {
                      costDiff = c[i][curMachine] + c[curTask][machine[i]] - c[i][machine[i]] - c[curTask][curMachine];
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
      T *= 0.9;			// arbitrary
  }


	return cost;
}


// move task from machine[task] to newMachine
void moveTask(int task, int newMachine) {
	int curMachine = machine[task];
	br[curMachine] += a[task][curMachine];
	br[newMachine] -= a[task][newMachine];
	cost += c[task][newMachine] - c[task][curMachine];
	machine[task] = newMachine;
}
