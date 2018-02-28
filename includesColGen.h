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

extern int cost, nn, mm, a[81][1600], b[81], c[81][1600], br[81], s_init[81][1600], machine[1600];
extern double time_instance;

void moveTask(int task, int newMachine);
int heuristique();
void printcursol();
void printdata();
void parse(std::string s);
