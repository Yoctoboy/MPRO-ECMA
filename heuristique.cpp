#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <map>
#include <deque>
#include <queue>
#include <climits>
#include <chrono>

using namespace std;
using namespace chrono;

#define D false

int c[81][1600], a[81][1600], b[81], n, m, br[81], machine[1600];

bool isnumber(char ch){
  return ( (ch-'0' >= 0 && ch-'0' <= 9) || ch == ' ' );
}

void parse(string s){
  stdin = fopen(s.c_str(), "r");
  stringstream ss;
  char ch;
  while(scanf("%c", &ch) != EOF){
    if(isnumber(ch)) ss << ch;
    else ss << ' ';
  }

  //parse n & m
  ss >> n >> m;

  //parse c
  for(int j = 0; j < m; j++)
    for(int i = 0; i < n; i++) ss >> c[j][i];

  //parse a
  for(int j = 0; j < m; j++)
    for(int i = 0; i < n; i++) ss >> a[j][i];

  //parse b
  for(int j = 0; j < m; j++) ss >> b[j];
  b[m] = INT_MAX;

  for(int j = 0; j <= m; j++) br[j] = b[j];
}

int heuristique(){

  int cost = 0;

  //init
  for(int i = 0; i < n; i++){
    int mina = 100000;
    int minIndex = m;
    for(int j = 0; j < m; j++){
      if(mina > a[j][i] && a[j][i] <= br[j]){
        mina = a[j][i];
        minIndex = j;
      }
    }
    machine[i] = minIndex;
    br[minIndex] -= mina;
    cost += c[minIndex][i];
  }


  //algo
  int cur_client, cur_machine, best_machine;
  auto start = steady_clock::now();
  int iter = 0;
  while(duration_cast<chrono::milliseconds>(steady_clock::now() - start).count() < 3000){
    cur_client = rand()%n;
    cur_machine = machine[cur_client];
    best_machine = machine[cur_client];
    for(int j = 0; j < m; j++){
      if(j == cur_machine) continue;
      if(a[j][cur_client] <= br[j] && c[j][cur_client] < c[best_machine][cur_client]) best_machine = j;
    }
    machine[cur_client] = best_machine;
    br[cur_machine] += a[cur_machine][cur_client];
    cost -= c[cur_machine][cur_client];
    cost += c[best_machine][cur_client];
    br[best_machine] -= a[best_machine][cur_client];
  }

  return cost;
}

int main(){

  srand(time(NULL));

  string s;
  ifstream instances;
  instances.open("Liste_instances.txt");
  while(instances >> s){
    parse(s);
    printf("%s : %d\n", s.c_str(), heuristique());
  }
  instances.close();

  return 0;
}


























//
