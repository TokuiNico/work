#include <stdio.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

class Edge{
	public:
		int rid_f;
		int rid_b;
		float weight;
};
class region{
	public:
		int rid; //rid_b
		float score;
		vector<int> inLink; //rid_f
		vector<float> weight; //rid_f -> rid_b
};

vector<region> PR;
float seekScore(int rid){
	float score;
	for(int i=0; i<PR.size(); i++){
		if(rid==PR.at(i).rid){
			score=PR.at(i).score; 
			break;
		}
	}
	return score;
}

int main(int argc, char *argv[])
{
	/********************************************************************************************************************
		Compute scores from inlinks 
		Parameters
	*********************************************************************************************************************/
	//Lines: 47,60,125
	float alpha=0.2;
	int totalRound=100;	
	
	ifstream in_1("link/grid3x/ridlist.in");	//<------------------------------------------------------------------------

	int rid, score, numRid=0;
	while(in_1>>rid){
		in_1>>score;
		region tmp;
		tmp.rid=rid;
		tmp.score=score;
		PR.push_back(tmp);
		numRid++;
	}	
	in_1.close();

	ifstream in_2("link/grid3x/weight.in");	//<------------------------------------------------------------------------
	int rid_f,rid_b;
	float weight;
	vector<Edge> w;
	while(in_2>>rid_f){
		in_2>>rid_b;
		in_2>>weight;
		for(int i=0; i<PR.size(); i++){
			if(PR.at(i).rid==rid_b){
				vector<int> tmp1;
				vector<float> tmp2;
				if(PR.at(i).inLink.size()>0){
					tmp1=PR.at(i).inLink;
					tmp2=PR.at(i).weight;
				}
				tmp1.push_back(rid_f);
				tmp2.push_back(weight);
				PR.at(i).inLink=tmp1;
				PR.at(i).weight=tmp2;
				break;
			}
		}
		Edge e;
		e.rid_f=rid_f;
		e.rid_b=rid_b;
		e.weight=weight;
		w.push_back(e);
	}	
	in_2.close();
	
	for(int round=0; round<totalRound; round++){
		cout<<(round+1)<<" - Round .........."<<endl;
		float tempPRscore[50000];
		for(int i=0; i<PR.size(); i++){
			rid=PR.at(i).rid;
			//score function			
			float tempScore;
			tempScore=(1-alpha)*1;
			for(int j=0; j<PR.at(i).inLink.size(); j++){
				float _s;
				rid_f=PR.at(i).inLink.at(j);
				weight=PR.at(i).weight.at(j);
				_s=seekScore(rid_f);
				tempScore=tempScore+alpha*weight*_s;
			}
			tempPRscore[i]=tempScore; //PR[i][1]=tempScore;
			if(tempScore>10) cout<<rid<<"\t"<<tempPRscore[i]<<endl;
			
			if(round==totalRound-1){
				float error;
				error=tempPRscore[i]-PR.at(i).score;
				if(error<0) error=0-error;
				if(error>0.0001) cout<<rid<<"\t"<<error<<endl;
			}
					
			
		}
		//END of Compute scores	********************************************************************************
		//Update scores
		for(int i=0; i<PR.size(); i++){
			PR.at(i).score=tempPRscore[i];
		}
	}
	
	//Update scores
	ofstream out_1("link/grid3x/grid3x_PRscore_new_0.2.txt");	//grid_hits_PRscore.txt<------------------------------------------------------------------------
	for(int i=0; i<PR.size(); i++){
		out_1<<PR.at(i).rid<<"\t"<<PR.at(i).score<<endl;
	}
	out_1.close();
	return 0;
}