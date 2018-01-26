#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <deque>
#include <queue>
#include <climits>

using namespace std;

int c[81][1600], a[81][1600], b[81], n, m, br[81], machine[1600], cost = 0;

void parse(){
  string s;
  int iter;
  char ch;

  //parse n
  getline(cin, s);
  s.pop_back();
  iter = s.length()-1;
  while(s[iter] != '=' && s[iter] != ' ') iter--;
  n = stoi(s.substr(iter+1, s.length()-iter));

  //parse m
  getline(cin, s);
  s.pop_back();
  iter = s.length()-1;
  while(s[iter] != '=' && s[iter] != ' ') iter--;
  m = stoi(s.substr(iter+1, s.length()-iter));

  //parse c
  getline(cin, s);
  for(int j = 0; j < m; j++){
    scanf("%c", &ch);
    while(ch == ' ' || ch == '\n') scanf("%c", &ch);
    for(int i = 0; i < n; i++){
      scanf("%d", &c[j][i]);
    }
    if(j != m-1) scanf("%c%c", &ch, &ch);
  }
  scanf("%c", &ch);
  while(ch == ' ') scanf("%c", &ch);
  scanf("%c", &ch);
  getline(cin, s);
  for (int i = 0; i < n; i++) c[m][i] = 100000;

  //parse a
  getline(cin, s);
  for(int j = 0; j < m; j++){
    scanf("%c", &ch);
    while(ch == ' ' || ch == '\n') scanf("%c", &ch);
    for(int i = 0; i < n; i++){
      scanf("%d", &a[j][i]);
    }
    if(j != m-1) scanf("%c%c", &ch, &ch);
  }
  scanf("%c", &ch);
  while(ch == ' ') scanf("%c", &ch);
  scanf("%c", &ch);
  getline(cin, s);
  for(int i = 0; i < n; i++) a[m][i] = 1;

  //parse b
  scanf("%c%c", &ch, &ch);
  while(ch == ' ' || ch == '\n') scanf("%c", &ch);
  scanf("%c", &ch);
  while(ch == ' ' || ch == '\n') scanf("%c", &ch);
  for(int j = 0; j < m; j++) scanf("%d", &b[j]);
  b[m] = INT_MAX;
  for(int j = 0; j <= m; j++) br[j] = b[j];
}

int main(){
  parse();
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

  cout << cost << endl;

  return 0;
}


























//
