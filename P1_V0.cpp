#include <fstream.h>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>

using namespace std;

class Sensor{
        public:
		int id;
		int y;
		char dir;
		int numTs;
		vector<int> Ts;	 //index of the time slot
        vector<int> Sp;	 //Speed
};

class Itemset{
	public:
	vector<int> item;
};

int main(int argc, char *argv[])
{
	ifstream infile("input/N1 from 2008-1-15-0-00 to 2008-1-15-23-50_data.txt");
	
	//parameters
	int sizeW=6;		//size of the window
	float eSpeed=5;	//error of the speed
	float r=10*1000;	 //near in r meters
	
	//variables
	int numW;	//number of windows
	int numColumns;
	int numSen;	//number of sensors
	int numTimeSlots;
    char dir[2];	//dir:direction
	int indexSen;	//index of sensors
	int locationSen;	//location of the sensor
	int Speed;
	int timeMerge;
	vector<Sensor> listS;

	/*--	For input	----------------------------------------------------------------------------------*/
	Sensor tempS;

	infile>>numColumns;
	tempS.numTs=numColumns-2;
	numW=(numColumns-2)/sizeW;
	infile>>numSen;
	infile>>dir[0];	
	tempS.dir=dir[0];
	//cout<<numColumns<<endl;
	//cout<<numSen<<endl;
	//cout<<dir[0]<<endl;		
	
	for(int i=0; i<numSen; i++){		
		infile>>indexSen;
		tempS.id=indexSen;
		infile>>locationSen;
		tempS.y=locationSen;
		//cout<<"Sensor= "<<indexSen;
		//cout<<", Location= "<<locationSen<<"	::	";
		
		for(int j=0; j<numColumns-2; j++){
			infile>>Speed;
			tempS.Ts.push_back(j+1);
			tempS.Sp.push_back(Speed);
			//cout<<Speed<<" ";
		}
		listS.push_back(tempS);		
		tempS.Ts.erase(tempS.Ts.begin(), tempS.Ts.end());
		tempS.Sp.erase(tempS.Sp.begin(), tempS.Sp.end());
		//cout<<endl;		
	}

	#if 0
	infile>>dir[1];

	for(int i=0; i<numSen; i++){
		infile>>indexSen;
		cout<<"Sensor= "<<indexSen;
		infile>>locationSen;
		cout<<", Location= "<<locationSen<<"	::	";
		for(int j=0; j<numColumns-2; j++){
			infile>>Speed;
			cout<<Speed<<" ";
		}
		cout<<endl;		
	}
	#endif
	
	//output the listS
	#if 0	
	for(int i=0; i<listS.size(); i++){	
		cout<<listS.at(i).id<<", "<<listS.at(i).y<<endl;
		for(int j=0; j<listS.at(i).Ts.size(); j++){
			cout<<listS.at(i).Sp.at(j)<<" ";									
		}        
		cout<<endl;			
	}
	getch();	
	#endif

	/*--	Method 1	-----------------------------------------------------------------------------------*/
	//1st round to compute all distance and dissimilarity	
	float M[1][79][79];	//numSen=79	
	vector<Itemset> R;	//result of clustering

	for(int iW=0; iW<numW; iW++){			
		//initial
		timeMerge=0;
		for(int i=0; i<numSen; i++){
			for(int j=0; j<numSen; j++){
				if(i>j){
					M[0][i][j]=-1;
				}				
			}
		}
	
		//compute dissimilarity
		for(int i=0; i<listS.size(); i++){
			for(int j=i+1; j<listS.size(); j++){
				float m=0;
				for(int k=0; k<sizeW; k++){
					m=m+pow(listS.at(i).Sp.at(iW+k)-listS.at(j).Sp.at(iW+k),2);
				}					
				M[0][i][j]=sqrt(m/sizeW);	
				if(M[0][i][j]<=eSpeed){
					M[0][j][i]=0;		//possible link
				}
			}
		}

		//compute distance for the link relation
		for(int j=0; j<listS.size(); j++){
			for(int i=j+1; i<listS.size(); i++){
				if(M[0][i][j]==0 && abs(listS.at(i).y - listS.at(j).y)<= r){
					M[0][i][j]=1;	 //directed link
				}
			}
		}
	

		//show the information
		#if 0
		for(int i=0; i<12; i++){
			for(int j=0; j<12; j++){
				cout<<M[0][i][j]<<"  ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
		getch();
		#endif
		
		
		//Start Clustering----------------------------------------------------------- 
		int tolClusters=numSen;
		int tempClusters;	

		Itemset tempI;
		bool select[79];	//numSen
		for(int i=0; i<numSen; i++){
			select[i]=0;
		}
		//1st Round
		for(int j=0; j<listS.size(); j++){
			if(select[j]==0){
				select[j]=1;
				tempI.item.push_back(j+1);
				for(int i=j+1; i<listS.size(); i++){
					if(select[i]==0	&&	M[0][i][j]==1){
						select[i]=1;
						tempI.item.push_back(i+1);
						timeMerge++;
						for(int k=i+1; k<listS.size(); k++){
							if(select[k]==0){
								if(M[0][k][j]==-1 || M[0][k][j]==0) break;
								else {
									bool insert=1;
									for(int pos=0; pos<tempI.item.size(); pos++){
										if(M[0][k][tempI.item.at(pos)]!=1 ){
											insert=0;
											break;
										}
									}
									if(insert==1){
										select[k]=1;
										tempI.item.push_back(k+1);
										timeMerge++;
									}						
								}
							
							}						
						}												
					}
				}
				R.push_back(tempI);
				tempI.item.erase(tempI.item.begin(), tempI.item.end());
			}	
		}

		#if 0
		cout<<iW+1<<"-Window: "<<timeMerge<<"-Round ==> "<<R.size()<<" Clusters"<<endl;
		for(int i=0; i<R.size(); i++){
			cout<<"{";
			for(int j=0; j<R.at(i).item.size(); j++){
				cout<<R.at(i).item.at(j)<<",";
			}
			cout<<"}"<<endl;
		}
		cout<<endl<<endl;
		getch();
		#endif		
		
		
		
		//Other Rounds
		tempClusters=R.size();
		int timeRound=1;
		vector<Itemset> tempR;
		Itemset selectI;

		#if 1
		do{
			//Clustering
			for(int i=0; i<R.size(); i++){
				selectI.item.push_back(0);
			}
			for(int i=0; i<R.size(); i++){
				if(selectI.item.at(i)==0){		
					selectI.item.at(i)=1;
					for(int ii=0; ii<R.at(i).item.size(); ii++){
						tempI.item.push_back(R.at(i).item.at(ii));
					}
					bool insert=0;
					for(int j=i+1; j<R.size(); j++){
						if(insert==0	&&	 selectI.item.at(j)==0){
							insert=1;
							bool pos_insert=0;	
							for(int ii=R.at(i).item.size()-1; ii>=0; ii--){
								for(int jj=0; jj<R.at(j).item.size(); jj++){									
									int front=R.at(i).item.at(ii)-1,back=R.at(j).item.at(jj)-1;
									//testing whether exists "a bridge"
									if(front<back){
										if(M[0][back][front]==1){
											pos_insert=1;
											break;
										}
									}else{
										if(M[0][front][back]==1){
											pos_insert=1;
											break;
										}
									}							
								}
								if (pos_insert==1) break;
							}							
							if(pos_insert==1){								
								for(int ii=0; ii<R.at(i).item.size(); ii++){
									for(int jj=0; jj<R.at(j).item.size(); jj++){
										int front=R.at(i).item.at(ii)-1,back=R.at(j).item.at(jj)-1;
										if(front<back){
											if(M[0][back][front]==-1){
												insert=0;
												break;
											}
										}else{
											if(M[0][front][back]==-1){
												insert=0;
												break;
											}
										}	
									}
									if(insert==0) break;
								}								
								if(insert==1){
									selectI.item.at(j)=1;
									for(int jj=0; jj<R.at(j).item.size(); jj++){
										tempI.item.push_back(R.at(j).item.at(jj));
									}
									timeMerge++;
								}
							}else insert=0;
						}
						if(insert==1) break;
					}
					tempR.push_back(tempI);
					tempI.item.erase(tempI.item.begin(), tempI.item.end());
				}
			}
	
			tempClusters=tempR.size();

			if(tempClusters>=tolClusters	||	tempClusters==0){				
				break;
			}else{
				tolClusters=tempClusters;
				timeRound++;
				#if 0
				cout<<iW+1<<"-Window: "<<timeMerge<<"-Round ==> "<<tolClusters<<" Clusters"<<endl;
				for(int i=0; i<tempR.size(); i++){
					cout<<"{";
					for(int j=0; j<tempR.at(i).item.size(); j++){
						cout<<tempR.at(i).item.at(j)<<",";
					}
					cout<<"}"<<endl;
				}
				cout<<endl<<endl;
				getch();
				#endif		
			}
			R.erase(R.begin(), R.end());
			R=tempR;
			tempR.erase(tempR.begin(), tempR.end());
		}while(tolClusters>1);
		#endif	
				
		#if 1
		cout<<iW+1<<"-Window: "<<timeMerge<<"-Round ==> "<<R.size()<<" Clusters"<<endl;		
		/*
		for(int i=0; i<R.size(); i++){
			cout<<"{";
			for(int j=0; j<R.at(i).item.size(); j++){
				cout<<R.at(i).item.at(j)<<",";
			}
			cout<<"}"<<endl;
		}
		cout<<endl<<endl;		
		getch();
		*/		
		#endif
			
		R.erase(R.begin(), R.end());
	}//End this window
	//getch();
	return 0;
}
