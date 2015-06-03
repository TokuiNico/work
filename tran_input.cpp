#include <fstream.h>
#include <iostream.h>

int main(int argc, char *argv[])
{
	ifstream infile("input/1230.txt");
	int c;
	int r;
	int temp;
	int speed;
	float location;
	infile>>c>>r;	
	//cout<<c<<endl;
	//cout<<r<<endl;
	for(int i=0; i<r; i++){
		infile>>temp;
		infile>>location;
		infile>>speed;
			
		cout<<"1"<<"	"<<location/1000<<"	 "<<speed<<endl;
		for(int j=2;j<=c-2; j++){
			infile>>speed;
			cout<<j<<"	"<<location/1000<<"	 "<<speed<<endl;
		}
		
		/*
		if(speed==0){
			cout<<"1"<<"	"<<location/1000<<"	 "<<"-1"<<endl;
			for(int j=2;j<=c-2; j++){
				infile>>speed;
				cout<<j<<"	"<<location/1000<<"	 "<<"-1"<<endl;
			}
		}else{
			cout<<"1"<<"	"<<location/1000<<"	 "<<speed<<endl;
			for(int j=2; j<=c-3; j++){
				infile>>speed;
				cout<<"1"<<"	"<<location/1000<<"	 "<<speed<<endl;
			}
		}
		*/
		cout<<endl;
	}
	
	return 0;
}
