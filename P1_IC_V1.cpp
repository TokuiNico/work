/*
	Modified from IC_V0.cpp 
	Method: Modify "generate other information of seeds, i.e. Update seedH and M[][]", SerachMin()
	Output: RunTime
*/

#include <fstream.h>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

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

int Max_diss=1000;
vector<Board> H,newH;	//Clustering result
float M[100][100];	//max. number of sensors=100, denote the relation between two sensors (1:, 0: , -1: initial value , -2: self)
float W[100][100];	//Upper for weights, Lower for clustering result

vector<int> SearchMin(float eSpeed);
void update_diss(vector<int> node_merge);
Itemset sort(Itemset C);
vector<Itemset> checkConnected(Itemset checkCluster);
vector<Itemset> checkComplete(vector<Itemset> checkCluster);

int main(int argc, char *argv[]){		
	//parameters
	int sizeW;		//size of the window
	float eSpeed;	//error of the speed
	float r;	 //near in r meters
	float w_threshold;	//weight threshold
	float error;	//User-defined, Error of Predicted Clustering Result

	//variables
	vector<Sensor> listS;
	int numW;	//number of windows
	int numColumns;
	int numSen;	//number of sensors
	int Speed;	
	

	/*--	For input	----------------------------------------------------------------------------------*/
	char *dirI="input/";
	char *filename;	
	char *input;
	filename=(char *)malloc(256);
	input=(char *)malloc(256);

	cin>>filename>>sizeW>>eSpeed>>r>>w_threshold>>error;
	cout<<sizeW<<"	"<<eSpeed<<"	"<<r<<"	"<<w_threshold<<"	"<<error<<endl;
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

	//For Computing Running Time
	struct time start,end;
	int eRate=0;	
	
	error=error*(((numSen)*(numSen-1))/2);	//error=error* C(numSen,2), between 0~1
	for(int iW=0; iW<numW; iW++){
		//Starting to count Runtime (Window Version)
		gettime(&start);
		//initial	
		for(int j=0; j<numSen; j++){
			for(int i=0; i<numSen; i++){
				if(i>j){
					M[i][j]=-6;
					M[j][i]=Max_diss;
				}
				//else if(i==j) M[i][j]=-10;				
			}
		}

		if(eRate>=error || iW==0){	//Do HBC and generate L,U (i.e. Initial Value)		
			eRate=0;
			/*--	Compute M and H	-------------------------------------------------------------------------------------*/
			//compute distance for upper triangle of M
			for(int i=0; i<listS.size(); i++){
				for(int j=i+1; j<listS.size(); j++){
					if(abs(listS.at(i).y - listS.at(j).y)<=r) M[j][i]=-1;	 //dist < r
					else M[j][i]=-2;	 //dist > r
				}
			}		
			//compute dissimilarity
			Board h;
			float s;
			Itemset tempI;
			for(int j=0; j<listS.size(); j++){
				//h.exist=true;
				h.C.item.push_back(j+1);
				for(int i=j+1; i<listS.size(); i++){
					tempI.item.push_back(i+1);
					h.NS.push_back(tempI);
					tempI.item.erase(tempI.item.begin(), tempI.item.end());
					s=0;
					for(int k=0; k<sizeW; k++){
						s=s+pow(listS.at(i).Sp.at(iW*sizeW+k)-listS.at(j).Sp.at(iW*sizeW+k),2);
					}					
					s=sqrt(s/sizeW);
					M[j][i]=s;
					h.diss.push_back(s);
					if(s<=eSpeed){
						M[i][j]=M[i][j]+2;		//trivial edge: -1 -> 1 or hidden edge: -2 -> 0 
					}else{
						M[i][j]=M[i][j]-2;
					}

				}
				H.push_back(h);
				h.C.item.erase(h.C.item.begin(), h.C.item.end());
				h.NS.erase(h.NS.begin(), h.NS.end());
				h.diss.erase(h.diss.begin(), h.diss.end());			
			}

			//show the information
			#if 0
			for(int i=0; i<numSen; i++){
				for(int j=0; j<numSen; j++){
					cout<<M[i][j]<<"	";
				}
				cout<<endl;
			}
			cout<<endl<<endl;
			#endif

			/*--	Hierachical-Based Clustering	---------------------------------------------------------------------------------------*/			
			bool exit=false;
			vector<int> node_merge;
			do{
				node_merge=SearchMin(eSpeed);	//Ci, Cj of H				
				if(node_merge.size()>0){
					update_diss(node_merge);
					if(newH.size()<H.size()){						
						H=newH;
						newH.erase(newH.begin(), newH.end());
					}else exit=true;			
				}else exit=true;			
				node_merge.erase(node_merge.begin(), node_merge.end());
			}while(exit!=true);

			#if 0
			for(int i=0; i<H.size(); i++){
				cout<<H.at(i).C.item.at(0)<<endl;
				for(int j=0; j<H.at(i).NS.size(); j++){
					for(int k=0; k<H.at(i).NS.at(j).item.size(); k++){
						cout<<H.at(i).NS.at(j).item.at(k)<<",";
					}
					cout<<"=>"<<H.at(i).diss.at(j)<<endl;
				}
				cout<<endl;
			}
			#endif	

			/*--	Generate U	 --------------------------------------------------------------------------------------------------------*/
			//Initial U for Weights (i.e. Upper of W[][]) and L for Clustering Results
			for(int i=0; i<numSen; i++){
				for(int j=i+1; j<numSen; j++){
					W[i][j]=0;	 // for U
					W[j][i]=0;	 // for L
				}			
			}			
			

		}else{	//IC	---------------------------------------------------------------------------------------------
			//Generate U for this window
			for(int i=0; i<numSen; i++){
				for(int j=i+1; j<numSen; j++){
					W[i][j]=W[i][j]/2+W[j][i]/2;
				}
			}

			//Generate Coarse Clusters
			Itemset choice;
			vector <Board> coarseH,seedH;
			Board temph;
			//Step1:	 Initialize the choice list of sensors
			for(int i=0; i<numSen; i++){
				choice.item.push_back(0);
			}			
			//Step2:	 Generating
			bool get=false;
			for(int i=0; i<numSen; i++){
				if(choice.item.at(i)==0){
					temph.C.item.push_back(i+1);
					choice.item.at(i)=1;
					for(int j=i+1; j<numSen; j++){
						if(choice.item.at(j)==0){						
							bool ok=true;
							for(int k=0; k<temph.C.item.size(); k++){
								if(W[temph.C.item.at(k)-1][j]<w_threshold){
									ok=false;
								}
							}
							if(ok==true){
								temph.C.item.push_back(j+1);
								choice.item.at(j)=1;
							}
						}
					}
					coarseH.push_back(temph);
				}				
				temph.C.item.erase(temph.C.item.begin(), temph.C.item.end());
			}			

			//Output coarse clusters
			#if 0		
			cout<<iW+1<<"-Window:: "<<coarseH.size()<<" Clusters"<<endl;
			cout<<"Coarse Clusters"<<endl;
			for(int i=0; i<coarseH.size(); i++){
				cout<<"{";
				for(int j=0; j<coarseH.at(i).C.item.size(); j++){
					cout<<coarseH.at(i).C.item.at(j)<<",";
				}
				cout<<"},";
			}
			cout<<endl<<endl;
			#endif
			

			//Refine Coarse Clusters by computing the diss and dist
			for(int i=0; i<coarseH.size(); i++){								
				vector<Itemset> tempCluster; 
				
				//Compute the distance between two Sens of each coarse cluster				
				for(int j=0; j<coarseH.at(i).C.item.size(); j++){
					for(int k=j+1; k<coarseH.at(i).C.item.size(); k++){						
						if(abs(listS.at(coarseH.at(i).C.item.at(j)-1).y-listS.at(coarseH.at(i).C.item.at(k)-1).y)<=r) M[coarseH.at(i).C.item.at(k)-1][coarseH.at(i).C.item.at(j)-1]=-1;	//dist<r
						else M[coarseH.at(i).C.item.at(k)-1][coarseH.at(i).C.item.at(j)-1]=-2;	//dist >r
						
					}	
				}
				//compute the dissimilarity between two Sens of each coarse cluster
				for(int j=0; j<coarseH.at(i).C.item.size(); j++){
					float s=0;
					for(int k=j+1; k<coarseH.at(i).C.item.size(); k++){
						for(int t=0; t<sizeW; t++){
							s=s+pow(listS.at(coarseH.at(i).C.item.at(j)-1).Sp.at(iW*sizeW+t)-listS.at(coarseH.at(i).C.item.at(k)-1).Sp.at(iW*sizeW+t),2);
						}
						s=sqrt(s/sizeW);
						M[coarseH.at(i).C.item.at(j)-1][coarseH.at(i).C.item.at(k)-1]=s;
						if(s<=eSpeed){							
							M[coarseH.at(i).C.item.at(k)-1][coarseH.at(i).C.item.at(j)-1]=M[coarseH.at(i).C.item.at(k)-1][coarseH.at(i).C.item.at(j)-1]+2;	//Hidden edge: -2 -> 0 ; Trivial edge: -1 -> 1							
						}else M[coarseH.at(i).C.item.at(k)-1][coarseH.at(i).C.item.at(j)-1]=M[coarseH.at(i).C.item.at(k)-1][coarseH.at(i).C.item.at(j)-1]-2;						
					}				
				}			
				
				//Check connected via trivial edge
				tempCluster=checkConnected(coarseH.at(i).C);	
				#if 0				
				cout<<"After Check Connected:	";
				for(int x=0; x<tempCluster.size(); x++){
					cout<<"{";
					for(int y=0; y<tempCluster.at(x).item.size(); y++){
						cout<<tempCluster.at(x).item.at(y)<<"	 ,";
					}
					cout<<"},	";
				}
				cout<<endl;
				#endif
				//Check complete via both trivial and hidden edges
				tempCluster=checkComplete(tempCluster);		
				#if 0
				cout<<"After Check Complete:	";
				for(int x=0; x<tempCluster.size(); x++){
					cout<<"{";
					for(int y=0; y<tempCluster.at(x).item.size(); y++){
						cout<<tempCluster.at(x).item.at(y)<<"	 ,";
					}
					cout<<"},	";
				}
				cout<<endl;
				#endif				
				
				//Generate seeds of coarse clusters
				for(int j=0; j<tempCluster.size(); j++){
					for(int k=0; k<tempCluster.at(j).item.size(); k++){
						temph.C.item.push_back(tempCluster.at(j).item.at(k));
					}
					seedH.push_back(temph);
					temph.C.item.erase(temph.C.item.begin(), temph.C.item.end());
				}
				tempCluster.erase(tempCluster.begin(), tempCluster.end());
			}						
			coarseH.erase(coarseH.begin(), coarseH.end());
			#if 0			
			for(int i=0; i<numSen; i++){
				for(int j=0; j<numSen; j++){
					cout<<M[i][j]<<"	";
				}
				cout<<endl;
			}
			cout<<endl<<endl;
			#endif			

			#if 0
			cout<<"Seed Cluster"<<endl;
			for(int i=0; i<seedH.size(); i++){
				cout<<"{";
				for(int j=0; j<seedH.at(i).C.item.size(); j++){
					cout<<seedH.at(i).C.item.at(j)<<",";
				}
				cout<<"},";
			}
			cout<<endl<<endl;
			#endif

			//Generate other information of seeds, i.e. Update seedH and M[][]			
			for(int C1=0; C1<seedH.size(); C1++){
				for(int C2=C1+1; C2<seedH.size(); C2++){
					float diss=Max_diss;	//Max diss
					Itemset _I;
					_I=seedH.at(C2).C;
					seedH.at(C1).NS.push_back(_I);
					_I.item.erase(_I.item.begin(), _I.item.end());
					//Step 1: Check complete relations , i.e. compute eSpeed
					bool possible_eSpeed=true;
					for(int i=0; i<seedH.at(C1).C.item.size(); i++){
						for(int j=0; j<seedH.at(C2).C.item.size(); j++){
							int front,back,_t;
							front=seedH.at(C1).C.item.at(i);
							back=seedH.at(C2).C.item.at(j);
							if(front>back){
								_t=front;
								front=back;
								back=_t;
							}							
							if(M[back-1][front-1]==-6){
								float s=0;
								for(int t=0; t<sizeW; t++){
									s=s+pow(listS.at(front-1).Sp.at(iW*sizeW+t)-listS.at(back-1).Sp.at(iW*sizeW+t),2);
								}
								s=sqrt(s/sizeW);
								M[front-1][back-1]=s;	
								if(s<=eSpeed){							
									M[back-1][front-1]=0;	// <= eSpeed, dist > r	or dist < r		
									if(abs(listS.at(front-1).y-listS.at(back-1).y)<=r){
										M[back-1][front-1]=M[back-1][front-1]+1;	//dist<r												
									}	//Else:	dist >r		
								}else {
									M[back-1][front-1]=-3;	// > eSpeed, dist > r	or dist < r	
									possible_eSpeed=false;
								}
							}else if(M[back-1][front-1]<0){
								possible_eSpeed=false;
							}
							if(possible_eSpeed==false)	break;							
						}
						if(possible_eSpeed==false)	break;	
					}
					
					//Step 2: Compute diss between two seeds
					if(possible_eSpeed==true){
						diss=0;
						for(int i=0; i<seedH.at(C1).C.item.size(); i++){
							for(int j=0; j<seedH.at(C2).C.item.size(); j++){
								int front,back,_t;
								front=seedH.at(C1).C.item.at(i);
								back=seedH.at(C2).C.item.at(j);
								if(front>back){
									_t=front;
									front=back;
									back=_t;
								}
								diss=diss+M[front-1][back-1];
							}								
						}
						diss=diss/(int(seedH.at(C1).C.item.size())*int(seedH.at(C2).C.item.size()));			
					}			
					seedH.at(C1).diss.push_back(diss);
				}
			}
			
			#if 0
			for(int i=0; i<numSen; i++){
				for(int j=0; j<numSen; j++){
					cout<<M[i][j]<<"	";
				}
				cout<<endl;
			}
			cout<<endl<<endl;
			#endif
			
			
			/*--	Do HBC	----------------------------------------------------------------------------------------------------------------*/
			H.erase(H.begin(), H.end());
			H=seedH;	
			seedH.erase(seedH.begin(), seedH.end());
			if(iW==0){
			#if 0
			for(int i=0; i<H.size(); i++){
				cout<<"{";
				for(int j=0; j<H.at(i).C.item.size(); j++){
					cout<<H.at(i).C.item.at(j)<<",";
				}
				cout<<"}"<<endl;				
				for(int j=0; j<H.at(i).NS.size(); j++){
					for(int k=0; k<H.at(i).NS.at(j).item.size(); k++){
						cout<<H.at(i).NS.at(j).item.at(k)<<",";
					}
					cout<<"=>"<<H.at(i).diss.at(j)<<endl;
				}
				cout<<endl;
			}
			#endif
			}
			bool exit=false;
			vector<int> node_merge;
			do{
				node_merge=SearchMin(eSpeed);	//Ci, Cj of H	
				if(node_merge.size()>0){
					
					#if 0
					cout<<"Size of Merging:"<<node_merge.size()<<endl;
					cout<<"Merge {";
					for(int x=0; x<H.at(node_merge.at(0)).C.item.size(); x++){
						cout<<H.at(node_merge.at(0)).C.item.at(x)<<",";
					}
					cout<<"},	{";
					for(int x=0; x<H.at(node_merge.at(0)+node_merge.at(1)+1).C.item.size(); x++){
						cout<<H.at(node_merge.at(0)+node_merge.at(1)+1).C.item.at(x)<<",";
					}
					cout<<"}"<<endl;
					#endif
					
					update_diss(node_merge);
					if(newH.size()<H.size()){						
						H=newH;
						newH.erase(newH.begin(), newH.end());
					}else exit=true;			
				}else exit=true;			
				node_merge.erase(node_merge.begin(), node_merge.end());
			}while(exit!=true);
			
		}		

		//Generate L for Clustering Result (i.e. Lower of W[][])		
		for(int i=0; i<H.size(); i++){
			int _i, _j;
			for(int j=0; j<H.at(i).C.item.size(); j++){
				_i=H.at(i).C.item.at(j);
				for(int k=j+1; k<H.at(i).C.item.size(); k++){
					_j=H.at(i).C.item.at(k);
					if(_i<_j){
						W[_j-1][_i-1]=1;
					}else{
						W[_i-1][_j-1]=1;
					}
				}
			}
 		}		

		//Compute eRate
		for(int i=0; i<numSen; i++){
			for(int j=i+1; j<numSen; j++){
				if(	 (W[j][i]==1 && W[i][j]<w_threshold)	||	(W[j][i]==0 && W[i][j]>w_threshold)){
					eRate++;
				}
			}
		}
		
		cout<<iW+1<<"	";
		//Output Runtime(Window Version)
		#if 1   
		gettime(&end);
		double runtime;
		runtime=(double(end.ti_hour)-double(start.ti_hour))*60; //min
		runtime=(runtime+double(end.ti_min)-double(start.ti_min))*60; //sec
		runtime=(runtime+double(end.ti_sec)-double(start.ti_sec))*1000; //msec
		runtime=runtime+double(end.ti_hund)*10-double(start.ti_hund)*10;  //msec
		//cout<<"Runtime: "<<runtime/1000<<" sec"<<endl;	
	   cout<<runtime<<"	";
	   #endif

		//output results
		cout<<H.size()<<endl;
		#if 0
		cout<<iW+1<<"-Window:: "<<H.size()<<" Clusters"<<endl;
		for(int i=0; i<H.size(); i++){
			cout<<"{";
			for(int j=0; j<H.at(i).C.item.size(); j++){
				cout<<H.at(i).C.item.at(j)<<",";
			}
			cout<<"},";
		}
		cout<<endl<<endl;
		#endif
		H.erase(H.begin(), H.end());
		//cout<<"----------------------------------------------------------------------------------------------"<<endl;
	}
	return 0;
}

vector<int> SearchMin(float eSpeed){
	vector<int> I;
	I.erase(I.begin(), I.end());
	int c1,c2,n1,n2;
	bool get=false;
	float temp_min=eSpeed;
	for(int i=0; i<H.size(); i++){
		for(int j=0; j<H.at(i).NS.size(); j++){
			if(H.at(i).diss.at(j)<temp_min && H.at(i).diss.at(j)<=eSpeed){
				//cout<<"tempmin= "<<H.at(i).diss.at(j);
				if(H.at(i).C.item.size()==1 && H.at(i).NS.at(j).item.size()==1){
					int front,back,_t;
					front=H.at(i).C.item.at(0);
					back=H.at(i).NS.at(j).item.at(0);
					if(front>back){
						_t=front;
						front=back;
						back=_t;
					}
					if(M[back-1][front-1]==1){					
						get=true;
						c1=i;
						c2=j;
						temp_min=H.at(i).diss.at(j);								
					}						
				}else if(H.at(i).C.item.size()==1 && H.at(i).NS.at(j).item.size()>1){
					bool bridge=false;
					bool done=true;
					int front,back;
					front=H.at(i).C.item.at(0);
					for(int k=0; k<H.at(i).NS.at(j).item.size(); k++){
						int newfront=front;
						back=H.at(i).NS.at(j).item.at(k);					
						if(back<front){
							newfront=back;
							back=front;
						}
						if(M[back-1][newfront-1]<0){
							done=false;
							break;
						}else if(M[back-1][newfront-1]==1) bridge=true;
					}
					if(done==true && bridge==true){
						get=true;
						c1=i;
						c2=j;
						temp_min=H.at(i).diss.at(j);	
						//cout<<"min==>"<<temp_min<<endl;
					}				
				}else if(H.at(i).C.item.size()>1 && H.at(i).NS.at(j).item.size()==1){
					bool bridge=false;
					bool done=true;
					int front,back;
					front=H.at(i).NS.at(j).item.at(0);					
					for(int k=0; k<H.at(i).C.item.size(); k++){
						int newfront=front;
						back=H.at(i).C.item.at(k);					
						if(back<front){
							newfront=back;
							back=front;
						}
						if(M[back-1][newfront-1]<0){
							done=false;
							break;
						}else if(M[back-1][newfront-1]==1) bridge=true;
					}
					if(done==true && bridge==true){
						get=true;
						c1=i;
						c2=j;
						temp_min=H.at(i).diss.at(j);						
					}			
				}else if(H.at(i).C.item.size()>1 && H.at(i).NS.at(j).item.size()>1){
					bool bridge=false;
					bool done=true;
					int front,back;
					for(int k=0; k<H.at(i).C.item.size(); k++){
						front=H.at(i).C.item.at(k);
						int newfront=front;
						for(int kk=0; kk<H.at(i).NS.at(j).item.size(); kk++){
							back=H.at(i).NS.at(j).item.at(kk);
							if(back<front){
								newfront=back;
								back=front;
							}
							if(M[back-1][newfront-1]<0){
								done=false;
								break;
							}else if(M[back-1][newfront-1]==1) bridge=true;
						}
						if(done==false) break;
					}
					if(done==true && bridge==true){
						get=true;
						c1=i;						
						c2=j;
						temp_min=H.at(i).diss.at(j);	
					}
				}
			}
			//cout<<"-->min="<<temp_min<<endl;
		}
	}	
	if(get!=false){
		I.push_back(c1);
		I.push_back(c2);
		//I.push_back(n1);
		//I.push_back(n2);
	}
	return I;
}

void update_diss(vector<int> node_merge){
	vector<Board> tempH;
	Board newh;
	Itemset newC;
	int c1=node_merge.at(0);
	int c2=c1+node_merge.at(1)+1;

	for(int i=0; i<H.at(c1).C.item.size(); i++){
		newC.item.push_back(H.at(c1).C.item.at(i));
	}
	for(int i=0; i<H.at(c2).C.item.size(); i++){
		newC.item.push_back(H.at(c2).C.item.at(i));
	}	

	//vector<Board>::iterator pos=H.begin();
	int front,back;
	for(int i=0; i<H.size(); i++){
		//cout<<i<<endl;
		newh=H.at(i);		
		if(i<c1){		
			//cout<<"i<c1"<<endl;
			vector<Itemset>::iterator pos1=newh.NS.begin();
			vector<float>::iterator pos2=newh.diss.begin();
			bool del=false;
			for(int j=0; j<newh.NS.size(); j++){
				if((i+j+1)==c1){
					newh.NS.at(j)=newC;
					newh.diss.at(j)=(newh.diss.at(j)+newh.diss.at(j+c2-c1))/2;
				}else if(del==false && (i+j+1)==c2){
					del=true;
					newh.NS.erase(pos1);
					newh.diss.erase(pos2);
				}
				pos1++;
				pos2++;
			}
			tempH.push_back(newh);
		}else if(i==c1){	
			//cout<<"i==c1"<<endl;
			newh.C.item.erase(newh.C.item.begin(), newh.C.item.end());
			newh.C=newC;
			/*
			for(int h=0; h<newh.C.item.size(); h++){
				cout<<newh.C.item.at(h)<<",";
			}
			cout<<endl;
			*/
			vector<Itemset>::iterator pos1=newh.NS.begin();
			vector<float>::iterator pos2=newh.diss.begin();			
			int j=0;
			int newj=0;
			bool del=false;
			do{
				//cout<<newh.NS.size()<<"-->"<<j<<", "<<newj<<endl;
				if(del==false && (i+j+1)==c2){
					del=true;
					newh.NS.erase(pos1);
					newh.diss.erase(pos2);
				}else{
					if((c1+j+1)<c2){
						front=c1+j+1;
						back=c2-c1-j-2;	//c2-(c1+j+1)-1
					}else{
						front=c2;
						back=c1+j-c2;	//(c1+j+1)-c2-1
					}
					newh.diss.at(newj)=(newh.diss.at(newj)+H.at(front).diss.at(back))/2;
					pos1++;
					pos2++;
					newj++;
				}
				j++;
			}while(pos1!=newh.NS.end());
			tempH.push_back(newh);
		}else if(i>c1 && i<c2){
			//cout<<"i>c1 && i<c2"<<endl;
			vector<Itemset>::iterator pos1=newh.NS.begin();
			vector<float>::iterator pos2=newh.diss.begin();
			bool del=false;
			for(int j=0; j<newh.NS.size(); j++){
				if(del==false && (i+j+1)==c2){
					del=true;
					newh.NS.erase(pos1);
					newh.diss.erase(pos2);
				}
				pos1++;
				pos2++;
			}
			tempH.push_back(newh);		
		}else if(i==c2){
			//do not insert newh
		}else if(i>c2){			
			tempH.push_back(newh);
		}	
		newh.C.item.erase(newh.C.item.begin(), newh.C.item.end());
		newh.NS.erase(newh.NS.begin(), newh.NS.end());
		newh.diss.erase(newh.diss.begin(), newh.diss.end());		
	}
	newH=tempH;
	tempH.erase(tempH.begin(), tempH.end());
}

Itemset sort(Itemset C){
	Itemset sortedC;
	vector<int>::iterator posC=C.item.begin();	
	while(posC!=C.item.end()){
		int insertSen;
		insertSen=*posC;
		if(sortedC.item.size()==0){
			sortedC.item.push_back(insertSen);		
		}else{
			vector<int>::iterator pos1=sortedC.item.begin();
			while(pos1!=sortedC.item.end()){
				if(insertSen>*pos1){
					if(pos1==sortedC.item.end()){
						sortedC.item.push_back(insertSen);
						break;
					}else pos1++;					
				}else{
					sortedC.item.insert(pos1,insertSen);
					break;
				}
			}
		}
		posC++;
	}
	return sortedC;
}

vector<Itemset> checkConnected(Itemset checkCluster){
	vector<Itemset> out;
	Itemset oldI,tempI;
	
	oldI=checkCluster;

	do{		
		tempI.item.push_back(oldI.item.at(0));
		oldI.item.erase(oldI.item.begin(), oldI.item.begin()+1);
		
		int inSen, testSen;
		int numNotSelected;
		numNotSelected=oldI.item.size();
		int index=0;

		//Form a connected cluster
		while(index<numNotSelected){
			bool add=false;
			//cout<<"INDEX=	"<<index<<endl;
			testSen=oldI.item.at(index);
			for(int i=0; i<tempI.item.size(); i++){				
				inSen=tempI.item.at(i);
				//cout<<inSen<<"	,	"<<testSen;
				int front, back;
				if(inSen<testSen){
					front=inSen;
					back=testSen;
				}else{
					front=testSen;
					back=inSen;
				}
				//cout<<"	=>	"<<M[back-1][front-1]<<endl;
				if(M[back-1][front-1]==1){	//Exists a trivial Edge
					tempI.item.push_back(testSen);										
					oldI.item.erase(oldI.item.begin()+index,oldI.item.begin()+index+1);									
					numNotSelected--;
					add=true;
					break;
				}
			}
			//cout<<add<<endl;
			if(add==false) index++;			
		}
		//tempI=sort(tempI);
		out.push_back(tempI);
		tempI.item.erase(tempI.item.begin(), tempI.item.end());
	}while(oldI.item.size()>0);
	return out;
}

vector<Itemset> checkComplete(vector<Itemset> checkCluster){
	vector<Itemset> out;
	Itemset testCluster,tempI;	
	for(int i=0; i<checkCluster.size(); i++){
		testCluster=checkCluster.at(i);
		/*
		for(int j=0; j<testCluster.item.size(); j++){
			cout<<testCluster.item.at(j)<<"	 ";
		}
		cout<<endl;
		*/
		bool ok=true;
		//check relations
		for(int j=0; j<testCluster.item.size(); j++){
			for(int k=j+1; k<testCluster.item.size(); k++){				
				int front, back, _t;
				front=testCluster.item.at(j);
				back=testCluster.item.at(k);
				if(front>back){
					_t=front;
					front=back;
					back=_t;
				}
				if(M[back-1][front-1]<0){
					//cout<<testCluster.item.at(j)<<"	 ,	"<<testCluster.item.at(k)<<"=>"<<M[testCluster.item.at(j)-1][testCluster.item.at(k)-1]<<endl;
					ok=false;
					break;
				}
			}
			if(ok==false) break;
		}
		if(ok==false){
			//Split each item to a single cluster, PS: Furthermore, we could split into a max. cluster if they are complete
			for(int j=0; j<testCluster.item.size(); j++){
				tempI.item.push_back(testCluster.item.at(j));
				out.push_back(tempI);
				tempI.item.erase(tempI.item.begin(), tempI.item.end());
			}
		}else{
			out.push_back(testCluster);			
		}
		testCluster.item.erase(testCluster.item.begin(), testCluster.item.end());
	}
	return out;
}