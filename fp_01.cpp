#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include<iomanip>

using namespace std;

#define NC "\e[0m"
#define RED "\e[0;91m"
#define GREEN "\e[0;92m"
#define BLUE "\e[0;94m"


const int initialTotalChips = 10;
const int cardInHand = 7;
const int totalCardInGame = 44;

class Card
{
protected:
    bool isSymbol;
    bool visibility;
public:
    Card(bool isSymbol) : isSymbol(isSymbol), visibility(true) {};
    virtual ~Card(){};
    bool isSymbolCard() {return this->isSymbol;}
    bool getVisibilty() {return this->visibility;};
    virtual string getValue() = 0;
    virtual void printCard() = 0;
    virtual void setVisibility(bool isPublid) = 0;
};

class SymbolCard : public Card
{
private:
    string symbol;

public:
    SymbolCard(string symbol) : Card(true), symbol(symbol){};
    string getValue();
    void printCard();
    void setVisibility(bool isPublid) {this->visibility = true;};
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
    void setVisibility(bool isPublic) {this->visibility = isPublic;};
    bool getVisibility() {return this->visibility;};
};

string NumberCard::getValue()
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

class Character
{
protected:
    vector<Card*> cardArr;
    int totalChips;
    int chipBiddenThisRound;
    bool bigOrSmall;      // true for big, otherwise
    bool isFoldThisRound; // 棄牌 放棄本回合
    string name;
public:
    Character(const string &name); // constructor
    virtual ~Character();          // destructor
    void addCard(Card *c);
    void swapCard(Card *c1, Card *c2);
    double calCard(); // 計算牌的大小
    void printAllCard();
    void printPublicCard();
    virtual void sortCard(){};
    virtual int biddingChips(const int currChip, const int limitChip) = 0; // 前一人下注的籌碼
    void printName();
    friend class Game;
};

Character::Character(const string &name)
{
    this->name = name;
    this->totalChips = initialTotalChips;
    this->chipBiddenThisRound = 0;
    this->bigOrSmall = false;
    this->isFoldThisRound = false;
    this->cardArr.reserve(cardInHand);
}

Character::~Character()
{
    for (int i = 0; i < cardInHand; i++)
        delete this->cardArr[i];
}

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

void Character::printAllCard()
{
    this->printName();
    cout << " : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        this->cardArr[i]->printCard();
        cout << " ";
    }
    cout << endl;
}

void Character::printPublicCard()
{
    this->printName();
    cout << " : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        if(this->cardArr[i]->getVisibilty() == true){
            this->cardArr[i]->printCard();
            cout << " ";
        }
    }
    cout << endl;
}

void Character::printName()
{
    cout.setf(ios::right);
    cout << setw(12) << this->name;
    cout.unsetf(ios::right);
}




// 玩家
class Player : public Character
{
private:
public:
    // Player(const string& playername) : Character("playername") {};
    Player(const string &playername) : Character(playername){};
    ~Player(){};
    void sortCard(string order);
    int biddingChips(const int currChip, const int limitChip);
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

void Player::sortCard(string order)
{
    int cnt = order.length();
    Card *newCardArr[cardInHand];
    // cout << order<<endl;
    for (int i = 0; i < cnt; i++)
    {
        // cout << order[i] << " ";
        string str(1, order[i]);

        // cout << endl;
        for (int j = 0; j < cnt; j++)
        {
            if (this->cardArr[j]->getValue() == str)
            {
                newCardArr[i] = this->cardArr[j];
                newCardArr[i] = this->cardArr[j];
                this->cardArr[j] = new SymbolCard("g");
                break;
            }
        }
    }

    for (int i = 0; i < cnt; i++)
    {
        delete this->cardArr[i];
        this->cardArr[i] = newCardArr[i];
    }
}




class Drunkard : public Character
{
private:
public:
    Drunkard() : Character("Drunkard"){};
    ~Drunkard(){};
    void sortCard();
    bool _findNextIdx(bool isSymbol, int &idx);
    int biddingChips(const int currChip, const int limitChip); // 前一人下注的籌碼
};

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

    for (int i = 0; i < cardInHand; i++)
        this->cardArr[i] = newCardArr[i];
}

// int Drunkard::biddingChips(const int currChip, const int limitChip)
int Drunkard::biddingChips(const int currChip, const int limitChip)
{
    int lst = currChip - this->chipBiddenThisRound;

    if (currChip == this->chipBiddenThisRound)
    {
        cout << "stop ";
        return 0;
    }

    int bid = rand() % (limitChip - lst + 1) + lst;
    // if (currChip < 7)
    //     bid = 5;
    // else
    //     bid = 1;

    this->chipBiddenThisRound += bid;
    return bid;
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
};

Rich::Rich() : Character("The rich")
{
    this->totalChips += 10;  // defalut setting: the rich has ten more chips than other
    this->bigOrSmall = true; // 富豪喜歡賭大的
};

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
    return this->totalChips; // 要怎麼知道現在最少的籌碼有幾個
}



class Math : public Character
{
private:
public:
    Math(const std::string &n) : Character(n){};
    ~Math(){};
    // 數學家的下注邏輯
    bool decideToBet()
    {
        double result = calculateOptimalValue();
        if (result == 19 or result == 20 or result == 21 or result == 22)
            return true;
        return false;
    }

    // 下注籌碼數量
    int betChips()
    {
        double result = calculateOptimalValue();
        if (result == 19)
            return totalChips * 0.76;
        if (result == 20)
            return totalChips * 0.78;
        if (result == 21)
            return totalChips * 0.77;
        if (result == 22)
            return totalChips * 0.75;
    }

    // 數學家的出牌演算法
    double calculateOptimalValue()
    {
        vector<Card *> numberCards; // 數字卡牌
        vector<Card *> symbolCards; // 符號卡牌

        // 將數字卡片和符號卡片分別放入對應的向量中
        for (int i = 0; i < 7; ++i)
        {
            if (cardArr[i] != nullptr)
            {
                if (NumberCard *numCard = dynamic_cast<NumberCard *>(cardArr[i]))
                {
                    numberCards.push_back(numCard);
                }
                else if (SymbolCard *symCard = dynamic_cast<SymbolCard *>(cardArr[i]))
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

            } while (next_permutation(numberCards.begin(), numberCards.end())); // 產生所有數字卡牌的排列組合

        } while (next_permutation(symbolCards.begin(), symbolCards.end())); // 產生所有符號卡牌的排列組合

        return result;
    }
};

class Game
{
private:
    vector<Character *> playerList;
    vector<Card *> cardList;
    int roundN;
    int chipsInRound;
    int currChip;
    int leastChips;
    void _shuffle();
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
    void printPublicCard();
    void biddingPerRound(int rnd);
    void update();
    void printResult();
    void printPlayerList();

    void gameStart();
    void calChips();
};

Game::Game()
{
    this->cardList.reserve(totalCardInGame);
    roundN = 0;
    chipsInRound = 0;
    currChip = 0;
    leastChips = 1000; // 下注最高限制
}

void Game::addPlayer(Player &pyptr)
{
    this->playerList.push_back(&pyptr);
    Drunkard *d = new Drunkard();
    this->playerList.push_back(d);
    // cout << this->playerList[0]->name;
    // cout << this->playerList.size();
}

void Game::initCardList()
{
    for(int i = 0; i < 11; i++){
        NumberCard* _nc = new NumberCard(i, "R");
        this->cardList.push_back(_nc);
    }

    for(int i = 0; i < 11; i++){
        NumberCard* _nc = new NumberCard(i, "G");
        this->cardList.push_back(_nc);
    }

    for(int i = 0; i < 11; i++){
        NumberCard* _nc = new NumberCard(i, "B");
        this->cardList.push_back(_nc);
    }

    for(int i = 0; i < 11; i++){
        NumberCard* _nc = new NumberCard(i, "W");
        this->cardList.push_back(_nc);
    }
    this->_shuffle();
}

void Game::_shuffle()
{
    const int shuffleTime = 9507014;
    for(int i = 0; i < shuffleTime; i++){
        int idx1 = rand() % totalCardInGame;
        int idx2 = rand() % totalCardInGame;
        Card* temp = this->cardList[idx1];
        this->cardList[idx1] = this->cardList[idx2];
        this->cardList[idx2] = temp;
    }
}

void Game::dealCard(int rnd)
{
    static int cardListIdx = 0;
    if(rnd == 0){
        for(int i = 0; i < this->playerList.size(); i++){
            SymbolCard* add = new SymbolCard("+");
            SymbolCard* sub = new SymbolCard("-");
            SymbolCard* mul = new SymbolCard("*");
            playerList[i]->addCard(add);
            playerList[i]->addCard(sub);
            playerList[i]->addCard(mul);
        }
    }
    else if(rnd == 1){
        for(int i = 0; i < this->playerList.size(); i++){
            this->cardList[cardListIdx]->setVisibility(false);
            playerList[i]->addCard(this->cardList[cardListIdx]);
            cardListIdx++;
        }

        for(int i = 0; i < this->playerList.size(); i++){
            playerList[i]->addCard(this->cardList[cardListIdx]);
            cardListIdx++;
        }

        for(int i = 0; i < this->playerList.size(); i++){
            playerList[i]->addCard(this->cardList[cardListIdx]);
            cardListIdx++;
        }
    }
    else if(rnd == 2){
        for(int i = 0; i < this->playerList.size(); i++){
            playerList[i]->addCard(this->cardList[cardListIdx]);
            cardListIdx++;
        }
    }
}

void Game::printPublicCard()
{
    for(int i = 0 ; i < this->playerList.size(); i++){
        this->playerList[i]->printPublicCard();
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
    cout << "==Player List==" << endl;
    for (int i = 0; i < playerList.size(); i++)
    {
        this->playerList[i]->printName();
        //cout << this->playerList[i]->name;
        cout << endl;
    }
    cout << "===============" << endl;
}

void Game::gameStart()
{
    cout << "[遊戲名稱] 開始！" << endl;
    Drunkard* d = new Drunkard();
    Rich* r = new Rich();
    //Math* m = new Math(); //沒有複寫biddingChips函數
    //隨機分配三個角色的順序，最後再加上player
    int ran = rand()%3;
    if(ran == 0)
    {
        this->playerList.push_back(d);
        this->playerList.push_back(r);
        //this->playerList.push_back(m);
        cout << "你的對手為：酒鬼、富豪、數學家" << endl;
    }
    else if(ran == 1)
    {
        this->playerList.push_back(r);
        //this->playerList.push_back(m);
        this->playerList.push_back(d);
        cout << "你的對手為：富豪、數學家、酒鬼" << endl;
    }
    else
    {
        //this->playerList.push_back(m);
        this->playerList.push_back(d);
        this->playerList.push_back(r);
        cout << "你的對手為：數學家、酒鬼、富豪" << endl;
    }
    //結束後跳回main新增player
}

void Game::calChips()
{
    cout << "[name] 目前籌碼數量：";
    if(playerList[3]->totalChips == 0)
        cout << "您的籌碼數量歸零 GAME OVER....." << endl;
    else
    {
        for(int i = 0; i < 4; i++)
        {
            playerList[i]->printName();
            cout << ":" << playerList[i]->totalChips;
        }
        for(int i = 0; i < 4; i++)
        {
            if(playerList[i]->totalChips <= 0)
            {
                cout << "玩家 ";
                playerList[i]->printName();
                cout << " 籌碼數量歸零，退出遊戲。";
            }
        }
    }
}

int main()
{
    // main for test
    cout << "請輸入玩家名稱: ";
    string playerName = "player1";
    // cin >> playerName;
    Player py(playerName);
    py.printName();
    cout << endl;
    Game G;
    G.addPlayer(py);
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
    G.printPublicCard();

    // cout << "請輸入最終數學式: \n";
    // string cardOrder;
    // cin >> cardOrder;
    // py.sortCard(cardOrder);

    // py.printAllCard();
    // cout << "數學式大小: " << py.calCard() << endl;
    return 0;
}
