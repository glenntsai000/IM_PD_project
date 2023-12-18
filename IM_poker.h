#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <cmath>
using namespace std;


const int initialTotalChips = 10;
const int cardInHand = 7;


class Card
{
protected:
    bool isSymbol;
    bool visibility;
public:
    Card(bool);
    bool isSymbolCard();
    bool getVisibility();
    virtual string getValue() const = 0;
    virtual string getColor() const = 0;
    virtual void printCard() = 0;
    virtual void setVisibility(bool) = 0;
    virtual bool operator>(const Card &) const = 0;
    virtual bool operator<(const Card &) const = 0;
};


class SymbolCard : public Card
{
private:
    string symbol;

public:
    SymbolCard(string);
    string getValue() const;
    void printCard();
    void setVisibility(bool);
    bool operator>(const Card &) const;
    bool operator<(const Card &) const;
    string getColor() const;
};


class NumberCard : public Card
{
private:
    int num;
    string color;

public:
    NumberCard(int, string);
    string getValue() const;
    void printCard();
    void setVisibility(bool);
    bool operator>(const Card &) const;
    bool operator<(const Card &) const;
    string getColor() const;
};




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
    Character(const string &, const bool, const string); // constructor                                             // destructor
    void addCard(Card *);
    void swapCard(Card *, Card *);
    void printName();
    int getTotalChips();
    double calCard(); // 計算牌的大小
    virtual void printHandCard();
    virtual void sortCard() = 0;
    virtual int biddingChips(const int, const int) = 0; // 前一人下注的籌碼
    virtual void printWinner() = 0;
    virtual void throwCard(Card *); // 只能丟+ or -
    friend class Game;
    NumberCard *getMaxCard();
    NumberCard *getMinCard();
};


class Player : public Character
{
private:
public:
    Player(const string &);
    void sortCard();
    int biddingChips(const int, const int);
    void printHandCard(); // 使玩家可以印出隱藏牌
    void printWinner();
    void throwCard(Card *);
    friend class Game;
};


class Drunkard : public Character
{
private:
    bool _findNextIdx(bool, int &);
public:
    Drunkard(const string);
    void sortCard();
    int biddingChips(const int, const int); // 前一人下注的籌碼
    void throwCard(Card *);
    void printWinner();
};


class Rich : public Character
{
private:
    void swapPtr(Card *&, Card *&);
public:
    Rich(const string); // constructor
    void sortCard();
    bool _findNextIdx(bool, int &);
    int biddingChips(const int, const int);
    void printWinner();
    void throwCard(Card *);
};


class Math : public Character
{
private:
public:
    Math(const std::string &);
    void sortCard();
    int biddingChips(const int, const int); // 前一人下注的籌碼(要先呼叫過sortCard才能呼叫biddingChips)
    void printWinner();
    void throwCard(Card *);
};


class JuDaKo : public Character
{
friend class Game;
private:
    void swapPtr(Card *&, Card *&);
public:
    JuDaKo(const string); // constructor
    void sortCard();
    bool _findNextIdx(bool, int &);
    int biddingChips(const int, const int);
    void printWinner();
    void throwCard(Card *);
    void rez(int);
};


class Landlord : public Character
{
private:
    int landNum;
    int landPrice;
    bool isSmart;
    string location;
    bool _findNextIdx(bool, int &);
public:
    Landlord(const string); // constructor
    void sortCard();
    int biddingChips(const int, const int);
    void printWinner();
    void throwCard(Card *);
    void obtainRent();
    void sellLand();
    void payTax();
};


class AbsoluteLoser : public Character
{
private:
public:
    AbsoluteLoser(const string&);
    void sortCard();
    int biddingChips(const int, const int); // 前一人下注的籌碼(要先呼叫過sortCard才能呼叫biddingChips)
    void printWinner();
    void throwCard(Card *);
};


class Coward: public Character
{
private:
    int biddingRnd;
    bool beBrave;
    bool _findNextIdx(bool, int &);

public:
    Coward(const string); // constructor
    void sortCard();
    int biddingChips(const int, const int); 
    void printWinner();
    void throwCard(Card *);
};




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
    int cardListIdx;
    bool playerAlive;
    bool playerFold;
    bool playerOutOfGame;
    void _shuffle(int);
    void _swapPlayer(int, int);
public:
    Game();
    void addPlayer(Player &);
    void initPlayerRnd();
    void initCardList();
    void dealCard(int);
    void printPlayersCard();
    void biddingPerRound(int);
    void biddingPrint();
    void printResult();
    void printPlayerList();
    void enemySort();
    void gameStart(Player &, const int);
    void calChips();
    void decisionInput();
    void printFinalResult();
    void kickoutPlayer();
    bool endRound();
    void printShortRule();
};