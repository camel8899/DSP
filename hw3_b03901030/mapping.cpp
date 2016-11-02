#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

int main(int argc, char* argv[])
{
	vector<string>V[15000];
	map<string, int>ZtoI;
	string in = argv[1];
	string out = argv[2];
	ifstream input(in.c_str());
	ofstream output(out.c_str());
	string str,same[5];
	int count=0;
	while(getline(input,str))
	{
		int samew = 0;
		string chara = str.substr(0,2);
		for(int i=0; i<str.size(); i++)
		{
			bool flag = 0;
			if(str[i] == ' '||str[i] == '/')
			{	
				same[samew] = str.substr(i+1,2);
                         	if(ZtoI.count(same[samew])==0)
				{
					ZtoI[same[samew]]=count++;
				}
				for(int j=0; j<samew; j++)
				{
					if(same[samew]==same[j])
					flag=1;
				}
				if(!flag)
					samew ++;
			}
		}
		for(int i=0; i<samew; i++)
		{
			V[ZtoI[same[i]]].push_back(chara);
		}
		if(ZtoI.count(chara)==0)
		{
			ZtoI[chara] = count;
			count ++;
		}
		V[ZtoI[chara]].push_back(chara);
	}
	map<string ,int>::iterator it;
	for(it = ZtoI.begin(); it != ZtoI.end(); it++)
	{
		string Z = it->first;
		int num = it->second;
		output << Z << " ";
		for(int i=0; i<V[num].size(); i++)
		{
			output << V[num][i];
			if(i<V[num].size()-1)
				output << " ";
			else
				output << endl;
		}
	}
	input.close();
	output.close();
	return 0;	
}
