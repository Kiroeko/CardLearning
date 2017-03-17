#ifndef _TRAIN_SCENE_H_
#define _TRAIN_SCENE_H_

#include "Card.h"
#include "cocos2d.h"

using namespace std;
class TrainScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuCallback(cocos2d::Ref* sender);
	void cardSMenuCallback(cocos2d::Ref* sender);
	void menuCallbacktd(cocos2d::Ref* sender);
	void DMmenuCallback(cocos2d::Ref* sender);
	void DMmenuCallback2(cocos2d::Ref* sender);
	void DMmenuCallback3(cocos2d::Ref* sender);

	void initCardGameData();

	void gameStart();

	void endThisGame(bool isWin);

	void playerDrawCard(int id);

	void AIDrawCard(int id);

	void humanTurn();

	void AITurn();

	void changHeroBlood(bool isMyHero,int attNum);

	void scheduleCallBack(float delta);
	void scheduleCallBack2(float delta);
	void scheduleCallBack3(float delta);
	void ETGscheduleCallBack(float delta);

	void useMyDama();
	void useAIDama(Card &C);

	void attBeAttacked(int attId, int blood, bool isMyAtt);
	void AllAIAttBeAttacked(int blood);
	void AllMyAttBeAttacked(int blood);

	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_ecent);

	void DMSelectEvent(int camp,int role,int scanBlood);

	std::vector<int> AISelectCard(bool canUseAttCard);

	virtual bool onTouchBeganMATT(cocos2d::Touch *touch, cocos2d::Event *unused_ecent);
	virtual void onTouchMovedMATT(cocos2d::Touch *touch, cocos2d::Event *unused_ecent);
	virtual void onTouchEndedMATT(cocos2d::Touch *touch, cocos2d::Event *unused_ecent);

	CREATE_FUNC(TrainScene);
};

#endif // _TRAIN_SCENE_H_