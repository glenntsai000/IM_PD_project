#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <cmath>
#include <chrono>
#include <thread>
using namespace std;

#define NC "\e[0m"
#define RED "\e[0;91m"
#define GREEN "\e[0;92m"
#define BLUE "\e[0;94m"
#define BOLD "\033[1m"

const int initialTotalChips = 10;
const int cardInHand = 7;

class Card
{
protected:
    bool isSymbol;
    bool visibility;

public:
    Card(bool isSymbol) : isSymbol(isSymbol), visibility(true){};
    virtual ~Card(){};
    bool isSymbolCard() { return this->isSymbol; }
    bool getVisibility() { return this->visibility; };
    virtual string getValue() const = 0;
    virtual string getColor() const = 0;
    virtual void printCard() = 0;
    virtual void setVisibility(bool isPublid) = 0;
    virtual bool operator>(const Card &c) const = 0;
    virtual bool operator<(const Card &c) const = 0;
};

class SymbolCard : public Card
{
private:
    string symbol;

public:
    SymbolCard(string symbol) : Card(true), symbol(symbol){};
    string getValue() const;
    void printCard();
    void setVisibility(bool isPublid) { this->visibility = true; };
    bool operator>(const Card &c) const { return false; };
    bool operator<(const Card &c) const { return false; };
    string getColor() const { return "W"; };
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
    NumberCard(int num, string color) : Card(false), num(num), color(color){};
    string getValue() const;
    void printCard();
    void setVisibility(bool isPublic) { this->visibility = isPublic; };
    bool getVisibility() { return this->visibility; };
    bool operator>(const Card &c) const;
    bool operator<(const Card &c) const;
    string getColor() const { return this->color; };
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

bool NumberCard::operator>(const Card &c) const
{
    if (this->getValue() > c.getValue())
    {
        return true;
    }
    else if (this->getValue() == c.getValue())
    {
        if (this->getColor().compare("R") == 0)
        {
            return true;
        }
        else if (this->getColor().compare("G") == 0 && c.getColor().compare("R") != 0)
        {
            return true;
        }
        else if (this->getColor().compare("B") == 0 && c.getColor().compare("R") != 0 && c.getColor().compare("G") != 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool NumberCard::operator<(const Card &c) const
{
    return !(*this > c);
}

class Character
{
protected:
    vector<Card *> cardArr;
    int totalChips;
    int chipBiddenThisRound;
    int chipsRaised;      // 每次的加注金額
    bool bigOrSmall;      // true for big, otherwise
    bool isFoldThisRound; // 棄牌 放棄本回合
    bool isAlive;         // 籌碼為0，則踢出遊戲，isAlive = false
    const bool isPlayer;  // 區分是否為玩家
    string name;
    string type;

public:
    Character(const string &name, const bool isPlayer, const string type); // constructor
    virtual ~Character(){};                                                // destructor
    void addCard(Card *c);
    void swapCard(Card *c1, Card *c2);
    void printName();
    int getTotalChips() { return this->totalChips; };
    double calCard(); // 計算牌的大小
    virtual void printHandCard();
    virtual void sortCard() = 0;
    virtual int biddingChips(const int currChip, const int limitChip) = 0; // 前一人下注的籌碼
    virtual void printWinner() = 0;
    virtual void throwCard(Card *c); // 只能丟+ or -
    friend class Game;
    NumberCard *getMaxCard();
    NumberCard *getMinCard();
};

Character::Character(const string &name, const bool isPlayer, const string type) : isPlayer(isPlayer)
{
    this->name = name;
    this->type = type;
    this->totalChips = initialTotalChips;
    this->chipBiddenThisRound = 0;
    this->bigOrSmall = false;
    this->isFoldThisRound = false;
    this->isAlive = true;
    this->chipsRaised = 0;

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
    double value = 0;

    if (this->cardArr[3]->getValue().compare("*") == 0 || this->cardArr[3]->getValue().compare("/") == 0)
    {
        if ((this->cardArr[1]->getValue().compare("*") == 0 || this->cardArr[1]->getValue().compare("/") == 0) && (this->cardArr[5]->getValue().compare("*") != 0 && this->cardArr[5]->getValue().compare("/") != 0))
        {
            // 乘除連號出現在前面 a*b/c+d
            value += stod(this->cardArr[0]->getValue());
            if (this->cardArr[1]->getValue().compare("*") == 0)
            {
                value *= stod(this->cardArr[2]->getValue());
            }
            else if (this->cardArr[1]->getValue().compare("/") == 0)
            {
                value /= stod(this->cardArr[2]->getValue());
            }

            if (this->cardArr[3]->getValue().compare("*") == 0)
            {
                value *= stod(this->cardArr[4]->getValue());
            }
            else if (this->cardArr[3]->getValue().compare("/") == 0)
            {
                value /= stod(this->cardArr[4]->getValue());
            }

            if (this->cardArr[5]->getValue().compare("+") == 0)
            {
                value += stod(this->cardArr[6]->getValue());
            }
            else if (this->cardArr[5]->getValue().compare("-") == 0)
            {
                value -= stod(this->cardArr[6]->getValue());
            }
        }
        else if ((this->cardArr[1]->getValue().compare("*") != 0 && this->cardArr[1]->getValue().compare("/") != 0) && (this->cardArr[5]->getValue().compare("*") == 0 || this->cardArr[5]->getValue().compare("/") == 0))
        {
            // 乘除連號出現在後面 a+b/c*d
            value += stod(this->cardArr[2]->getValue());
            if (this->cardArr[3]->getValue().compare("*") == 0)
            {
                value *= stod(this->cardArr[4]->getValue());
            }
            else if (this->cardArr[3]->getValue().compare("/") == 0)
            {
                value /= stod(this->cardArr[4]->getValue());
            }

            if (this->cardArr[5]->getValue().compare("*") == 0)
            {
                value *= stod(this->cardArr[6]->getValue());
            }
            else if (this->cardArr[5]->getValue().compare("/") == 0)
            {
                value /= stod(this->cardArr[6]->getValue());
            }

            if (this->cardArr[1]->getValue().compare("+") == 0)
            {
                value += stod(this->cardArr[0]->getValue());
            }
            else if (this->cardArr[1]->getValue().compare("-") == 0)
            {
                value = stod(this->cardArr[0]->getValue()) - value;
            }
        }
        else if ((this->cardArr[1]->getValue().compare("*") == 0 || this->cardArr[1]->getValue().compare("/") == 0) && (this->cardArr[5]->getValue().compare("*") == 0 || this->cardArr[5]->getValue().compare("/") == 0))
        {
            // 所有符號都是乘除 a*b/c*d
            value += stod(this->cardArr[0]->getValue());
            if (this->cardArr[1]->getValue().compare("*") == 0)
            {
                value *= stod(this->cardArr[2]->getValue());
            }
            else if (this->cardArr[1]->getValue().compare("/") == 0)
            {
                value /= stod(this->cardArr[2]->getValue());
            }

            if (this->cardArr[3]->getValue().compare("*") == 0)
            {
                value *= stod(this->cardArr[4]->getValue());
            }
            else if (this->cardArr[3]->getValue().compare("/") == 0)
            {
                value /= stod(this->cardArr[4]->getValue());
            }

            if (this->cardArr[5]->getValue().compare("*") == 0)
            {
                value *= stod(this->cardArr[6]->getValue());
            }
            else if (this->cardArr[5]->getValue().compare("/") == 0)
            {
                value /= stod(this->cardArr[6]->getValue());
            }
        }
        else if ((this->cardArr[1]->getValue().compare("*") != 0 && this->cardArr[1]->getValue().compare("/") != 0) && (this->cardArr[5]->getValue().compare("*") != 0 && this->cardArr[5]->getValue().compare("/") != 0))
        {
            //  a+b*c+d
            value += stod(this->cardArr[0]->getValue());
            double value2 = stod(this->cardArr[2]->getValue());

            if (this->cardArr[3]->getValue().compare("*") == 0)
            {
                value2 *= stod(this->cardArr[4]->getValue());
            }
            else if (this->cardArr[3]->getValue().compare("/") == 0)
            {
                value2 /= stod(this->cardArr[4]->getValue());
            }

            if (this->cardArr[1]->getValue().compare("+") == 0)
            {
                value += value2;
            }
            else if (this->cardArr[1]->getValue().compare("-") == 0)
            {
                value -= value2;
            }

            if (this->cardArr[5]->getValue().compare("+") == 0)
            {
                value += stod(this->cardArr[6]->getValue());
            }
            else if (this->cardArr[5]->getValue().compare("-") == 0)
            {
                value -= stod(this->cardArr[6]->getValue());
            }
        }
    }
    else
    {
        // 若中間的符號不是＊／，不能出現乘除連號
        // 判斷是a*b+c+d 還是a*b+c*d
        bool mul_devAt5 = false;
        if (this->cardArr[5]->getValue().compare("*") == 0 || this->cardArr[5]->getValue().compare("/") == 0)
            mul_devAt5 = true;

        value += stod(this->cardArr[0]->getValue());
        if (mul_devAt5 == false)
        {
            for (int i = 1; i < cardInHand; i += 2)
            {
                if (this->cardArr[i]->getValue().compare("+") == 0)
                {
                    value += stod(this->cardArr[i + 1]->getValue());
                }
                else if (this->cardArr[i]->getValue().compare("-") == 0)
                {
                    value -= stod(this->cardArr[i + 1]->getValue());
                }
                else if (this->cardArr[i]->getValue().compare("*") == 0)
                {
                    value *= stod(this->cardArr[i + 1]->getValue());
                }
                else if (this->cardArr[i]->getValue().compare("/") == 0)
                {
                    value /= stod(this->cardArr[i + 1]->getValue());
                }
            }
        }
        else
        {
            double value2 = stod(this->cardArr[4]->getValue());

            if (this->cardArr[1]->getValue().compare("+") == 0)
            {
                value += stod(this->cardArr[2]->getValue());
            }
            else if (this->cardArr[1]->getValue().compare("-") == 0)
            {
                value -= stod(this->cardArr[2]->getValue());
            }
            else if (this->cardArr[1]->getValue().compare("*") == 0)
            {
                value *= stod(this->cardArr[2]->getValue());
            }
            else if (this->cardArr[1]->getValue().compare("/") == 0)
            {
                value /= stod(this->cardArr[2]->getValue());
            }

            if (this->cardArr[5]->getValue().compare("*") == 0)
            {
                value2 *= stod(this->cardArr[6]->getValue());
            }
            else if (this->cardArr[5]->getValue().compare("/") == 0)
            {
                value2 /= stod(this->cardArr[6]->getValue());
            }

            if (this->cardArr[3]->getValue().compare("+") == 0)
            {
                value += value2;
            }
            else if (this->cardArr[3]->getValue().compare("-") == 0)
            {
                value -= value2;
            }
        }
    }

    return value;
}

void Character::printHandCard()
{
    cout << setw(12) << right << this->name << " : " << left;
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        if (this->cardArr[i]->getVisibility() == true)
        {
            // Public card
            this->cardArr[i]->printCard();
            cout << " ";
        }
        else
        {
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

void Character::throwCard(Card *c)
{
    this->cardArr[0] = c;
}

NumberCard *Character::getMaxCard()
{
    NumberCard *maxNumberCard = nullptr;
    for (Card *card : this->cardArr)
    {
        // 檢查是否為數字卡牌
        NumberCard *numberCard = dynamic_cast<NumberCard *>(card);
        if (numberCard)
        {
            // 如果找到更大的數字卡牌，更新 maxNumberCard
            if (!maxNumberCard or (*numberCard > *maxNumberCard))
            {
                maxNumberCard = numberCard;
            }
        }
    }
    return maxNumberCard;
}

NumberCard *Character::getMinCard()
{
    NumberCard *minNumberCard = nullptr;
    for (Card *card : this->cardArr)
    {
        // 檢查是否為數字卡牌
        NumberCard *numberCard = dynamic_cast<NumberCard *>(card);
        if (numberCard)
        {
            // 如果找到更小的數字卡牌，更新 minNumberCard
            if (!minNumberCard or (*numberCard < *minNumberCard))
            {
                minNumberCard = numberCard;
            }
        }
    }
    return minNumberCard;
}

// 玩家
class Player : public Character
{
private:
public:
    Player(const string &playername) : Character(playername, true, "Player"){};
    ~Player(){};
    void sortCard();
    int biddingChips(const int currChip, const int limitChip);
    void printHandCard(); // 使玩家可以印出隱藏牌
    void printWinner();
    void throwCard(Card *c);
    friend class Game;
};

int Player::biddingChips(const int currChip, const int limitChip)
{
    // this->printHandCard();
    // cout << this->name << "已下注數量: " << this->chipBiddenThisRound << endl;

    int lst = currChip - chipBiddenThisRound;
    if (lst == limitChip)
        cout << this->name << "請進行加注(請輸入 " << lst
             << " ，若欲放棄這回合不繼續下注請輸入 x ): ";
    else
        cout << this->name << "請進行加注(請輸入 " << lst << "~" << limitChip
             << " 的值，若欲放棄這回合不繼續下注請輸入 x ): ";
    string inputBid;
    int playerBid = 0;
    do
    {
        try
        {
            cin >> inputBid;
            if (inputBid == "x" || inputBid == "X")
            {
                // 使用者選擇放棄這回合不繼續下注
                playerBid = -1;
                this->isFoldThisRound = true;
                return playerBid;
            }

            playerBid = stoi(inputBid);
            if (playerBid > limitChip || (playerBid < lst && playerBid != -1))
                throw invalid_argument("請輸入在限制範圍內的數字");
        }
        catch (invalid_argument err)
        {
            if (err.what() == string("請輸入在限制範圍內的數字"))
            {
                cerr << err.what() << "，請重新輸入：";
            }
            else
            {
                cerr << "請輸入數字：";
            }
            // 清空錯誤狀態，以避免無窮迴圈
            cin.clear();
            // 忽略之前輸入的內容
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (playerBid > limitChip || (playerBid < lst && playerBid != -1));

    this->chipBiddenThisRound += playerBid;

    return playerBid;
}

void Player::sortCard()
{
    int cnt = 0;
    bool isFound = false;
    string cardOrder;
    vector<Card *> tempArr;

    for (int i = 0; i < this->cardArr.size(); i++)
        tempArr.push_back(this->cardArr[i]);

    cout << "請輸入最終數學式: ";
    while (isFound == false)
    {
        try
        {
            cin >> cardOrder;
            if (cardOrder.length() != 7)
                throw range_error("wrong length");
            for (int i = 1; i < cardOrder.length(); i += 2)
            {
                if (cardOrder[i] != '+' && cardOrder[i] != '-' && cardOrder[i] != '*' && cardOrder[i] != '/')
                    throw logic_error("wrong symbol");
                else
                {
                    if (cardOrder[i - 1] == '+' && cardOrder[i - 1] == '-' && cardOrder[i - 1] == '*' && cardOrder[i - 1] == '/' && cardOrder[i + 1] == '+' && cardOrder[i + 1] == '-' && cardOrder[i + 1] == '*' && cardOrder[i + 1] == '/')
                        throw logic_error("consecutive symbol");
                }
            }

            cnt = cardOrder.length();

            for (int i = 0; i < cnt; i++)
            {
                isFound = false;
                string str(1, cardOrder[i]);
                for (int j = 0; j < tempArr.size(); j++)
                {
                    if (tempArr[j]->getValue().compare(str) == 0)
                    {
                        isFound = true;
                        tempArr.erase(tempArr.begin() + j);
                        break;
                    }
                }
                if (isFound == false)
                    break;
            }

            if (isFound == true)
                break;
            else
            {
                throw exception();
            }
        }
        catch (range_error err)
        {
            cout << "輸入的數學式未包含所有手牌，請重新輸入：";
        }
        catch (logic_error err)
        {
            cout << "輸入的數學式邏輯有誤，請重新輸入：";
        }
        catch (exception err)
        {
            cout << "輸入的數學式包含非手牌的卡，請重新輸入：";
            tempArr.clear();
            for (int i = 0; i < this->cardArr.size(); i++)
                tempArr.push_back(this->cardArr[i]);
        }
    }
    tempArr.clear();

    Card *newCardArr[7];
    for (int i = 0; i < cnt; i++)
    {
        string str(1, cardOrder[i]);
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
        delete this->cardArr[i];
        this->cardArr[i] = newCardArr[i];
    }
}

void Player::printHandCard()
{
    cout << "-------------" << setw(10) << right << this->name << setw(10) << left << "'s hand"
         << "-------------" << endl;
    cout << setw(15) << "Hand : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        this->cardArr[i]->printCard();
        cout << " ";
    }
    cout << endl;
    cout << setw(15) << "Public : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        if (this->cardArr[i]->getVisibility() == true)
        {
            this->cardArr[i]->printCard();
            cout << " ";
        }
    }
    cout << endl;
    cout << setw(15) << "Hidden : ";
    for (int i = 0; i < this->cardArr.size(); i++)
    {
        if (this->cardArr[i]->getVisibility() == false)
        {
            this->cardArr[i]->printCard();
            cout << " ";
        }
    }
    cout << endl;
    cout << setw(46) << setfill('-') << "" << setfill(' ') << endl;
}

void Player::printWinner()
{
    cout << "恭喜你成為本次遊戲的贏家！" << endl;
}

void Player::throwCard(Card *c)
{
    if(this->isFoldThisRound == true)
        return;    
    if (this->cardArr[0]->getValue().compare("*") != 0 && this->cardArr[1]->getValue().compare("*") != 0)
    {
        this->printHandCard();
        cout << "抽到＊需要丟掉一張＋或—，請選擇(輸入q丟棄＊）：";
    }
    else if (this->cardArr[0]->getValue().compare("*") == 0 && this->cardArr[0]->getValue().compare("*") != 0)
    {
        this->printHandCard();
        cout << "抽到＊需要丟掉一張—，請選擇是否丟棄—(Y/N）：";
    }
    else if (this->cardArr[0]->getValue().compare("*") != 0 && this->cardArr[0]->getValue().compare("*") == 0)
    {
        this->printHandCard();
        cout << "抽到＊需要丟掉一張＋，請選擇是否丟棄＋(Y/N）：";
    }
    else
        return;

    string symbol;
    if (this->cardArr[0]->getValue().compare("*") != 0 && this->cardArr[1]->getValue().compare("*") != 0)
    {
        do
        {
            try
            {
                cin >> symbol;
                if (symbol != "+" && symbol != "-" && symbol != "q")
                    throw invalid_argument("wrong input");
            }
            catch (invalid_argument err)
            {
                cout << "輸入＋或—以外的符號，請重新輸入：";
            }
        } while (symbol != "+" && symbol != "-" && symbol != "q");
        if (symbol == "+")
            this->cardArr[0] = c;
        else if (symbol == "-")
            this->cardArr[1] = c;
        else
            return;
    }
    else
    {
        do
        {
            try
            {
                cin >> symbol;
                if (symbol != "Y" && symbol != "N")
                {
                    throw invalid_argument("Neither Y nor N");
                }
            }
            catch (invalid_argument e)
            {
                cout << "輸入不是Y或N，請重新輸入(Y/N)：";
            }
        } while (symbol != "Y" && symbol != "N");
        if (symbol == "Y")
        {
            if (this->cardArr[0]->getValue() == "*")
                this->cardArr[1] = c;
            else
                this->cardArr[0] = c;
        }
        else
            return;
    }
}

class Drunkard : public Character
{
private:
    bool _findNextIdx(bool isSymbol, int &idx);

public:
    Drunkard(const string n);
    ~Drunkard(){};
    void sortCard();
    int biddingChips(const int currChip, const int limitChip); // 前一人下注的籌碼
    void throwCard(Card *c);
    void printWinner();
};

Drunkard::Drunkard(const string n) : Character(n, false, "Drunkard")
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

    for (int i = 0; i < 10; i++)
    {
        int idx1 = 2 * (rand() % 4);
        int idx2 = 2 * (rand() % 4);
        swapCard(newCardArr[idx1], newCardArr[idx2]);
    }

    for (int i = 0; i < cardInHand; i += 2)
    {
        if (newCardArr[i]->getValue().compare("/") == 0)
        {
            if (newCardArr[i + 1]->getValue().compare("0") == 0)
                swapCard(newCardArr[i - 1], newCardArr[i + 1]);
        }
    }
    for (int i = 0; i < cardInHand; i++)
        this->cardArr[i] = newCardArr[i];
}

int Drunkard::biddingChips(const int currChip, const int limitChip)
{
    int diff = currChip - this->chipBiddenThisRound;
    if (this->totalChips == 0)
    {
        return -1; // 棄牌?
    }

    int bid = 0;
    // if (diff >= 0)
    {
        int delta = min(limitChip, this->totalChips) - currChip + 1;
        if (delta > 0)
            bid = (rand() % delta) / 2+ diff;
        else
        {
            if (rand() % 2 == 0)
                bid = diff;
            else
                bid = -1;
        }
    }
    if(bid != -1)
        this->chipBiddenThisRound += bid;
    
    return bid;
}

void Drunkard::throwCard(Card *c)
{
    int rand1 = rand() % 2;
    int rand2 = rand() % 2;
    if (rand1)
        return;
    else
        this->cardArr[rand2] = c;
}

void Drunkard::printWinner()
{
    cout << "你輸了..." << '\n'
         << "最終贏家為： " << this->name << endl;
}

class Rich : public Character
{
private:
    void swapPtr(Card *&p1, Card *&p2);

public:
    Rich(const string n); // constructor
    ~Rich(){};            // destructor
    void sortCard();
    bool _findNextIdx(bool isSymbol, int &idx);
    int biddingChips(const int currChip, const int limitChip);
    void printWinner();
    void throwCard(Card *c);
};

Rich::Rich(const string n) : Character(n, false, "Rich")
{
    this->totalChips += 10;  // defalut setting: the rich has ten more chips than other
    this->bigOrSmall = true; // 富豪喜歡賭大的
};

void Rich::throwCard(Card *c)
{
    this->cardArr[1] = c; // 把減號丟掉
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
    // swithch symbol card: * or / put in middle
    if (this->cardArr[1]->getValue() == "*" || this->cardArr[1]->getValue() == "/")
        swapPtr(this->cardArr[1], this->cardArr[3]);
    else if (this->cardArr[1]->getValue() == "*" || this->cardArr[1]->getValue() == "/")
        swapPtr(this->cardArr[5], this->cardArr[3]);

    int calRe = this->calCard();
    if (abs(calRe - 20) >= 10)
        swapPtr(this->cardArr[0], this->cardArr[4]);
}

int Rich::biddingChips(const int currChip, const int limitChip)
{
    // all in
    // if (currChip == this->chipBiddenThisRound)
    //     return 0;
    if (limitChip * 0.5 >= currChip)
    {
        this->chipBiddenThisRound += 0.5 * limitChip;
        return 0.5 * limitChip;
    }
    else if (limitChip * 0.75 >= currChip)
    {
        this->chipBiddenThisRound += 0.75 * limitChip;
        return 0.75 * limitChip;
    }
    else
    {
        this->chipBiddenThisRound += limitChip;
        return limitChip; // 要怎麼知道現在最少的籌碼有幾個
    }
}

void Rich::printWinner()
{
    cout << "你輸了..." << '\n'
         << "最終贏家為： " << this->name << endl;
}

class Math : public Character
{
private:
public:
    Math(const std::string &n) : Character(n, false, "Math"){};
    ~Math(){};
    void sortCard();
    int biddingChips(const int currChip, const int limitChip); // 前一人下注的籌碼(要先呼叫過sortCard才能呼叫biddingChips)
    void printWinner();
    void throwCard(Card *c);
};

void Math::throwCard(Card *c)
{
    this->cardArr[0] = c; // 把加號丟掉
}

int Math::biddingChips(const int currChip, const int limitChip)
{
    int bid = 0;
    // Modify by 蔡宗耘
    // 先複製this->cardArr到tempArr
    // 然後sort this->cardArr
    // 計算結果下注
    // 下注後將tempArr複製回this->cardArr，讓cardArr回復原來的順序
    vector<Card *> tempArr;

    for (int i = 0; i < 7; i++)
    {
        tempArr.push_back(cardArr[i]);
    }
    // modify end
    if (this->totalChips > 0) // 還有chips
    {
        if (this->cardArr.size() < 7)
        {
            bid = currChip - chipBiddenThisRound; // 下最少
            this->chipBiddenThisRound += bid;
        }
        else
        {
            // modify by 蔡宗耘
            this->sortCard();
            // modify end
            double result = this->calCard(); // 先取optimal solution(要先呼叫過sortCard才能呼叫biddingChips)

            bid = 0;
            double difference = 0;
            double target = 0;

            if (this->bigOrSmall == true)
                target = 20;
            else
                target = 1;

            difference = abs(result - target); // 計算跟target的差距

            if (difference <= 1)
                bid = limitChip; // difference < 1 就 all in
            else if (difference > 1 and difference <= 3)
                bid = ((currChip - chipBiddenThisRound) + limitChip) / 2; // 1 <  difference <= 3 就取中間
            else if (difference > 3 and difference <= 20)
                bid = currChip - chipBiddenThisRound; // 3 <  difference <= 20 就下最少
            else
            {
                this->isFoldThisRound = true; // difference > 10 就棄牌
                bid = -1;                     // fold 回傳 -1
            }

            // 沒棄牌
            if (this->isFoldThisRound == false)
                this->chipBiddenThisRound += bid;
        }
    }
    else
    {
        this->isFoldThisRound = true; // difference > 10 就棄牌
        bid = -1;                     // 棄牌回傳 -1
    }
    // modify by 蔡宗耘
    for (int i = 0; i < 7; i++)
    {
        this->cardArr[i] = tempArr[i];
    }
    // modify end
    return bid;
}

// 數學家的出牌演算法：找出optimal solution
void Math::sortCard()
{
    vector<Card *> numberCards; // 數字卡牌
    vector<Card *> symbolCards; // 符號卡牌

    // 將數字卡片和符號卡片分別放入對應的向量中
    for (int i = 0; i < 7; i++)
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

    // 儲存最小差距的絕對值和對應的結果
    double minDifference = numeric_limits<double>::infinity();
    double minDifference1 = numeric_limits<double>::infinity();
    double minDifference20 = numeric_limits<double>::infinity();
    vector<Card *> updatedCardArr; // 暫存更新後的卡牌排列

    // 窮舉所有可能的排列組合
    // 遍歷 4! * 3! = 144種可能性，找出optimal solution
    do
    {
        do
        {
            bool invalid = false; // 判斷是不是除以0
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
                minDifference1 = currentDifference1;
                minDifference20 = currentDifference20;


                // 將數字卡片和符號卡片放入 updatedCardArr
                symbolIndex = 0;
                updatedCardArr.clear(); // 清空先前的內容
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
        } while (next_permutation(numberCards.begin(), numberCards.end())); // 產生所有數字卡牌的排列組合
    } while (next_permutation(symbolCards.begin(), symbolCards.end()));     // 產生所有符號卡牌的排列組合

    if (minDifference20 < minDifference1)
        this->bigOrSmall = true;


    // 複製更新後的卡牌陣列回原始 cardArr
    for (size_t i = 0; i < 7; i++)
    {
        cardArr[i] = updatedCardArr[i]; // 複製新的卡牌
    }
}

void Math::printWinner()
{
    cout << "你輸了..." << '\n'
         << "最終贏家為： " << this->name << endl;
}

class JuDaKo : public Character
{
friend class Game;
private:
    void swapPtr(Card *&p1, Card *&p2);
public:
    JuDaKo(const string n); // constructor
    ~JuDaKo(){};            // destructor
    void sortCard();
    bool _findNextIdx(bool isSymbol, int &idx);
    int biddingChips(const int currChip, const int limitChip);
    void printWinner();
    void throwCard(Card *c);
    void rez(int leastChips);
};

void JuDaKo::swapPtr(Card *&p1, Card *&p2)
{
    Card *temp = p1;
    p1 = p2;
    p2 = temp;
}

JuDaKo::JuDaKo(const string n) : Character(n, false, "JuDaKo")
{  
    this->bigOrSmall = rand() % 2;
};

bool JuDaKo::_findNextIdx(bool isSymbol, int &idx)
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

void JuDaKo::sortCard()
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
    // 豬大哥會把數字由小到大排列
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3 - i - 1; j++)
        {
            if (stoi(this->cardArr[2 * j]->getValue()) - stoi(this->cardArr[2 * j + 2]->getValue()) < 0)
                swapPtr(this->cardArr[2 * j], this->cardArr[2 * j + 2]);
        }
    }
}

void JuDaKo::rez(int leastChips)
{
    cout << "豬大哥" << this->name << "復活儀式進行中..." << endl;
    int rebornTime = rand() % (2501) + 2500;
    this_thread::sleep_for(chrono::milliseconds(rebornTime));
    int ran = rand() % 2;
    if(ran == 1)
    {
        cout << "豬大哥" << this->name << " 復活成功，從大尾鱸鰻3劇組借到" << leastChips << "個籌碼" << endl;
        this->totalChips = leastChips;
        this->isAlive = true;
    }
    else
    {
        cout << "豬大哥" << this->name << "復活失敗..." << endl;
        this->isAlive = false;
    }
}

int JuDaKo::biddingChips(const int currChip, const int limitChip)
{
    int diff = currChip - this->chipBiddenThisRound;
    if(diff > this->totalChips){
        //如果currChip > 豬大哥目前有得籌碼數就棄牌
        return -1;
    }
    else{
        return diff;
    }
}

void JuDaKo::throwCard(Card *c)
{
    int ran = rand() % 2;
    this->cardArr[ran] = c;
}

void JuDaKo::printWinner()
{
    cout << "豬...大..哥...." << this->name << "獲勝" << endl;
}


class Landlord : public Character
{
private:
public:
    Landlord(const string n) : Character(n, false, "Landlord") {}; // constructor
    void sortCard();
    int biddingChips(const int currChip, const int limitChip);
    void printWinner();
    void throwCard(Card *c);
};

// borrow from Math::sortCard
void Landlord::sortCard()
{
    vector<Card *> numberCards; // 數字卡牌
    vector<Card *> symbolCards; // 符號卡牌

    // 將數字卡片和符號卡片分別放入對應的向量中
    for (int i = 0; i < 7; i++)
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

    // 儲存最小差距的絕對值和對應的結果
    double minDifference = numeric_limits<double>::infinity();
    double minDifference1 = numeric_limits<double>::infinity();
    double minDifference20 = numeric_limits<double>::infinity();
    vector<Card *> updatedCardArr; // 暫存更新後的卡牌排列

    // 窮舉所有可能的排列組合
    // 遍歷 4! * 3! = 144種可能性，找出optimal solution
    do
    {
        do
        {
            bool invalid = false; // 判斷是不是除以0
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
                minDifference1 = currentDifference1;
                minDifference20 = currentDifference20;


                // 將數字卡片和符號卡片放入 updatedCardArr
                symbolIndex = 0;
                updatedCardArr.clear(); // 清空先前的內容
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
        } while (next_permutation(numberCards.begin(), numberCards.end())); // 產生所有數字卡牌的排列組合
    } while (next_permutation(symbolCards.begin(), symbolCards.end()));     // 產生所有符號卡牌的排列組合

    if (minDifference20 < minDifference1)
        this->bigOrSmall = true;


    // 複製更新後的卡牌陣列回原始 cardArr
    for (size_t i = 0; i < 7; i++)
    {
        cardArr[i] = updatedCardArr[i]; // 複製新的卡牌
    }
}

int Landlord::biddingChips(const int currChip, const int limitChip){
    int diff = currChip - this->chipBiddenThisRound;
    if(diff <= this->totalChips){
        this->chipBiddenThisRound += diff;
        return diff;
    }
    else{
        return -1;
    }
}

void Landlord::throwCard(Card *c)
{
    static int idx = 0;
    if (idx = 0){
        this->cardArr[idx] = c;
        idx = 1;
    }
    else{
        this->cardArr[idx] = c;
        idx = 0;
    }
}

void Landlord::printWinner()
{
    cout << "信義區大地主" << this->name << "贏了" << endl;
}


class Game
{
private:
    vector<Character *> playerList;
    vector<Card *> cardList;
    vector<Character *> playerListPerRnd;
    int roundN;
    int chipsInRound;
    int currChip;
    int leastChips;
    int totalCardInGame;
    bool playerAlive;
    bool playerFold;
    void _shuffle(int startIdx);
    void _swapPlayer(int idx1, int idx2);

public:
    Game();
    void addPlayer(Player &pyptr);
    void initPlayerRnd();
    void initCardList();
    void dealCard(int rnd);
    void printPlayersCard();
    void printPlayerChips(int num);
    void biddingPerRound(int rnd);
    void biddingPrint();
    void update();
    void printResult();
    void printPlayerList();
    void enemySort();
    void gameStart(Player &pyptr, const int playerNum);
    void calChips();
    void decisionInput();
    void printFinalResult();
    void kickoutPlayer();
    bool endRound();
    void printShortRule();
};

Game::Game()
{
    this->roundN = 0;
    this->chipsInRound = 0;
    this->currChip = 0;
    this->totalCardInGame = 0;
    this->leastChips = 1000; // 下注最高限制
    this->playerAlive = true;
    this->playerFold = false;
}

void Game::addPlayer(Player &pyptr)
{
    this->playerList.push_back(&pyptr);
}

void Game::initPlayerRnd()
{
    this->playerListPerRnd.clear();
    for(int i = 0; i < this->playerList.size(); i++)
        this->playerListPerRnd.push_back(this->playerList[i]);
    this->playerFold = false;
    for(int i = 0; i  < this->playerList.size(); i++){
        if(this->playerList[i]->type == "Landlord"){
            this->playerList[i]->totalChips += 5;
        }
    }
}

void Game::initCardList()
{
    // 清空卡池
    this->cardList.clear();
    // 重新加入數字牌
    for (int i = 0; i < 10; i++)
    {
        NumberCard *_nc = new NumberCard(i, "R");
        this->cardList.push_back(_nc);
        this->totalCardInGame++;
    }

    for (int i = 0; i < 10; i++)
    {
        NumberCard *_nc = new NumberCard(i, "G");
        this->cardList.push_back(_nc);
        this->totalCardInGame++;
    }

    for (int i = 0; i < 10; i++)
    {
        NumberCard *_nc = new NumberCard(i, "B");
        this->cardList.push_back(_nc);
        this->totalCardInGame++;
    }

    for (int i = 0; i < 10; i++)
    {
        NumberCard *_nc = new NumberCard(i, "W");
        this->cardList.push_back(_nc);
        this->totalCardInGame++;
    }
    this->_shuffle(0);
}

void Game::_shuffle(int startIdx)
{
    const int shuffleTime = 2 * this->cardList.size();
    for (int i = 0; i < shuffleTime; i++)
    {
        int idx1 = rand() % (this->cardList.size() - startIdx) + startIdx;
        int idx2 = rand() % (this->cardList.size() - startIdx) + startIdx;
        Card *temp = this->cardList[idx1];
        this->cardList[idx1] = this->cardList[idx2];
        this->cardList[idx2] = temp;
    }
}

void Game::_swapPlayer(int idx1, int idx2)
{
    Character *tempPtr = this->playerList[idx1];
    this->playerList[idx1] = this->playerList[idx2];
    this->playerList[idx2] = tempPtr;
}

void Game::dealCard(int rnd)
{
    static int cardListIdx = 0;
    if (rnd == 0)
    {
        for (int i = 0; i < this->playerList.size(); i++)
        {
            SymbolCard *add = new SymbolCard("+");
            SymbolCard *sub = new SymbolCard("-");
            SymbolCard *dev = new SymbolCard("/");
            playerList[i]->addCard(add);
            playerList[i]->addCard(sub);
            playerList[i]->addCard(dev);
        }
    }
    else if (rnd == 1)
    {
        for (int i = 0; i < this->playerList.size(); i++)
        {
            this->cardList[cardListIdx]->setVisibility(false);
            playerList[i]->addCard(this->cardList[cardListIdx]);
            cardListIdx++;
        }
        // add |x|
        for (int i = 0; i < this->playerList.size(); i++)
        {
            SymbolCard *mul = new SymbolCard("*");
            this->cardList.push_back(mul);
            this->totalCardInGame++;
        }
        _shuffle(cardListIdx);

        for (int i = 0; i < this->playerList.size(); i++)
        {
            if (this->cardList[cardListIdx]->getValue().compare("*") != 0)
            {
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
            else
            {
                while (this->cardList[cardListIdx]->getValue().compare("*") == 0)
                {
                    playerList[i]->throwCard(this->cardList[cardListIdx]);
                    cardListIdx++;
                }
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
        }

        for (int i = 0; i < this->playerList.size(); i++)
        {
            if (this->cardList[cardListIdx]->getValue().compare("*") != 0)
            {
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
            else
            {
                while (this->cardList[cardListIdx]->getValue().compare("*") == 0)
                {
                    playerList[i]->throwCard(this->cardList[cardListIdx]);
                    cardListIdx++;
                }
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
        }
    }
    else if (rnd == 2)
    {
        for (int i = 0; i < this->playerList.size(); i++)
        {
            if (this->cardList[cardListIdx]->getValue().compare("*") != 0)
            {
                playerList[i]->addCard(this->cardList[cardListIdx]);
                cardListIdx++;
            }
            else
            {
                while (this->cardList[cardListIdx]->getValue().compare("*") == 0)
                {
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
    // print enemies' first
    cout << "-------------" << setw(10) << right << "Other Player's Cards" << setw(10) << left << "-------------"
         << endl;
    // cout << "Other Player's Cards" << endl;
    for (int i = 0; i < this->playerListPerRnd.size(); i++)
    {
        this_thread::sleep_for(chrono::milliseconds(500));
        if (this->playerListPerRnd[i]->isPlayer == false)
            this->playerListPerRnd[i]->printHandCard();
    }
    // print player's
    for (int i = 0; i < this->playerListPerRnd.size(); i++)
    {
        if (this->playerListPerRnd[i]->isPlayer)
            this->playerListPerRnd[i]->printHandCard();
    }
}

void Game::printPlayerChips(int num)
{
    // num == 1 已下注數量, num == 2 目前手中籌碼數量
    cout << endl;
    if (num == 1)
        cout << "===========已下注數量===========" << endl;
    else
        cout << "=========目前手中籌碼數量========" << endl;
    for (int i = 0; i < this->playerListPerRnd.size(); i++)
    {
        if (this->playerListPerRnd[i]->isPlayer == true)
            cout << BOLD;
        cout << setw(7) << left << this->playerListPerRnd[i]->name << ": ";
        if (num == 1)
            cout << this->playerListPerRnd[i]->chipBiddenThisRound;
        else
            cout << this->playerListPerRnd[i]->totalChips;
        cout << NC << endl;
    }
}

void Game::biddingPerRound(int rnd)
{
    if (rnd == 0) // 基本下注1
    {
        cout << "\n";
        cout << "本回合開始，進行基本下注，每位玩家下注1個籌碼" << endl;
        cout << "\n";
        
        chipsInRound = 0;
        currChip = 0;
        leastChips = 1000; // 這回合擁有最少籌碼的人的籌碼數 為本回合下注的最高限制數量
        for (int i = 0; i < playerListPerRnd.size(); i++)
        {
            this->playerListPerRnd[i]->isFoldThisRound = false;
            if (this->playerListPerRnd[i]->totalChips < leastChips)
                leastChips = this->playerListPerRnd[i]->totalChips;
        }

        // 每個人基本下注1
        for (int i = 0; i < playerListPerRnd.size(); i++)
        {
            this->playerListPerRnd[i]->totalChips--;
            chipsInRound++;
            this->playerListPerRnd[i]->chipBiddenThisRound++;
        }
        currChip = 1;
    }
    else // 第一次下注與第二次下注
    {
        cout << "\n";
        bool bidEnd = false;
        if (currChip == leastChips){
            for (int i = 0; i < playerListPerRnd.size(); i++)
            {
                this->playerListPerRnd[i]->chipsRaised = 0;
            }
            cout << "已達最高下注數，不繼續進行加注" << endl;
        }
        else
        {
            cout << "\n";
            cout << "加注開始" << endl;
            while (bidEnd == false)
            {
                for (int i = 0; i < playerListPerRnd.size(); i++)
                {
                    this->playerListPerRnd[i]->chipsRaised = 0;
                }
                bool startBid = true;
                for (int i = 0; i < playerListPerRnd.size(); i++)
                {
                    // if (this->playerListPerRnd[i]->isFoldThisRound == true)
                    //     continue; // 已棄牌
                    // cout << this->playerListPerRnd[i]->name << " 已下注數量: " <<
                    // this->playerListPerRnd[i]->chipBiddenThisRound << endl; 加注
                    //if(this->playerListPerRnd[i]->isFoldThisRound)
                    //    continue;
                    int pyBidNum = 0;
                    if(this->playerListPerRnd[i]->chipBiddenThisRound == leastChips)
                        continue;
                    if (startBid == true && currChip == this->playerListPerRnd[i]->chipBiddenThisRound) // 此輪非第一次加注，但加注總數量與上次相同
                        pyBidNum = 0;
                    else
                    {
                        if (this->playerListPerRnd[i]->isPlayer)
                        {
                            Game::printPlayersCard();
                            Game::biddingPrint();
                        }
                        pyBidNum = this->playerListPerRnd[i]->biddingChips(currChip, (leastChips - this->playerListPerRnd[i]->chipBiddenThisRound));
                    }
                    this->playerListPerRnd[i]->chipsRaised = pyBidNum;
                    // is fold
                    if (pyBidNum == -1)
                    {
                        cout << setw(10) << left;
                        cout << this->playerListPerRnd[i]->name << "放棄這回合" << endl;
                        this_thread::sleep_for(chrono::milliseconds(500));
                        this->playerListPerRnd[i]->isFoldThisRound = true;
                        if (this->playerListPerRnd[i]->isPlayer == true)
                            this->playerFold = true;
                        continue;
                    }

                    // remove from playerlist this rnd
                    if (this->playerListPerRnd[i]->chipBiddenThisRound < currChip)
                    {
                        pyBidNum = currChip - this->playerListPerRnd[i]->chipBiddenThisRound;
                        this->playerListPerRnd[i]->chipBiddenThisRound = currChip;
                    }

                    // print player bidding situation
                    // cout << setw(9) << left;
                    // cout << this->playerListPerRnd[i]->name << " 加注數量: " << setw(2) << right << pyBidNum;
                    // cout << " || "
                    //      << " 總下注數量: " << this->playerListPerRnd[i]->chipBiddenThisRound << endl;

                    currChip = max(this->playerListPerRnd[i]->chipBiddenThisRound, currChip);
                    chipsInRound += pyBidNum;
                    this->playerListPerRnd[i]->totalChips -= pyBidNum;
                    startBid = false;
                }

                for (int i = 0; i < playerListPerRnd.size(); i++)
                {
                    if (this->playerListPerRnd[i]->isFoldThisRound)
                    {
                        this->playerListPerRnd.erase(this->playerListPerRnd.begin() + i);
                        // cout << "someone fold;; " << playerListPerRnd.size() <<
                        // playerList.size() << endl;
                        //i--;
                    }
                }

                // for (int i = 0; i < playerListPerRnd.size(); i++)
                // {
                //     if (this->playerListPerRnd[i]->isFoldThisRound == true)
                //         continue; // 已棄牌
                //     cout << setw(10) << left;
                //     cout << this->playerListPerRnd[i]->name << " 目前手中籌碼總數: ";
                //     cout << this->playerListPerRnd[i]->totalChips << endl;
                // }
                // Game::printPlayerChips(2);

                int cntSame = 0;
                for (int i = 0; i < playerListPerRnd.size(); i++)
                {
                    if (this->playerListPerRnd[i]->chipBiddenThisRound == currChip)
                        cntSame++;
                }
                if (cntSame == playerListPerRnd.size())
                    bidEnd = true;
            }
        }
        cout << "\n" << setw(17) << setfill('-') << ""
             << "Stop bidding" << setw(17) << ""  << setfill(' ') << endl;
        for (int i = 0; i < playerListPerRnd.size(); i++)
        {
            if (this->playerListPerRnd[i]->isFoldThisRound)
            {
                this->playerListPerRnd.erase(this->playerListPerRnd.begin() + i);
                // cout << "someone fold;; " << playerListPerRnd.size() <<
                // playerList.size() << endl;
                //i--;
            }
        }
        Game::biddingPrint();
        cout << "本回合現在下注總數: " << chipsInRound << endl
             << endl;
    }
};

void Game::biddingPrint()
{
    printf("%-13s|%-10s|%-10s|%-10s\n", "NAME", "Raised", "Bidding", "Total Chips");
    cout << setw(46) << setfill('-') << "" << setfill(' ') << endl;

    for (int i = 0; i < playerListPerRnd.size(); i++)
    {
        string raised;
        if(this->playerListPerRnd[i]->chipsRaised == -1)
            raised = "x";
        else
            raised = to_string(this->playerListPerRnd[i]->chipsRaised);

        printf("%-13s|%-10s|%-10d|%-10d\n", this->playerListPerRnd[i]->name.c_str(), raised.c_str(), this->playerListPerRnd[i]->chipBiddenThisRound, this->playerListPerRnd[i]->totalChips);
    }
    cout << setw(46) << setfill('-') << "" << setfill(' ') << endl;
};

void Game::printPlayerList()
{
    cout << "=================Players List=================" << endl;
    cout << setw(9)<< " " <<setw(12) << left << "NAME"
         << setw(12) << "CHARACTER" 
         << setw(5) << "CHIPS" << endl;
    cout << "----------------------------------------------" << endl;
    for (int i = 0; i < playerList.size(); i++)
    {
        if (this->playerList[i]->isPlayer == true)
            cout << BOLD;
        cout << setw(9) << " " << setw(12);
        this->playerList[i]->printName();
        cout << setw(12) << this->playerList[i]->type;
        cout << setw(5) << this->playerList[i]->getTotalChips();
        cout << NC << endl;
    }
    cout << "=============================================" << endl;
}

void Game::enemySort()
{
    for (int i = 0; i < this->playerListPerRnd.size(); i++)
    {
        if (this->playerListPerRnd[i]->isPlayer == false)
            this->playerListPerRnd[i]->sortCard();
    }
}

void Game::gameStart(Player &pyptr, const int playerNum)
{
    vector<string> nameList = {"Fourier", "Jay Chou", "Euler", "Ramam", "Newton", "Swift", "Faker", "Lee", "Chen", "Yttria", "GodTone", "Roger", "Calvin", "Einstein", "Maxwell", "Euclidean", "003"};

    for (int i = 0; i < 2 * playerNum; i++)
    {
        int idx1 = rand() % nameList.size();
        int idx2 = rand() % nameList.size();
        iter_swap((nameList.begin() + idx1), (nameList.begin() + idx2));
    }

    cout << "資管盃慈善德州撲克大賽 開始！" << endl;
    int ran;
    this->addPlayer(pyptr);
    // 隨機加入不同角色的電腦玩家
    for (int i = 1; i < playerNum; i++)
    {
        ran = rand() % 5;
        if (ran == 0)
        {
            Drunkard *d = new Drunkard(nameList.back());
            this->playerList.push_back(d);
        }
        else if (ran == 1)
        {
            Rich *r = new Rich(nameList.back());
            this->playerList.push_back(r);
        }
        else if (ran == 2)
        {
            Math *m = new Math(nameList.back());
            this->playerList.push_back(m);
        }
        else if (ran == 3)
        {
            JuDaKo *j = new JuDaKo(nameList.back());
            this->playerList.push_back(j);
        }
        else if (ran == 4)
        {
            Landlord *j = new Landlord(nameList.back());
            this->playerList.push_back(j);
        }
        nameList.pop_back();
    }
    // 重新分配玩家順序
    for (int i = 0; i < 2 * this->playerList.size(); i++)
    {
        int idx1 = rand() % this->playerList.size();
        int idx2 = rand() % this->playerList.size();
        iter_swap((this->playerList.begin() + idx1), (this->playerList.begin() + idx2));
    }
}

void Game::calChips()
{
    // find player's position
    //int playerPos = 0;
    //for (int i = 0; i < this->playerListPerRnd.size(); i++)
    //{
    //    if (this->playerListPerRnd[i]->isPlayer == true)
    //    {
    //        playerPos = i;
    //        break;
    //    }
    //}
    /*
    //cout << this->playerListPerRnd[playerPos]->name << "目前籌碼數量：" << playerListPerRnd[playerPos]->totalChips << endl;
    if (playerListPerRnd[playerPos]->totalChips == 0)
    {
        for (int i = 1; i < this->playerListPerRnd.size(); i++)
        {
            cout << setw(10) << left;
            playerListPerRnd[i]->printName();
            cout << right;
            cout << ": " << playerListPerRnd[i]->totalChips << endl;
        }
        for (int i = 0; i < this->playerListPerRnd.size(); i++)
        {
            if (playerListPerRnd[i]->totalChips == 0)
            {
                cout << "玩家";
                cout << setw(10);
                playerListPerRnd[i]->printName();
                cout << " 籌碼數量歸零，退出遊戲。" << endl;
                if(playerListPerRnd[i]->type == "JuDaKo")
                    dynamic_cast<JuDaKo*>(playerListPerRnd[i])->rez(this->leastChips);
                else
                    this->playerListPerRnd[i]->isAlive = false;
            }
        }
    }
    else
    {
        for (int i = 0; i < this->playerListPerRnd.size(); i++)
        {
            cout << setw(10) << left;
            playerListPerRnd[i]->printName();
            cout << right;
            cout << ": " << playerListPerRnd[i]->totalChips << endl;
        }
        for (int i = 0; i < this->playerListPerRnd.size(); i++)
        {
            if (playerListPerRnd[i]->totalChips <= 0)
            {
                cout << "玩家";
                cout << setw(10);
                playerListPerRnd[i]->printName();
                cout << " 籌碼數量歸零，退出遊戲。" << endl;
                if(playerListPerRnd[i]->type == "JuDaKo")
                    dynamic_cast<JuDaKo*>(playerListPerRnd[i])->rez(this->leastChips);
                else{
                    if(this->playerListPerRnd[i]->isPlayer == true)
                        this->playerAlive = false;
                    this->playerListPerRnd[i]->isAlive = false;
                }
            }
        }
    }
    */
    cout << "|" << setw(12) << left << "NAME" << "|" << setw(10) << "CHARACTER" << "|" <<setw(21) << "REMAIN CHIPS"<< endl;
    cout << setw(46) << setfill('-') << "" << setfill(' ') << endl;
    for (int i = 0; i < this->playerListPerRnd.size(); i++)
    {
        cout << "|" << setw(12) << left;
        playerListPerRnd[i]->printName();
        cout << "|" << setw(10) << this->playerListPerRnd[i]->type;
        cout << "|" << setw(10) << right << playerListPerRnd[i]->totalChips << left << endl;
    }
    cout << setw(46) << setfill('-') << "" << setfill(' ') << endl;
    for (int i = 0; i < this->playerListPerRnd.size(); i++)
    {
        if (playerListPerRnd[i]->totalChips <= 0)
        {
            cout << "玩家";
            cout << setw(10);
            playerListPerRnd[i]->printName();
            cout << " 籌碼數量歸零，退出遊戲。" << endl;
            if(playerListPerRnd[i]->type == "JuDaKo")
                dynamic_cast<JuDaKo*>(playerListPerRnd[i])->rez(this->leastChips);
            else{
                if(this->playerListPerRnd[i]->isPlayer == true)
                    this->playerAlive = false;
                this->playerListPerRnd[i]->isAlive = false;
            }
        }
    }
    this_thread::sleep_for(chrono::milliseconds(5000));
}

void Game::printResult()
{
    // 計算大家的結果，找出贏的人
    vector<double> playerValue;
    for (int i = 0; i < this->playerListPerRnd.size(); i++)
        playerValue.push_back(playerListPerRnd[i]->calCard());

    // 回合結果公布：
    Character *bigWinner = nullptr;
    Character *smallWinner = nullptr;
    int bigWinneridx = 0;
    int smallWinneridx = 0;
    double closestBigDifference = numeric_limits<double>::infinity();
    double closestSmallDifference = numeric_limits<double>::infinity();

    for (int i = 0; i < this->playerListPerRnd.size(); i++)
    {
        double targetValue = (playerListPerRnd[i]->bigOrSmall) ? 20.0 : 1.0;
        double difference = abs(playerValue[i] - targetValue);

        if (playerListPerRnd[i]->bigOrSmall)
        {
            // 賭大的情況
            if (difference < closestBigDifference)
            {
                closestBigDifference = difference;
                bigWinner = playerListPerRnd[i];
                bigWinneridx = i;
            }
            else if (difference == closestBigDifference)
            {
                if(bigWinner != nullptr){
                    // 在賭大的情況下，處理同分的情況
                    if (playerListPerRnd[i]->getMaxCard() > bigWinner->getMaxCard())
                    {
                        bigWinner = playerListPerRnd[i];
                        bigWinneridx = i;
                    }
                }
                else{
                    bigWinner = playerListPerRnd[i];
                    bigWinneridx = i;
                }
            }
        }
        else
        {
            // 賭小的情況
            if (difference < closestSmallDifference)
            {
                closestSmallDifference = difference;
                smallWinner = playerListPerRnd[i];
                smallWinneridx = i;
            }
            else if (difference == closestSmallDifference)
            {
                if(smallWinner != nullptr){
                    // 在賭小的情況下，處理同分的情況
                    if (playerListPerRnd[i]->getMinCard() < smallWinner->getMinCard())
                    {
                        smallWinner = playerListPerRnd[i];
                        smallWinneridx = i;
                    }
                }
                else{
                    smallWinner = playerListPerRnd[i];
                    smallWinneridx = i;
                }
            }
        }
    }

    
    // 回合結果公布：
    if (this->playerAlive == true && this->playerFold == false)
    {
        if ((bigWinner != nullptr and bigWinner->isPlayer == true) or (smallWinner != nullptr and smallWinner->isPlayer == true))
            cout << "恭喜你贏了這一回合！" << endl;
        //else
        //    cout << "下一回合再接再厲qq" << endl;
    }
    // 輸出結果
    if (bigWinner != nullptr)
    {
        cout << "賭大的贏家是： ";
        cout << setw(10) << bigWinner->name;
        cout << " 數學式結果為：" << setw(10) << right <<fixed << setprecision(6) << playerValue[bigWinneridx] << left << endl;
    }
    else
    {
        cout << "沒有賭大的贏家" << endl;
    }
    this_thread::sleep_for(chrono::milliseconds(500));
    if (smallWinner != nullptr)
    {
        cout << "賭小的贏家是： ";
        cout << setw(10) << smallWinner->name;
        cout << " 數學式結果為：" <<setw(10) << right << fixed << setprecision(6) << playerValue[smallWinneridx] << left << endl;
    }
    else
    {
        cout << "沒有賭小的贏家" << endl;
    }
    this_thread::sleep_for(chrono::milliseconds(500));
    // 輸出其餘玩家的名稱和數學式結果
    if(this->playerListPerRnd.size() > 2 || (bigWinner == nullptr || smallWinner == nullptr))
        cout << "其餘玩家： " << endl;
    for (int i = 0; i < this->playerListPerRnd.size(); i++)
    {
        if ((playerListPerRnd[i] != bigWinner) and (playerListPerRnd[i] != smallWinner))
        {
            cout << setw(10) << left;
            playerListPerRnd[i]->printName();
            cout << right;
            if (playerListPerRnd[i]->bigOrSmall == true)
                cout << " 賭大 ";
            else
                cout << " 賭小 ";
            cout << " 數學式結果為：" << setw(10) << right << fixed << setprecision(6) << playerValue[i] << left << endl;
           this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
    
    // 分配籌碼
    // chipsinround//此輪總籌碼
    if (bigWinner != nullptr and smallWinner != nullptr) // bigWinner和smallWinner都有
    {
        playerListPerRnd[bigWinneridx]->totalChips += (chipsInRound / 2);
        playerListPerRnd[smallWinneridx]->totalChips += (chipsInRound / 2);
    }
    else if (bigWinner != nullptr and smallWinner == nullptr) // 只有bigWinner
    {
        playerListPerRnd[bigWinneridx]->totalChips += chipsInRound;
    }
    else if (bigWinner == nullptr and smallWinner != nullptr) // 只有smallWinner
    {
        playerListPerRnd[smallWinneridx]->totalChips += chipsInRound;
    }
    
}

void Game::decisionInput()
{
    if (this->playerAlive == true && this->playerFold == false)
    {
        // 第二輪下注結束
        //  找玩家的位置
        int playerPos = 0;
        for (int i = 0; i < this->playerListPerRnd.size(); i++)
        {
            if (this->playerListPerRnd[i]->isPlayer == true)
            {
                playerPos = i;
                break;
            }
        }

        bool bidDirection = true;
        char input;
        cout << "請決定賭大 / 小，並輸入您的最終數學式" << endl;
        cout << "輸入賭注方(B / S) : ";
        try
        {
            cin >> input;
            if (input != 'S' && input != 'B')
                throw invalid_argument("invalid input");
        }
        catch (invalid_argument err)
        {
            while (input != 'S' && input != 'B')
            {
                cout << "必須輸入B或S，重新輸入賭注方(B / S) : ";
                cin >> input;
            }
        }
        if (input == 'S')
            bidDirection = false;

        playerListPerRnd[playerPos]->bigOrSmall = bidDirection;
        playerListPerRnd[playerPos]->sortCard();
    }
}

void Game::printFinalResult()
{
    // 潛在問題：玩家名字和電腦角色名字相同
    Character *winner = nullptr;
    int winnerChips = 0;
    // int winner = 0;
    for (int i = 0; i < this->playerList.size(); i++)
    {
        if (playerList[i]->totalChips > winnerChips)
        {
            winner = playerList[i];
            winnerChips = playerList[i]->totalChips;
        }
    }
    winner->printWinner();
}

void Game::kickoutPlayer()
{
    for(int i = 0;  i < this->playerList.size(); i++){
        if(this->playerList[i]->totalChips == 0){
            this->playerList.erase(this->playerList.begin() + i);
            i--;
        }
    }
}

bool Game::endRound()
{
    if (this->playerList.size() == 1)
    {
        return false;
    }
    for (int i = 0; i < this->playerList.size(); i++)
    {
        this->playerList[i]->cardArr.clear();
        this->playerList[i]->chipBiddenThisRound = 0;
        this->playerList[i]->isFoldThisRound = false;
    }
    this->playerFold == false;
    return true;
}

void Game::printShortRule()
{
    cout << "歡迎來到" << BOLD << "資管盃慈善德州撲克大賽" << NC <<endl;
    cout << "每位玩家要根據自己的手牌，組出一個數學式，使得該數學式結果盡可能接近或等於20或1，並選擇下注大(B)或小(S)" << endl;
    cout << "遊戲流程如下：" << endl;
    cout << "根據玩家人數進行對應的回合數，每回合包括第一輪發牌、第一輪下注、第二輪發牌、第二輪下注、選擇下注的方向與排列數字牌與公布結果" << endl;
    cout << "發牌：每個玩家回合一開始都有三張基本牌(+, -, /)，第一輪發牌會發一張數字牌作為暗牌，兩張公開牌(可能包含 * ，抽到需丟棄一張 + 或 -，並重新抽一張數字牌)，" << endl;
    cout << "      第二輪發牌會再拿到一張公開牌，至此玩家手上有七張牌。" << endl << endl;
    cout << "下注：在每回合開始時，玩家需要下注一個籌碼，下注時每一位玩家下注的下限是前一位玩家這回合下注的數量，上限是該回合玩家擁有籌碼數量的最小值，直到所有下注玩家下注的籌碼相同。" << endl << endl;
    cout << "玩家可根據手牌與其他玩家的公開牌選擇下注大或小，選擇大者，需要排列手牌使數學式結果接近或等於20，反之。與目標差距最小者獲勝，該回合賭大獲勝者與賭小獲勝者會平分該回合下注的籌碼。" << endl;
    cout << "若遇到差距相同則比較數字牌大小，一共有四種顏色：" << RED << "|1| " << GREEN << "|1| " << BLUE << "|1| " << NC << "|1|，大小順序為：" << RED << "|1|" << NC << ">" GREEN << "|1|" << NC << ">"<< BLUE << "|1|" << NC << ">" << "|1|，"
         << "賭大的回合中擁有最大數字牌，或賭小回合中擁有最小數字牌的玩家(數字相同會比較牌的顏色)將得以勝出" << endl;
    cout << "PS. x / 0 = inf for all x ≠ 0， 0 / 0 is undefined，會直接判定該回合落敗" << endl;
    cout << "按Enter繼續...";
    cin.get();
    getchar();
    cout << endl << endl;
}


int main()
{
    srand(time(nullptr));
    int playerNum = 3; // 預設值3
    cout << "資管盃慈善德州撲克大賽" << endl;
    cout << "請輸入玩家人數(2~10): ";
    while (true)
    {
        try
        {
            cin >> playerNum;
            if (cin.fail())
                throw invalid_argument("非數字");
            if (playerNum > 10 || playerNum < 2)
                throw invalid_argument("請輸入在限制範圍內的數字");
            break;
        }
        catch (invalid_argument err)
        {
            if (err.what() == string("請輸入在限制範圍內的數字"))
                cerr << err.what() << "，請重新輸入：";
            else if (err.what() == string("非數字"))
                cerr << "請輸入數字：";
            // 清空錯誤狀態，以避免無窮迴圈
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    // main for test
    cout << "請輸入玩家名稱: ";
    string playerName;
    cin >> playerName;
    Player py(playerName);
    
    py.printName();
    cout << " 歡迎加入遊戲！" << endl;
    cout << "\n";
    Game G;
    //G.printShortRule();
    G.gameStart(py, playerNum);
    bool continueGame = true;

    for (int i = 1; i <= playerNum; i++)
    {
        cout << setw(19) << setfill('-') << ""
             << BOLD << "ROUND" << setw(3) << right << setfill(' ') << i << setfill('-') << left << NC << setw(19) << "" << setfill(' ') << endl;
        G.initPlayerRnd();
        G.printPlayerList();
        G.initCardList();
        G.dealCard(0);
        //cout << setw(14) << setfill('-') << ""
        //     << "發基本符號牌三張" << setw(14) << "" << setfill(' ') << endl;
        G.biddingPerRound(0);
        G.dealCard(1);
        cout << setw(20) << setfill('-') << ""
             << "BID  1" << setw(20) << "" << setfill(' ') << endl;
        G.biddingPerRound(1);
        G.dealCard(2);
        cout << setw(20) << setfill('-') << ""
             << "BID  2" << setw(20) << "" << setfill(' ') << endl;
        G.biddingPerRound(2);
        G.printPlayersCard();
        G.enemySort();
        G.decisionInput();
        G.printResult();
        G.calChips();
        G.kickoutPlayer();                                 // 將籌碼歸零的玩家移除playerList;
        continueGame = G.endRound();
        cout << setw(15) << setfill('-') << ""
             << BOLD << "END OF ROUND" << setw(3) << right << setfill(' ') << i << setfill('-') << left << NC << setw(16) << "" << setfill(' ') << endl;
        if (continueGame == false)
            break;
    }

    G.printFinalResult();
    return 0;
}