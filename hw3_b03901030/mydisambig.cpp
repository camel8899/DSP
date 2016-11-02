#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "Ngram.h"

using namespace std;

int main(int argc, char *argv[])
{
	vector<string>V[15000];
	map<string, int>M;    
	string t,m,l;
	for(int i=1; i<argc; i++)
	{
		string get = argv[i];
		if(get == "-text")
			t = argv[i+1];
		else if(get == "-map")
			m = argv[i+1];
		else if(get == "-lm")
			l = argv[i+1];
	}
	ifstream text_in(t.c_str()),map_in(m.c_str()),lm_in(l.c_str());
	int ngram_order = 2;
	Vocab voc;
	Ngram lm( voc, ngram_order );
	{
		File lmFile("./bigram.lm", "r" );
        	lm.read(lmFile);
        	lmFile.close();
    	}
	
	string str,words;
	int count = 0;
	while(getline(map_in,str))
	{
		string chara = str.substr(0,2);
		if(M.count(chara)==0)
		{
			M[chara] = count;
			count++;
		}
		int num = M[chara];
		for(int i=0; i<str.size(); i++)
		{
			if(str[i] == ' '||str[i] == '/')
			{	
				words = str.substr(i+1,2);
				V[num].push_back(words);
			}
		}
	}
	string inputline,temp;
	vector<string>Nspace,Done;
	while(getline(text_in,inputline))
	{	
		VocabIndex wid;
		//change to no space
		for(int i=0; i<inputline.size(); i++)
		{
			if(inputline[i]!=' ')
			{
				Nspace.push_back(inputline.substr(i,2));
				i++;
			}
		}
		//start decoding with no space
		vector<double>prob[Nspace.size()];
		vector<int>back[Nspace.size()];
		//first character with unigram
		for(int i=0; i<V[M[Nspace[0]]].size(); i++)
		{
			wid = voc.getIndex(V[M[Nspace[0]]][i].c_str());
			if(wid == Vocab_None)	
				wid = voc.getIndex(Vocab_Unknown);
			VocabIndex context[] = {Vocab_None,Vocab_None};
			prob[0].push_back(lm.wordProb(wid,context));
			back[0].push_back(0);
		}
		//second charactor to end
		for(int i=1; i<Nspace.size(); i++)
		{
			int now = M[Nspace[i]],prev = M[Nspace[i-1]];
			for(int j=0; j<V[now].size(); j++)
			{
				double max = -10000000;
				int backsig;
				VocabIndex wid1 = voc.getIndex(V[now][j].c_str());
				if(wid1 == Vocab_None)
					 wid1 = voc.getIndex(Vocab_Unknown);
				for(int k=0; k<prob[i-1].size(); k++)
				{
					VocabIndex wid2 = voc.getIndex(V[prev][k].c_str());
					if(wid2 == Vocab_None)
						wid2 = voc.getIndex(Vocab_Unknown);
					VocabIndex context[] = {wid2,Vocab_None};
					double temp = prob[i-1][k] + lm.wordProb(wid1,context);
					if (temp > max)
						{
							max = temp;
							backsig = k;
						}
				}
				prob[i].push_back(max);
				back[i].push_back(backsig);
			}
		}
		double max = prob[Nspace.size()-1][0];
		int endat = 0 ;
		for(int i=1; i<prob[Nspace.size()-1].size(); i++)
		{
			if(prob[Nspace.size()-1][i] > max)
			{
				max = prob[Nspace.size()-1][i];
				endat = i;
			}		
		}
		int trace = endat;
		Done.push_back(V[M[Nspace[Nspace.size()-1]]][endat]);
		for(int i=Nspace.size()-1; i>0; i--)
		{
			Done.push_back(V[M[Nspace[i-1]]][back[i][trace]]);
			trace = back[i][trace];
		}
		cout << "<s>" << " "; 
		for(int i=Done.size()-1; i>=0; i--)
			cout << Done[i] << " ";
		cout << "</s>" << endl;
		Nspace.clear();
		Done.clear();
	}
	text_in.close();
	map_in.close();
	lm_in.close();
	return 0;
}

