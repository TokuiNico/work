#include <math.h>
#include <vector>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

class Sensor{
        public:
			int id;	//sensor id
			float x;	//location
			float y;	//location
			//char dir;	//N->0, S->1, E, W
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

vector<Board> H,newH;	//Clustering result
int M[500][500];	//max. number of sensors=100

vector<int> SearchMin(float eSpeed);
void update_diss(vector<int> node_merge);

int main(int argc, char *argv[]){		
	//parameters
	int sizeW;		//size of the window
	float eSpeed;	//error of the speed
	float r;	    //near in r meters
	
	//variables
	vector<Sensor> listS;
	int numW;		//number of windows
	int numColumns;
	int numSen;		//number of sensors
	int Speed;	
	int timeMerge;	
	int avgRound=0;
	int avgCluster=0;
	double start_time;

	/*--	For input	----------------------------------------------------------------------------------*/
	char *dirI="input/";
	char *filename;	
	char *input;
	filename=(char *)malloc(sizeof(char)*10);
	input=(char *)malloc(sizeof(char)*20);

	cin>>filename>>sizeW>>eSpeed>>r;
	cout<<sizeW<<"	"<<eSpeed<<"	"<<r<<endl;
	r=r*r;
	strcpy(input,"\0");
	strcat(input,dirI);
	strcat(input,filename);
	ifstream infile(input);	

	Sensor tempS;
	infile>>numColumns;

	tempS.numTs=numColumns-3;
	numW=(numColumns-3)/sizeW;
	infile>>numSen;
	//infile>>tempS.dir;	
	//cout<<numColumns<<endl;
	//cout<<numSen<<endl;	
	//cout<<tempS.dir;

	for(int i=0; i<numSen; i++){		
		infile>>tempS.id;
		infile>>tempS.x;
		infile>>tempS.y;				
		for(int j=0; j<tempS.numTs; j++){
			infile>>Speed;	
			tempS.Sp.push_back(Speed);			
		}
		tempS.x = (int) tempS.x;
		tempS.y = (int) tempS.y;
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

	start_time =(double)(clock())/(double)CLOCKS_PER_SEC; 
		
	for(int iW=0; iW<numW; iW++){
		//Windows
		double start;
		start =(double)(clock())/(double)CLOCKS_PER_SEC;

		//initial 下三角
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
				M[i][j]= (int)( (listS.at(i).y - listS.at(j).y)*(listS.at(i).y - listS.at(j).y) + (listS.at(i).x - listS.at(j).x)*(listS.at(i).x - listS.at(j).x) );
			}
		}	
		//compute dissimilarity
		Board h;
		float s;
		Itemset tempI;
		for(int b=0; b<listS.size(); b++){
			//h.exist=true;
			h.C.item.push_back(b+1);
			for(int i=b+1; i<listS.size(); i++){
				tempI.item.push_back(i+1);
				h.NS.push_back(tempI);
				tempI.item.erase(tempI.item.begin(), tempI.item.end());
				s=0;  // i和b的速度差
				for(int k=0; k<sizeW; k++){
					s=s+pow(listS.at(i).Sp.at(iW*sizeW+k)-listS.at(b).Sp.at(iW*sizeW+k),2);
				}					
				s=sqrt(s/sizeW);
				h.diss.push_back(s);
				if(s<=eSpeed){
					if(M[b][i]<=r) M[i][b]=1;	//immedate edge
					else M[i][b]=0;	//hidden edge
				}
			}
			H.push_back(h);
			h.C.item.erase(h.C.item.begin(), h.C.item.end());
			h.NS.erase(h.NS.begin(), h.NS.end());
			h.diss.erase(h.diss.begin(), h.diss.end());			
		}

		//show the information
		#if 0
		for(i=0; i<numSen; i++){
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
					for(i=0; i<H.at(node_merge.at(0)).C.item.size(); i++){
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
				for(i=0; i<H.size(); i++){
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
		
		#if 1
			char f_[100] ="\0";
			//int look_iW = 2;
			//if( look_iW == iW+1 )
			//{
				sprintf(f_,"iW/iW_%d.txt",iW+1);
				ofstream oss(f_);
				oss<<H.size()<<endl;
				for(int e=0; e<H.size(); e++)
				{
					oss<<H.at(e).C.item.size()<<endl;
					for(int r=0; r<H.at(e).C.item.size(); r++)
					{
						if( r != H.at(e).C.item.size()-1 )
							oss<<H.at(e).C.item.at(r)<<" ";
						else
						{
							oss<<H.at(e).C.item.at(r)<<endl;
							oss.flush();
						}
					}				
					//cout<<endl;
				}
				oss.close();
			
				ifstream infile2(f_);
				int group_num = 0;
				int id;
				infile2>>group_num;
				for(int g=0; g<group_num; g++)
				{		
					int member_num = 0;
					char file_[100] ="\0";
					sprintf(file_,"iW/iW_%d_group%d.dat",iW+1,g);
					ofstream oss(file_);
					infile2>> member_num;
					for(int e=0; e<member_num; e++)
					{
						infile2>>id;
						id--;
						oss<<id << " " << listS.at(id).x << " " << listS.at(id).y << endl;
					}
				}
				infile2.close();
				
				//plt
				//only one plt file
				sprintf(f_,"iW/iW.plt");
				ofstream oss3(f_, ios::out | ios::app);
				if(iW==0)
				{
					oss3<<"reset\n\n";
					oss3<<"cd "<<"\"C:/P1EXP/HBC/iW\"\n\n";
					oss3<<"set term postscript eps color enhanced 22\n\n";
				}
				 
				oss3<<"set output \"iW_"<<iW+1<<".eps\"\n";
				oss3<<"set title 'Cluster Result (HBC) - iW-"<<iW+1<<" : "<<group_num<<" clusters'\n\n";
				oss3<<"plot ";
				for(int g=0; g<group_num; g++)
				{
					if(g<group_num-1)
						oss3<<"\""<<"iW_"<<iW+1<<"_group"<<g<<".dat\" using 2:3 notitle, \\\n";
					else
						oss3<<"\""<<"iW_"<<iW+1<<"_group"<<g<<".dat\" using 2:3 notitle\n\n";
				}
				
				oss3.close();
				//each plt file
				//char filePath[] = "D:/P1_EXP/output/IC/";
				sprintf(f_,"iW/iW_%d.plt",iW+1);
				ofstream oss2(f_);
				oss2<<"reset\n\n";
				oss2<<"cd "<<"\"C:/P1EXP/HBC/iW\"\n\n";
				oss2<<"set term postscript eps color enhanced 22\n";
				oss2<<"set output \"iW_"<<iW+1<<".eps\"\n\n";
				oss2<<"set title 'Cluster Result (HBC) - iW_"<<iW+1<<"'\n\n";
				oss2<<"plot ";
				for(int g=0; g<group_num; g++)
				{
					if(g<group_num-1)
						oss2<<"\""<<"iW_"<<iW+1<<"_group"<<g<<".dat\" using 2:3 notitle, \\\n";
					else
						oss2<<"\""<<"iW_"<<iW+1<<"_group"<<g<<".dat\" using 2:3 notitle";
				}
				oss2.close();
			//}
		#endif

		
		cout<<iW+1<<"	";
		//cout<<iW+1<<" window:	 ";
		//windows
		//counting Runtime and output
	   #if 1   
		double runtime;
		double end;
		end =(double)(clock())/(double)CLOCKS_PER_SEC;
		runtime=end-start;
		cout<<runtime<<"	";
	   #endif

		//output results
		cout<<H.size()<<endl;
		//cout<<"Number of Clusters="<<H.size()<<endl;
		#if 0
		ofstream oss3("test");
		//#oss3<<iW+1<<"-Window:: "<<H.size()<<" Clusters"<<endl;
		
		for(int i=0; i<H.size(); i++){
			oss3<<i<<"  {";
			for(int j=0; j<H.at(i).C.item.size(); j++){
				oss3<<H.at(i).C.item.at(j)<<",";
			}
			oss3<<"}\n";
		}
		oss3<<endl<<endl;
		#endif
		H.erase(H.begin(), H.end());
	}
	double end_time, run_time; 
	end_time=(double)(clock())/(double)CLOCKS_PER_SEC;
	run_time=end_time-start_time;
	cout<<"Runtime ="<<run_time<<"\n\n";
	return 0;
}

vector<int> SearchMin(float eSpeed){
	vector<int> I;
	I.erase(I.begin(), I.end());
	int c1,c2;
	//int n1,n2;
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
						}else newfront=front;
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
						}else newfront=front;
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
						//cout<<front<<"	->	\n";
						int newfront=front;
						for(int kk=0; kk<H.at(i).NS.at(j).item.size(); kk++){
							back=H.at(i).NS.at(j).item.at(kk);
							//cout<<back<<"	:	";
							if(back<front){
								newfront=back;
								back=front;
							}else newfront=front;
							//cout<<"M["<<back-1<<"]["<<newfront-1<<"]="<<M[back-1][newfront-1]<<"\n";
							if(M[back-1][newfront-1]<0){
								done=false;
								break;
							}else if(M[back-1][newfront-1]==1){
								//cout<<"get bridge!\n";
								bridge=true;
							}
						}
						//cout<<"\n";
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

	for(int k=0; k<H.at(c1).C.item.size(); k++){
		newC.item.push_back(H.at(c1).C.item.at(k));
	}
	for(int j=0; j<H.at(c2).C.item.size(); j++){
		newC.item.push_back(H.at(c2).C.item.at(j));
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
