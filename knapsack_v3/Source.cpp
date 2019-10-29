
#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <ctime>
#include <iomanip>
using namespace std;

class Item  {
public:
	int id, profit, weight;
	float profitPerWeight;
};
class Node {
public:
	string id;
	int level;
	int *includeArray;
	int lowerBound;
	float cost;
};

bool fileExists(const std::string& filename) {
	std::ifstream ifile(filename.c_str());
	return (bool)ifile;
}
int readNumItems(string filename) {
	int count = 0;
	string line;
	ifstream file(filename);
	while (getline(file, line))
		count++;
	file.close();
	return count;
}
int readC(string filename) {
	string line;
	ifstream file(filename);
	int val;
	while (file >> val);
	file.close();
	return val;
}
void readW(string filename, Item *itemsArray, int size) {
	string line;
	ifstream file(filename);
	int val;
	int i = 0;
	while (file >> val) {
		itemsArray[i].id = i;
		itemsArray[i].weight = val;
		i++;
	}
	file.close();
}
void readP(string filename, Item *itemsArray, int size) {
	string line;
	ifstream file(filename);
	int val;
	int i = 0;
	while (file >> val) {
		itemsArray[i].profit = val;
		itemsArray[i].profitPerWeight = (float)itemsArray[i].profit / (float)itemsArray[i].weight;
		i++;
	}
	file.close();
}
void readS(string filename, int *solutionArray, int size) {
	string line;
	ifstream file(filename);
	int val;
	int i = 0;
	while (file >> val) {
		solutionArray[i] = val;
		i++;
	}
	file.close();
}

void sortItemsArray(Item *itemsArray, int size) {
	for (int i = 1; i < size; i++) {
		Item temp = itemsArray[i];
		int j = i - 1;
		while (temp.profitPerWeight > itemsArray[j].profitPerWeight && (j >= 0)) {
			itemsArray[j + 1] = itemsArray[j];
			j = j - 1;
		}
		itemsArray[j + 1] = temp;
	}
}
void profitToNegative(Item *itemsArray, int size) {
	for (int i = 0; i < size; i++) {
		itemsArray[i].profit = 0 - itemsArray[i].profit;
		itemsArray[i].profitPerWeight = 0 - itemsArray[i].profitPerWeight;
	}

}

Node createRootNode( int size) {
	Node rootNode;
	rootNode.id = "";
	rootNode.level = -1;
	rootNode.cost = 0;
	rootNode.lowerBound = 0;
	rootNode.includeArray = new int[size];
	for (int i = 0; i < size; i++) {
		rootNode.includeArray[i] = -1;
	}
	return rootNode;
}
void sortNodeVector(vector<Node> &nodeVector) {
		
	for (int i = 1; i < nodeVector.size(); i++) {
		Node temp = nodeVector[i];
		int j = i - 1;
		while (((j >= 0) && temp.cost > nodeVector[j].cost)) {// Least cost (highest profit) at back
			nodeVector[j + 1] = nodeVector[j];
			j = j - 1;
		}
		nodeVector[j + 1] = temp;
	}
}

int branchNode(Node currentNode,  int size, Item *itemsArray, int capacity, int upperBound, int *bestItemInclude, vector<Node> &nodeVector, int include) {
	//create new node
	Node node1;
	node1.id = currentNode.id + to_string(include);
	node1.level = currentNode.level + 1;
	node1.includeArray = new int[size];
	for (int i = 0; i < size; i++) {
		node1.includeArray[i] = currentNode.includeArray[i];
	}
	node1.includeArray[node1.level] = include;// branch na include

	//calculate for node
	//check if total weight<capacity
	int guaranteedWeight = 0;
	int guaranteedProfit = 0;
	int newWeight = 0;
	int newProfit = 0;
	float newProfitFraction = 0;
	float fraction = 0;
	for (int i = 0; i < size; i++) {
		if (node1.includeArray[i] == 1) {
			guaranteedWeight += itemsArray[i].weight;
			guaranteedProfit += itemsArray[i].profit;
		}
	}
	
	if (guaranteedWeight <= capacity) {

		int i = node1.level + 1;
		newWeight = guaranteedWeight;
		newProfit = guaranteedProfit;
		while (i < size && node1.includeArray[i] == -1 && (newWeight + itemsArray[i].weight) <= capacity) {
			newWeight += itemsArray[i].weight;
			newProfit += itemsArray[i].profit;
			i++;
		}
		//cant put next full item
		fraction = capacity - newWeight;
		fraction = (float)fraction * itemsArray[i].profitPerWeight;
		newProfitFraction = (float)newProfit + fraction;

		//add node to vector
		node1.lowerBound = newProfit;
		node1.cost = newProfitFraction;
		if (node1.level < size - 1 && guaranteedWeight!=capacity) {
			nodeVector.push_back(node1);
		}

		//check upperbound
		if (upperBound >= node1.lowerBound) {
			upperBound = node1.lowerBound;

			for (int i = 0; i < size; i++) {
				bestItemInclude[i] = node1.includeArray[i];
			}
			for (int i = 0; i < nodeVector.size(); i++) {
				if (nodeVector[i].cost > upperBound) {
					nodeVector.erase(nodeVector.begin() + i);
				}
			}
		}
	}

	return upperBound;
}

void sortBestItemInclude(int *sortedBestItemInclude, int *bestItemInclude, Item *itemsArray, int size) {
	for (int i = 0; i < size; i++) {
		int itemId = itemsArray[i].id;
		if (bestItemInclude[i] == -1) {
			bestItemInclude[i] = 0;
		}
		sortedBestItemInclude[itemId] = bestItemInclude[i];
	}
}

int compute(Item *itemsArray, int size, int capacity, int *sortedBestItemInclude) {
	
	Node rootNode= createRootNode(size);

	// prepare algorithm 
	vector<Node> nodeVector;
	int upperBound = 0;
	int *bestItemInclude = new int[size];
	for (int i = 0; i < size; i++) {
		bestItemInclude[i] = -1;
	}
	nodeVector.push_back(rootNode);		

	Node currentNode;
	while (!nodeVector.empty()) {
		if (nodeVector.size() > 1) {
			sortNodeVector(nodeVector);
		}

		if (nodeVector.back().level < size) {
			currentNode = nodeVector.back();
			nodeVector.pop_back();

			//branch
			upperBound = branchNode(currentNode, size, itemsArray, capacity, upperBound, bestItemInclude, nodeVector, 1);
			upperBound = branchNode(currentNode, size, itemsArray, capacity, upperBound, bestItemInclude, nodeVector, 0);
		}
	}

	
	sortBestItemInclude(sortedBestItemInclude, bestItemInclude, itemsArray, size);

	return 0-upperBound; //make positive value
}
bool compare(int *solutionArray, int *sortedBestItemInclude, int size) {
	for (int i = 0; i < size; i++) {
		if(solutionArray[i]!= sortedBestItemInclude[i]){
			return false;
		}
	}
	return true;
}
void printSolution( string file, bool correct, int size, int solution, int *solutionBestItemInclude, double time) {
	if (correct) {
		cout << file << " size: " << size << setprecision(3) << fixed<< " time: " << time << "s solution: " << solution << " items: ";
		for (int i = 0; i < size; i++) {
			cout << solutionBestItemInclude[i];
		}
		cout << endl;
	}
	else {
		cout << file << " failed " << endl;
	}
}
void knapsack(string file) {
	string file_c = file + "_c.txt";
	string file_w = file + "_w.txt";
	string file_p = file + "_p.txt";
	string file_s = file + "_s.txt";

	if (!fileExists(file_c) && !fileExists(file_w) && !fileExists(file_p) && !fileExists(file_s)) {
		cout << "Error. One or all needed files do not exist";
	}
	else {
		int capacity = readC(file_c);
		int itemsNum = readNumItems(file_w);

		Item *itemArray = new Item[itemsNum];
		int *solutionArray = new int[itemsNum];

		readW(file_w, itemArray, itemsNum);
		readP(file_p, itemArray, itemsNum);
		readS(file_s, solutionArray, itemsNum);

		sortItemsArray(itemArray, itemsNum);
		profitToNegative(itemArray, itemsNum); // needed for minimalisation

		int *solutionBestItemInclude = new int[itemsNum];

		clock_t begin = clock();
		int solution = compute(itemArray, itemsNum, capacity, solutionBestItemInclude);
		clock_t end = clock();

		double time = double(end - begin) / CLOCKS_PER_SEC;
		bool correct = compare(solutionArray, solutionBestItemInclude, itemsNum);

		printSolution(file, correct, itemsNum, solution, solutionBestItemInclude, time);
	}	
}

int main() {
	const int numOfFiles = 7;
	string fileArray[numOfFiles] = { "p01", "p02", "p03", "p04", "p05", "p06", "p07" };
	
	for (int f = 0; f < numOfFiles; f++) {
		string file = fileArray[f];
		knapsack(file);
	}	

	system("pause");
	return 0;
}