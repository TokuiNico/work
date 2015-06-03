/*
	Modified from V1.cpp 
	Method: Consider minCost!
*/

#include <fstream>
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

	cin>>filename>>sizeW>>eSpeed>>r;
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
	
	for(int iW=0; iW<numW; iW++){
		//initial
		timeMerge=0;
		for(int j=0; j<numSen; j++){
			for(int i=0; i<numSen; i++){
				if(i>j) M[i][j]=-1;
				else if(i==j) M[i][j]=-2;				
			}
		}
		/*--	Compute M and H	-------------------------------------------------------------------------------------*/
		//compute distance for upper triangle of M
		for(int i=0; i<listS.size(); i++){
			for(int j=i+1; j<listS.size(); j++){
				M[i][j]=abs(listS.at(i).y - listS.at(j).y);
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
				h.diss.push_back(s);
				if(s<=eSpeed){
					if(M[j][i]<=r) M[i][j]=1;	//immedate edge
					else M[i][j]=0;	//hidden edge
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

		/*--	Clustering	---------------------------------------------------------------------------------------*/
		//1-Round for immediate edges
		bool exit=false;
		vector<int> node_merge;
		int time_merge=0;
		do{
			node_merge=SearchMin(eSpeed);	//Ci, Cj of H				
			if(node_merge.size()>0){
				update_diss(node_merge);
				if(newH.size()<H.size()){
					//output which nodes are used to merge
					#if 0
					cout<<"Merge= "<<time_merge<<" :: ";
					for(int i=0; i<H.at(node_merge.at(0)).C.item.size(); i++){
						cout<<H.at(node_merge.at(0)).C.item.at(i)<<",";
					}
					cout<<" <==> ";
					for(int i=0; i<H.at(node_merge.at(0)).NS.at(node_merge.at(1)).item.size(); i++){
						cout<<H.at(node_merge.at(0)).NS.at(node_merge.at(1)).item.at(i)<<",";
					}
					cout<<endl;
					#endif	
					H=newH;
					time_merge++;
					newH.erase(newH.begin(), newH.end());
				}else exit=true;
				#if 0
				for(int i=0; i<H.size(); i++){
					for(int k=0; k<H.at(i).C.item.size(); k++){
						cout<<H.at(i).C.item.at(k)<<",";
					}
					cout<<endl;
					for(int j=0; j<H.at(i).NS.size(); j++){
						for(int k=0; k<H.at(i).NS.at(j).item.size(); k++){
							cout<<H.at(i).NS.at(j).item.at(k)<<",";
						}
						cout<<"=>"<<H.at(i).diss.at(j)<<endl;
					}
					cout<<endl;
				}
				#endif				
			}else exit=true;			
			node_merge.erase(node_merge.begin(), node_merge.end());
		}while(exit!=true);
		//output results
		#if 1
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
					if(M[H.at(i).NS.at(j).item.at(0)-1][H.at(i).C.item.at(0)-1]==1){					
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
						if(M[back-1][newfront-1]==-1){
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
						if(M[back-1][newfront-1]==-1){
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
							if(M[back-1][newfront-1]==-1){
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