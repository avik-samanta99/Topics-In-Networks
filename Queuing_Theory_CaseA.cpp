#include<bits/stdc++.h>
using namespace std;

double lambda, mu, totalTime;
queue<pair<double, double>> serviceQueue[2];
double recentArrival, totalWaitingTime, totalResponseTime;
int arrivalCount, checkCount;
string outputFilename;

double RandomProbability(){
	srand(time(0) + rand());
	return (double)rand()/(double)RAND_MAX;
}

double ExponentialServiceTime(){
	return (-1.0 * ((double)log(1.0 - RandomProbability()) / mu));
}

double ExponentialInterArrivalTime(){
	return (-1.0 * ((double)log(1.0 - RandomProbability()) / lambda));
}

double ComputeCompletionTime(int selectQueue, double serviceTime){
	if(serviceQueue[selectQueue].empty()){
		return (recentArrival + serviceTime);
	}
	else{
		if(serviceQueue[selectQueue].back().second < recentArrival)
			return (recentArrival + serviceTime);
		else return (serviceQueue[selectQueue].back().second + serviceTime);
	}
}

void EnterQueue_arrived(int selectQueue){
	double serviceTime = ExponentialServiceTime();
	double completionTime = ComputeCompletionTime(selectQueue, serviceTime);
	serviceQueue[selectQueue].push(make_pair(recentArrival, completionTime));
	totalWaitingTime += (completionTime - recentArrival - serviceTime);
	totalResponseTime += (completionTime - recentArrival);
	recentArrival += ExponentialInterArrivalTime();
}

void LeaveQueue_checked(int selectQueue){
	checkCount ++;
	serviceQueue[selectQueue].pop();
}

void WriteInformation(){
    ofstream outPtr(outputFilename);
    if(!outPtr){
    	cout << "File access error..\n";
    	exit(0);
    }
    outPtr << "Total observation time : " << totalTime << endl;
    outPtr << "Workers' arrival rate : " << lambda << endl;
    outPtr << "Officers' service rate : " << mu << endl;
    outPtr << "No of arrival : " << arrivalCount << endl;
	outPtr << "No of workers getting checked : " << checkCount << endl;
    outPtr << "Average no. of workers getting checked : " << (double)checkCount / totalTime << endl;
	outPtr << "Average response time for getting checked : " << totalResponseTime / (double)arrivalCount << endl;
	outPtr << "Average waiting time until getting checked : " << totalWaitingTime / (double)arrivalCount << endl;
	outPtr << "Average no of workers waiting at the queue : " << totalResponseTime / totalTime << endl;
	outPtr.close();
}

int main(){
	cout << "Enter the value of Lambda : ";
	cin >> lambda;
	cout << "Enter the value of mu : ";
	cin >> mu;
	cout << "Enter the total observation time : ";
	cin >> totalTime;
	cout << "Enter the filename for storing the o/p : ";
	cin >> outputFilename;

	recentArrival = 0.0;
	totalWaitingTime = 0.0;
	totalResponseTime = 0.0;
	arrivalCount = 0;
	checkCount = 0;

	if(lambda >= mu){
		cout << "Invalid values for lambda and mu. PLease enter again...\n";
		main();
	}
	else{
		while(recentArrival < totalTime){
			arrivalCount ++;
			srand(time(0) + rand());
			int selectQueue = rand() % 2;
			if(recentArrival < serviceQueue[selectQueue].front().second){
				EnterQueue_arrived(selectQueue);
			}
			else{
				while(!serviceQueue[selectQueue].empty() and recentArrival > serviceQueue[selectQueue].front().second){
					LeaveQueue_checked(selectQueue);
				}
				EnterQueue_arrived(selectQueue);
			}
		}
	}

	cout << "No of arrival : " << arrivalCount << endl;
	cout << "No of people checked : " << checkCount << endl;

	/*
	double rho, En, Et, Ew, Eq;
	rho = (lambda / mu);
	En = rho / (1 - rho);
	Et = En / lambda;
	Ew = Et - (1.0 / mu);
	Eq = lambda * Ew;
	*/

	cout << "Average no. of workers getting checked : " << (double)checkCount / totalTime << endl;
	// cout << " [Theory says : " << En << "]\n";
	cout << "Average response time for getting checked : " << totalResponseTime / (double)arrivalCount << endl;
	// cout << " [Theory says : " << Et << "]\n";
	cout << "Average waiting time until getting checked : " << totalWaitingTime / (double)arrivalCount << endl;
	// cout << " [Theory says : " << Ew << "]\n";
	cout << "Average no of workers waiting at the queue : " << totalResponseTime / totalTime << endl;
	// cout << " [Theory says : " << Eq << "]\n";

	WriteInformation();
 
	return 0;
}