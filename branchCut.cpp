#include <ilcplex/ilocplex.h>
#include "includesBranchCut.h"

using namespace std;

const double RC_EPS = 0.0001;
int cost, nn, mm, a[81][1600], b[81], c[81][1600];

void branchCut(string instance, bool verbose);
int mostFracVar(IloNumArray & values);


//tourne sur toutes les instances à la suite
int main() {
	string instance;
	char verbose;

	printf("Entrez l'instance que vous voulez résoudre (par exemple 'GAP-a05100.dat') : ");
	cin >> instance;
	printf("Verbose ? (y/n) ");
	cin >> verbose;

	branchCut("GAP/"+instance, verbose == 'y' ? true : false);
	return 0;
}


void branchCut(string instance, bool verbose) {
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
	cuts.add(x[0] >= 0);

	//IloNumArray values(env);
	IloNumArray bestValues(env, n * m);
	int lb = 0;
	int ub = INT_MAX;
	double curObj;

	map<int, int> allocation;
	stack<map<int, int> > allocationsStack;
	allocationsStack.push(allocation);

	int iter = 0;
	while (!allocationsStack.empty()) {
		try {
			allocation = allocationsStack.top();
			allocationsStack.pop();

			IloModel master(env);
			master.add(obj);
			master.add(allocationConstr);
			master.add(capacityConstr);

			// (k = m*i+j, 0/1) dans allocation --> force i dans la machine j ou dans n'importe laquelle sauf j
			for (auto it = allocation.begin(); it != allocation.end(); it++) {
				master.add(x[it->first] == it->second);
			}

			IloCplex masterSolver(master);

			masterSolver.setOut(env.getNullStream());
			cuts = masterSolver.addCuts(cuts);
			/*if (iter==260)
				masterSolver.exportModel("masterModel.lp");*/
			if (!masterSolver.solve()) {
				if(verbose) printf("Iteration %d : pas de solution realisable, abandon du noeud\n", ++iter);
				continue;
			}
			curObj = masterSolver.getObjValue();
			IloNumArray values(env);
			masterSolver.getValues(values, x);
			int k = mostFracVar(values);

			if(verbose) printf("Iteration %d (%d noeuds restants) : valeur %lf\n", ++iter, allocationsStack.size(), curObj);

			// variables entieres
			if (k < 0) {
				// update ub
				if (curObj < ub) {
					ub = floor(curObj);
					for (int k = 0; k < m * n; k++) bestValues[k] = values[k];
					if(verbose) printf("Solution entiere ameliorante de valeur %d\n", ub);
				}
				else if(verbose) printf("Solution entiere (non ameliorante) de valeur %d\n", curObj);
			}

			// sinon, on branche sur x[k]
			else {
				// potentiellement meilleur que la meilleure solution actuelle
				if (ceil(curObj) < ub) {
					// update lb
					if (curObj > lb) lb = ceil(curObj);
					allocation[k] = 1;
					allocationsStack.push(allocation);
					allocation[k] = 0;
					allocationsStack.push(allocation);
					if(verbose) printf("On branche sur x[j=%d][i=%d]\n", k%m, k/m);


					// ajout d'une nouvelle coupe
					if (rand() % 100 < 5) {
						int j = rand() % m;
						IloEnv envAux;
						IloModel aux(envAux);
						IloBoolVarArray z(envAux, n);

						IloExpr exprObj(envAux);
						IloExpr exprConstr(envAux);
						for (int i = 0; i < n; i++) {
							exprObj += (1 - values[i]) * z[i];
							exprConstr += a[j][i] * z[i];
						}
						aux.add(IloObjective(envAux, exprObj, IloObjective::Minimize));
						aux.add(exprConstr > b[j]);
						exprObj.end();
						exprConstr.end();

						IloCplex auxSolver(aux);
						auxSolver.setOut(env.getNullStream());
						//auxSolver.exportModel("auxModel.lp");
						auxSolver.solve();

						// ajout de la coupe trouvee
						if (auxSolver.getObjValue() < 1) {
							IloNumArray zValues(envAux);
							auxSolver.getValues(zValues, z);
							IloExpr newCut(env);
							for (int i = 0; i < n; i++) {
								newCut += (1 - x[m * i + j]) * zValues[i];
							}
							cuts.add(newCut >= 1);
							newCut.end();
							zValues.end();
							if(verbose) printf("Nouvelle coupe sur la machine %d\n", j);
						}
						auxSolver.end();
						aux.end();
						z.end();
						envAux.end();
					}
				}
				else {
					if(verbose) puts("abandon du noeud");
				}
			}
			values.end();
			masterSolver.end();
			master.end();
		}
		catch (IloMemoryException e) {
			puts("\nMemoire insuffisante : arret de l'algorithme");
			break;
		}
	}

	printf("\n\nSolution de valeur %d\n", ub);
	for (int j = 0; j < m; j++) {
		printf("Machine %d :\n", j);
		for (int i = 0; i < n; i++) {
			if (bestValues[m*i + j] > 1 - RC_EPS)
				printf("%d ", i);
		}
		cout << endl;
	}
}
