/*
	Generate link list among PR
*/

#include <vector>
#include <time.h>
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <algorithm>


using namespace std;

//#include "linkR.h"

class Item{
	public:
		int rid;
		int links;
		vector<int> tids;
};

class Region{
	public:
		int rid;
		vector<Item> outlink;
		vector<Item> inlink;
		int density;
		int score;
		int totalOut; // for total outlinks
};

vector<Region> PR;	
vector<Region> tempPR;
int seekIndex(int rid);
void updateTempPR(int tid, int index_f, int index_b, int Rid_f, int Rid_b);
bool seekList(int index, vector<int> list);
void updatePR(int tid,int index, Region tempR);

int main(int argc, char *argv[]){	
	//parameters ***********************************************************************************************//
	//Lines:67,207
	//ifstream infile("input/PRtest1.txt"); 
	//ifstream infile("input/test2-PR-HsinchuThre0.001.dat");
	ifstream infile("data/grid3x_densityThre13.dat");
	//ifstream infile("data/grid_hits_densityThre0.001.dat");
	//**********************************************************************************************************//
	
	int rid;
	while(infile>>rid){
		int density;
		Region tempR;
		infile>>density;
		tempR.rid=rid;
		//if(rid==20277) cout<<rid;
		tempR.density=density;
		PR.push_back(tempR);
	}
	infile.close();		
	//ifstream in_1("input/traDBtest1.txt"); //****************************************************************************************************
	//ifstream in_1("input/test2-TraDB-Hshinchu-1.dat");
	ifstream in_1("data/grid3x_transformedTraDBF_13.dat");
	vector<Region> emptyPR;
	emptyPR=PR;
	int tid;
	
	while(in_1>>tid){
		//cout<<"TID: "<<tid<<endl;
		vector<int> updateList;	//index in PR
		/*
		try {
			std::vector<int> updateList;
			//updateList.at(0);
		}
		catch (std::out_of_range outOfRange) {
			std::cout << "Caught std::out_of_range" << std::endl;
		}
		*/
		tempPR=emptyPR;
		int numRid;
		int Rid_f,Rid_b;
		in_1>>numRid;
		if(numRid>=2){
			int index_f,index_b;
			bool inserted;
			in_1>>Rid_f;
			for(int i=1; i<numRid; i++){
				//cout<<i<<" ";
				int outIndex,inIndex;
				inserted=0;
				in_1>>Rid_b;
				index_f=seekIndex(Rid_f);
				index_b=seekIndex(Rid_b);
				//if(tid==120470 && inserted==0) cout<<index_f<<","<<index_b<<"\t"<<"Edge: "<<Rid_f<<" => "<<Rid_b<<endl;
				updateTempPR(tid,index_f,index_b,Rid_f,Rid_b);
				//cout<<"Complete updateTempPR\n";
				inserted=seekList(index_f,updateList);

				if(inserted==0) updateList.push_back(index_f);
				//if(tid==120470 && inserted==0) cout<<"Insert Index="<<index_f<<endl; 
				Rid_f=Rid_b;
			}
			inserted=seekList(index_b,updateList);
			if(inserted==0) updateList.push_back(index_b);
			#if 0
			if(tid==142545){
			cout<<"## tempPR ###########################"<<endl;
			for(int i=0; i<tempPR.size(); i++){
				if(PR.at(i).rid==10600515 || PR.at(i).rid==10608630){//
				cout<<"Rid: "<<tempPR.at(i).rid<<endl;
				for(int j=0; j<tempPR.at(i).outlink.size(); j++){
					cout<<"	Out: "<<tempPR.at(i).outlink.at(j).rid<<" ,density: "<<tempPR.at(i).outlink.at(j).links<<" ,Tids: ";
					for(int k=0; k<tempPR.at(i).outlink.at(j).links; k++){
						cout<<tempPR.at(i).outlink.at(j).tids.at(k)<<",";
					}
					cout<<endl;
				}
				for(int j=0; j<tempPR.at(i).inlink.size(); j++){
					cout<<"	In: "<<tempPR.at(i).inlink.at(j).rid<<" ,density: "<<tempPR.at(i).inlink.at(j).links<<" ,Tids: ";
					for(int k=0; k<tempPR.at(i).inlink.at(j).links; k++){
						cout<<tempPR.at(i).inlink.at(j).tids.at(k)<<",";
					}
					cout<<endl;
				}
				}//
			}
			}//
			#endif			
			//update PR			
			for(int i=0; i<updateList.size(); i++){
				//cout<<"before index...\n";						
				int index=updateList.at(i);
				//cout<<"after index...\n";	
				Region tempR=tempPR.at(index);
				//if(tid==149931) cout<<"UPDATE... Rid= "<<tempR.rid<<"	,Index= "<<index<<endl;
				updatePR(tid,index,tempR);
				#if 0
				cout<<"## PR ###########################"<<endl;
				for(int i=0; i<PR.size(); i++){							
					if(PR.at(i).rid==12864354){//
					cout<<"Rid: "<<PR.at(i).rid<<endl;
					for(int j=0; j<PR.at(i).outlink.size(); j++){
						cout<<"	Out: "<<PR.at(i).outlink.at(j).rid<<" ,density: "<<PR.at(i).outlink.at(j).links<<" ,Tids: ";
						for(int k=0; k<PR.at(i).outlink.at(j).tids.size(); k++){
							cout<<PR.at(i).outlink.at(j).tids.at(k)<<",";
						}
						cout<<endl;
					}
					for(int j=0; j<PR.at(i).inlink.size(); j++){
						cout<<"	In: "<<PR.at(i).inlink.at(j).rid<<" ,density: "<<PR.at(i).inlink.at(j).links<<" ,Tids: ";
						for(int k=0; k<PR.at(i).inlink.at(j).tids.size(); k++){
							cout<<PR.at(i).inlink.at(j).tids.at(k)<<",";
						}
						cout<<endl;
					}
					}//
				}
				#endif
			}
			#if 0
			//if(tid==97474){//
			cout<<"## PR ###########################"<<endl;
			for(int i=0; i<PR.size(); i++){							
				if(PR.at(i).rid==10600515 || PR.at(i).rid==10608630){//
				cout<<"Rid: "<<PR.at(i).rid<<endl;
				for(int j=0; j<PR.at(i).outlink.size(); j++){
					cout<<"	Out: "<<PR.at(i).outlink.at(j).rid<<" ,density: "<<PR.at(i).outlink.at(j).links<<" ,Tids: ";
					for(int k=0; k<PR.at(i).outlink.at(j).tids.size(); k++){
						cout<<PR.at(i).outlink.at(j).tids.at(k)<<",";
					}
					cout<<endl;
				}
				for(int j=0; j<PR.at(i).inlink.size(); j++){
					cout<<"	In: "<<PR.at(i).inlink.at(j).rid<<" ,density: "<<PR.at(i).inlink.at(j).links<<" ,Tids: ";
					for(int k=0; k<PR.at(i).inlink.at(j).tids.size(); k++){
						cout<<PR.at(i).inlink.at(j).tids.at(k)<<",";
					}
					cout<<endl;
				}
				}//
			}
			//}
			#endif
		}else{
			in_1>>Rid_f;
		}	
		updateList.erase(updateList.begin(),updateList.end());
		//if(tid==142545) system("pause");
	}
	in_1.close();	
	//update totalout of PR
	for(int i=0; i<PR.size(); i++){
		int temp=0;
		for(int j=0; j<PR.at(i).outlink.size(); j++){
			temp=temp+PR.at(i).outlink.at(j).links;
		}
		PR.at(i).totalOut=temp;
	}
	tempPR.erase(tempPR.begin(),tempPR.end());
	//For output the result of linkR
	#if 1
	char dirI[]="./link/grid3x_13/";	//****************************************************************************************************
	char *filename;
	char *filename1;
	char *file;
	filename=(char *)malloc(sizeof(char)*50);
	filename1=(char *)malloc(sizeof(char)*50);
	file=(char *)malloc(sizeof(char)*50);
	strcpy(filename1,dirI);
	strcat(filename1,"ridlist.in");
	//cout<<filename1<<endl;
	ofstream out(filename1);
	
	for(int i=0; i<PR.size(); i++){
		//cout<<"PR Index:"<<i<<"/"<<PR.size()<<endl;
		
		if(PR.at(i).outlink.size()>0 || PR.at(i).inlink.size()>0){
			rid=PR.at(i).rid;
			out<<rid<<" "<<"1"<<endl; //initial score
			
			sprintf(file,"%d",rid); //ubuntu 
			//itoa(rid,file,10); //win
			strcpy(filename,dirI);
			strcat(filename,file);
			ofstream out_1(filename);
			//cout<<"PR size= "<<PR.size()<<"	; "<<filename<<endl;
			//out_1<<"1"<<endl; //initial score

			//if(PR.at(i).totalOut>PR.at(i).density || PR.at(i).inlink.size()>PR.at(i).density){
			//	cout<<rid<<"\t"<<PR.at(i).density<<"\t"<<PR.at(i).totalOut<<"\t"<<PR.at(i).inlink.size()<<endl;
			//	int maxDegree=max(int(PR.at(i).totalOut),int(PR.at(i).inlink.size()));
			//	PR.at(i).density=maxDegree;	
			//}			
			out_1<<PR.at(i).density<<endl;
			out_1<<PR.at(i).totalOut<<endl;
			
			int tmp;
				
			//for outlinks
			tmp=PR.at(i).outlink.size();		
			out_1<<tmp<<endl;
			for(int j=0; j<tmp; j++){
				out_1<<PR.at(i).outlink.at(j).rid<<" "<<PR.at(i).outlink.at(j).links;
				for(int k=0; k<PR.at(i).outlink.at(j).tids.size(); k++){
					out_1<<" "<<PR.at(i).outlink.at(j).tids.at(k);
				}
				out_1<<endl;
			}
			//for inlinks
			tmp=PR.at(i).inlink.size();
			out_1<<tmp<<endl;
			for(int j=0; j<tmp; j++){
				out_1<<PR.at(i).inlink.at(j).rid<<" "<<PR.at(i).inlink.at(j).links;
				for(int k=0; k<PR.at(i).inlink.at(j).tids.size(); k++){
					out_1<<" "<<PR.at(i).inlink.at(j).tids.at(k);
				}
				out_1<<endl;
			}
			out_1.close();
			
		}
		
	}
	out.close();
	free (filename);
	free (filename1);
	free (file);
	#endif
	
	return 0;
}

int seekIndex(int rid){
	int index;
	for(int i=0; i<PR.size(); i++){
		if(PR.at(i).rid==rid){
			index=i;
			break;
		}
	}
	return index;
}

void updateTempPR(int tid, int index_f, int index_b, int Rid_f, int Rid_b){
	Region tempR;
	Item tempI;
	int length;
	//For Outlink
	tempR.inlink=tempPR.at(index_f).inlink;
	length=tempPR.at(index_f).outlink.size();
	if(length==0){
		//insert links
		//if(tid==37398 && Rid_f==20165) cout<<"Outlink Insert Rid="<<Rid_b<<endl;
		tempI.rid=Rid_b;
		tempI.links=1;
		tempI.tids.push_back(tid);
		tempR.outlink.push_back(tempI);
		tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
	}else{
		int i=0;
		int rid;
		bool insert=0;
		for(i; i<length; i++){
			rid=tempPR.at(index_f).outlink.at(i).rid;
			if(rid<Rid_b){
				//copy rid < Rid_b
				tempI.rid=rid;
		    	tempI.tids=tempPR.at(index_f).outlink.at(i).tids;
				tempI.links=tempPR.at(index_f).outlink.at(i).links;
				tempR.outlink.push_back(tempI);
				tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());				
			}else if(rid>Rid_b){
				tempI.rid=Rid_b;
				tempI.links=1;
				tempI.tids.push_back(tid);
				tempR.outlink.push_back(tempI);
				tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());		
				insert=1;
				break;
			}else{
				i=i+1;
				break;
			}
		}	
		if(insert==0){
			tempI.rid=Rid_b;
			tempI.links=1;
			tempI.tids.push_back(tid);
			tempR.outlink.push_back(tempI);
			tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());			
		}
		//copy rid > Rid_b
		for(i; i<length; i++){
			tempI.rid=tempPR.at(index_f).outlink.at(i).rid;
			tempI.tids=tempPR.at(index_f).outlink.at(i).tids;
			tempI.links=tempPR.at(index_f).outlink.at(i).links;
			tempR.outlink.push_back(tempI);
			tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());				
		}
	}
	tempPR.at(index_f)=tempR;
	/*
	for(int k=0; k<tempR.outlink.size(); k++)
		if(tid==111862 && Rid_f==10685210) cout<<Rid_f<<" => "<<tempR.outlink.at(k).rid<<endl;
	for(int k=0; k<tempPR.at(index_f).outlink.size(); k++)
		if(tid==111862 && Rid_f==10685210) cout<<Rid_f<<" => "<<tempPR.at(index_f).outlink.at(k).rid<<endl;
	*/
	tempR.outlink.erase(tempR.outlink.begin(),tempR.outlink.end());
	tempR.inlink.erase(tempR.inlink.begin(),tempR.inlink.end());
	//For Inlink
	tempR.outlink=tempPR.at(index_b).outlink;
	length=tempPR.at(index_b).inlink.size();
	if(length==0){
		//insert links
		tempI.rid=Rid_f;
		tempI.links=1;
		tempI.tids.push_back(tid);
		tempR.inlink.push_back(tempI);
		tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
	}else{
		int i=0;
		int rid;
		bool insert=0;
		for(i; i<length; i++){
			rid=tempPR.at(index_b).inlink.at(i).rid;
			if(rid<Rid_f){
				//copy rid < Rid_f
				tempI.rid=rid;
		    	tempI.tids=tempPR.at(index_b).inlink.at(i).tids;
				tempI.links=tempPR.at(index_b).inlink.at(i).links;
				tempR.inlink.push_back(tempI);
				tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());				
			}else if(rid>Rid_f){
				tempI.rid=Rid_f;
				tempI.links=1;
				tempI.tids.push_back(tid);
				tempR.inlink.push_back(tempI);
				tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());		
				insert=1;
				break;
			}else{
				i=i+1;
				break;
			}
		}	
		if(insert==0){
			tempI.rid=Rid_f;
			tempI.links=1;
			tempI.tids.push_back(tid);
			tempR.inlink.push_back(tempI);
			tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());		
			insert=1;
		}
		//copy rid > Rid_f
		for(i; i<length; i++){
			tempI.rid=tempPR.at(index_b).inlink.at(i).rid;
			tempI.tids=tempPR.at(index_b).inlink.at(i).tids;
			tempI.links=tempPR.at(index_b).inlink.at(i).links;
			tempR.inlink.push_back(tempI);
			tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());				
		}
	}
	tempPR.at(index_b)=tempR;
}

bool seekList(int index, vector<int> list){
	bool found=0;
	for(int i=0; i<list.size(); i++){
		if(index==list.at(i)){
			found=1;
			break;
		}
	}
	return found;
}

void updatePR(int tid, int index, Region tempR){
	int length;
	vector<Item> tempList;
	//For outlink
	length=PR.at(index).outlink.size();
	//cout<<"Outlinks Length= "<<length<<endl;
	int tmp=tempR.outlink.size();
	if(length==0){
		Item tempI;
		for(int i=0; i<tmp; i++){
			tempI=tempR.outlink.at(i);
			PR.at(index).outlink.push_back(tempI);
		}
	}else{
		int i=0,j=0;
		Item tempI;
		if(tmp>0){
			bool run=0;
			for(i; i<tmp; i++){		
				int rid=tempR.outlink.at(i).rid;	
				for(j; j<length; j++){
					if(rid==PR.at(index).outlink.at(j).rid){
						//update link
						//cout<<"Out-1...."<<endl;
						tempI.rid=rid;
						tempI.links=PR.at(index).outlink.at(j).links+1;
						tempI.tids=PR.at(index).outlink.at(j).tids;
						tempI.tids.push_back(tid);
						tempList.push_back(tempI);
						tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
						if(j==length-1 && i<tmp && tmp>1) { i=i+1; run=1;}
						j=j+1;
						break;
					}else if(rid<PR.at(index).outlink.at(j).rid){ //not in PR
						//insert link
						//cout<<"Out-2...."<<endl;
						tempI.rid=rid;
						tempI.links=1;
						tempI.tids.push_back(tid);
						tempList.push_back(tempI);
						tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
						break;
					}else if(rid>PR.at(index).outlink.at(j).rid){
						//cout<<"Out-3...."<<endl;
						tempI=PR.at(index).outlink.at(j);
						tempList.push_back(tempI);
						tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
						if(j==length-1) run=1;
					}
				}
				if(j==length && i<tmp && run==1){
					//cout<<"Out-4...."<<endl;
					tempI.rid=tempR.outlink.at(i).rid;	
					tempI.links=1;
					tempI.tids.push_back(tid);
					tempList.push_back(tempI);
					tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
				}	
			}
			
			if(i==tmp && j<length){ //Insert 
				//cout<<"length= "<<length<<endl;
				for(j; j<length; j++){
					//cout<<"Out-5....  j= "<<j<<endl;
					tempI=PR.at(index).outlink.at(j);
					tempList.push_back(tempI);
					tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
				}
			}
			
			//update PR
			PR.at(index).outlink.erase(PR.at(index).outlink.begin(),PR.at(index).outlink.end());
			PR.at(index).outlink=tempList;
			tempList.erase(tempList.begin(),tempList.end());
		}
	}
	//For inlink
	length=PR.at(index).inlink.size();
	//cout<<"Inlinks Length= "<<length<<endl;
	tmp=tempR.inlink.size();
	if(length==0){
		Item tempI;
		for(int i=0; i<tmp; i++){
			tempI=tempR.inlink.at(i);
			PR.at(index).inlink.push_back(tempI);
		}
	}else{
		int i=0,j=0;
		Item tempI;
		if(tmp>0){
			bool run=0;
			for(i=0; i<tmp; i++){
				int rid=tempR.inlink.at(i).rid;	
				for(j; j<length; j++){
					if(rid==PR.at(index).inlink.at(j).rid){
						//update link
						//cout<<"In-1...."<<endl;
						tempI.rid=rid;
						tempI.links=PR.at(index).inlink.at(j).links+1;
						tempI.tids=PR.at(index).inlink.at(j).tids;
						tempI.tids.push_back(tid);
						tempList.push_back(tempI);
						tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
						if(j==length-1 && i<tmp && tmp>1) { i=i+1; run=1;}
						j=j+1;
						break;
					}else if(rid<PR.at(index).inlink.at(j).rid){ //not in PR
						//insert link
						//cout<<"In-2...."<<endl;
						tempI.rid=rid;
						tempI.links=1;
						tempI.tids.push_back(tid);
						tempList.push_back(tempI);
						tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
						break;
					}else{
						//cout<<"In-3...."<<endl;
						tempI=PR.at(index).inlink.at(j);
						tempList.push_back(tempI);
						tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
						if(j==length-1) run=1;
					}
				}
				if(j==length && i<tmp && run==1){
					//cout<<"In-4...."<<endl;
					tempI.rid=tempR.inlink.at(i).rid;
					tempI.links=1;
					tempI.tids.push_back(tid);
					tempList.push_back(tempI);
					tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
				}
			}
			if(i==tmp && j<length){
				//cout<<"length= "<<length<<endl;
				for(j; j<length; j++){
					//cout<<"Out-5....  j= "<<j<<endl;
					tempI=PR.at(index).inlink.at(j);
					tempList.push_back(tempI);
					tempI.tids.erase(tempI.tids.begin(),tempI.tids.end());
				}
			}
			//update PR
			PR.at(index).inlink.erase(PR.at(index).inlink.begin(),PR.at(index).inlink.end());
			PR.at(index).inlink=tempList;
			tempList.erase(tempList.begin(),tempList.end());
		}
	}
}
