/**
 * Compute scores of each PR from links
 *
 * Input
 *  link/grid3x/ridlist.in
 *      format: Rid density
 *  link/grid3x/weight.in
 *      format: rid_f rid_b weight
 * Output
 *  link/grid3x/grid3x_PRscore_new_0.2.txt
 *      format: rid score
 **/
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

/**
 * a directed edge with weight
 *
 * @param   rid_f       forward region id
 * @param   rid_b       backward region id
 * @param   weight      weight of edge
 *
 * rid_b <- rid f
 **/
class Edge{
	public:
		int rid_f;
		int rid_b;
		float weight;
};

/**
 *
 *
 * @param   rid         region id
 * @param   score       score of region
 * @param   inLink      the list containing rid_f
 * @param   weight      the list of weight of inlink rid_f
 **/
class region{
	public:
		int rid; //rid_b
		float score;
		vector<int> inLink; //rid_f
		vector<float> weight; //rid_f -> rid_b
};
vector<region> PR;

/**
 * return the score of given region
 *
 * @param   rid         region id
 * @return              the score of region
 **/
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
	//Lines: 47,60,125

	
    /**
     * Step1:   Get  a list of rid and score from ridlist.in,
     *          then save the rid and score of file to a list of region PR
     *
     * @param   rid         region id
     * @param   score       density of region
     * @param   PR          a list of saving regions
     **/
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

    /**
     * Step2:   Get a list of rid_f, rid_b, and weight from weight.in,
     *          then push the rid_f and weight to PR if rid_b is matching
     *
     * @param   rid_f       the region id link to
     * @param   rid_b       the region id to link
     * @param   weight      the calculated weight of each edge
     **/
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
	
    /**
     * Step3:   Calculate the real score of each region.
     *          Find all region id, weight of edge, and score from all inlink regions,
     *          then add its score multiplying its weight and proportion
     *
     * @param   alpha       the proportion between the score itself and the score from inlink region
     * @param   totalRound  total round to calculate score
     **/
    float alpha=0.2;
	int totalRound=100;	
    
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
	
	/**
     * Step4:   Update scores, export rid and updated score to grid3x_PRscore_new_0.2.txt
     **/
	ofstream out_1("link/grid3x/grid3x_PRscore_new_0.2.txt");	//grid_hits_PRscore.txt<------------------------------------------------------------------------
	for(int i=0; i<PR.size(); i++){
		out_1<<PR.at(i).rid<<"\t"<<PR.at(i).score<<endl;
	}
	out_1.close();
	return 0;
}