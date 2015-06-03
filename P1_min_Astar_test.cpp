/*
	Modified from P1_min.cpp 
	Method: A* 
	Output: RunTime
*/

#include <fstream.h>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

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

class CanSet{
	public:
		vector<Itemset> Seq;
};

//int Max_diss=1000;
vector<Board> H,newH,tempH;	//Clustering result
int M[100][100];	//max. number of sensors=100
vector<CanSet> Can;
vector<Itemset> Base;

void GenBase(vector<Itemset> Base_t);
void GenCandidate(int numSen, int N, Itemset I);
int SelectItem(int next, int numSen, Itemset choice);
bool checkConnected(Itemset checkCluster);
bool checkComplete(Itemset checkCluster);

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

	struct time start,end;

	
	//vector<Itemset> Base;
	//cout<<"*****************************************************"<<endl;
	//cout<<"Number of Sensors= "<<numSen<<endl;
	for(int i=1; i<=numSen; i++){	// i = number of clusters
		//Generate possible |Ci| for each Ci
		if(i==1){
			Itemset _I;
			_I.item.push_back(1);
			_I.item.push_back(1);
			Base.erase(Base.begin(), Base.end());
			Base.push_back(_I);
			_I.item.erase(_I.item.begin(), _I.item.end());
		}else{
			GenBase(Base);
		}	
		//cout<<"Num= "<<i<<" Done!"<<endl;
	}
	//Rank Base
	vector<Itemset> tempBase;
	for(int i=0; i<Base.size(); i++){
		Itemset _I;
		_I=Base.at(i);
		//Search
		if(i==0){
			tempBase.push_back(_I);
		}else{
			int num;
			num=_I.item.at(0);
			vector <Itemset>::iterator pos=tempBase.begin();
			int j=0;			
			while(j<tempBase.size()){
				if(num<tempBase.at(j).item.at(0)){
					break;
				}else{
					j++;
					pos++;
				}
			}
			tempBase.insert(pos, _I);
		}
	}
	Base=tempBase;
	tempBase.erase(tempBase.begin(), tempBase.end());

	#if 0 
	cout<<endl;
	cout<<"Base:"<<endl;
	for(int i=0; i<Base.size(); i++){
		cout<<Base.at(i).item.at(0)<<" => ";
		for(int j=1; j<Base.at(i).item.size(); j++){
			cout<<Base.at(i).item.at(j)<<" ";
		}
		cout<<endl;
	}
	#endif

	for(int iW=0; iW<numW; iW++){
		//Windows
		//Starting to count Runtime
		if(iW>6 && iW!=23){ 

		gettime(&start);

		//initial		
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

		#if 1
		//vector<CanSet> Can;
		CanSet Result;
		Result.Seq.erase(Result.Seq.begin(), Result.Seq.end());

		bool found=false;
		for(int i=1; i<=numSen; i++){	//number of clusters
			if(found==true) break;
			//Generate possible combinations for each Ci
			for(int j=0; j<Base.size(); j++){
				if(Base.at(j).item.at(0)==i){
					#if 0
					cout<<"Combinations: => ";
					for(int k=1; k<Base.at(j).item.size(); k++){
						cout<<Base.at(j).item.at(k)<<" ";
					}
					cout<<endl;
					#endif
					Can.erase(Can.begin(), Can.end());
					GenCandidate(numSen, i, Base.at(j));
					//Can=GenCandidate(numSen, i, Base.at(j));
					#if 0
					for(int k=0; k<Can.size(); k++){
						for(int t=0; t<Can.at(k).Seq.size(); t++){
							cout<<"< ";
							for(int s=0; s<Can.at(k).Seq.at(t).item.size(); s++){				
								cout<<Can.at(k).Seq.at(t).item.at(s)<<",";								
							}
							cout<<" > ";							
						}						
						cout<<endl;
					}
					#endif		
					#if 0
					for(int k=0; k<Can.size(); k++){
						bool relation=true;
						for(int t=0; t<Can.at(k).Seq.size(); t++){	//<()()...()>
							//Check connected
							relation=checkConnected(Can.at(k).Seq.at(t));
							if(relation==false) break;
							else{
								//Check complete
								relation=checkComplete(Can.at(k).Seq.at(t));
								if(relation==false) break;							
							}
						}
						if(relation==true){
							found=true;
							Result=Can.at(k);
							break;
						}
					}
					#endif
					if(Can.size()>0){
						found=true;
						Result=Can.at(0);
						break;
					}
				}else if(Base.at(j).item.at(0)>i) break;		
				if(found==true) break;
			}			
		}
		#endif


		cout<<iW+1<<"	";
		//windows
		//counting Runtime and output
		#if 1   
		gettime(&end);
	   double runtime;
	   runtime=(double(end.ti_hour)-double(start.ti_hour))*60; //min
	   runtime=(runtime+double(end.ti_min)-double(start.ti_min))*60; //sec
	   runtime=(runtime+double(end.ti_sec)-double(start.ti_sec))*1000; //msec
	   runtime=runtime+double(end.ti_hund)*10-double(start.ti_hund)*10;  //msec
	  // cout<<"Runtime: "<<runtime/1000<<" sec"<<endl;	
	  cout<<runtime<<"	";
	   #endif

		//output results
		cout<<Result.Seq.size()<<endl;
		#if 0
		cout<<iW+1<<"-Window:: "<<Result.Seq.size()<<" Clusters"<<endl;
		for(int i=0; i<Result.Seq.size(); i++){
			cout<<"{";
			for(int j=0; j<Result.Seq.at(i).item.size(); j++){
				cout<<Result.Seq.at(i).item.at(j)<<",";
			}
			cout<<"},";
		}
		cout<<endl<<endl;
		#endif
		H.erase(H.begin(), H.end());
	}
	
	}// for iW
	return 0;
}

void GenBase(vector<Itemset> Base_t){
	vector<Itemset> Result;
	Result.erase(Result.begin(), Result.end());
	Itemset _I,tempI;
	_I.item.erase(_I.item.begin(), _I.item.end());
	int num;
	int target;
	for(int i=0; i<Base_t.size(); i++){		
		tempI=Base_t.at(i);
		num=tempI.item.at(0);
		//the last number +1
		target=num;
		if(num==1){
			_I.item.push_back(num);
			_I.item.push_back(tempI.item.at(target)+1);
			Result.push_back(_I);
			_I.item.erase(_I.item.begin(), _I.item.end());
		}else if(num>1){
			if(tempI.item.at(target-1)>=(tempI.item.at(target)+1)){
				_I.item.push_back(num);
				for(int j=1; j<target; j++){
					_I.item.push_back(tempI.item.at(j));
				}
				_I.item.push_back(tempI.item.at(target)+1);		
				Result.push_back(_I);
				_I.item.erase(_I.item.begin(), _I.item.end());				
			}
		}
		//insert 1
		target=num+1;
		_I.item.push_back(target);
		for(int j=1; j<target; j++){
			_I.item.push_back(tempI.item.at(j));
		}
		_I.item.push_back(1);
		Result.push_back(_I);
		_I.item.erase(_I.item.begin(), _I.item.end());	
	}
	Base=Result;
	//return Result;
}

void GenCandidate(int numSen, int N, Itemset I){
	vector<CanSet> tempCan;
	vector<Itemset> K, newK,  chooseSen, chooseSen_next, tempchooseSen, newchooseSen;	//element of Can	
	Itemset _I, tempI, choice, newChoice;
	vector<int> posList,tempposList;	//for K
	int item, tempNumSen;

	//initial
	for(int i=1; i<=numSen; i++){
		choice.item.push_back(0);
		//out.push_back(0);
	}
	Can.erase(Can.begin(), Can.end());
	
	K.erase(K.begin(), K.end());
	//Generate items for each cluster
	tempNumSen=numSen;
	for(int i=0; i<N; i++){
		//cout<<"Index of Clusters= "<<i<<endl;
		chooseSen_next.erase(chooseSen_next.begin(), chooseSen_next.end());
		_I.item.erase(_I.item.begin(), _I.item.end());
		int pos=0;
		int numItems=I.item.at(i+1);			
		if(i==0){
			for(int j=0; j<numItems; j++){
				//cout<<j<<" => ";
				if(K.size()==0){	
					posList.erase(posList.begin(), posList.end());
					for(int k=pos; k<(numSen-numItems+1); k++){
						//cout<<k<<":";
						newChoice=choice;
						item=SelectItem(k, numSen, newChoice);
						//cout<<"< "<<item<<endl;
						_I.item.push_back(item);
						newK.push_back(_I);					
						newChoice.item.at(item-1)=1;
						tempposList.push_back(item);
						newchooseSen.push_back(newChoice);
						_I.item.erase(_I.item.begin(), _I.item.end());
						newChoice.item.erase(newChoice.item.begin(), newChoice.item.end());
					}
				}else{
					//cout<<"K size:"<<K.size()<<" = "<<chooseSen.size()<<endl;
					for(int h=0; h<K.size(); h++){					
						_I=K.at(h);
						choice=chooseSen.at(h);					
						pos=posList.at(h);
						
						for(int k=pos; k<(numSen-(numItems-j)+1); k++){
							//cout<<k<<":";
							newChoice=choice;
							tempI=_I;
							//check possible
							int count=0;
							for(int s=k; s<numSen; s++){
								if(newChoice.item.at(s)==0)	 count++;
							}
							//cout<<"	count="<<count;
							if(count>=(numItems-j)){
								item=SelectItem(k, numSen, newChoice);
								//cout<<"< "<<item<<endl;
								tempI.item.push_back(item);

								//Check connected
								bool relation=checkConnected(tempI);
								if(relation==true){
									//Check complete
									relation=checkComplete(tempI);
									if(relation==true){
										newK.push_back(tempI);	
										newChoice.item.at(item-1)=1;
										tempposList.push_back(item);
										newchooseSen.push_back(newChoice);
									}							
								}
								tempI.item.erase(tempI.item.begin(), tempI.item.end());								
								newChoice.item.erase(newChoice.item.begin(), newChoice.item.end());					
							}
						}		
					}
				}		
				K=newK;			
				chooseSen=newchooseSen;				
				posList=tempposList;
				newK.erase(newK.begin(), newK.end());
				newchooseSen.erase(newchooseSen.begin(), newchooseSen.end());		
				tempposList.erase(tempposList.begin(), tempposList.end());
			}

			bool relation;
			CanSet Set;
			tempchooseSen=chooseSen;
			chooseSen.erase(chooseSen.begin(), chooseSen.end());
			for(int j=0; j<K.size(); j++){
				//Check connected
				relation=checkConnected(K.at(j));
				if(relation==true){
					//Check complete
					relation=checkComplete(K.at(j));
					if(relation==true){
						Set.Seq.push_back(K.at(j));
						chooseSen.push_back(tempchooseSen.at(j));
						tempCan.push_back(Set);
						Set.Seq.erase(Set.Seq.begin(), Set.Seq.end());					
					}							
				}
			}	

		}else{	//end if i=0
			
			CanSet Set, tempSet; //subsequence of Can

			for(int t=0; t<Can.size(); t++){
				K.erase(K.begin(), K.end());
				newchooseSen.erase(newchooseSen.begin(), newchooseSen.end());

				choice=chooseSen.at(t);			
				Set.Seq.erase(Set.Seq.begin(), Set.Seq.end());
				for(int tt=0; tt<Can.at(t).Seq.size(); tt++){
					Set.Seq.push_back(Can.at(t).Seq.at(tt));
				}	

				for(int j=0; j<numItems; j++){
					if(j==0){				
						_I.item.erase(_I.item.begin(), _I.item.end());
						for(int t=0; t<numSen; t++){
							if(choice.item.at(t)==0){
								pos=t;
								break;
							}
						}
						while(pos<(numSen-numItems+1)){
							newChoice=choice;
							int count=0;
							for(int t=pos; t<numSen; t++){
								if(choice.item.at(t)==0){
									pos=t;
									break;
								}
							}
							for(int s=pos; s<numSen; s++){
								if(newChoice.item.at(s)==0)	 count++;
							}							
							if(count>=(numItems-j)){
								item=SelectItem(pos, numSen, newChoice);
								_I.item.push_back(item);

								//Check connected
								bool relation=checkConnected(_I);
								if(relation==true){
									//Check complete
									relation=checkComplete(_I);
									if(relation==true){
										newK.push_back(_I);					
										newChoice.item.at(item-1)=1;
										tempposList.push_back(item);
										newchooseSen.push_back(newChoice);
									}							
								}
								_I.item.erase(_I.item.begin(), _I.item.end());
								newChoice.item.erase(newChoice.item.begin(), newChoice.item.end());
							}
							pos++;
						}

					}else{
						for(int h=0; h<K.size(); h++){					
							_I=K.at(h);

							choice=tempchooseSen.at(h);
							pos=posList.at(h);
							//cout<<"POS= "<<pos<<endl;						
							while (pos<(numSen-(numItems-j)+1) ){								
								for(int t=pos; t<numSen; t++){
									if(choice.item.at(t)==0){
										pos=t;
										break;
									}
								}
								newChoice=choice;
								int count=0;
								for(int s=pos; s<numSen; s++){
									if(newChoice.item.at(s)==0)	 count++;
								}	
								Itemset tempI;
								tempI=_I;
								if(count>=(numItems-j)){
									item=SelectItem(pos, numSen, newChoice);
									tempI.item.push_back(item);

									//Check connected
									bool relation=checkConnected(tempI);
									if(relation==true){
										//Check complete
										relation=checkComplete(tempI);
										if(relation==true){
											newK.push_back(tempI);
											newChoice.item.at(item-1)=1;
											tempposList.push_back(item);
											newchooseSen.push_back(newChoice);
										}							
									}
									tempI.item.erase(tempI.item.begin(), tempI.item.end());	
									newChoice.item.erase(newChoice.item.begin(), newChoice.item.end());				
							
								}
								pos++;
							}													
						}				
					}			
					K=newK;
					tempchooseSen=newchooseSen;
					posList=tempposList;
					newK.erase(newK.begin(), newK.end());
					newchooseSen.erase(newchooseSen.begin(), newchooseSen.end());	
					tempposList.erase(tempposList.begin(), tempposList.end());
				}// End for a cluster

				bool relation;
				for(int k=0; k<K.size(); k++){
					//Check connected
					relation=checkConnected(K.at(k));
					if(relation==true){
						//Check complete
						relation=checkComplete(K.at(k));
						if(relation==true){
							Set.Seq.push_back(K.at(k));
							tempCan.push_back(Set);
							Set.Seq.pop_back();	
							chooseSen_next.push_back(tempchooseSen.at(k));				
						}							
					}
				}
				tempchooseSen.erase(tempchooseSen.begin(), tempchooseSen.end());
			}			
		}	//End if-Else index i >0 (of clusters) 

		Can=tempCan;
		if(i>0) chooseSen=chooseSen_next;
		//if(i>0) chooseSen=tempchooseSen;
		tempCan.erase(tempCan.begin(), tempCan.end());
		chooseSen_next.erase(chooseSen_next.begin(), chooseSen_next.end());


	}
	//return Can;
}

int SelectItem(int next, int numSen, Itemset choice){
	int pos=next;
	int item=-1;

	for(int i=pos; i<numSen; i++){
		if(choice.item.at(i)==0){
			item=i+1;
			break;
		}
	}
	
	return item;
}

bool checkConnected(Itemset checkCluster){
	bool outcome; 

	Itemset oldI,tempI;	
	oldI=checkCluster;

			
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
	
	if(tempI.item.size()==checkCluster.item.size()) outcome=true;
	else outcome=false;

	return outcome;
}


bool checkComplete(Itemset checkCluster){
	bool outcome;
	Itemset testCluster,tempI;	
	testCluster=checkCluster;

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
	if(ok==false) outcome=false;
	else outcome=true;
	
	return outcome;
}



