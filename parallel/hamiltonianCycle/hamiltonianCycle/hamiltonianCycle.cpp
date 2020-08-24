// hamiltonianCycle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Helper.h"
#include <iostream>
#include <mpi.h>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <iostream>
#include <chrono>
#include <queue>

using namespace std;

struct NODE {
    int no;
    vector<int> adj;
};

void generateFiles(int nodes) {
    for (int i = 1; i <= nodes; i++) {
        createFiles("input" + to_string(i) + ".txt", i, nodes);
    }
}



bool isValid(vector<int> path, int newPathElem) {
    for (int i = 0; i < path.size(); i++) {
        if (path[i] == newPathElem) {
            return false;
        }
    }
    return true;
}


vector<int> sendNodesToChild(vector<int> nodAdj1, int nrProc, vector<int> path1, bool keep) {
    int count = 0;
    int start = 1;
    vector<int> newPath;
    vector<int> nodAdj = nodAdj1;
    vector<int> path = path1;
    int size = path.size();
    int end = nodAdj.size() / (nrProc - 1);
    int rest = nodAdj.size() % (nrProc - 1);
    count = end;
    if (keep) {
        for (int i = 0; i < (nrProc - 1); i++)
        {
            if (rest) {
                end = end + 1;
                rest--;
            }

            MPI_Send(&start, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&size, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            for (int k = 0; k < size; k++) {

                MPI_Send(&path[k], 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            }
            MPI_Status status;
            for (int l = start - 1; l < end; l++) {
                MPI_Send(&nodAdj[l], 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            }

            int done;

            MPI_Recv(&done, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD, &status);

            for (int j = 0; j < done; j++)
            {
                int value;
                MPI_Recv(&value, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD, &status);
                newPath.push_back(value);
            }

            start = end + 1;
            end = end + count;
        }
    }
    else {
        for (int i = 0; i < (nrProc - 1); i++)
        {
            int done = -1;
            MPI_Send(&done, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }
    }
    return newPath;
}

void doChildHamilton() {

    while (true) {
        MPI_Status status;
        int start, end, done;
        int size;
        int pathNode;
        vector<int> aux;
        vector<int> path;
        int aj;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        if (start == -1) {
            break;
        }
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
        for (int lm = 0; lm < size; lm++) {
            MPI_Recv(&pathNode, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            path.push_back(pathNode);
        }
        
        for (int i = start; i <= end; i++)
        {
            
            MPI_Recv(&aj, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            
            if (isValid(path, aj)) {
                
                aux.push_back(aj);
            }
            
        }
        
        done = aux.size();
        
        MPI_Send(&done, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        for (int i = 0; i < aux.size(); i++) {
            
            MPI_Send(&aux[i], 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }

    }

}

void ActionParentProcess(int p, int nrProc) {

    vector<vector<int>> nodes;
    auto startTimer = chrono::high_resolution_clock::now();
    int count = 0;
    //cout << nrProc << endl;
    int start = 1;
    int end = p / (nrProc - 1);
    int rest = p % (nrProc - 1);
    count = end;
    for (int i = 0; i < (nrProc - 1); i++)
    {
        if (rest) {
            end = end + 1;
            rest--;
        }

        MPI_Send(&start, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        MPI_Send(&end, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);


        MPI_Status status;
        

        for (int l = start; l <= end; l++) {
            vector<int> aux1;

            int done;
            MPI_Recv(&done, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < done; j++)
            {
                int value;
                MPI_Recv(&value, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD, &status);
                
                aux1.push_back(value);
            }
            nodes.push_back(aux1);
        }
        start = end + 1;
        end = end + count;

    }
    
    
    int pos = 0;
    vector<int> path;
    vector<queue<int>> q;
    queue<int> aux;
    aux.push(1);
    q.push_back(aux);
    bool sch = false;
    while (pos>-1 && sch==false) {
        
        int nod;
        
        if (pos > -1 && !q[pos].empty()) {
            nod = q[pos].front();
            
            path.push_back(nod);
            q[pos].pop();
            vector<int> newPaths = sendNodesToChild(nodes[nod - 1], nrProc, path, true);
            
            if (newPaths.size() != 0) {
                pos++;
                queue<int> aux2;
                for (int cnp = 0; cnp < newPaths.size(); cnp++) {
                    aux2.push(newPaths[cnp]);
                }
                q.push_back(aux2);
            }
            else {
                
                
            }
        }
        
        if (path.size() >= p) {
            if (std::find(nodes[path[path.size() - 1]-1].begin(), nodes[path[path.size() - 1]-1].end(), path[0]) != nodes[path[path.size() - 1]-1].end())
                sch = true;
        }
        if (sch == false) {
            if (q[pos].empty()) {
                while (q[pos].empty() && pos > -1) {
                    pos--;
                    q.pop_back();
                    path.pop_back();
                }
            }
        }
        
    }
    
    auto endTimer = chrono::high_resolution_clock::now();
    cout << "\n" << "Timp: " << chrono::duration_cast<std::chrono::nanoseconds>(endTimer - startTimer).count() << endl;

    sendNodesToChild(nodes[0], nrProc, path, false);

    writeToFile(path);
    /*for (int pq = 0; pq < path.size(); pq++) {
        cout << path[pq] << " " << endl;
    }*/


}

void ActionChildProcess() {

    MPI_Status status;
    int start, end, done;
    vector<vector<int>> nodes;

    MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    for (int i = start; i <= end; i++)
    {
        ifstream file("input" + to_string(i) + ".txt");
        int grad, coef;
        vector<int> aux;
        while (file >> grad)
        {
            aux.push_back(grad);
        }
        nodes.push_back(aux);

        done = aux.size();
        MPI_Send(&done, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        for (int i = 0; i < aux.size(); i++) {
            MPI_Send(&aux[i], 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }
    }
    doChildHamilton();

}




int main(char* argv[])
{
    int nr = 5;
    //generateFiles(nr);
    //start();
    int rank, nrProc;
    int rc = MPI_Init(NULL, NULL);
    if (rc != MPI_SUCCESS) {
        std::cout << "Err MPI init\n";
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &nrProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        ActionParentProcess(nr, nrProc);
    }
    else {
        ActionChildProcess();
    }

    MPI_Finalize();


    return 0;
}
