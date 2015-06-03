/*
	Modified from V1.cpp 
	Method: Consider minCost!
*/

#include <fstream.h>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>

using namespace std;

class Sensor{
        public:
			int id;	//sensor id
			float y;	//location
			char dir;	//N->0, S->1, E, W
			int numTs;	//the total number of time slots
			vector<int> Sp;	 //Speed
};

class Itemset{
	public:
	vector<int> item;
};

class Board{	 //Record the distance between sensors
	public:
		//bool exist;
		Itemset C;	 //C={Si,...,Sj}
		vector<Itemset> NS;	//node list for merging
		vector<float> diss;	//list of diss(C,n), for all n in NS
};

class SimRecord{
	public:
		int id;
		vector<float> r[3];	//[0]->start for S1, [1]->start for S2, [2]->dissimilarity
};

//int Max_diss=1000;
vector<Board> H,newH;	//Clustering result
int M[100][100];	//max. number of sensors=100

vector<int> SearchMin(float eSpeed);
void update_diss(vector<int> node_merge);

int main(int argc, char *argv[]){		
	//parameters
	int sizeW;		//size of the window
	float eSpeed;	//error of the speed
	float r;	 //near in r meters
	
	//variables
	vector<Sensor> listS;
	int numW;	//number of windows
	int numColumns;
	int numSen;	//number of sensors
	int Speed;	
	int timeMerge;	
	int avgRound=0;
	int avgCluster=0;

	/*--	For input	----------------------------------------------------------------------------------*/
	char *dirI="input/";
	char *filename;	
	char *input;
	filename=(char *)malloc(256);
	input=(char *)malloc(256);
	float min,max;
	cin>>filename>>sizeW>>eSpeed>>r>>min>>max;
	cout<<sizeW<<"	"<<eSpeed<<"	"<<r<<endl;
	r=r*1000;
	input=strcat(dirI,filename);
	ifstream infile(input);	

	Sensor tempS;
	infile>>numColumns;

	tempS.numTs=numColumns-2;
	numW=(numColumns-2)/sizeW;
	infile>>numSen;
	infile>>tempS.dir;	
	//cout<<numColumns<<endl;
	//cout<<numSen<<endl;	
	//cout<<tempS.dir;

	for(int i=0; i<numSen; i++){		
		infile>>tempS.id;	
		infile>>tempS.y;				
		for(int j=0; j<tempS.numTs; j++){
			infile>>Speed;	
			tempS.Sp.push_back(Speed);			
		}
		listS.push_back(tempS);		
		tempS.Sp.erase(tempS.Sp.begin(), tempS.Sp.end());		
	}	
	//output the listS
	#if 0	
	for(int i=0; i<listS.size(); i++){	
		cout<<listS.at(i).id<<endl<<listS.at(i).y<<endl;
		for(int j=0; j<listS.at(i).Sp.size(); j++){
			cout<<listS.at(i).Sp.at(j)<<" ";									
		}        
		cout<<endl;			
	}		
	#endif
	vector<SimRecord> simR;
	SimRecord tempR;
	//compute dissimilarity
	for(int i=0; i<listS.size(); i++){
		for(int j=i+1; j<listS.size(); j++){
			tempR.id=listS.at(j).id;			
			float temp[3];
			for(int start=0; start+(sizeW-1)<listS.at(i).Sp.size(); start++){
				if(start+1+(sizeW-1)<listS.at(i).Sp.size()){
					for(int ss=start; ss+(sizeW-1)<listS.at(i).Sp.size(); ss++){
						tempR.r[0].push_back(start);
						tempR.r[1].push_back(ss);
						float e=0;
						for(int k=0; k<sizeW; k++){
							e=e+pow(listS.at(i).Sp.at(start+k)-listS.at(j).Sp.at(ss+k),2);
						}					
						e=sqrt(e/sizeW);
						tempR.r[2].push_back(e);	
					}
				}				
			}
			simR.push_back(tempR);
			tempR.r[0].erase(tempR.r[0].begin(), tempR.r[0].end());
			tempR.r[1].erase(tempR.r[1].begin(), tempR.r[1].end());
			tempR.r[2].erase(tempR.r[2].begin(), tempR.r[2].end());
		}
	}
	int index=0;
	for(int i=0; i<listS.size(); i++){
		for(int j=0; j<listS.size()-(i+1); j++){			
			for(int k=0; k<simR.at(j).r[0].size(); k++)
			//if(simR.at(index).r[2].at(k)>=min && simR.at(index).r[2].at(k)<max) cout<<i+1<<" , "<<simR.at(index).id<<" <==> "<<simR.at(index).r[0].at(k)<<" , "<<simR.at(index).r[1].at(k)<<" ===>"<<simR.at(index).r[2].at(k)<<endl;
			if((abs(simR.at(index).r[0].at(k)-simR.at(index).r[1].at(k))<20) && (simR.at(index).r[2].at(k)>=min && simR.at(index).r[2].at(k)<max))
				cout<<i+1<<" , "<<simR.at(index).id<<" <==> "<<simR.at(index).r[0].at(k)<<" , "<<simR.at(index).r[1].at(k)<<" ===>"<<simR.at(index).r[2].at(k)<<endl;			
			
			index++;
		}
		cout<<endl<<endl;
	}
	return 0;
}
