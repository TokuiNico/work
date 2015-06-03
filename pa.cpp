#include <fstream.h>
using namespace std;

int main(int argc, char *argv[])
{
	ifstream infile("pa.txt");

	char *dirO="pa/";
	char *output;
	output=(char *)malloc(256);

	int w,e,r;
	int filename=1;
	int line=0;
	while (infile){
		infile>>w>>e>>r;
		line++;
		snprintf(output, 256, "%s/%d", dirO, filename);	
		ofstream outfile(output);
		outfile<<"2008-1-15-0-00-2008-1-15-23-50_data.txt"<<"	 "<<w<<"	"<<e<<"	"<<r<<endl;
		filename++;
		outfile.close();
	}

	ofstream outfile("pa.bat");
	for(int i=1; i<=line; i++){
		outfile<<"P1_V1.exe<pa\\"<<i<<endl;
	}
	return 0;
}
