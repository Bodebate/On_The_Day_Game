#pragma once
#include <string>
#include <random>
#include <cctype>
#include <vector>



class GameLogicClass
{
private:
	std::string FinalMorseCode;
	std::string FinalCypherCode;
	const std::string CodeOptions[5] ={"Belgium_Campus_Is_Awsome","Information_Technology_Is_The_Future","Belgium_Campus_ITversity","The_Future_Is_Code","It's The Way We're Wired"};
	const std::string MoseCode[26] = { ".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--.." };

	int Random(int Lower, int Upper) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distrib(Lower, Upper);
		return distrib(gen);
	}

	void SelectCode() {
		int RandomIndex = Random(0, 4);
		FinalMorseCode = CodeOptions[RandomIndex];
		FinalCypherCode = CodeOptions[RandomIndex+1];

		return;
	}


public:


	GameLogicClass() {
		//constructor Code
		SelectCode();
	}

	bool CheckMorseCode(std::string TestString) {
		if (TestString == FinalMorseCode) {
			return true;
		}else
		{
			return false;
		}
	}

	std::string GenerateMorseCode(std::string InString) {
		std::vector<char> chars(InString.begin(), InString.end());
		std::string outString = "";
		int i = 0;
		for (char C : chars)
		{
			i++;
			C = std::tolower(C);
			char lower_char = std::tolower(static_cast<unsigned char>(C));
			if (lower_char >= 'a' && lower_char <= 'z') {
				outString += MoseCode[lower_char - 'a'] + "/";
			}
			else {
				outString += "_";
			}
		}
		return outString;
	}

	std::string GetMorseCode() { return GenerateMorseCode(FinalMorseCode); }

};

