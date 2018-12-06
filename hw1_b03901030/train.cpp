#include<math.h>
#include<iostream>
#include<cstdlib>
#include<string>
#include<vector>
#include<fstream>
#include "hmm.h"
using namespace std;
void alphaM(HMM&, double**, int*, int);
void betaM(HMM&, double**, int*, int);
void gammaM(HMM&, double**,double**, double**, int);
void epsilonM(HMM&, double**, double**, double**, int*, int);
void output(int, string, string, string);
void train(HMM&,vector<string>&);

int main(int argc ,char* argv[])
{
	int iter = atoi(argv[1]);
	string init = argv[2];
	string seq = argv[3];
	string trained  = argv[4];
	output(iter,init, seq, trained);	
	return 0;
}	
void alphaM(HMM &hmm,double alpha[][MAX_SEQ], int ob[MAX_SEQ], int l)
{
	for(int i=0; i<l; i++)
	{
		for(int j=0; j<hmm.state_num; j++)
		{
			if(i==0)
		  	alpha[j][i] = hmm.initial[j]*hmm.observation[ob[0]][j];
			else
			{
				double sum = 0;
				for(int k=0; k<hmm.state_num ; k++)
					sum += alpha[k][i-1]*hmm.transition[k][j];
				alpha[j][i] = sum * hmm.observation[ob[i]][j];
			}
		}
	}
}
void betaM(HMM &hmm, double beta[][MAX_SEQ], int ob[MAX_SEQ], int l)
{
	for(int i=l-1;i>=0; i--)
	{
		for(int j=0; j<hmm.state_num; j++)
		{
			if(i==l-1)
			beta[j][i] = 1;
			else
			{
				beta[j][i] = 0;
				for(int k=0; k<hmm.state_num; k++)
					beta[j][i] += beta[k][i+1]*hmm.transition[j][k]*hmm.observation[ob[i+1]][k];
			}		
		}
	}
}
void gammaM(HMM &hmm, double gamma[][MAX_SEQ], double alpha[][MAX_SEQ], double beta[][MAX_SEQ], int l)
{
	for(int i=0; i<hmm.state_num; i++)
	{
		for(int j=0; j<l; j++)
		{
		double sum = 0;
		for(int k=0; k<hmm.state_num; k++)
			sum += alpha[k][j]*beta[k][j];
		gamma[i][j] = alpha[i][j]*beta[i][j]/sum;
		}		
	}
}
void epsilonM(HMM &hmm, double epsilon[][MAX_STATE][MAX_SEQ], double alpha[][MAX_SEQ], double beta[][MAX_SEQ],int ob[MAX_SEQ], int l)
{
	double sum = 0;
	for(int i=0; i<hmm.state_num; i++)
		sum+=alpha[i][l-1];
	for(int t=0; t<l-1; t++)
	{
		for(int i=0; i<hmm.state_num; i++)
			for(int j=0; j<hmm.state_num; j++)
				epsilon[i][j][t]= alpha[i][t]*hmm.transition[i][j]*hmm.observation[ob[t+1]][j]*beta[j][t+1]/sum;
	}
}
void train(HMM &hmm, vector<string> &v)
{
	int ob[MAX_SEQ] = {};
	double alpha[MAX_STATE][MAX_SEQ]= {};
	double beta[MAX_STATE][MAX_SEQ] = {};
	double gamma[MAX_STATE][MAX_SEQ] = {};
	double epsilon[MAX_STATE][MAX_STATE][MAX_SEQ]= {};
	double gamma_sum[MAX_STATE] = {};
	double gamma_for_new_beta[MAX_OBSERV][MAX_STATE] = {};
	double gamma_1sumT[MAX_STATE] = {};
	double gamma_1sumt[MAX_STATE] = {};
	double epsilon_sum[MAX_STATE][MAX_STATE] = {};	
	for(int i=0; i<v.size(); i++)
	{
		string seq = v[i];
		int strl = seq.length();
		for(int j=0; j<strl; j++)
			ob[j] = seq[j]-'A';	
		alphaM(hmm, alpha, ob, strl);
		betaM(hmm, beta, ob, strl);
		gammaM(hmm, gamma, alpha, beta, strl);
		epsilonM(hmm, epsilon, alpha, beta, ob, strl);
		for(int j=0; j<hmm.state_num; j++)
		{
			gamma_sum[j] += gamma[j][0];
			for(int t=0; t<strl; t++)
			{
				if(t<strl-1)gamma_1sumt[j] += gamma[j][t];
				gamma_1sumT[j] += gamma[j][t];
				gamma_for_new_beta[ob[t]][j] += gamma[j][t];
			}
			for(int k=0; k<hmm.state_num; k++)
				for(int t=0; t<strl-1; t++)	
					epsilon_sum[j][k] += epsilon[j][k][t];
		}
	}
	for(int i=0; i<hmm.state_num; i++)
	{
		hmm.initial[i] = gamma_sum[i]/v.size();
		for(int j=0; j<hmm.state_num; j++)
		{
			hmm.transition[i][j] = epsilon_sum[i][j]/gamma_1sumt[i];
		}
		for(int j=0; j<hmm.observ_num; j++)
			hmm.observation[j][i] = gamma_for_new_beta[j][i]/gamma_1sumT[i];
	}
}
void output(int iter, string init_model, string seq_model, string trained_model)
{
	HMM initial;
	loadHMM(&initial,init_model.c_str());
	vector<string> seqs;
	string input_seq;
	ifstream ifile(seq_model.c_str());
	while(getline(ifile, input_seq, '\n'))
		seqs.push_back(input_seq);
	for(int i=0; i<iter; i++)
	{	
		cout << i << endl;
		train(initial, seqs);
	}
	FILE *ofile;
	ofile = fopen(trained_model.c_str(),"w");
	dumpHMM(ofile, &initial);
	ifile.close();
	fclose(ofile);
}
