/**
 * Filter transformedTraDB.dat with "non-repeating" Rid in each Trajectory
 * And filter trajectories that do not have regions (i.e., numRid=0)
 *
 * @param infile    input file name, containing trajectory id and its region ids
 * @param outfile   output file name, to save trajectory id and non-repeating region id
 **/

#include <vector>
#include <time.h>
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
using namespace std;
/*
int tids[1000];

bool found(int tid, int length){
	bool found=0;
	for(int i=0; i<length; i++){
		if(tid==tids[i]){
			found=1;
			break;
		}
	}
	return found;
}
*/
 
int main(int argc, char *argv[]){	
	int tid;
	/*
	ifstream in("input/HsinchuTid.txt");
	int index=0;
	while(in>>tid){
		tids[index]=tid;
		//cout<<tid<<" ";
		index++;
	}
	*/
    
	ifstream infile("./data/grid3x_transformedTraDB_13.dat");
	ofstream outfile("./data/grid3x_transformedTraDBF_13.dat");

	int numRid;
	while(infile>>tid){
		//if(found(tid,index)==1 && tid<144663){ ///////////////////////////////////
		//cout<<tid<<endl;
		infile>>numRid;
		if(numRid>0){
			int listTra[7000];			
			//initial
			for(int i=0; i<7000; i++){
				listTra[i]=0;
			}
			int preRid;
			int tempRid;
			int indexRid=0;
			infile>>preRid;
			for(int i=1; i<numRid; i++){
				infile>>tempRid;
				if(tempRid!=preRid){
					listTra[indexRid]=preRid;
					indexRid++;
					preRid=tempRid;
				}
			}
			//for last Rid
			listTra[indexRid]=preRid;
			indexRid++;
			//For output
			outfile<<tid;
			outfile<<" "<<indexRid;
			for(int i=0; i<indexRid; i++){
				outfile<<" "<<listTra[i];
			}
			outfile<<endl;
		}

		//}//////////////////////////////////////////
	}
	infile.close();
	outfile.close();
	return 0;
}