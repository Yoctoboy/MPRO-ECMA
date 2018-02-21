#include <ilcplex/ilocplex.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
#include <deque>
#include <queue>
#include <climits>
#include <algorithm>
#include <math.h>
#include <ctime>

using namespace std;

// Precision des couts reduits
// (au dessus de -RC_EPS, l'algorithme est interrompu)
const float RC_EPS = 0.0001;

int cost, nn, mm, a[81][1600], b[81], c[81][1600], br[81], s_init[81][1600], machine[1600], time_instance;

// move task from machine[task] to newMachine
void moveTask(int task, int newMachine) {
	int curMachine = machine[task];
	br[curMachine] += a[curMachine][task];
	br[newMachine] -= a[newMachine][task];
	cost += c[newMachine][task] - c[curMachine][task];
	machine[task] = newMachine;
}

// afficher la solution courante
void printcursol(){
	for(int j = 0; j < mm; j++) memset(s_init[j], 0, sizeof(s_init[j]));
	for(int i = 0; i < nn; i++) s_init[machine[i]][i] = 1;
	for(int j = 0; j < mm; j++){
		for(int i = 0; i < nn; i++){
			cout << s_init[j][i];
		}
		cout << endl;
	}
	cout << cost << endl;
}

//heuristique pour obtenir une solution initiale
int heuristique() {

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
	sort(sorted.begin(), sorted.end());
	//reverse(sorted.begin(), sorted.end());


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
    int curTask, curMachine, bestMachine, bestTask, costDiff, bestDiff, iter = 0;
    double T = 1.;
    while (iter++ < 400000) {
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

	return cost;
}

//parser les instances
void parse(string s){
	//reinitialiser la memoire
	for(int i = 0; i < 81; i++){
		memset(a[i], 0, sizeof(a[i]));
		memset(c[i], 0, sizeof(c[i]));
		memset(s_init[i], 0, sizeof(s_init[i]));
	}
	memset(machine, 0, sizeof(machine));
	memset(br, 0, sizeof(br));
	memset(b, 0, sizeof(b));

	//obtenir tous les caractères de l'instance
	ifstream f;
	f.open(s.c_str());
	stringstream ss;
	char ch;
	while(f.get(ch)){
		if((ch-'0' >= 0 && ch-'0' <= 9)) ss << ch;
		else ss << ' ';
	}

	//parse n & m
	ss >> nn >> mm;

	//parse c
	for(int j = 0; j < mm; j++)
		for(int i = 0; i < nn; i++) ss >> c[j][i];
	for (int i = 0; i < nn; i++) c[mm][i] = 100000;

	//parse a
	for(int j = 0; j < mm; j++)
		for(int i = 0; i < nn; i++) ss >> a[j][i];
	for (int i = 0; i < nn; i++) a[mm][i] = 1;

	//parse b
	for(int j = 0; j < mm; j++) ss >> b[j];
	b[mm] = INT_MAX;

	for(int j = 0; j <= mm; j++) br[j] = b[j];
}

//génération de colonnes
void colGen(string instance){

	parse(instance);
	int current_value = heuristique();

	IloEnv env;
	IloInt n = nn;
	IloInt m = mm;

	IloModel PMR(env);
	IloObjective obj = IloAdd(PMR, IloMinimize(env));

	// x[k = i*m+j] = i-eme affectation sur la j-eme machine
	IloNumVarArray x(env);

	// contrainte 1 : chaque tache est executee sur exactement 1 machine (une seule fois)
	/*public IloRangeArray (const IloEnv env, IloInt n, IloNum lb, IloNum ub)
	This constructor creates an array of n elements, each with the lower bound lb and the upper bound ub. */
	IloRangeArray contTasks = IloAdd(PMR, IloRangeArray(env, n, 1, 1));

	// contrainte 2 : chaque machine execute exactement 1 affectation de taches
	IloRangeArray contMachines = IloAdd(PMR, IloRangeArray(env, m, 1, 1));

	// colonnes qui entrent dans le PMR au d�part
	for (int j = 0; j < m; j++) {
		IloInt coefObj = 0;
		IloNumArray coefContT(env, n);

		for (int i = 0; i < n; i++) {
			coefObj += c[j][i] * s_init[j][i];
			coefContT[i] += s_init[j][i];
		}

		x.add(IloNumVar(obj(coefObj) + contTasks(coefContT) + contMachines[j](1)));
	}
	IloCplex PMRSolver(PMR);


	// Desactivation des logs de cplex
	PMRSolver.setOut(env.getNullStream());


	// Procedure de generation de colonnes :
	// lambda variables duales associees aux contraintes sur les taches
	IloNumArray lambda(env, n);
	// mu variables duales associees aux contraintes sur les machines
	IloNumArray mu(env, m);
	// nouvelle affectation trouvee par resolution du probleme auxiliaire
	IloNumArray affectation(env, n);

	bool isOver = false;
	int id = m;

	clock_t start = clock();
	// Tant que le sous-probleme fournit une solution de valeur negative
	while (!isOver && (clock()-start)/(double)CLOCKS_PER_SEC < time_instance) {

		PMRSolver.solve();
		printf("Current value : %lf\n", PMRSolver.getObjValue());

		puts("test");
		// on recupere les variables duales
		for (int i = 0; i < n; i++) lambda[i] = PMRSolver.getDual(contTasks[i]);
		for (int j = 0; j < m; j++) mu[j] = PMRSolver.getDual(contMachines[j]);
		puts("test");

		bool allPositive = true;

		// Problemes auxiliaires : generation d'affectations
		for (int j = 0; j < m; j++) {
			IloModel PA(env);
			// objectif = plus petit cout reduit
			IloObjective coutReduit = IloAdd(PA, IloMinimize(env, -mu[j]));
			// variable z : z[i] = 1 ssi la tache i fait partie de l'affectation
			IloBoolVarArray z(env, n);

			// contrainte de capacite
			IloNumArray aj(env, n);
			for (int i = 0; i < n; i++) aj[i] = a[j][i];
			PA.add(IloScalProd(aj, z) <= b[j]);

			IloCplex PASolver(PA);
			PASolver.setOut(env.getNullStream());

			// coefficients dans l'objectif
			for (int i = 0; i<n; i++) coutReduit.setLinearCoef(z[i], c[j][i] - lambda[i]);
			PASolver.solve();


			allPositive = allPositive && (PASolver.getValue(coutReduit) > -RC_EPS);

			PASolver.getValues(affectation, z);

			/*if (id == m) {
				PASolver.exportModel("pamodel.lp");
				IloNumArray sol(env, n);
				PASolver.getValues(sol, z);
				cout << "Affectation : " << sol << endl;
			}*/

			id++;

			// calcul des coefficients associes a la nouvelle variable
			IloNum coefObj = 0;
			IloNumArray coefContT(env, n);

			for (int i = 0; i < n; i++) {
				coefObj += c[j][i] * affectation[i];
				coefContT[i] += affectation[i];
			}

			x.add(IloNumVar(obj(coefObj) + contTasks(coefContT) + contMachines[j](1)));

			// libération de la mémoire
			coutReduit.end();
			z.end();
			aj.end();
			PA.end();
			PASolver.end();
		}
		puts("test");
		if((int)PMRSolver.getObjValue() < current_value){
			current_value = (int)PMRSolver.getObjValue();
			printf("Amélioration à l'iteration %d : %d\n", id/m, current_value);
		}
		puts("test");
		isOver = allPositive;

	}

	PMRSolver.solve();
	cout << "Solution apres generation de colonnes : " << PMRSolver.getObjValue() << endl;



	//system("PAUSE");
	return;
}

int main() {

	//seed aleatoire pour la generation de nombres aleatoires
	srand(time(NULL));

	//temps passe sur chaque instance
	printf("Combien de temps (en s) pour chaque instance ?  ");
	scanf("%d", &time_instance);
  printf("Execution de l'algorithme en %ds par instance\n", time_instance);

	//algo
  string s;
  ifstream instances;
  instances.open("Liste_instances.txt");
  while(instances >> s){
		cout << "Instance " + s + " :" << endl;
		colGen(s);
	}

	return 0;
}
















//
