#include<iostream>
#include<string>
#include<cstdlib>
using namespace std;

class Card
{
protected:
    bool isSymbol;
public:
    Card(bool isSymbol) : isSymbol(isSymbol) {};
    virtual ~Card() {};
    bool isSymbolCard() {return this->isSymbol;}
    virtual string getValue() = 0;
    virtual void printCard() = 0;
};

class SymbolCard : public Card
{
private:
    string symbol;
public:
    SymbolCard(string symbol) : Card(true), symbol(symbol) {};
    string getValue();
    void printCard();
};

string SymbolCard::getValue()
{
    return this->symbol;
}

void SymbolCard::printCard()
{
    cout << "|" << this->symbol << "|";
}

class NumberCard : public Card
{
private:
    int num;
    string color;
public:
    NumberCard(int num, string color) : Card(false), num(num), color(color) {};
    string getValue();
    void printCard();
};

string NumberCard::getValue()
{
    return to_string(this->num);
}

void NumberCard::printCard()
{
    cout << "|" << this->color << this->num << "|";
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
	virtual ~Character(); // destructor
    void addCard(Card* c);
    void swapCard(Card* c1, Card* c2);
	int calCard(); // 計算牌的大小
    void printAllCard();
    virtual void sortCard() {};
	virtual int biddingChips(int currChip) = 0; //前一人下注的籌碼
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

void Character::swapCard(Card* c1, Card* c2)
{
    Card* temp = c1;
    c1 = c2;
    c2 = temp;
}

int Character::calCard()
{
    int value = stoi(this->cardArr[0]->getValue());
    int idx = 1;

    while(idx < 7){
        if(this->cardArr[idx]->getValue().compare("+") == 0){
            idx++;
            value += stoi(this->cardArr[idx]->getValue());
            idx++;
        }
        else if(this->cardArr[idx]->getValue().compare("-") == 0){
            idx++;
            value -= stoi(this->cardArr[idx]->getValue());
            idx++;
        }
        else if(this->cardArr[idx]->getValue().compare("*") == 0){
            idx++;
            value *= stoi(this->cardArr[idx]->getValue());
            idx++;
        }
        else if(this->cardArr[idx]->getValue().compare("/") == 0){
            idx++;
            value /= stoi(this->cardArr[idx]->getValue());
            idx++;
        }
    }

    return value;
}

void Character::printAllCard()
{
    for(int i = 0; i < 7; i++){
        this->cardArr[i]->printCard();
        cout << " ";
    }
    cout << endl;
}

class Drunkard : public Character
{
private:
public:
    Drunkard() : Character("酒鬼") {};
    ~Drunkard() {};
    void sortCard();
    bool _findNextIdx(bool isSymbol, int& idx);
    int biddingChips(const int currChip); //前一人下注的籌碼
};

bool Drunkard::_findNextIdx(bool isSymbol, int& idx)
{
    for(int i = idx + 1; i < 7; i++){
        if(isSymbol){
            if(this->cardArr[i]->isSymbolCard() == true){
                idx = i;
                return true;
            }
        }
        else{
            if(this->cardArr[i]->isSymbolCard() == false){
                idx = i;
                return true;
            }
        }
    }
    return false;
}

void Drunkard::sortCard()
{
    int symbolIdx = 0;
    int numIdx = 0;
    Card* newCardArr[7];
    int currIdx = 0;
    //找目前cardArr中的第一張符號牌
    for(int i = 0; i < 7; i++){
        if(this->cardArr[i]->isSymbolCard() == true){
            symbolIdx = i;
            break;
        }
    }

    //找目前cardArr中的第一張數字牌
    for(int i = 0; i < 7; i++){
        if(this->cardArr[i]->isSymbolCard() == false){
            numIdx = i;
            break;
        }
    }
    //如果符號排在數字前面就互換位置，放進新陣列
    newCardArr[currIdx] = this->cardArr[numIdx];
    currIdx++;
    newCardArr[currIdx] = this->cardArr[symbolIdx];
    currIdx++;
    
    bool symbolFlag = true, numFlag = true;
    while(symbolFlag || numFlag){
        if(numFlag)
            numFlag = _findNextIdx(false, numIdx);
        if(symbolFlag)
            symbolFlag = _findNextIdx(true, symbolIdx);
        if(numFlag == true && symbolFlag == true){
            newCardArr[currIdx] = this->cardArr[numIdx];
            currIdx++;
            newCardArr[currIdx] = this->cardArr[symbolIdx];
            currIdx++;
        }
        else if(numFlag == true && symbolFlag == false){
            newCardArr[currIdx] = this->cardArr[numIdx];
            currIdx++;
        }
    }
    

    for(int i = 0; i < 7; i++)
        this->cardArr[i] = newCardArr[i];
}

int Drunkard::biddingChips(const int currChip)
{
    return rand()%(this->chips - currChip + 1) + currChip;
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
    //the logic of rich man to manage the cardArray
    //隨機指定賭大或賭小
    //if(bigOrSmall)
}

int Rich::biddingChips()
{
    
}


int main()
{
    // main for test
    Drunkard d;
    SymbolCard* c1 = new SymbolCard("+");
    d.addCard(c1);
    SymbolCard* c2 = new SymbolCard("-");
    d.addCard(c2);
    SymbolCard* c3 = new SymbolCard("/");
    d.addCard(c3);
    NumberCard* c4 = new NumberCard(1, "R");
    d.addCard(c4);
    NumberCard* c5 = new NumberCard(2, "R");
    d.addCard(c5);
    NumberCard* c6 = new NumberCard(2, "R");
    d.addCard(c6);
    NumberCard* c7 = new NumberCard(4, "R");
    d.addCard(c7);

    d.printAllCard();
    d.sortCard();
    d.printAllCard();
    cout << d.calCard() << endl;

    return 0;
}