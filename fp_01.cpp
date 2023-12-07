#include<iostream>
#include<string>
using namespace std;

class Card
{
protected:
public:
    Card();
    virtual string getValue() = 0;
};

class SymbolCard : public Card
{
private:
    string symbol;
public:
    SymbolCard(string symbol) : symbol(symbol) {};
    string getValue();
};

string SymbolCard::getValue()
{
    return this->symbol;
}

class NumberCard : public Card
{
private:
    int num;
    string color;
public:
    NumberCard(int num, string color) : num(num), color(color) {};
    string getValue();
};

string NumberCard::getValue()
{
    return to_string(this->num);
}



class Character
{
protected:
    Card* cardArr[7];
    int chips;
    bool bigOrSmall; // true for big, otherwise
    string name;
public:
    Character(const string& name); // constructor
	~Character(); // destructor
    void addCard(Card* c);
	int calCard(); // 計算牌的大小
	virtual int biddingChips();
};

Character::Character(const string& name)
{
    this->name = name;
    this->chips = 0;
    this->bigOrSmall = false;
    for(int i = 0; i < 7; i++)
        this->cardArr[i] = nullptr;
}

Character::~Character()
{
    for(int i = 0; i < 7; i++)
        delete this->cardArr[i];
}

void Character::addCard(Card* c)
{
    for(int i = 0; i < 7; i++){
        if(this->cardArr[i] == nullptr){
            this->cardArr[i] = c;
            return;
        }
    }
}

int Character::calCard()
{
    int value = stoi(this->cardArr[0]->getValue());
    Card* tempPtr = this->cardArr[1];

    while(tempPtr != nullptr){
        if(tempPtr->getValue().compare("+") == 0){
            tempPtr++;
            value += stoi(tempPtr->getValue());
            tempPtr++;
        }
        else if(tempPtr->getValue().compare("-") == 0){
            tempPtr++;
            value -= stoi(tempPtr->getValue());
            tempPtr++;
        }
        else if(tempPtr->getValue().compare("*") == 0){
            tempPtr++;
            value *= stoi(tempPtr->getValue());
            tempPtr++;
        }
        else if(tempPtr->getValue().compare("/") == 0){
            tempPtr++;
            value /= stoi(tempPtr->getValue());
            tempPtr++;
        }
    }

    return value;
}

class Rich : public Character
{
private:
public:
    Rich(const string& name); // constructor
    ~Rich(); //destructor
    void thought(Card* cardArr[]);
    int biddingChips();
};

Rich::Rich(const string& name) : Character(name)
{
    this->chips += 10;//defalut setting: the rich has ten more chips than other
}

void Rich::thought(Card* cardArr[])
{
    //the logic of rich man to manage the card
    
}