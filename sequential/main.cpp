#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>

using namespace std;

#define NMAX 100

int readFromFile(bool mat[NMAX][NMAX]){
    fill(*mat, *mat + NMAX * NMAX, 0);

    int dim = 0;
    ifstream file("graph.txt");
    string line, item;


    if (file.is_open()) {
        getline(file, line);
        dim = stoi(line);
        while (getline(file, line)) {
            vector<string> elems;
            stringstream ss(line);
            while (getline(ss, item, ' ')) {
                elems.push_back(item);
            }

            mat[stoi(elems[0])][stoi(elems[1])] = 1;
            mat[stoi(elems[1])][stoi(elems[0])] = 1;
        }

    }
    else {
        cout << "Unable to open the file";
    }

    return dim;
}

void printResult(int cycle[], int dim) {
    cout<<"The hamiltonian cycle is: ";
    for(int i = 0; i<dim; i++){
        cout<<cycle[i]<<" ";
    }
    cout<<cycle[0]<<"\n";
}

bool isValid(int v, bool graph[NMAX][NMAX], int path[], int pos) {
    if (graph[path[pos - 1]][v] == 0) {
        return false;
    }

    for (int i = 0; i < pos; i++) {
        if (path[i] == v) {
            return false;
        }
    }
    return true;
}

bool hamiltonianCycle(bool graph[NMAX][NMAX], int path[], int pos, int dim) {
    if (pos == dim) {
        return graph[path[pos - 1]][path[0]] == 1;
    }

    for (int i = 1; i < dim; i++) {
        if (isValid(i, graph, path, pos)) {
            path[pos] = i;

            if (hamiltonianCycle(graph, path, pos + 1, dim)) {
                return true;
            }
            path[pos] = -1;
        }
    }
    return false;
}

void doHamiltonian(bool graph[NMAX][NMAX], int dim){
    int *path = new int[dim];
    for (int i = 0; i<dim; i++){
        path[i] = -1;
    }

    path[0] = 0;

    if(hamiltonianCycle(graph, path, 1, dim)){
        printResult(path, dim);
    }else {
        cout<<"There is no solution \n";
    }
}

int main() {
    auto start = chrono::high_resolution_clock::now();
    bool mat[NMAX][NMAX];
    int dim = readFromFile(mat); ;

    doHamiltonian(mat, dim);
    auto stop = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::microseconds>(stop - start).count();
    return 0;
}