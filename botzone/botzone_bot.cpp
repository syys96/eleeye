#include <stdio.h>
#include "../base/base2.h"
#include "../base/parse.h"
#include "../eleeye/ucci.h"
#include "../eleeye/pregen.h"
#include "../eleeye/position.h"
#include "../eleeye/hash.h"
#include "../eleeye/search.h"
//#include "jsoncpp/json.h"
#include <string>
#include <iostream>

using std::string;
using std::cin;
using std::cout;

const int INTERRUPT_COUNT = 4096; // 搜索若干结点后调用中断

inline void PrintLn(const char *sz) {
    printf("%s\n", sz);
    fflush(stdout);
}

int main() {
    int i;
    bool bPonderTime;
    UcciCommStruct UcciComm;
    PositionStruct posProbe;

    LocatePath(Search.szBookFile, "BOOK.DAT");
    bPonderTime = false;
    PreGenInit();
    NewHash(24); // 24=16MB, 25=32MB, 26=64MB, ...
    Search.pos.FromFen(cszStartFen);
    Search.pos.nDistance = 0;
    Search.pos.PreEvaluate();
    Search.nBanMoves = 0;
    Search.bQuit = Search.bBatch = Search.bDebug = false;
    Search.bUseHash = Search.bNullMove = Search.bKnowledge = true;
    Search.bUseBook = false;
    Search.bIdle = false;
    Search.nCountMask = INTERRUPT_COUNT - 1;
    Search.nRandomMask = 0;
    Search.rc4Random.InitRand();

    string str;

//    getline(cin, str);
    str = string("{\"requests\":[{\"source\":\"-1\",\"target\":\"-1\"}],\"responses\":[]}");

    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    int turnID = input["responses"].size();
    // 第一回合收到的起点是"-1", 说明我是红方
    int currBotColor =
            input["requests"][(Json::Value::UInt) 0]["source"].asString() == "-1" ?  1 : 0;
    string curSource, curTarget;
    for (int j = 0; j < turnID; j++)
    {
        // 根据这些输入输出逐渐恢复状态到当前回合
        curSource = input["requests"][j]["source"].asString();
        curTarget = input["requests"][j]["target"].asString();
        if (curSource != "-1") {
            int sqSrc, sqDst;
            sqSrc = COORD_XY(curSource[0] - 'a' + FILE_LEFT, '9' - curSource[1] + RANK_TOP);
            sqDst = COORD_XY(curTarget[0] - 'a' + FILE_LEFT, '9' - curTarget[1] + RANK_TOP);
            if (IN_BOARD(sqSrc) && IN_BOARD(sqDst)) {
                int mv = MOVE(sqSrc, sqDst);
                if (mv != 0) {
                    if (Search.pos.LegalMove(mv) && Search.pos.MakeMove(mv) && Search.pos.LastMove().CptDrw > 0) {
                        // 始终让pos.nMoveNum反映没吃子的步数
                        Search.pos.SetIrrev();
                    } else {
                        throw std::runtime_error("input is not valid!");
                    }
                } else {
                    throw std::runtime_error("input is not valid!");
                }
            } else {
                throw std::runtime_error("input is not valid!");
            }
        }

        curSource = input["responses"][j]["source"].asString();
        curTarget = input["responses"][j]["target"].asString();
        {
            int sqSrc, sqDst;
            sqSrc = COORD_XY(curSource[0] - 'a' + FILE_LEFT, '9' - curSource[1] + RANK_TOP);
            sqDst = COORD_XY(curTarget[0] - 'a' + FILE_LEFT, '9' - curTarget[1] + RANK_TOP);
            if (IN_BOARD(sqSrc) && IN_BOARD(sqDst)) {
                int mv = MOVE(sqSrc, sqDst);
                if (mv != 0) {
                    if (Search.pos.LegalMove(mv) && Search.pos.MakeMove(mv) && Search.pos.LastMove().CptDrw > 0) {
                        // 始终让pos.nMoveNum反映没吃子的步数
                        Search.pos.SetIrrev();
                    } else {
                        throw std::runtime_error("input is not valid!");
                    }
                } else {
                    throw std::runtime_error("input is not valid!");
                }
            } else {
                throw std::runtime_error("input is not valid!");
            }
        }
    }
    curSource = input["requests"][turnID]["source"].asString();
    curTarget = input["requests"][turnID]["target"].asString();
    if (curSource != "-1") {
        int sqSrc, sqDst;
        sqSrc = COORD_XY(curSource[0] - 'a' + FILE_LEFT, '9' - curSource[1] + RANK_TOP);
        sqDst = COORD_XY(curTarget[0] - 'a' + FILE_LEFT, '9' - curTarget[1] + RANK_TOP);
        if (IN_BOARD(sqSrc) && IN_BOARD(sqDst)) {
            int mv = MOVE(sqSrc, sqDst);
            if (mv != 0) {
                if (Search.pos.LegalMove(mv) && Search.pos.MakeMove(mv) && Search.pos.LastMove().CptDrw > 0) {
                    // 始终让pos.nMoveNum反映没吃子的步数
                    Search.pos.SetIrrev();
                } else {
                    throw std::runtime_error("input is not valid!");
                }
            } else {
                throw std::runtime_error("input is not valid!");
            }
        } else {
            throw std::runtime_error("input is not valid!");
        }
    }

    Search.pos.nDistance = 0;
    Search.pos.PreEvaluate();
    Search.nBanMoves = 0;
    Search.nGoMode = GO_MODE_TIMER;
    Search.nProperTimer = 950;
    Search.nMaxTimer = 950;
    SearchMainBotzone(UCCI_MAX_DEPTH);

    DelHash();
    PrintLn("bye");
    return 0;
}

