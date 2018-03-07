#include <ilcplex/ilocplex.h>
#include <chrono>
#include "includesBranchCut.h"

using namespace std;
using namespace std::chrono;

const double RC_EPS = 0.0001;
int cost, nn, mm, a[81][1600], b[81], c[81][1600];

void branchCut(string instance);
int mostFracVar(IloNumArray & values);

int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int getValue(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}


int main() {
	//seed aleatoire pour la generation de nombres aleatoires
	srand(time(NULL));

	//algo
	string s;
	ifstream instances;
	instances.open("Liste_instances.txt");
	while (instances >> s) {
		branchCut(s);
	}

	return 0;
}


void branchCut(string instance) {
	parse(instance);

	IloEnv env;
	IloInt n = nn;
	IloInt m = mm;

	// Programme lineaire compact
	IloNumVarArray x(env, m * n, 0, 1);

	// Objectif
	IloExpr exprObj(env);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			exprObj += c[j][i] * x[m * i + j];
		}
	}
	IloObjective obj = IloObjective(env, exprObj, IloObjective::Minimize);
	exprObj.end();

	// contraintes d'affectation des taches
	IloConstraintArray allocationConstr(env);
	for (int i = 0; i < n; i++) {
		IloExpr expr_i(env);
		for (int j = 0; j < m; j++) {
			expr_i += x[m * i + j];
		}
		allocationConstr.add(expr_i == 1);
		expr_i.end();
	}

	// contraintes de capacite des machines
	IloConstraintArray capacityConstr(env);
	for (int j = 0; j < m; j++) {
		IloExpr expr_j(env);
		for (int i = 0; i < n; i++) {
			expr_j += a[j][i] * x[m * i + j];
		}
		capacityConstr.add(expr_j <= b[j]);
		expr_j.end();
	}

	// coupes (inegalites de couverture) a ajouter dynamiquement
	IloConstraintArray cuts(env);

	//IloNumArray values(env);
	IloNumArray bestValues(env, n * m);
	int lb = 0;
	int ub = INT_MAX;
	double curObj;

	map<int, int> allocation;
	queue<map<int, int> > allocationsQueue;
	allocationsQueue.push(allocation);

	int iter = 0;
  auto start = steady_clock::now();
	while (!allocationsQueue.empty() && duration_cast<chrono::milliseconds>(steady_clock::now() - start).count() < 20000) {
    //printf("Current Memory Usage : %dMb\n", getValue()/1000);
		allocation = allocationsQueue.front();
		allocationsQueue.pop();

		IloModel master(env);
		master.add(obj);
		master.add(allocationConstr);
		master.add(capacityConstr);
		master.add(cuts);

		// (k = m*i+j, 0/1) dans allocation --> force i dans la machine j ou dans n'importe laquelle sauf j
		for (auto it = allocation.begin(); it != allocation.end(); it++) {
			master.add(x[it->first] == it->second);
		}

		IloCplex masterSolver(master);
		masterSolver.setOut(env.getNullStream());
		/*if (iter==260)
			masterSolver.exportModel("masterModel.lp");*/
		if (!masterSolver.solve()) {
			//cout << "iteration " << ++iter << " : pas de solution realisable, abandon du noeud" << endl;
			//system("PAUSE");
			continue;
		}
		curObj = masterSolver.getObjValue();
		IloNumArray values(env);
		masterSolver.getValues(values, x);
		int k = mostFracVar(values);

		//cout << "iteration " << ++iter << " (" << allocationsQueue.size() << " noeuds restants) : valeur " << curObj << ", ";

		// variables entieres
		if (k < 0) {
			// update ub
			if (curObj < ub) {
				ub = floor(curObj);
				for (int k = 0; k < m * n; k++) bestValues[k] = values[k];
				//cout << "solution entiere ameliorante de valeur " << ub << endl;
			}
			//cout << "solution entiere (non ameliorante) de valeur " << curObj << endl;
		}

		// sinon, on branche sur x[k]
		else {
			// potentiellement meilleur que la meilleure solution actuelle
			if (ceil(curObj) < ub) {
				// update lb
				if (curObj > lb) lb = ceil(curObj);
				allocation[k] = 1;
				allocationsQueue.push(allocation);
				allocation[k] = 0;
				allocationsQueue.push(allocation);
				//cout << "on branche sur x[j=" << k%m << "][i=" << k/m << "]" << endl;

        if(rand()%100 < 5){
				  // ajout d'une nouvelle coupe
				  int j = rand() % m;
				  for (int count = 0; count < m; count++) {
					IloModel aux(env);
					IloBoolVarArray z(env, n);

					IloExpr exprObj(env);
					IloExpr exprConstr(env);
					for (int i = 0; i < n; i++) {
						exprObj += (1 - values[i]) * z[i];
						exprConstr += a[j][i] * z[i];
					}
					aux.add(IloObjective(env, exprObj, IloObjective::Minimize));
					aux.add(exprConstr > b[j]);
					exprObj.end();
					exprConstr.end();

					IloCplex auxSolver(aux);
					auxSolver.setOut(env.getNullStream());
					//auxSolver.exportModel("auxModel.lp");
					auxSolver.solve();

					// ajout de la coupe trouvee
					if (auxSolver.getObjValue() < 1) {
						IloNumArray zValues(env);
						auxSolver.getValues(zValues, z);
						IloExpr newCut(env);
						for (int i = 0; i < n; i++) {
							newCut += (1 - x[m * i + j]) * zValues[i];
						}
						cuts.add(newCut >= 1);
						newCut.end();
						zValues.end();
						//cout << "nouvelle coupe sur la machine " << j << endl;
						break;
					}
					else {
						//cout << "inegalites de couverture toutes respectees pour la machine " << j << endl;
						j = (j + 1) % m;
					}using namespace std::chrono;
					auxSolver.end();
					aux.end();
					z.end();
				}
        }
			}

		}
		values.end();
		masterSolver.end();
		master.end();
	}
  cout << instance << endl;
	cout << "Solution de valeur " << ub << endl;
  cout << "Borne inf : " << lb << endl << endl << endl;

	/*for (int j = 0; j < m; j++) {
		cout << "Machine " << j << " : ";
		for (int i = 0; i < n; i++) {
			if (bestValues[m*i + j] > 1 - RC_EPS)
				cout << i << " ";
		}
		cout << endl;
	}*/
}
