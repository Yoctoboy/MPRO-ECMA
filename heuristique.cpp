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

using namespace std;

int c[81][1600], a[81][1600], b[81], n, m, br[81], machine[1600];

bool isnumber(char ch){
  return ( (ch-'0' >= 0 && ch-'0' <= 9) || ch == ' ' );
}

void parse(){
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
  return cost;
}

int main(){

  string s;
  ifstream instances;
  instances.open("Liste_instances.txt");
  while(!instances.eof()){
    instances >> s;
    stdin = fopen(s.c_str(), "r");
    parse();
    cout << s << " " << heuristique() << endl;
  }
  instances.close();

  return 0;
}


























//
