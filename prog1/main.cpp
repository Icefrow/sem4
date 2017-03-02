#include <iostream>
#include <map>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <locale>

const int WORDS_COUNT = 50;

struct Statistic
{
	int count;
	std::string word;
} ;

std::string prepare(std::string &s)
{
	std::string result;
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	result = s;
	result.erase(std::remove_if(result.begin(), result.end(), ::ispunct), result.end());
	return result;
}

bool comp(Statistic &a, Statistic &b)
{
	return a.count > b.count;
}

int main()
{
	Statistic Stat;
	std::ifstream file("file.txt");
	std::map<std::string, int> m;
	std::map<std::string, int>::iterator p;
	if (file.is_open())
	{
		std::string word;
		while (!file.eof())
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
		i++;
	}

	std::sort(s.begin(), s.end(), comp);
	for (i = 0; i < WORDS_COUNT; i++)
		std::cout << s.at(i).word << std::endl;
	file.close();
	}
	return 0;
}
