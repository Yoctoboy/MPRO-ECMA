#include <ilcplex/ilocplex.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
#include <deque>
#include <stack>
#include <climits>
#include <algorithm>
#include <math.h>
#include <ctime>

extern int cost, nn, mm, a[81][1600], b[81], c[81][1600];
extern const double RC_EPS;

void printdata();
void parse(std::string s);

int mostFracVar(IloNumArray & values);
