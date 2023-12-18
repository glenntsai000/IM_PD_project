#include "IM_poker.h"
using namespace std;

#define BOLD "\033[1m"
#define NC "\e[0m"

int main()
{
    srand(time(nullptr));
    int playerNum = 3; // 預設值3
    cout << "資管盃慈善德州撲克大賽" << endl;
    cout << "請輸入玩家人數(3~10): ";
    while (true)
    {
        try
        {
            cin >> playerNum;
            if (cin.fail())
                throw invalid_argument("非數字");
            if (playerNum > 10 || playerNum < 3)
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
    cout << "請輸入玩家名稱(10個英文字母以內): ";
    string playerName;
    while (true)
    {
        try
        {
            cin >> playerName;
            if (playerName.length() > 10)
                throw invalid_argument("請輸入10個字元內的名稱");
            break;
        }
        catch (invalid_argument err)
        {
            cerr << err.what() << "，請重新輸入：";
            // 清空錯誤狀態，以避免無窮迴圈
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    Player py(playerName);
    
    py.printName();
    cout << " 歡迎加入遊戲！" << endl;
    cout << "\n";
    Game G;
    G.printShortRule();
    G.gameStart(py, playerNum);
    bool continueGame = true;

    int totalRnd = playerNum > 5 ? 5 : playerNum;

    for (int i = 1; i <= totalRnd; i++)
    {
        cout << "\n";
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
        G.biddingPerRound(1);
        G.dealCard(2);
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
