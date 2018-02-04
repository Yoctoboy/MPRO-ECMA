#include <ilcplex/ilocplex.h>

using namespace std;

// Precision des couts reduits
// (au dessus de -RC_EPS, l'algorithme est interrompu)
const float RC_EPS = 0.0001;

int main() {

	IloEnv env;

	IloInt n = 100;
	IloInt m = 5;

	// affectations initiales dans le probleme maitre restreint

	int a[5][100] = { { 15, 8, 11, 16, 24, 8, 19, 9, 15, 7, 6, 13, 10, 12, 25, 16, 21,
		18, 21, 21, 22, 8, 18, 11, 12, 24, 15, 13, 17, 21, 24, 23, 23, 19, 20, 17, 13,
		12, 9, 22, 13, 14, 25, 7, 9, 25, 20, 13, 21, 20, 25, 5, 11, 20, 10, 17, 5, 25, 11,
		6, 14, 24, 17, 7, 16, 10, 15, 9, 15, 13, 23, 13, 12, 13, 5, 8, 15, 19, 7, 13,
		21, 10, 12, 9, 11, 11, 23, 5, 18, 13, 19, 20, 24, 9, 22, 25, 21, 14, 19, 21 },{
			8, 10, 7, 23, 20, 9, 7, 11, 21, 9, 16, 20, 9, 25, 10, 14, 20, 23, 23, 25, 13, 14
			, 7, 14, 9, 25, 18, 17, 10, 14, 21, 6, 15, 8, 10, 24, 17, 25, 15, 21, 20, 12, 15
		, 16, 7, 17, 12, 8, 21, 19, 14, 19, 7, 25, 16, 20, 17, 18, 21, 8, 11, 10, 19, 11
		, 16, 13, 11, 14, 16, 15, 17, 13, 7, 10, 5, 25, 7, 12, 18, 12, 16, 12, 16, 14, 8
		, 17, 8, 16, 11, 23, 22, 22, 23, 17, 16, 8, 18, 19, 10, 7 },{ 22, 9, 13, 12, 22,
		7, 10, 8, 15, 12, 15, 15, 12, 16, 7, 15, 17, 20, 16, 12, 21, 17, 10, 19, 16, 21,
		23, 23, 6, 21, 24, 18, 8, 11, 8, 10, 22, 16, 5, 9, 5, 12, 18, 15, 16, 8, 22, 5,
		5, 12, 17, 5, 18, 24, 12, 20, 13, 25, 17, 7, 9, 21, 23, 20, 5, 20, 13, 12, 9, 8
		, 6, 7, 8, 21, 5, 21, 17, 13, 9, 8, 17, 11, 17, 25, 12, 8, 11, 6, 8, 5, 23, 25,
		18, 25, 18, 5, 12, 8, 7, 9 },{ 6, 5, 24, 24, 24, 23, 8, 8, 6, 18, 20, 7, 8, 6, 7,
		5, 21, 25, 5, 18, 7, 13, 11, 5, 23, 20, 15, 8, 21, 23, 15, 23, 20, 9, 6, 17, 23
		, 15, 19, 25, 17, 15, 17, 23, 5, 23, 7, 15, 17, 5, 13, 15, 9, 15, 21, 12, 19, 24
		, 19, 19, 13, 5, 9, 24, 15, 9, 18, 7, 10, 12, 19, 5, 25, 20, 23, 18, 10, 18, 22,
		9, 20, 12, 6, 15, 23, 10, 16, 15, 22, 12, 7, 9, 7, 12, 9, 14, 22, 15, 22, 13 },
		{ 23, 19, 18, 17, 25, 14, 15, 12, 11, 5, 25, 14, 8, 8, 16, 17, 11, 12, 8, 14, 10,
		9, 20, 9, 9, 13, 21, 22, 10, 5, 15, 12, 6, 19, 10, 12, 16, 23, 11, 6, 14, 12, 14,
		22, 5, 23, 9, 18, 7, 9, 17, 18, 22, 22, 8, 9, 18, 14, 16, 6, 12, 20, 15, 21,
		23, 7, 18, 16, 20, 14, 22, 21, 13, 22, 18, 18, 14, 9, 10, 15, 18, 6, 7, 6, 20, 22,
		25, 18, 25, 12, 17, 18, 25, 24, 14, 17, 22, 13, 11, 22 } };
	int c[5][100] = { { 36, 46, 34, 25, 29, 17, 15, 29, 38, 50, 39, 50, 37, 39, 25, 13
		, 11, 11, 32, 50, 23, 44, 15, 25, 42, 13, 41, 31, 16, 44, 49, 25, 31, 14, 45, 44
		, 36, 50, 49, 30, 15, 22, 31, 45, 40, 41, 22, 34, 13, 26, 14, 34, 31, 22, 47, 37
		, 42, 47, 42, 24, 32, 25, 41, 40, 35, 20, 43, 10, 29, 28, 30, 23, 20, 28, 26, 47
		, 46, 30, 18, 47, 17, 17, 40, 45, 43, 44, 48, 24, 48, 23, 38, 45, 27, 36, 20, 11
		, 26, 35, 27, 41 },{ 12, 33, 26, 10, 32, 49, 46, 44, 41, 49, 41, 45, 43, 44, 24,
		14, 17, 21, 32, 39, 25, 29, 28, 38, 38, 12, 17, 42, 22, 43, 31, 47, 15, 10, 40,
		12, 39, 34, 28, 23, 35, 46, 48, 40, 29, 13, 38, 37, 14, 16, 30, 32, 20, 37, 48,
		32, 48, 38, 41, 11, 49, 35, 43, 19, 10, 19, 45, 32, 28, 23, 23, 11, 23, 40, 14,
		21, 44, 41, 25, 41, 40, 41, 42, 13, 45, 31, 43, 13, 20, 25, 30, 19, 10, 10, 43,
		31, 35, 21, 31, 45 },{ 34, 23, 36, 34, 21, 39, 48, 37, 34, 10, 44, 39, 41, 13, 45
		, 11, 35, 43, 35, 46, 27, 36, 19, 23, 21, 20, 46, 29, 29, 12, 40, 11, 14, 50, 25
		, 34, 32, 37, 28, 36, 35, 15, 25, 37, 30, 41, 28, 32, 48, 32, 40, 32, 40, 41, 17
		, 41, 34, 40, 19, 42, 36, 15, 50, 26, 49, 47, 20, 35, 39, 20, 10, 23, 42, 39, 14
		, 44, 44, 22, 34, 23, 26, 50, 26, 35, 15, 39, 46, 26, 48, 17, 47, 16, 44, 39, 50
		, 46, 21, 50, 50, 17 },{ 10, 42, 25, 24, 16, 31, 11, 25, 21, 43, 41, 27, 38, 31,
		36, 35, 14, 17, 18, 36, 25, 27, 44, 18, 24, 46, 43, 44, 21, 20, 11, 42, 20, 24,
		33, 42, 41, 32, 50, 25, 47, 15, 38, 29, 32, 46, 25, 10, 42, 33, 27, 20, 11, 33,
		41, 31, 11, 23, 49, 12, 20, 50, 39, 47, 45, 38, 50, 19, 45, 49, 33, 49, 25, 44,
		32, 30, 42, 47, 25, 21, 45, 11, 13, 33, 17, 48, 19, 37, 27, 29, 17, 44, 34, 37,
		19, 50, 43, 22, 27, 38 },{ 39, 19, 24, 16, 50, 20, 39, 11, 26, 40, 18, 44, 37, 26
		, 13, 25, 16, 12, 39, 19, 31, 29, 34, 48, 42, 11, 32, 27, 39, 26, 47, 37, 39, 49
		, 37, 50, 33, 26, 12, 12, 26, 38, 22, 13, 11, 20, 40, 44, 25, 38, 29, 37, 11, 19
		, 39, 33, 27, 50, 33, 35, 40, 47, 47, 15, 23, 11, 38, 29, 36, 50, 39, 12, 47, 21
		, 50, 24, 29, 47, 35, 39, 25, 42, 35, 30, 19, 23, 22, 28, 16, 39, 10, 46, 37, 29
		, 18, 30, 16, 32, 30, 33 } };
	int b[5] = { 342, 342, 342, 342, 342 };
	int s_init[5][100] = { { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1
		, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
		0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0 },
		{ 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1
		, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0 },{ 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1
		, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
		0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0
		, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0 },{ 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
		0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1
		, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0,
		0, 0 } };


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

		for (int i = 0; i < 100; i++) {
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

	// Tant que le sous-probleme fournit une solution de valeur negative
	while (!isOver) {

		PMRSolver.solve();

		// on recupere les variables duales
		for (int i = 0; i < n; i++) lambda[i] = PMRSolver.getDual(contTasks[i]);
		for (int j = 0; j < m; j++) mu[j] = PMRSolver.getDual(contMachines[j]);


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
			cout << "affectation de la machine " << j << " numero " << int(id / m) << endl;// << " : " << affectation << endl;

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

			for (int i = 0; i < 100; i++) {
				coefObj += c[j][i] * affectation[i];
				coefContT[i] += affectation[i];
			}

			x.add(IloNumVar(obj(coefObj) + contTasks(coefContT) + contMachines[j](1)));
		}

		isOver = allPositive;

	}

	PMRSolver.solve();
	cout << "solution apres generation de colonnes : " << PMRSolver.getObjValue() << endl;



	system("PAUSE");
	return 0;
}