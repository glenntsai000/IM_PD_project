#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include<iomanip>
#include <algorithm>

using namespace std;

#define NC "\e[0m"
#define RED "\e[0;91m"
#define GREEN "\e[0;92m"
#define BLUE "\e[0;94m"


const int initialTotalChips = 10;
const int cardInHand = 7;

class Card
{
protected:
    bool isSymbol;
    bool visibility;
public:
    Card(bool isSymbol) : isSymbol(isSymbol), visibility(true) {};
    virtual ~Card(){};
    bool isSymbolCard() {return this->isSymbol;}
    bool getVisibility() {return this->visibility;};
    virtual string getValue() const = 0;
    virtual string getColor() const = 0;
    virtual void printCard() = 0;
    virtual void setVisibility(bool isPublid) = 0;
    virtual bool operator>(const Card& c) const = 0;
    virtual bool operator<(const Card& c) const = 0;
};

class SymbolCard : public Card
{
private:
    string symbol;

public:
    SymbolCard(string symbol) : Card(true), symbol(symbol){};
    string getValue() const;
    void printCard();
    void setVisibility(bool isPublid) {this->visibility = true;};
    bool operator>(const Card& c) const {return false;};
    bool operator<(const Card& c) const {return false;};
    string getColor() const {return "W";};
};

string SymbolCard::getValue() const
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
    string getValue() const;
    void printCard();
    void setVisibility(bool isPublic) {this->visibility = isPublic;};
    bool getVisibility() {return this->visibility;};
    bool operator>(const Card& c) const;
    bool operator<(const Card& c) const;
    string getColor() const { return this->color;};
};

string NumberCard::getValue() const
{
    return to_string(this->num);
}

void NumberCard::printCard()
{
    if (this->color.compare("R") == 0)
        cout << RED << "|" << this->num << "|" << NC;
    else if (this->color.compare("G") == 0)
        cout << GREEN << "|" << this->num << "|" << NC;
    else if (this->color.compare("B") == 0)
        cout << BLUE << "|" << this->num << "|" << NC;
    else
        cout << "|" << this->num << "|";
}

bool NumberCard::operator>(const Card& c) const
{
    if(this->getValue() > c.getValue()){
        return true;
    }
    else if(this->getValue() == c.getValue()){
        if(this->getColor().compare("R") == 0){
            return true;
        }
        else if(this->getColor().compare("G") == 0 && c.getColor().compare("R") != 0){
            return true;
        }
        else if(this->getColor().compare("B") == 0 && c.getColor().compare("R") != 0 && c.getColor().compare("G") != 0){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

bool NumberCard::operator<(const Card& c) const
{
    return !(*this > c);
}

class Character
{
protected:
    vector<Card*> cardArr;
    int totalChips;
    int chipBiddenThisRound;
    bool bigOrSmall;      // true for big, otherwise
    bool isFoldThisRound; // 棄牌 放棄本回合
    bool isAlive; //籌碼為0，則踢出遊戲，isAlive = false
    string name;
public:
    Character(const string &name); // constructor
    virtual ~Character();          // destructor
    void addCard(Card *c);
    void swapCard(Card *c1, Card *c2);
    void printName();
    int getTotalChips() {return this->totalChips;};
    double calCard(); // 計算牌的大小
    virtual void printHandCard();
    virtual void sortCard(){};
    virtual void sortCard(string order) {}; // Implement?
    virtual int biddingChips(const int currChip, const int limitChip) = 0; // 前一人下注的籌碼
    virtual void setBigOrSmall(bool isBig) {this->bigOrSmall = isBig;};
    virtual void printWinner() = 0;
    virtual void throwCard(Card* c); //只能丟+ or -
    friend class Game;
};

Character::Character(const string &name)
{
    this->name = name;
    this->totalChips = initialTotalChips;
    this->chipBiddenThisRound = 0;
    this->bigOrSmall = false;
    this->isFoldThisRound = false;
    this->isAlive = true;
}

Character::~Character()
{}

void Character::addCard(Card *c)
{
    this->cardArr.push_back(c);
}

void Character::swapCard(Card *c1, Card *c2)
{
    Card *temp = c1;
    c1 = c2;
    c2 = temp;
}

double Character::calCard()
{
    double value = stod(this->cardArr[0]->getValue());
    int idx = 1;

    while (idx < cardInHand)
    {
        if (this->cardArr[idx]->getValue().compare("+") == 0)
        {
            idx++;
            value += stod(this->cardArr[idx]->getValue());
            idx++;
        }
        else if (this->cardArr[idx]->getValue().compare("-") == 0)
        {
            idx++;
            value -= stod(this->cardArr[idx]->getValue());
            idx++;
        }
        else if (this->cardArr[idx]->getValue().compare("*") == 0)
        {
            // idx = 符號
            if (idx >= 3)
            {
                bool frontIsPlus = true;
                if (this->cardArr[idx - 2]->getValue().compare("+") == 0)
                    value -= stod(this->cardArr[idx - 1]->getValue());
                else if (this->cardArr[idx - 2]->getValue().compare("-") == 0)
                {
                    value += stod(this->cardArr[idx - 1]->getValue());
                    frontIsPlus = false;
                }

                idx++;
                // idx = 數字
                double first = 0;
                first = stod(this->cardArr[idx - 2]->getValue()) * stod(this->cardArr[idx]->getValue());
                if (frontIsPlus)
                    value += first;
                else
                    value -= first;
            }
            else
            {
                idx++;
                // idx = 數字
                value *= stod(this->cardArr[idx]->getValue());
            }
            idx++;
        }
        else if (this->cardArr[idx]->getValue().compare("/") == 0)
        {
            // idx = 符號
            if (idx >= 3)
            {
                bool frontIsPlus = true;
                if (this->cardArr[idx - 2]->getValue().compare("+") == 0)
                    value -= stod(this->cardArr[idx - 1]->getValue());
                else if (this->cardArr[idx - 2]->getValue().compare("-") == 0)
                {
                    value += stod(this->cardArr[idx - 1]->getValue());
                    frontIsPlus = false;
                }
                idx++;
                // idx = 數字
                double first = 0;
                first = stod(this->cardArr[idx - 2]->getValue()) / stod(this->cardArr[idx]->getValue());
                if (frontIsPlus)
                    value += first;
                else
                    value -= first;
            }
            else
            {
                idx++;
                // idx = 數字
                value /= stod(this->cardArr[idx]->getValue());
            }

            idx++;
        }
        // cout << value << endl;
    }

    // cout << "d";
    return value;
}

void Character::printHandCard()
{
    cout << setw(10) << this->name << " : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        if(this->cardArr[i]->getVisibility() == true){
            // Public card
            this->cardArr[i]->printCard();
            cout << " ";
        }
        else{
            // Hidden card : |?|
            cout << "|?| ";
        }
    }
    cout << endl;
}

void Character::printName()
{
    cout << this->name;
}

void Character::throwCard(Card* c)
{
    this->cardArr[0] = c;
}



// 玩家
class Player : public Character
{
private:
public:
    // Player(const string& playername) : Character("playername") {};
    Player(const string &playername) : Character(playername){};
    ~Player(){};
    void sortCard();
    int biddingChips(const int currChip, const int limitChip);
    void printHandCard(); // 使玩家可以印出隱藏牌
    void printWinner();
    friend class Game;
};

int Player::biddingChips(const int currChip, const int limitChip)
{
    // throw error
    int lst = currChip - chipBiddenThisRound;
    cout << "請進行下注: (請輸入" << currChip - chipBiddenThisRound << "~" << limitChip << "的值，若要放棄下注請輸入-1)" << endl;
    int playerbid;
    cin >> playerbid;

    if (playerbid + this->chipBiddenThisRound < currChip && playerbid > 0)
        playerbid = 0;
    else if (playerbid > limitChip)
        playerbid = limitChip;

    this->chipBiddenThisRound += playerbid;

    if (playerbid == -1)
    {
        this->isFoldThisRound = true;
    }

    return playerbid;
}

void Player::sortCard()
{
    
    cout << "請輸入最終數學式: \n";
    string cardOrder;
    cin >> cardOrder;

    int cnt = cardOrder.length();
    Card *newCardArr[7];
    // cout << order<<endl;
    for (int i = 0; i < cnt; i++)
    {
        // cout << order[i] << " ";
        string str(1, cardOrder[i]);

        // cout << endl;
        for (int j = 0; j < cnt; j++)
        {
            if (this->cardArr[j]->getValue() == str)
            {
                newCardArr[i] = this->cardArr[j];
                this->cardArr[j] = new SymbolCard("g");
                break;
            }
        }
    }

    for (int i = 0; i < cnt; i++)
    {
        //delete this->cardArr[i];
        this->cardArr[i] = newCardArr[i];
    }
}

void Player::printHandCard()
{
    cout << "-------------" << setw(10) << this->name << setw(10) << setiosflags(ios::left) << "'s hand" << resetiosflags(ios::left) << "-------------" << endl;
    cout << setw(13) << "Hand : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        this->cardArr[i]->printCard();
        cout << " ";
    }
    cout << endl;
    cout << setw(13) << "Public : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        if(this->cardArr[i]->getVisibility() == true){
            this->cardArr[i]->printCard();
            cout << " ";
        }
    }
    cout << endl;
    cout << setw(13) << "Hidden : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        if(this->cardArr[i]->getVisibility() == false){
            this->cardArr[i]->printCard();
            cout << " ";
        }
    }
    cout << endl;
    cout << setw(46) << setfill('-')  << "" << setfill(' ') << endl;
}

void Player::printWinner()
{
    cout << "恭喜你成為本次遊戲的贏家！" << endl;
}

class Drunkard : public Character
{
private:
    bool _findNextIdx(bool isSymbol, int &idx);
public:
    Drunkard();
    ~Drunkard() {};
    void sortCard();
    int biddingChips(const int currChip, const int limitChip); // 前一人下注的籌碼
    void throwCard(Card* c);
    void printWinner();
};

Drunkard::Drunkard() : Character("Drunkard")
{
    this->bigOrSmall = rand() % 2;
}

bool Drunkard::_findNextIdx(bool isSymbol, int &idx)
{
    for (int i = idx + 1; i < cardInHand; i++)
    {
        if (isSymbol)
        {
            if (this->cardArr[i]->isSymbolCard() == true)
            {
                idx = i;
                return true;
            }
        }
        else
        {
            if (this->cardArr[i]->isSymbolCard() == false)
            {
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
    Card *newCardArr[cardInHand];
    int currIdx = 0;
    // 找目前cardArr中的第一張符號牌
    for (int i = 0; i < cardInHand; i++)
    {
        if (this->cardArr[i]->isSymbolCard() == true)
        {
            symbolIdx = i;
            break;
        }
    }

    // 找目前cardArr中的第一張數字牌
    for (int i = 0; i < cardInHand; i++)
    {
        if (this->cardArr[i]->isSymbolCard() == false)
        {
            numIdx = i;
            break;
        }
    }
    // 如果符號排在數字前面就互換位置，放進新陣列
    newCardArr[currIdx] = this->cardArr[numIdx];
    currIdx++;
    newCardArr[currIdx] = this->cardArr[symbolIdx];
    currIdx++;

    bool symbolFlag = true, numFlag = true;
    while (symbolFlag || numFlag)
    {
        if (numFlag)
            numFlag = _findNextIdx(false, numIdx);
        if (symbolFlag)
            symbolFlag = _findNextIdx(true, symbolIdx);
        if (numFlag == true && symbolFlag == true)
        {
            newCardArr[currIdx] = this->cardArr[numIdx];
            currIdx++;
            newCardArr[currIdx] = this->cardArr[symbolIdx];
            currIdx++;
        }
        else if (numFlag == true && symbolFlag == false)
        {
            newCardArr[currIdx] = this->cardArr[numIdx];
            currIdx++;
        }
    }

    for(int i = 0; i < 10; i++){
        int idx1 = 2 * (rand() % 4);
        int idx2 = 2 * (rand() % 4);
        swapCard(newCardArr[idx1], newCardArr[idx2]);
    }

    for(int i = 1; i < cardInHand; i+=2){
        if(newCardArr[i]->getValue().compare("/") == 0){
            if(newCardArr[i + 1]->getValue().compare("0") == 0)
                swapCard(newCardArr[i - 1], newCardArr[i + 1]);
        }
    }
    for (int i = 0; i < cardInHand; i++)
        this->cardArr[i] = newCardArr[i];
}

// int Drunkard::biddingChips(const int currChip, const int limitChip)
int Drunkard::biddingChips(const int currChip, const int limitChip)
{
    int lst = currChip - this->chipBiddenThisRound;
    if (currChip == this->chipBiddenThisRound || lst <= 0)
    {
        return 0;
    }

    int bid = rand() % (limitChip - lst + 1) + lst;

    this->chipBiddenThisRound += bid;
    return bid;
}

void Drunkard::throwCard(Card* c)
{
    this->cardArr[rand() % 2] = c;
}

void Drunkard::printWinner()
{
    cout << "你輸了..." << '\n' << "最終贏家為： " << this->name<< endl;
}


class Rich : public Character
{
private:
    void swapPtr(Card *&p1, Card *&p2);

public:
    Rich();    // constructor
    ~Rich(){}; // destructor
    void sortCard();
    bool _findNextIdx(bool isSymbol, int &idx);
    int biddingChips(const int currChip, const int limitChip);
    void printWinner();
    void throwCard(Card* c);
};

Rich::Rich() : Character("The rich")
{
    this->totalChips += 10;  // defalut setting: the rich has ten more chips than other
    this->bigOrSmall = true; // 富豪喜歡賭大的
};

void Rich::throwCard(Card* c)
{
    this->cardArr[1] = c;//把減號丟掉
}

void Rich::swapPtr(Card *&p1, Card *&p2)
{
    Card *temp = p1;
    p1 = p2;
    p2 = temp;
}

bool Rich::_findNextIdx(bool isSymbol, int &idx)
{
    for (int i = idx + 1; i < 7; i++)
    {
        if (isSymbol)
        {
            if (this->cardArr[i]->isSymbolCard() == true)
            {
                idx = i;
                return true;
            }
        }
        else
        {
            if (this->cardArr[i]->isSymbolCard() == false)
            {
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
    Card *newCardArr[7];
    int currIdx = 0;
    // 找目前cardArr中的第一張符號牌
    for (int i = 0; i < 7; i++)
    {
        if (this->cardArr[i]->isSymbolCard() == true)
        {
            symbolIdx = i;
            break;
        }
    }

    // 找目前cardArr中的第一張數字牌
    for (int i = 0; i < 7; i++)
    {
        if (this->cardArr[i]->isSymbolCard() == false)
        {
            numIdx = i;
            break;
        }
    }
    // 如果符號排在數字前面就互換位置，放進新陣列
    newCardArr[currIdx] = this->cardArr[numIdx];
    currIdx++;
    newCardArr[currIdx] = this->cardArr[symbolIdx];
    currIdx++;

    bool symbolFlag = true, numFlag = true;
    while (symbolFlag || numFlag)
    {
        if (numFlag)
            numFlag = _findNextIdx(false, numIdx);
        if (symbolFlag)
            symbolFlag = _findNextIdx(true, symbolIdx);
        if (numFlag == true && symbolFlag == true)
        {
            newCardArr[currIdx] = this->cardArr[numIdx];
            currIdx++;
            newCardArr[currIdx] = this->cardArr[symbolIdx];
            currIdx++;
        }
        else if (numFlag == true && symbolFlag == false)
        {
            newCardArr[currIdx] = this->cardArr[numIdx];
            currIdx++;
        }
    }

    for (int i = 0; i < 7; i++)
        this->cardArr[i] = newCardArr[i];
    // 富豪會把數字由大到小排列
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3 - i - 1; j++)
        {
            if (stoi(this->cardArr[2 * j]->getValue()) - stoi(this->cardArr[2 * j + 2]->getValue()) > 0)
                swapPtr(this->cardArr[2 * j], this->cardArr[2 * j + 2]);
        }
    }
}

int Rich::biddingChips(const int currChip, const int limitChip)
{
    // all in
    this->chipBiddenThisRound += limitChip;
    return limitChip; // 要怎麼知道現在最少的籌碼有幾個
}

void Rich::printWinner()
{
    cout << "你輸了..." << '\n' << "最終贏家為： " << this->name<< endl;
}

class Math : public Character
{
private:
public:
    Math(const std::string& n) : Character(n) {};
    ~Math() {};
    void sortCard();
    //bool _findNextIdx(bool isSymbol, int& idx);
    int biddingChips(const int currChip, const int limitChip); // 前一人下注的籌碼(要先呼叫過sortCard才能呼叫biddingChips)
    void printWinner();
};

int Math::biddingChips(const int currChip, const int limitChip)
{
    int bid = 0;
    if (this->totalChips > 0)//還有chips
    {
        if (this->cardArr.size() < 7)
        {
            bid = currChip - chipBiddenThisRound; // 下最少
            this->chipBiddenThisRound += bid; 
        }
        else
        {
            double result = this->calCard(); // 先取optimal solution(要先呼叫過sortCard才能呼叫biddingChips)

            bid = 0;
            double difference = 0;
            double target = 0;

            if (this->bigOrSmall == true)
                target = 20;
            else
                target = 1;

            difference = abs(result - target);// 計算跟target的差距

            if (difference <= 3)
                bid = limitChip;// difference < 3 就 all in
            else if (difference > 3 and difference <= 6)
                bid = ((currChip - chipBiddenThisRound) + limitChip) / 2; // 3 <  difference <= 6 就取中間
            else if (difference > 6 and difference <= 10)
                bid = currChip - chipBiddenThisRound; // 6 <  difference <= 10 就下最少
            else
                this->isFoldThisRound = true;// difference > 10 就棄牌

            // 沒棄牌
            if (this->isFoldThisRound == false)
                this->chipBiddenThisRound += bid;
        }
    }
    else
        this->isFoldThisRound = true;
    return bid;
}

// 數學家的出牌演算法：找出optimal solution
void Math::sortCard()
{
    vector<Card*> numberCards; //數字卡牌
    vector<Card*> symbolCards; //符號卡牌

    // 將數字卡片和符號卡片分別放入對應的向量中
    for (int i = 0; i < 7; i++)
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

    // 儲存最小差距的絕對值和對應的結果
    double minDifference = numeric_limits<double>::infinity();
    double result = 0;
    vector<Card*> updatedCardArr;  //暫存更新後的卡牌排列


    // 窮舉所有可能的排列組合
    //遍歷 4! * 3! = 144種可能性，找出optimal solution
    do
    {
        do
        {
            bool invalid = false;// 判斷是不是除以0
            int firstValue = stoi(numberCards[0]->getValue());
            size_t symbolIndex = 0;

            // 用來儲存中間結果的容器
            vector<double> intermediateResults;
            intermediateResults.push_back(firstValue);

            for (size_t i = 1; i < numberCards.size(); i++)
            {
                // 根據符號卡片運算
                if (symbolIndex < symbolCards.size())
                {
                    string symbol = symbolCards[symbolIndex]->getValue();
                    if (symbol == "*" or symbol == "/")
                    {
                        // 如果是乘法或除法，先將結果存入中間結果容器
                        int operand = stoi(numberCards[i]->getValue());
                        if (symbol == "*")
                        {
                            intermediateResults.back() *= operand;
                        }
                        else if (symbol == "/")
                        {
                            if (operand != 0)
                            {
                                intermediateResults.back() /= operand;
                            }
                            // 處理除以零的情況
                            else
                            {
                                invalid = true;
                                break;
                            }
                        }
                    }
                    else
                    {
                        // 如果是加法或減法，將當前數字與中間結果容器的最後一個元素進行運算
                        int operand = stoi(numberCards[i]->getValue());
                        if (symbol == "+")
                        {
                            intermediateResults.push_back(operand);
                        }
                        else if (symbol == "-")
                        {
                            intermediateResults.push_back(-operand);
                        }
                    }

                    // 移至下一個符號
                    symbolIndex++;
                }
            }

            // 除以零的情況
            if (invalid == true)
                continue;

            // 將中間結果容器中的所有數字相加得到最終結果
            double currentResult = 0;
            for (double tempresult : intermediateResults)
            {
                currentResult += tempresult;
            }

            // 計算差距的絕對值
            double currentDifference1 = abs(currentResult - 1.0);
            double currentDifference20 = abs(currentResult - 20.0);

            // 更新最小差距的絕對值和對應的結果
            if (currentDifference1 < minDifference or currentDifference20 < minDifference)
            {
                minDifference = min(currentDifference1, currentDifference20);
                result = currentResult;

                if (currentDifference20 < currentDifference1)
                    this->setBigOrSmall(true);

                // 將數字卡片和符號卡片放入 updatedCardArr
                symbolIndex = 0;
                updatedCardArr.clear();  // 清空先前的內容
                for (size_t i = 0; i < numberCards.size(); i++)
                {
                    updatedCardArr.push_back(numberCards[i]);
                    if (symbolIndex < symbolCards.size())
                    {
                        updatedCardArr.push_back(symbolCards[symbolIndex]);
                        symbolIndex++;
                    }
                }
            }
        } while (next_permutation(numberCards.begin(), numberCards.end())); //產生所有數字卡牌的排列組合
    } while (next_permutation(symbolCards.begin(), symbolCards.end())); //產生所有符號卡牌的排列組合

    // 複製更新後的卡牌陣列回原始 cardArr
    for (size_t i = 0; i < 7; i++)
    {
        cardArr[i] = updatedCardArr[i];  // 複製新的卡牌
    }

}

void Math::printWinner()
{
    cout << "你輸了..." << '\n' << "最終贏家為： " << this->name<< endl;
}

class Game
{
private:
    vector<Character *> playerList;
    vector<Card *> cardList;
    int roundN;
    int chipsInRound;
    int currChip;
    int leastChips;
    int totalCardInGame;
    void _shuffle(int startIdx);
    void _swapPlayer(int idx1, int idx2);
public:
    Game();
    // ~Game()
    // {
    //     for (int i = 0; i < playerList.size(); i++)
    //         delete playerList[i]; // this is needed to free the memory
    // };
    void addPlayer(Player &pyptr);
    //發牌
    void initCardList();
    void dealCard(int rnd);
    //
    void printPlayersCard();
    void biddingPerRound(int rnd);
    void update();
    void printResult();
    void printPlayerList();
    void enemySort();
    void gameStart(Player &pyptr);
    void calChips();
    void decisionInput();
    void printFinalResult();
    void kickoutPlayer();
    void endRound();
};

const int totalPlayerNum = 4;

Game::Game()
{
    this->roundN = 0;
    this->chipsInRound = 0;
    this->currChip = 0;
    this->totalCardInGame = 0;
    this->leastChips = 1000; // 下注最高限制
}

void Game::addPlayer(Player &pyptr)
{
    this->playerList.push_back(&pyptr);
}

void Game::initCardList()
{
    //清空卡池
    for(int i = 0; i < this->cardList.size(); i++)
        this->cardList.pop_back();
    //重新加入數字牌
    for(int i = 0; i < 10; i++){
        NumberCard* _nc = new NumberCard(i, "R");
        this->cardList.push_back(_nc);
        this->totalCardInGame++;
    }

    for(int i = 0; i < 10; i++){
        NumberCard* _nc = new NumberCard(i, "G");
        this->cardList.push_back(_nc);
        this->totalCardInGame++;
    }

    for(int i = 0; i < 10; i++){
        NumberCard* _nc = new NumberCard(i, "B");
        this->cardList.push_back(_nc);
        this->totalCardInGame++;
    }

    for(int i = 0; i < 10; i++){
        NumberCard* _nc = new NumberCard(i, "W");
        this->cardList.push_back(_nc);
        this->totalCardInGame++;
    }
    this->_shuffle(0);
}

void Game::_shuffle(int startIdx)
{
    const int shuffleTime = 2 * this->cardList.size();
    for(int i = 0; i < shuffleTime; i++){
        int idx1 = rand() % (this->cardList.size() - startIdx) + startIdx;
        int idx2 = rand() % (this->cardList.size() - startIdx) + startIdx;
        Card* temp = this->cardList[idx1];
        this->cardList[idx1] = this->cardList[idx2];
        this->cardList[idx2] = temp;
    }
}

void Game::_swapPlayer(int idx1, int idx2)
{
    Character* tempPtr = this->playerList[idx1];
    this->playerList[idx1] = this->playerList[idx2];
    this->playerList[idx2] = tempPtr;
}

void Game::dealCard(int rnd)
{
    static int cardListIdx = 0;
    if(rnd == 0){
        for(int i = 0; i < this->playerList.size(); i++){
            SymbolCard* add = new SymbolCard("+");
            SymbolCard* sub = new SymbolCard("-");
            SymbolCard* dev = new SymbolCard("/");
            playerList[i]->addCard(add);
            playerList[i]->addCard(sub);
            playerList[i]->addCard(dev);
        }
    }
    else if(rnd == 1){
        for(int i = 0; i < this->playerList.size(); i++){
            this->cardList[cardListIdx]->setVisibility(false);
            playerList[i]->addCard(this->cardList[cardListIdx]);
            cardListIdx++;
        }
        // add |x|
        for(int i = 0; i < this->playerList.size(); i++){
            SymbolCard* mul = new SymbolCard("*");
            this->cardList.push_back(mul);
            this->totalCardInGame++;
        }
        _shuffle(cardListIdx);

        for(int i = 0; i < this->playerList.size(); i++){
            if(this->cardList[cardListIdx]->getValue().compare("*") != 0){
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
            else{
                while(this->cardList[cardListIdx]->getValue().compare("*") == 0){
                    playerList[i]->throwCard(this->cardList[cardListIdx]);
                    cardListIdx++;
                }
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
        }

        for(int i = 0; i < this->playerList.size(); i++){
            if(this->cardList[cardListIdx]->getValue().compare("*") != 0){
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
            else{
                while(this->cardList[cardListIdx]->getValue().compare("*") == 0){
                    playerList[i]->throwCard(this->cardList[cardListIdx]);
                    cardListIdx++;
                }
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
        }

    }
    else if(rnd == 2){
        for(int i = 0; i < this->playerList.size(); i++){
            if(this->cardList[cardListIdx]->getValue().compare("*") != 0){
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
            else{
                while(this->cardList[cardListIdx]->getValue().compare("*") == 0){
                    playerList[i]->throwCard(this->cardList[cardListIdx]);
                    cardListIdx++;
                }
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            };
        }
        cardListIdx = 0;
    }
}

void Game::printPlayersCard()
{
    for(int i = 0 ; i < this->playerList.size(); i++){
        this->playerList[i]->printHandCard();
    }
}

void Game::biddingPerRound(int rnd)
{
    if (rnd == 0) // 基本下注1
    {
        chipsInRound = 0;
        currChip = 0;
        leastChips = 1000; // 這回合擁有最少籌碼的人的籌碼數 為本回合下注的最高限制數量
        for (int i = 0; i < playerList.size(); i++)
        {
            this->playerList[i]->isFoldThisRound = false;
            if (this->playerList[i]->totalChips < leastChips)
                leastChips = this->playerList[i]->totalChips;
        }

        for (int i = 0; i < playerList.size(); i++)
        {
            this->playerList[i]->totalChips--;
            chipsInRound++;
            this->playerList[i]->chipBiddenThisRound++;
        }
        currChip = 1;
    }

    else // 第一次下注與第二次下注
    {
        int plus = 1;
        // bool firstRndStart = true;
        // if(rnd != 1)
        //     firstRndStart = false;
        do
        {
            for (int i = 0; i < playerList.size(); i++)
            {
                if (this->playerList[i]->isFoldThisRound == true)
                    continue; // 已棄牌

                cout << "目前下注最高數量, 最高限制下注數量: " << currChip << ", " << leastChips << endl;

                // 加注
                cout << this->playerList[i]->name << " 本回合已下注數量: " << this->playerList[i]->chipBiddenThisRound << endl;

                int pyBidNum = this->playerList[i]->biddingChips(currChip, (leastChips - this->playerList[i]->chipBiddenThisRound));
                //如果電腦沒籌碼可以下注了怎辦
                if (pyBidNum == -1)
                {
                    cout << this->playerList[i]->name << "放棄這回合" << endl;
                    continue;
                }

                cout << this->playerList[i]->name << " 加注: " << pyBidNum << endl;
                cout << this->playerList[i]->name << " 本回合已下注數量: " << this->playerList[i]->chipBiddenThisRound << endl
                     << endl;

                currChip = this->playerList[i]->chipBiddenThisRound;
                chipsInRound += pyBidNum;
                this->playerList[i]->totalChips -= pyBidNum;
                plus = pyBidNum;
            }

            for (int i = 0; i < playerList.size(); i++)
            {
                if (this->playerList[i]->isFoldThisRound == true)
                    continue; // 已棄牌
                cout << this->playerList[i]->name << " 目前手中籌碼總數: ";
                cout << this->playerList[i]->totalChips << endl;
            }
            cout << endl;
        } while (plus > 0);

        cout << "\n";
        cout << "本回合現在下注總數: " << chipsInRound << endl
             << endl;
    }
};

void Game::printPlayerList()
{
    cout << "===Player List===" << endl;
    for (int i = 0; i < playerList.size(); i++)
    {
        cout << setw(10);
        this->playerList[i]->printName();
        //cout << this->playerList[i]->name;
        cout << " : " << setw(4) << this->playerList[i]->getTotalChips();
        cout << endl;
    }
    cout << "=================" << endl;
}

void Game::enemySort()
{
    for(int i = 1; i < this->playerList.size(); i++){
        this->playerList[i]->sortCard();
    }
}

void Game::gameStart(Player &pyptr)
{
    cout << "[遊戲名稱] 開始！" << endl;
    Drunkard* d = new Drunkard();
    Rich* r = new Rich();
    Math* m = new Math("Math"); //沒有複寫biddingChips函數
    //隨機分配三個角色的順序，最後再加上player
    this->addPlayer(pyptr);
    int ran = rand()%3;
    if(ran == 0)
    {
        this->playerList.push_back(d);
        this->playerList.push_back(r);
        this->playerList.push_back(m);
        cout << "你的對手為：酒鬼、富豪、數學家" << endl;
    }
    else if(ran == 1)
    {
        this->playerList.push_back(r);
        this->playerList.push_back(m);
        this->playerList.push_back(d);
        cout << "你的對手為：富豪、數學家、酒鬼" << endl;
    }
    else
    {
        this->playerList.push_back(m);
        this->playerList.push_back(d);
        this->playerList.push_back(r);
        cout << "你的對手為：數學家、酒鬼、富豪" << endl;
    }
    //結束後跳回main新增player
}

void Game::calChips()
{
    cout << this->playerList[0]->name << "目前籌碼數量：" << playerList[0]->totalChips << endl;
    if(playerList[0]->totalChips == 0){
        for(int i = 1; i < totalPlayerNum; i++)
        {
            cout << setw(10) << left;
            playerList[i]->printName();
            cout << right;
            cout << ": " << playerList[i]->totalChips << endl;
        }
        for(int i = 0; i < this->playerList.size(); i++)
        {
            if(playerList[i]->totalChips <= 0)
            {
                cout << "玩家";
                cout << setw(10);
                playerList[i]->printName();
                cout << " 籌碼數量歸零，退出遊戲。" << endl;
                this->playerList[i]->isAlive = false;
            }
        }
        cout << "您的籌碼數量歸零 GAME OVER....." << endl;
    }
    else
    {
        for(int i = 0; i < this->playerList.size(); i++)
        {
            cout << setw(10) << left;
            playerList[i]->printName();
            cout << right;
            cout << ": " << playerList[i]->totalChips << endl;
        }
        for(int i = 0; i < this->playerList.size(); i++)
        {
            if(playerList[i]->totalChips <= 0)
            {
                cout << "玩家";
                cout << setw(10);
                playerList[i]->printName();
                cout << " 籌碼數量歸零，退出遊戲。" << endl;
                this->playerList[i]->isAlive = false;
            }
        }
    }
}

void Game::printResult()
{
    //計算大家的結果，找出贏的人
    double playerValue[totalPlayerNum];
    for (int i = 0; i < totalPlayerNum; i++)
        playerValue[i] = playerList[i]->calCard();


    //回合結果公布：
    Character* bigWinner = nullptr;
    Character* smallWinner = nullptr;
    int bigWinneridx = 0;
    int smallWinneridx = 0;
    double closestBigDifference = numeric_limits<double>::infinity();
    double closestSmallDifference = numeric_limits<double>::infinity();

    for (int i = 0; i < totalPlayerNum; i++)
    {
        double targetValue = (playerList[i]->bigOrSmall) ? 20.0 : 1.0;
        double difference = abs(playerValue[i] - targetValue);

        if (playerList[i]->bigOrSmall)
        {
            // 賭大的情況
            if (difference < closestBigDifference)
            {
                closestBigDifference = difference;
                bigWinner = playerList[i];
                bigWinneridx = i;
            }
        }
        else
        {
            // 賭小的情況
            if (difference < closestSmallDifference)
            {
                closestSmallDifference = difference;
                smallWinner = playerList[i];
                smallWinneridx = i;
            }
        }
    }

    //回合結果公布：
    if ((bigWinner != nullptr and bigWinner == playerList[0]) or (smallWinner != nullptr and smallWinner == playerList[0]))
        cout << "恭喜你贏了這一回合！" << endl;
    else
        cout << "下一回合再接再厲qq" << endl;

    // 輸出結果
    if (bigWinner != nullptr)
    {
        cout << "賭大的贏家是： ";
        cout << setw(10) << bigWinner->name;
        cout << " 數學式結果為：" << playerValue[bigWinneridx] << endl;
    }
    else
    {
        cout << "沒有賭大的贏家" << endl;
    }

    if (smallWinner != nullptr)
    {
        cout << "賭小的贏家是： ";
        cout << setw(10) << smallWinner->name;
        cout << " 數學式結果為：" << playerValue[smallWinneridx] << endl;
    }
    else
    {
        cout << "沒有賭小的贏家" << endl;
    }

    // 輸出其餘玩家的名稱和數學式結果
    cout << "其餘玩家： " << endl;
    for (int i = 0; i < 4; i++)
    {
        if ((playerList[i] != bigWinner) and (playerList[i] != smallWinner))
        {
            cout << setw(10) << left;
            playerList[i]->printName();
            cout << right; 
            if (playerList[i]->bigOrSmall == true)
                cout << " 賭大 ";
            else
                cout << " 賭小 ";
            cout << " 數學式結果為：" << playerValue[i] << endl;
        }
    }

    //分配籌碼
    //chipsinround//此輪總籌碼
    if (bigWinner != nullptr and smallWinner != nullptr)// bigWinner和smallWinner都有
    {
        playerList[bigWinneridx]->totalChips += (chipsInRound / 2);
        playerList[smallWinneridx]->totalChips += (chipsInRound / 2);
    }
    else if (bigWinner != nullptr and smallWinner == nullptr)// 只有bigWinner
    {
        playerList[bigWinneridx]->totalChips += chipsInRound;
    }
    else if (bigWinner == nullptr and smallWinner != nullptr)// 只有smallWinner
    {
        playerList[smallWinneridx]->totalChips += chipsInRound;
    }
}

void Game::decisionInput()
{
    //第二輪下注結束
    bool bidDirection = true;
    char input;
    cout << "請決定賭大 / 小，並輸入您的最終數學式" << endl;
    cout << "輸入賭注方：(B / S)";
    cin >> input;
    if (input == 'S')
        bidDirection = false;

    playerList[0]->setBigOrSmall(bidDirection);

    playerList[0]->sortCard();
}

void Game::printFinalResult()
{
    //潛在問題：玩家名字和電腦角色名字相同
    Character* winner = nullptr;
    int winnerChips = 0;
    //int winner = 0;
    for(int i = 0; i < this->playerList.size(); i++)
    {
        if(playerList[i]->totalChips > winnerChips)
        {
            winner = playerList[i];
            winnerChips = playerList[i]->totalChips;
            //尚未考慮同樣籌碼數要看顏色
        }
    }
    winner->printWinner();
}

void Game::kickoutPlayer()
{
    int idx = 0;
    while(idx < this->playerList.size()){
        if(this->playerList[idx]->isAlive == false){
            for(int j = idx; j < this->playerList.size() - 1; j++){
                _swapPlayer(j, j + 1);
            }
            this->playerList.pop_back();
        }
        else{
            idx++;
        }
    }
}

void Game::endRound()
{
    for(int i = 0; i < this->playerList.size(); i++){
        for(int j = 0; j < cardInHand; j++){
            this->playerList[i]->cardArr.pop_back();
        }
    }
}

int main()
{
    srand(time(nullptr));
    // main for test
    cout << "請輸入玩家名稱: ";
    string playerName = "player1";
    // cin >> playerName;
    Player py(playerName);
    py.printName();
    cout << endl;
    Game G;
    G.gameStart(py);

    for(int i = 0; i < 3; i++){
        G.printPlayerList();
        G.initCardList();
        G.dealCard(0);
        G.biddingPerRound(0);
        G.dealCard(1);
        cout << "bid 1---------------------------" << endl;
        G.biddingPerRound(1);
        G.dealCard(2);
        cout << "bid 2---------------------------" << endl;
        G.biddingPerRound(2);
        G.printPlayersCard();
        G.enemySort();
        G.decisionInput();
        G.printResult();;
        G.calChips();;
        G.kickoutPlayer(); // 將籌碼歸零的玩家移除playerList;
        G.printPlayerList();;
        G.endRound();
    }

    G.printFinalResult();
    return 0;
}
