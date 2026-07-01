#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <list>

typedef std::string MyString;
typedef std::list<MyString> Anagrams;
typedef std::list<MyString>::iterator AnagramsIter;

Anagrams anagrams;

std::vector<MyString> words;

void fillVector()
{
	words.push_back(std::string("amor"));
	words.push_back(std::string("21"));
	words.push_back(std::string("ramo"));
	words.push_back(std::string("casa"));
	words.push_back(std::string("roma"));
	words.push_back(std::string("copi"));
	words.push_back(std::string("pico"));
	words.push_back(std::string("12"));
	words.push_back(std::string("ipoc"));
	words.push_back(std::string("ipoc"));
	words.push_back(std::string("ipoc"));
	words.push_back(std::string("ipoc"));
	words.push_back(std::string("o8ci"));
	words.push_back(std::string("ipoc"));
	words.push_back(std::string("moar"));
	words.push_back(std::string("ipoc"));
}

/**
 * @brief Calcula si 2 palabras son anagramas
 *
 * @param pWordA Palabra 1.
 * @param pWordB Palabra 2.
 * @return true En caso que sean anagramas.
 * @return false En caso de ser numeros, palabras de distinto largo o no anagramas.
 */
bool isAnagram(const char *pWordA, const char *pWordB)
{
	if (strlen(pWordA) != strlen(pWordB))
		return false;
	if (strcmp(pWordA, pWordB) == 0)
		return false;

	int length = strlen(pWordA);
	bool anagramFinder = true, findCharacter = false;
	for (int i = 0; i < length; i++)
	{
		findCharacter = false;
		for (int j = 0; j < length; j++)
		{
			if ((pWordA[i] > 122 || pWordA[i] < 64) || (pWordB[j] > 122 || pWordB[j] < 64))
				return false;
			if (pWordA[i] == pWordB[j])
			{
				findCharacter = true;
				break;
			}
		}
		if (!findCharacter)
		{
			anagramFinder = false;
			break;
		}
	}
	return anagramFinder;
}

/**
 * @brief
 * Mantiene una lista con las palabras que son anagramas y analiza si la
 * palabra pasada por paramentro existe o no
 * @param word Palabra que se analiza
 * @return true se agrego a la lista
 * @return false ya existe y no se agrega a la lista
 */
bool analizeAnagramPair(const MyString word)
{
	bool find = false;
	for (AnagramsIter it = anagrams.begin(); it != anagrams.end(); ++it)
	{
		if (word.compare(*it) == 0)
		{
			find = true;
			break;
		}
	}
	if (!find)
		anagrams.push_back(word);
	return !find;
}

/**
 * @brief
 * Pequeño program que analiza cuantos anagramas tiene una lista de palabras
 * Tipico programa de hackerrank para detectar perfiles
 * La forma de compilarlo es: g++ testAnagram.cpp -o anagrama
 * para probarlo: ./anagrama
 * @return int
 */
int main()
{
	fillVector();
	std::cout << "Analizo " << words.size() << " palabras" << std::endl;
	for (int i = 0; i < words.size() / 2; i++){
		for (int j = words.size() - 1; j > words.size() / 2; j--){
			if (isAnagram(words[i].c_str(), words[j].c_str())){
				analizeAnagramPair(words[i]);
				analizeAnagramPair(words[j]);
			}
		}
	}
	int size = (words.size() * 128);
	char *pData = new char[size];
	memset(pData, 0, size);
	for (AnagramsIter it = anagrams.begin(); it != anagrams.end(); ++it)
	{
		strcat(pData, " ");
		strcat(pData, (*it).c_str());
	}

	std::cout << "Existen " << anagrams.size() << " Anagramas:" << pData << std::endl;

	delete[] pData;

	return 0;
}
