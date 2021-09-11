#include<bits/stdc++.h>
#include<fstream>
using namespace std;

#define maxNodes 100
#define maxConnections 500
#define MAX_INT 1000000
#define interfaceIn 0
#define labelIn 1
#define interfaceOut 2
#define labelOut 3


int topologyInformation[maxNodes][maxNodes][2];
int connectionRequests[maxConnections][5];
int nodes, edges, connections, establishedConnections;
int adjacencyCostMatrix_path1[maxNodes][maxNodes], adjacencyCostMatrix_path2[maxNodes][maxNodes];
double adjacencyBandwidthMatrix[maxNodes][maxNodes];
int destinationNodeParent_path1[maxNodes][maxNodes], destinationNodeParent_path2[maxNodes][maxNodes];
vector<vector<vector<int>>> shortestPath1, shortestPath2;
int shortestPath1Cost[maxNodes][maxNodes][2], shortestPath2Cost[maxNodes][maxNodes][2];
int forwardingTable[maxNodes][maxConnections][4], forwardingTableSize[maxNodes];
int labelID;


void ReadNetworkTopology(char topologyFile[]){
	ifstream inPtr(topologyFile);
    if(!inPtr){
    	cout << "Topology file not available.\n";
    	exit(0);
    }

	inPtr >> nodes >> edges;
	for(int i = 0; i < nodes; i ++){
		for(int j = 0; j < nodes; j ++){
			if(i == j) topologyInformation[i][j][0] = 0;
			else topologyInformation[i][j][0] = MAX_INT;
			topologyInformation[i][j][1] = adjacencyBandwidthMatrix[i][j] = 0.0;
		}
	}

	for(int i = 0; i < edges; i ++){
		if(inPtr.eof()){
			cout << "Unexpected error! Exiting...\n";
			exit(0);
		}
		int source, destination, cost, bandwidth;
		inPtr >> source >> destination >> cost >> bandwidth;
		topologyInformation[source][destination][0] = topologyInformation[destination][source][0] = cost;
		topologyInformation[source][destination][1] = topologyInformation[destination][source][1] = bandwidth;
		adjacencyBandwidthMatrix[source][destination] = adjacencyBandwidthMatrix[destination][source] = (double)bandwidth;
	}
	inPtr.close();
}

void DisplayTopology(){
	for(int i = 0; i < nodes; i ++){
		for(int j = 0; j < nodes; j ++){
			if(topologyInformation[i][j][0] != MAX_INT)
				cout << topologyInformation[i][j][0] << "/";
			else cout << "INF/";
			cout << topologyInformation[i][j][1] << "\t";
		}
		cout << endl;
	}	
}

void ReadConnections(char connectionFile[]){
	ifstream inPtr(connectionFile);
    if(!inPtr){
    	cout << "Connection file not available.\n";
    	exit(0);
    }

	inPtr >> connections;
	for(int i = 0; i < connections; i ++){
		if(inPtr.eof()){
			cout << "Unexpected error! Exiting...\n";
			exit(0);
		}
		inPtr >> connectionRequests[i][0] >> connectionRequests[i][1] >> connectionRequests[i][2] >> connectionRequests[i][3] >> connectionRequests[i][4];
	}
	inPtr.close();
}

void DisplayConnections(){
	for(int i = 0; i < connections; i ++){
		cout << connectionRequests[i][0] << " " << connectionRequests[i][1] << " " << connectionRequests[i][2] << " " <<  connectionRequests[i][3] << " " << connectionRequests[i][4] << endl;
	}
}

void InitializeAdjacencyMatrices(int path, char metric[]){
	for(int i = 0; i < nodes; i ++){
		for(int j = 0; j < nodes; j ++){
			if(path == 1){
				if(topologyInformation[i][j][0] != MAX_INT and topologyInformation[i][j][0] != 0){
					adjacencyCostMatrix_path1[i][j] = (strcmp(metric,"hop") == 0 ? 1 : topologyInformation[i][j][0]);
					destinationNodeParent_path1[i][j] = i;
				}
				else{
					adjacencyCostMatrix_path1[i][j] = topologyInformation[i][j][0];
					destinationNodeParent_path1[i][j] = -1;
				}
			}
			else if(path == 2){
				if(topologyInformation[i][j][0] != MAX_INT and topologyInformation[i][j][0] != 0){
					adjacencyCostMatrix_path2[i][j] = (strcmp(metric,"hop") == 0 ? 1 : topologyInformation[i][j][0]);
					destinationNodeParent_path2[i][j] = i;
				}
				else{
					adjacencyCostMatrix_path2[i][j] = topologyInformation[i][j][0];
					destinationNodeParent_path2[i][j] = -1;
				}
			}
		}
	}
}

void DisaplayAdjacencyMatrices(int path){
	cout << "Cost matrix :-\n";
	for(int i = 0; i < nodes; i ++){
		for(int j = 0; j < nodes; j ++){
			if(path == 1){
				if(adjacencyCostMatrix_path1[i][j] == MAX_INT) cout << "INF\t";
				else cout << adjacencyCostMatrix_path1[i][j] << "\t";
			}
			else if(path == 1){
				if(adjacencyCostMatrix_path2[i][j] == MAX_INT) cout << "INF\t";
				else cout << adjacencyCostMatrix_path2[i][j] << "\t";
			}
		}
		cout << endl;
	}
	cout << "Destination Parent matrix :-\n";
	for(int i = 0; i < nodes; i ++){
		for(int j = 0; j < nodes; j ++){
			if(path == 1)
				cout << destinationNodeParent_path1[i][j] << "\t";
			else if(path == 1)
				cout << destinationNodeParent_path2[i][j] << "\t";
		}
		cout << endl;
	}
}

void FloydWarshall_AllPairsShortestPath(int path){
	for(int k = 0; k < nodes; k ++){
	    for(int i = 0; i < nodes; i ++){
	    	for(int j = 0; j < nodes; j ++){
	    		if(path == 1){
		            if(adjacencyCostMatrix_path1[i][k] != MAX_INT and adjacencyCostMatrix_path1[k][j] != MAX_INT){
		            	if(adjacencyCostMatrix_path1[i][k] + adjacencyCostMatrix_path1[k][j] < adjacencyCostMatrix_path1[i][j]){
		            		adjacencyCostMatrix_path1[i][j] = adjacencyCostMatrix_path1[i][k] + adjacencyCostMatrix_path1[k][j];
		            		destinationNodeParent_path1[i][j] = destinationNodeParent_path1[k][j];
		            	}
		            }
		        }
		        else if(path == 2){
		            if(adjacencyCostMatrix_path2[i][k] != MAX_INT and adjacencyCostMatrix_path2[k][j] != MAX_INT){
		            	if(adjacencyCostMatrix_path2[i][k] + adjacencyCostMatrix_path2[k][j] < adjacencyCostMatrix_path2[i][j]){
		            		adjacencyCostMatrix_path2[i][j] = adjacencyCostMatrix_path2[i][k] + adjacencyCostMatrix_path2[k][j];
		            		destinationNodeParent_path2[i][j] = destinationNodeParent_path2[k][j];
		            	}
		            }
		        }
	    	}
	    }
	}
}

void ExtractSpecificPath(int path, int source, int destination){
	if(source == destination) return;
    if(path == 1){
    	shortestPath1[source][destination].push_back(destination);
    	int lastDestination = destination;
    	while(destinationNodeParent_path1[source][lastDestination] != -1){
    		shortestPath1[source][destination].insert(shortestPath1[source][destination].begin(), destinationNodeParent_path1[source][lastDestination]);
    		lastDestination = destinationNodeParent_path1[source][lastDestination];
    	}
    	if(shortestPath1[source][destination][0] != source){
    		shortestPath1[source][destination].clear();
    		shortestPath1Cost[source][destination][0] = 0;
    		shortestPath1Cost[source][destination][1] = MAX_INT;
    	}
    	else{
    		shortestPath1Cost[source][destination][0] = shortestPath1[source][destination].size() - 1;
    		shortestPath1Cost[source][destination][1] = 0;
    		for(int i = 1; i < shortestPath1[source][destination].size(); i ++){
    			shortestPath1Cost[source][destination][1] += topologyInformation[shortestPath1[source][destination][i-1]][shortestPath1[source][destination][i]][0];
    		}
    	}
    }
    else if(path == 2){
    	shortestPath2[source][destination].push_back(destination);
    	int lastDestination = destination;
    	while(destinationNodeParent_path2[source][lastDestination] != -1){
    		shortestPath2[source][destination].insert(shortestPath2[source][destination].begin(), destinationNodeParent_path2[source][lastDestination]);
    		lastDestination = destinationNodeParent_path2[source][lastDestination];
    	}
    	if(shortestPath2[source][destination][0] != source){
    		shortestPath2[source][destination].clear();
    		shortestPath2Cost[source][destination][0] = 0;
    		shortestPath2Cost[source][destination][1] = MAX_INT;
    	}
    	else{
    		shortestPath2Cost[source][destination][0] = shortestPath2[source][destination].size() - 1;
    		shortestPath2Cost[source][destination][1] = 0;
    		for(int i = 1; i < shortestPath2[source][destination].size(); i ++){
    			shortestPath2Cost[source][destination][1] += topologyInformation[shortestPath2[source][destination][i-1]][shortestPath2[source][destination][i]][0];
    		}
    	}
    }
}

void ExtractAllShortestPaths(){
    for(int i = 0; i < nodes; i ++){
    	for(int j = 0; j < nodes; j ++){
    		shortestPath1Cost[i][j][0] = 0;
    		shortestPath1Cost[i][j][1] = MAX_INT;
    		ExtractSpecificPath(1, i, j);
    	}
    }
}

void DisplayAllShortestPaths(int path){
	for(int i = 0; i < nodes; i ++){
    	for(int j = 0; j < nodes; j ++){
    		cout << i << " -> " << j << " : ";
    		if(path == 1){
	    		for(int k = 0; k < shortestPath1[i][j].size(); k ++)
	    			cout << shortestPath1[i][j][k] << " ";
	    		cout << "=> [" << shortestPath1Cost[i][j][0] << "," << shortestPath1Cost[i][j][1] << "]";
    		}
    		else if(path == 2){
    			for(int k = 0; k < shortestPath2[i][j].size(); k ++)
	    			cout << shortestPath2[i][j][k] << " ";
	    		cout << "=> [" << shortestPath2Cost[i][j][0] << "," << shortestPath2Cost[i][j][1] << "]";
    		}
    		cout << endl;
    	}
    }
}

void RemoveFirstShortestPath(int source, int destination){
	for(int i = 1; i < shortestPath1[source][destination].size(); i ++){
    	adjacencyCostMatrix_path2[shortestPath1[source][destination][i-1]][shortestPath1[source][destination][i]] = MAX_INT;
    	destinationNodeParent_path2[shortestPath1[source][destination][i-1]][shortestPath1[source][destination][i]] = -1;
    	adjacencyCostMatrix_path2[shortestPath1[source][destination][i]][shortestPath1[source][destination][i-1]] = MAX_INT;
    	destinationNodeParent_path2[shortestPath1[source][destination][i]][shortestPath1[source][destination][i-1]] = -1;
	}
}

void FindSecondShortestPaths(char metric[]){
	for(int i = 0; i < nodes; i ++){
		for(int j = 0; j < nodes; j ++){
 			InitializeAdjacencyMatrices(2, metric);
 			RemoveFirstShortestPath(i, j);
 			FloydWarshall_AllPairsShortestPath(2);
 			shortestPath2Cost[i][j][0] = 0;
    		shortestPath2Cost[i][j][1] = MAX_INT;
 			ExtractSpecificPath(2, i, j);
		}
	}
}

void WriteRoutingTable(char routingTableFile[], char metric[]){
	ofstream outPtr(routingTableFile);
	if(!outPtr){
		cout << "File access error..\n";
		exit(0);
	}
	for(int i = 0; i < nodes; i ++){
		outPtr << "Routing Table for node " << i << " :-\n";
		for(int j = 0; j < nodes; j ++){
			outPtr << j << "\t[ ";
			for(int k = 0; k < shortestPath1[i][j].size(); k ++) 
				outPtr << shortestPath1[i][j][k] << " ";
			outPtr << "] ";
			if(strcmp(metric,"hop") == 0) outPtr << shortestPath1Cost[i][j][0] << "\t\t\t\t";
			else outPtr << shortestPath1Cost[i][j][1] << "\t\t\t\t[ ";
			for(int k = 0; k < shortestPath2[i][j].size(); k ++) 
				outPtr << shortestPath2[i][j][k] << " ";
			outPtr << "] ";
			if(strcmp(metric,"hop") == 0) outPtr << shortestPath2Cost[i][j][0] << "\n";
			else outPtr << shortestPath2Cost[i][j][1] << "\n";
		}
		outPtr << "\n";
	}
	outPtr.close();
}

bool CheckIfCompatiable(int source, int destination, int path, double BWeq){
	if(path == 1){
		if(shortestPath1[source][destination].size() == 0) return false;
		double minBW = (double)MAX_INT;
		for(int i = 1; i < shortestPath1[source][destination].size(); i ++){
			if(adjacencyBandwidthMatrix[shortestPath1[source][destination][i-1]][shortestPath1[source][destination][i]] < minBW)
				minBW = adjacencyBandwidthMatrix[shortestPath1[source][destination][i-1]][shortestPath1[source][destination][i]];
		}
		if(minBW < BWeq) return false;
		else return true;
	}
	else if(path == 2){
		if(shortestPath2[source][destination].size() == 0) return false;
		double minBW = (double)MAX_INT;
		for(int i = 1; i < shortestPath2[source][destination].size(); i ++){
			if(adjacencyBandwidthMatrix[shortestPath2[source][destination][i-1]][shortestPath2[source][destination][i]] < minBW)
				minBW = adjacencyBandwidthMatrix[shortestPath2[source][destination][i-1]][shortestPath2[source][destination][i]];
		}
		if(minBW < BWeq) return false;
		else return true;
	}
	else return false;
}

void CreateForwardingTable(int source, int destination, int path, double BWeq, vector<int> &labelList){
    if(path == 1){
    	for(int i = 0; i < shortestPath1[source][destination].size(); i ++){
    		if(i == 0){
    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][interfaceIn] = -5;
    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][labelIn] = -5;
    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][interfaceOut] = shortestPath1[source][destination][i+1];
    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][labelOut] = labelID;
    			labelList.push_back(labelID);
    		}
    		else{
    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][interfaceIn] = shortestPath1[source][destination][i-1];
    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][labelIn] = labelID ++;
    			if(i == shortestPath1[source][destination].size()-1){
    				forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][interfaceOut] = -5;
	    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][labelOut] = -5;
	    		}
	    		else{
	    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][interfaceOut] = shortestPath1[source][destination][i+1];
	    			forwardingTable[shortestPath1[source][destination][i]][forwardingTableSize[shortestPath1[source][destination][i]]][labelOut] = labelID;
	    			labelList.push_back(labelID);
	    		}
	    		adjacencyBandwidthMatrix[shortestPath1[source][destination][i-1]][shortestPath1[source][destination][i]] -= BWeq;
	    		adjacencyBandwidthMatrix[shortestPath1[source][destination][i]][shortestPath1[source][destination][i-1]] -= BWeq;
	    	}
	    	forwardingTableSize[shortestPath1[source][destination][i]] ++;
    	}
    }
    else if(path == 2){
    	for(int i = 0; i < shortestPath2[source][destination].size(); i ++){
    		if(i == 0){
    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][interfaceIn] = -5;
    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][labelIn] = -5;
    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][interfaceOut] = shortestPath2[source][destination][i+1];
    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][labelOut] = labelID;
    			labelList.push_back(labelID);
    		}
    		else{
    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][interfaceIn] = shortestPath2[source][destination][i-1];
    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][labelIn] = labelID ++;
    			if(i == shortestPath2[source][destination].size()-1){
    				forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][interfaceOut] = -5;
	    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][labelOut] = -5;
	    		}
	    		else{
	    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][interfaceOut] = shortestPath2[source][destination][i+1];
	    			forwardingTable[shortestPath2[source][destination][i]][forwardingTableSize[shortestPath2[source][destination][i]]][labelOut] = labelID;
	    			labelList.push_back(labelID);
	    		}
	    		adjacencyBandwidthMatrix[shortestPath2[source][destination][i-1]][shortestPath2[source][destination][i]] -= BWeq;
	    		adjacencyBandwidthMatrix[shortestPath2[source][destination][i]][shortestPath2[source][destination][i-1]] -= BWeq;
	    	}
	    	forwardingTableSize[shortestPath2[source][destination][i]] ++;
    	}
    }
}

void DisplayConnectionOutput(int connection, int source, int destination, int path, vector<int> &labelList, char metric[]){
	cout << "Connection : " << connection << " Source : " << source << " Destination : " << destination << " Label List : [";
	for(int i = 0; i < labelList.size(); i ++){
		if(i == 0) cout << labelList[i];
	    else cout << ", " << labelList[i];
	}
	cout << "]" << " Cost : ";
	if(path == 1){
		if(strcmp(metric,"hop") == 0) cout << shortestPath1Cost[source][destination][0] << endl;
		else cout << shortestPath1Cost[source][destination][1] << endl;
	}
	else if(path == 2){
		if(strcmp(metric,"hop") == 0) cout << shortestPath2Cost[source][destination][0] << endl;
		else cout << shortestPath2Cost[source][destination][1] << endl;
	}
}

void EstablishConnectionRequests(int approach, char metric[]){
	labelID = 101;
	establishedConnections = 0;
	for(int i = 0; i < maxNodes; i ++){
		forwardingTableSize[i] = 0;
		for(int j = 0; j < maxConnections; j ++)
			for(int k = 0; k < 4; k ++)
				forwardingTable[i][j][k] = -1;
	}
	for(int i = 0; i < connections; i ++){
		int source = connectionRequests[i][0];
		int destination = connectionRequests[i][1];
		int BWmin = connectionRequests[i][2];
		int BWavg = connectionRequests[i][3];
		int BWmax = connectionRequests[i][4];
		double BWeq;
		if(approach == 0) BWeq = min((double)BWmax, (double)BWavg + 0.25*((double)BWmax - (double)BWmin));
		else BWeq = (double)BWmax;

		bool flag = CheckIfCompatiable(source, destination, 1, BWeq);
		// if(flag) cout << "Then why?\n";
		if(flag){
			establishedConnections ++;
			vector<int> labelList;
			CreateForwardingTable(source, destination, 1, BWeq, labelList);
			DisplayConnectionOutput(i, source, destination, 1, labelList, metric);
		}
		else{
			flag = CheckIfCompatiable(source, destination, 2, BWeq);
			if(flag){
				establishedConnections ++;
				vector<int> labelList;
				CreateForwardingTable(source, destination, 2, BWeq, labelList);
				DisplayConnectionOutput(i, source, destination, 2, labelList, metric);
			}
		}
	}
}

void WriteForwardingTable(char forwardingTableFile[]){
	ofstream outPtr(forwardingTableFile);
	if(!outPtr){
		cout << "File access error..\n";
		exit(0);
	}
	for(int i = 0; i < nodes; i ++){
		outPtr << "Forwarding Table for node " << i << " :-\n";
		for(int j = 0; j < forwardingTableSize[i]; j ++){
			if(forwardingTable[i][j][interfaceIn] != -5) outPtr << forwardingTable[i][j][interfaceIn] << "\t\t";
			else outPtr << "N/A\t\t";
			if(forwardingTable[i][j][labelIn] != -5) outPtr << forwardingTable[i][j][labelIn] << "\t\t";
			else outPtr << "N/A\t\t";
			if(forwardingTable[i][j][interfaceOut] != -5) outPtr << forwardingTable[i][j][interfaceOut] << "\t\t";
			else outPtr << "N/A\t\t";
			if(forwardingTable[i][j][labelOut] != -5) outPtr << forwardingTable[i][j][labelOut] << "\t\t";
			else outPtr << "N/A\t\t";
			outPtr << "\n";
		}
		outPtr << "\n";
	}
	outPtr.close();
}

void WritePathsFile(char pathsFile[]){
	ofstream outPtr(pathsFile);
	if(!outPtr){
		cout << "File access error..\n";
		exit(0);
	}
	outPtr << "Total no. of Connection Requests : " << connections << endl;
	outPtr << "Established connections : " << establishedConnections << endl;
	outPtr.close();
}

int main(int argc, char *argv[]){
	
	if(argc < 15){
    	cout << "Not enough arguments\n";
    	exit(0);
    }

    char topologyFile[100], connectionFile[100], routingTableFile[100];
    char forwardingTableFile[100], pathsFile[100], costMetric[100];
    int connectionApproach;

    strcpy(topologyFile, argv[2]);
    strcpy(connectionFile, argv[4]);
    strcpy(routingTableFile, argv[6]);
    strcpy(forwardingTableFile, argv[8]);
    strcpy(pathsFile, argv[10]);
    strcpy(costMetric, argv[12]);
    connectionApproach = stoi(argv[14]);

	/*
    string topologyFile = "top14.txt", costMetric = "dist", connectionFile = "NSFNET_100.txt";
    string routingTableFile = "routing.txt", forwardingTableFile = "forwarding.txt", pathsFile = "path.txt";
    int connectionApproach = 0;
	*/

    ReadNetworkTopology(topologyFile);
    // DisplayTopology();
    ReadConnections(connectionFile);
    // DisplayConnections();

    shortestPath1.resize(nodes);
    shortestPath2.resize(nodes);
    for(int i = 0; i < nodes; i ++){
    	shortestPath1[i].resize(nodes);
    	shortestPath2[i].resize(nodes);
    }

    InitializeAdjacencyMatrices(1, costMetric);
    // DisaplayAdjacencyMatrices(1);
    FloydWarshall_AllPairsShortestPath(1);
    ExtractAllShortestPaths();
    FindSecondShortestPaths(costMetric);
    // DisplayAllShortestPaths(1);
    // DisplayAllShortestPaths(2);
    WriteRoutingTable(routingTableFile, costMetric);
 	
 	EstablishConnectionRequests(connectionApproach, costMetric);
 	WriteForwardingTable(forwardingTableFile);
 	WritePathsFile(pathsFile);
 	cout << "\nTotal connections : " << connections;
    cout << ", Established Connections : " << establishedConnections << endl;

    return 0;
}