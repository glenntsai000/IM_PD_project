#include<iostream>
#include<string>
#include<cstdlib>
using namespace std;

#define NC "\e[0m"
#define RED "\e[0;91m"
#define GREEN "\e[0;92m"
#define BLUE "\e[0;94m"


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
    if(this->color.compare("R") == 0)
        cout << RED << "|" << this->num << "|" << NC;
    else if(this->color.compare("G") == 0)
        cout << GREEN << "|" << this->num << "|" << NC;
    else if(this->color.compare("B") == 0)
        cout << BLUE << "|" << this->num << "|" << NC;
    else
        cout << "|" << this->num << "|";
}

class Character
{
protected:
    Card* cardArr[7];
    int totalChips;
    int chipBiddenThisRound;
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
    this->totalChips = 0;
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
    return rand()%(this->totalChips - currChip + 1) + currChip;
}

class Rich : public Character
{
private:
    void swapPtr(Card*& p1, Card*& p2);
public:
    Rich(const string& name); // constructor
    ~Rich(); //destructor
    void sortCard();
    bool _findNextIdx(bool isSymbol, int& idx);
    int biddingChips(const int currChip);
};

Rich::Rich(const string& name) : Character(name)
{
    this->totalChips += 10;//defalut setting: the rich has ten more chips than other
    this->bigOrSmall = true;//富豪喜歡賭大的
}

void Rich::swapPtr(Card*& p1, Card*& p2) 
{
    Card* temp = p1;
    p1 = p2;
    p2 = temp;
}

bool Rich::_findNextIdx(bool isSymbol, int& idx)
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

void Rich::sortCard()
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
    //富豪會把數字由大到小排列
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3 - i - 1; j++)
        {
            if(stoi(this->cardArr[2 * j]->getValue()) - stoi(this->cardArr[2 * j + 2]->getValue()) > 0)
                swapPtr(this->cardArr[2 * j], this->cardArr[2 * j + 2]);
        }
    }
}

int Rich::biddingChips(const int currChip)
{
    //all in
    return this->totalChips;//要怎麼知道現在最少的籌碼有幾個
}



class Math : public Character
{
private:
public:
    Math(const std::string& n) : Character(n) {};
    ~Math() {};
    // 數學家的下注邏輯
    bool decideToBet() override
    {
        double result = calculateOptimalValue();
        if (result == 19 or result == 20 or result == 21 or result == 22)
            return true;
        return false;
    }

    // 下注籌碼數量
    int betChips() override
    {
        double result = calculateOptimalValue();
        if (result == 19)
            return chips * 0.76;
        if (result == 20)
            return chips * 0.78;
        if (result == 21)
            return chips * 0.77;
        if (result == 22)
            return chips * 0.75;
    }

    // 數學家的出牌演算法
    double calculateOptimalValue()
    {
        vector<Card*> numberCards; //數字卡牌
        vector<Card*> symbolCards; //符號卡牌

        // 將數字卡片和符號卡片分別放入對應的向量中
        for (int i = 0; i < 7; ++i)
        {
            if (cardArr[i] != nullptr)
            {
                if (NumberCard* numCard = dynamic_cast<NumberCard*>(cardArr[i]))
                {
                    numberCards.push_back(numCard);
                }
                else if (SymbolCard* symCard = dynamic_cast<SymbolCard*>(cardArr[i]))
                {
                    symbolCards.push_back(symCard);
                }
            }
        }

        // 存儲最小差距的絕對值和對應的結果
        double minDifference = numeric_limits<double>::infinity();
        double result = 0;

        // 窮舉所有可能的排列組合
        do
        {
            do
            {
                int currentResult = stoi(numberCards[0]->getValue());
                size_t symbolIndex = 0;

                for (size_t i = 1; i < numberCards.size(); ++i)
                {
                    // 根據符號卡片運算
                    if (symbolIndex < symbolCards.size())
                    {
                        string symbol = symbolCards[symbolIndex]->getValue();
                        if (symbol == "+")
                        {
                            currentResult += stoi(numberCards[i]->getValue());
                        }
                        else if (symbol == "-")
                        {
                            currentResult -= stoi(numberCards[i]->getValue());
                        }
                        else if (symbol == "*")
                        {
                            currentResult *= stoi(numberCards[i]->getValue());
                        }
                        else if (symbol == "/")
                        {
                            int divisor = stoi(numberCards[i]->getValue());
                            if (divisor != 0)
                            {
                                currentResult /= divisor;
                            }
                            // 處理除以零的情況
                        }

                        // 移至下一個符號
                        ++symbolIndex;
                    }
                }

                // 計算差距的絕對值
                double currentDifference1 = abs(currentResult - 1.0);
                double currentDifference20 = abs(currentResult - 20.0);

                // 更新最小差距的絕對值和對應的結果
                if (currentDifference1 < minDifference || currentDifference20 < minDifference)
                {
                    minDifference = min(currentDifference1, currentDifference20);
                    result = currentResult;
                }

            } while (next_permutation(numberCards.begin(), numberCards.end())); //產生所有數字卡牌的排列組合

        } while (next_permutation(symbolCards.begin(), symbolCards.end())); //產生所有符號卡牌的排列組合

        return result;
    }
};



class Game
{
private:
    Character* playerList[4];
    Card* cardList[52];
    int round;
    int chipsInRoundBig;
    int chipsInRoundSmall;
public:
    void dealCard();
    void printPublicCard(Character* player);
    void bidChip();
    void update();
    void printResult();
};

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
    NumberCard* c5 = new NumberCard(2, "G");
    d.addCard(c5);
    NumberCard* c6 = new NumberCard(2, "B");
    d.addCard(c6);
    NumberCard* c7 = new NumberCard(4, "W");
    d.addCard(c7);

    d.printAllCard();
    d.sortCard();
    d.printAllCard();
    cout << d.calCard() << endl;

    return 0;
}