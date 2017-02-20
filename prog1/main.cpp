#include <iostream>
#include <map>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <iterator>

const int WORDS_COUNT = 5;

struct Statistic
{
	int count;
	std::string word;
} Stat;

std::string prepare(std::string &s)
{
	std::string result;
	std::cout << "LOL" << std::endl;
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	result = s;
	std::cout << "LOL" << std::endl;
	const std::set<char> punctuationMarks = 
		{ '.', ',', ':', '!',';',';','<','>','#', '%','^','*','(',')' };
	std::set<char>::iterator it;
	for(int i = 0; i < punctuationMarks.size(); i++)
		if (punctuationMarks.find(s[i]) != punctuationMarks.end())
			result.pop_back();
	return result;
}

bool comp(Statistic &a, Statistic &b)
{
	return a.count > b.count;
}

int main()
{
	std::ifstream file("file.txt");
	std::map<std::string, int> m;
	std::map<std::string, int>::iterator p;
	if (file.is_open())
	{
		std::string word;
		while (!file.eof());
		{
			file >> word;
			word = prepare(word);
			m[word]++;
		}

	std::vector<Statistic> s;

	int i = 0;
	for (p = m.begin(); p != m.end(); p++)
	{
		s.push_back(Stat);
		s.at(i).count = p->second;
		s.at(i).word = p->first;
	}

	std::sort(s.begin(), s.end(), comp);
	for (i = 0; i < WORDS_COUNT; i++)
		std::cout << s.at(i).word << std::endl;
	file.close();
	}
	return 0;
}
