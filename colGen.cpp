#include "includesColGen.h"

using namespace std;

// Precision des couts reduits
// (au dessus de -RC_EPS, l'algorithme est interrompu)
const float RC_EPS = 0.0001;

int cost, nn, mm, a[81][1600], b[81], c[81][1600], br[81], s_init[81][1600], machine[1600];
double time_instance;


void colGen(string instance);


int main() {
	//seed aleatoire pour la generation de nombres aleatoires
	srand(time(NULL));

	//temps passe sur chaque instance
	printf("Combien de temps (en s) pour chaque instance ?  ");
	scanf("%lf", &time_instance);
	printf("Execution de l'algorithme en %ds par instance\n\n", (int)time_instance);
	time_instance /= 2;

	//algo
	string s;
	ifstream instances;
	instances.open("Liste_instances.txt");
	while (instances >> s) {
		colGen(s);
	}

	return 0;
}


//génération de colonnes
void colGen(string instance) {
	IloEnv env;

	parse(instance);
	int current_value = heuristique();
	cout << "Instance " << instance << " (Initial value = " << cost << ")" << endl;
	cout << flush;

	IloInt n = nn;
	IloInt m = mm;

	IloModel PMR(env);
	IloObjective obj = IloAdd(PMR, IloMinimize(env));

	// x[k = i*m+j] = i-eme affectation sur la j-eme machine
	IloNumVarArray x(env);

	// contrainte 1 : chaque tache est executee sur exactement 1 machine (une seule fois)
	IloRangeArray contTasks = IloAdd(PMR, IloRangeArray(env, n, 1, 1));

	// contrainte 2 : chaque machine execute exactement 1 affectation de taches
	IloRangeArray contMachines = IloAdd(PMR, IloRangeArray(env, m, 1, 1));

	for (int j = 0; j < mm; j++) memset(s_init[j], 0, sizeof(s_init[j]));
	for (int i = 0; i < nn; i++) s_init[machine[i]][i] = 1;

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
	while (!isOver && (clock() - start) / (double)CLOCKS_PER_SEC < time_instance) {

		PMRSolver.solve();

		if ((int)PMRSolver.getObjValue() < current_value) {
			current_value = (int)PMRSolver.getObjValue();
			printf("Amelioration a l'iteration %d : %d\n", id / m, current_value);
			cout << flush;
		}

		// on recupere les variables duales
		for (int i = 0; i < n; i++) lambda[i] = PMRSolver.getDual(contTasks[i]);
		for (int j = 0; j < m; j++) mu[j] = PMRSolver.getDual(contMachines[j]);

		bool allPositive = true;

		IloModel PA;
		IloObjective coutReduit;
		IloBoolVarArray z;
		IloNumArray aj, coefContT;
		IloCplex PASolver;

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
		isOver = allPositive;

	}
	PMRSolver.solve();
	cout << "Solution apres generation de colonnes : " << current_value << endl << endl;
	cout << flush;
}


