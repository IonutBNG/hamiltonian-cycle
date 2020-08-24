
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


using namespace std;

void createFiles(const string file_name, int number, int size) {
	ofstream outputFile;
	outputFile.open(file_name, ios::trunc);
	int count = 0;
	vector<int> nodes;
	int node;

	while (count < size) {
		if ((node = rand() % size + 1) != number) {
			nodes.push_back(node);
			count++;
		}
	}
	
	auto end = nodes.end();
	for (auto it = nodes.begin(); it != end; ++it) {
		end = std::remove(it + 1, end, *it);
	}
	nodes.erase(end, nodes.end());
	
	count = 0;
	while (count < nodes.size()) {
		outputFile << nodes[count] << " ";
		count++;
	}
	outputFile.close();
}


void writeToFile(vector<int> path) {
	ofstream outputFile;
	outputFile.open("output.txt", ios::trunc);
	for (int i = 0; i < path.size(); i++) {
		outputFile << path[i] << " ";
	}
	outputFile << path[0];

	outputFile.close();
}