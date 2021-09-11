#include<bits/stdc++.h>
using namespace std;

double lambda, mu, totalTime;
queue<double> serviceQueue;
double recentArrival, totalWaitingTime, totalResponseTime;
double currentServiceTime1, currentServiceTime2;
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
	currentServiceTime2 = 0.0;
	currentServiceTime1 = 0.0;

	if(lambda >= 2*mu){
		cout << "Invalid values for lambda and mu. PLease enter again...\n";
		main();
	}
	else{
		while(recentArrival < totalTime){
			if(serviceQueue.empty()){
				srand(time(0) + rand());
				int service = rand() % 2;
				if(service == 0 and currentServiceTime1 <= recentArrival){
					double serviceTime = ExponentialServiceTime();
					currentServiceTime1 = recentArrival + serviceTime;
					checkCount ++;
					totalResponseTime += currentServiceTime1 - recentArrival;
				}
				else if(service == 1 and currentServiceTime2 <= recentArrival){
					double serviceTime = ExponentialServiceTime();
					currentServiceTime2 = recentArrival + serviceTime;
					checkCount ++;
					totalResponseTime += currentServiceTime2 - recentArrival;
				}
				else{
					serviceQueue.push(recentArrival);
				}
				arrivalCount ++;
				recentArrival += ExponentialInterArrivalTime();
			}
			else{
				srand(time(0) + rand());
				int service = rand() % 2;
				if(service == 0 and currentServiceTime1 <= recentArrival){
					double serviceTime = ExponentialServiceTime();
					currentServiceTime1 = max(currentServiceTime1, serviceQueue.front()) + serviceTime;
					checkCount ++;
					totalResponseTime += currentServiceTime1 - serviceQueue.front();
					totalWaitingTime += currentServiceTime1 - serviceQueue.front() - serviceTime;
					serviceQueue.pop();
				}
				else if(service == 1 and currentServiceTime2 <= recentArrival){
					double serviceTime = ExponentialServiceTime();
					currentServiceTime2 = max(currentServiceTime2, serviceQueue.front()) + serviceTime;
					checkCount ++;
					totalResponseTime += currentServiceTime2 - serviceQueue.front();
					totalWaitingTime += currentServiceTime2 - serviceQueue.front() - serviceTime;
					serviceQueue.pop();
				}
				else{
					serviceQueue.push(recentArrival);
					arrivalCount ++;
					recentArrival += ExponentialInterArrivalTime();
				}
			}
	    }
	}

	cout << "No of arrival : " << arrivalCount << endl;
	cout << "No of people checked : " << checkCount << endl;

	cout << "Average no. of workers getting checked : " << (double)checkCount / totalTime << endl;
	cout << "Average response time for getting checked : " << totalResponseTime / (double)arrivalCount << endl;
	cout << "Average waiting time until getting checked : " << totalWaitingTime / (double)arrivalCount << endl;
	cout << "Average no of workers waiting at the queue : " << totalResponseTime / totalTime << endl;

	WriteInformation();
 
	return 0;
}