#include "hmm.h"
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
double viterbi(HMM&, int*, int);
void testmodels(string, string, string);
int main(int argc, char* argv[])
{
/*	string list = argv[1];
	HMM hmms[5];	
	load_models(list.c_str(), hmms, 5);
	ofstream ofile("r.txt");
	for(int i =0; i<5; i++)
	ofile << hmms[i].model_name << endl;
	ofile.close();
*/
	string list = argv[1];
	string test = argv[2];
	string result = argv[3];
	testmodels(list, test, result);	
	return 0;
}
double viterbi(HMM &hmm, int ob[MAX_SEQ], int l)
{
	double delta[MAX_STATE][MAX_SEQ];
	for(int i=0; i<l; i++)
	{
		for(int j=0; j<hmm.state_num; j++)
		{
			if(i==0)
				delta[j][i] = hmm.initial[j]*hmm.observation[ob[i]][j];
			else
			{
				double max = delta[0][i-1]*hmm.transition[0][j];
				for(int k=1; k<hmm.state_num; k++)
					if(delta[k][i-1]*hmm.transition[k][j]>max) max = delta[k][i-1]*hmm.transition[k][j];
				delta[j][i] = max*hmm.observation[ob[i]][j];
			}
		}	
	}
	double deltaT = delta[0][l-1];
	for(int i=1; i<hmm.state_num; i++)
		if(delta[i][l-1]>deltaT) deltaT = delta[i][l-1];
	return deltaT;
}
void testmodels(string list, string test, string result)
{
	HMM hmms[5];	
	load_models(list.c_str(), hmms, 5);
	ifstream ifile(test.c_str());
	ofstream ofile(result.c_str());
	vector<string>v;
	string input_seq;
	int ob[MAX_SEQ]={};
	while(ifile >> input_seq)
		v.push_back(input_seq);
	for(int i=0; i<v.size(); i++)
	{
		string seq = v[i];
		int strl = seq.length();
		for(int j=0; j<strl; j++)
			ob[j] = seq[j]-'A';
		double max = viterbi(hmms[0], ob, strl);
		int index = 0;
		for(int k=1; k<5; k++)
			if(viterbi(hmms[k], ob, strl)>max) max = viterbi(hmms[k], ob, strl), index = k;
		ofile << hmms[index].model_name << " " << max << endl;
	}
	ifile.close();
	ofile.close();
}
