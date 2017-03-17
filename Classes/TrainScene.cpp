#include "TrainScene.h"
#include "AudioEngine.h"
#include <random>
#include <ctime>
#include <sstream>
#include <fstream>

USING_NS_CC;
using namespace cocos2d::experimental;

//Extern Records Data
extern double visibleSizeW;
extern double visibleSizeH;

extern double originX;
extern double originY;

extern int audioId;
extern bool haveSound;

//Game Global Data//////////
//我方数据//////////////////
//牌库
vector<Card> myCardLib;
//手牌
vector<Card> myHandCard;
//可用手牌
vector<Card> myEHandCard;
//场上随从
vector<Attendant> myAtt;
//可用随从
vector<Attendant> myEAtt;
//本轮水晶
int myJams;
//可用水晶
int myEJams;
//我方英雄血量
int myHeroBlood;
//我方水晶精灵
vector<Sprite*> myJamSpr;
//我方英雄血量标签
Label* myHL;
//玩家尝试抽牌失败的次数
int myDrawWrong = 0;
//玩家进行的回合数
int myTurnCount = 0;

//AI数据//////////////////
//牌库
vector<Card> AICardLib;
//手牌
vector<Card> AIHandCard;
//可用手牌
vector<Card> AIEHandCard;
//场上随从
vector<Attendant> AIAtt;
//可用随从
vector<Attendant> AIEAtt;
//本轮水晶
int AIJams;
//可用水晶
int AIEJams;
//AI英雄血量
int AIHeroBlood;
//AI水晶数组
vector<Sprite*> AIJamSpr;
//AI英雄血量标签
Label* AIHL;
//AI尝试抽牌失败的次数
int AIDrawWrong = 0;
//AI进行的回合数
int AITurnCount = 0;

//其他数据
bool isHumanTurn = false;
bool gameOver = false;

MenuItemImage *CB;
MenuItemImage *ET;
Menu *MainMenu;

//出牌选择的菜单
vector<Sprite*> ptrB;
//监听出牌
EventListenerTouchOneByOne *TouchListener,* MoveAttListener;
//被选中的牌
Card pkOne;
Sprite* bSelectOne = nullptr;
EventListenerTouchOneByOne *DMTouchListener = nullptr;

vector<Sprite*> canUseSpr;

int DMScan = 0;

vector<string> AILogs;

bool needTouchMove = false;
Attendant *chooseAtt = nullptr;
Vec2 YVec;

Scene* TrainScene::createScene()
{
	auto scene = Scene::create();
	auto layer = TrainScene::create();

	scene->addChild(layer);
	return scene;
}

bool TrainScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	//init Global Data
	initCardGameData();

	//Load Background
	auto BG = Sprite::create("Game/bg.png");
	BG->setPosition(visibleSizeW / 2, visibleSizeH / 2);
	addChild(BG);

	//Load Main Menu
	auto setButton = MenuItemImage::create("UI/backB1.png", "UI/backBP1.png", CC_CALLBACK_1(TrainScene::menuCallback, this));
	auto handCardAreaButton = MenuItemImage::create("UI/handCardAreaB.png", "UI/handCardAreaBP.png", CC_CALLBACK_1(TrainScene::menuCallback, this));
	auto CardLibButton = MenuItemImage::create("UI/cardLibB.png", "UI/cardLibBP.png","UI/cardLibBP.png", CC_CALLBACK_1(TrainScene::menuCallback, this));
	auto endTurnButton = MenuItemImage::create("UI/endTurnB.png", "UI/endTurnBP.png", "UI/endTurnD.png", CC_CALLBACK_1(TrainScene::menuCallback, this));

	CB = CardLibButton;
	ET = endTurnButton;

	setButton->setTag(0);
	setButton->setScale(0.7);
	handCardAreaButton->setTag(1);
	CardLibButton->setTag(2);
	endTurnButton->setTag(15);

	setButton->setPosition((originX + visibleSizeW - 35), (visibleSizeH - 35));
	handCardAreaButton->setPosition(originX + 1000, originX + 70);
	CardLibButton->setPosition(originX + 30, (originY + visibleSizeH) / 2);
	endTurnButton->setPosition(visibleSizeW-53,visibleSizeH/2+5);

	endTurnButton->setEnabled(false);

	auto Menu = Menu::create(setButton, handCardAreaButton, CardLibButton, endTurnButton, nullptr);
	Menu->setPosition(originX, originY);
	addChild(Menu, 10);

	MainMenu = Menu;

	//startGame
	gameStart();

	return true;
}

void TrainScene::menuCallback(Ref *sender)
{
	if (haveSound) { AudioEngine::play2d("Sounds/bi.mp3", false, 0.3); }

	auto selector = static_cast<MenuItemImage*>(sender);

	switch (selector->getTag())
	{
		//Set Button Was Pushed
	case 0:
	{
		auto dialog = Sprite::create("UI/dialog.png");
		dialog->setPosition(visibleSizeW / 2, visibleSizeH / 2);
		dialog->setTag(3);

		auto noButton = MenuItemImage::create("UI/noB.png", "UI/noBP.png", CC_CALLBACK_1(TrainScene::menuCallback, this));
		auto yesButton = MenuItemImage::create("UI/yesB.png", "UI/yesBP.png", CC_CALLBACK_1(TrainScene::menuCallback, this));

		noButton->setTag(4);
		yesButton->setTag(5);

		noButton->setPosition(visibleSizeW / 2 - 100, visibleSizeH / 2 - 50);
		yesButton->setPosition(visibleSizeW / 2 + 100, visibleSizeH / 2 - 50);

		auto Menu = Menu::create(noButton, yesButton, nullptr);
		Menu->setPosition(originX, originY);
		Menu->setTag(6);

		addChild(dialog,11);
		addChild(Menu, 11);
	}
	break;

	//Hand Card Area Was Pushed
	case 1:
	{
		if (isHumanTurn)
		{
			Card t;
			pkOne = t;

			CB->setEnabled(false);
			ET->setEnabled(false);
			isHumanTurn = false;

			for (auto &c:myHandCard)
			{
				if (c.fee <= myEJams)
				{
					myEHandCard.push_back(c);
				}
			}

			if (myEHandCard.size()<=5)
			{
				for (int i = 0; i<myEHandCard.size(); ++i)
				{
					auto bt = Sprite::create(myEHandCard[i].getCardRes().c_str());
					bt->setScale(0.75);
					bt->setPosition(180+i*220,visibleSizeH/2);
					ptrB.push_back(bt);
				}
			}
			else
			{
				for (int i = 0; i<5; ++i)
				{
					auto bt = Sprite::create(myEHandCard[i].getCardRes().c_str());
					bt->setScale(0.75);
					bt->setPosition(180 + i * 220, visibleSizeH / 4*3-20);
					ptrB.push_back(bt);
				}

				for (int i = 0; i<myEHandCard.size()-5; ++i)
				{
					auto bt = Sprite::create(myEHandCard[i + 5].getCardRes().c_str());
					bt->setScale(0.75);
					bt->setPosition(180 + i * 220, visibleSizeH / 4 +20);
					ptrB.push_back(bt);
				}
			}
			
			for (auto &s:ptrB)
			{
				addChild(s,11);
			}

			auto sureButton = MenuItemImage::create("UI/sureB.png", "UI/sureBP.png", CC_CALLBACK_1(TrainScene::cardSMenuCallback, this));
			sureButton->setTag(0);
			sureButton->setPosition(visibleSizeW-70,originY+70);

			auto dMenu = Menu::create(sureButton,nullptr);
			dMenu->setPosition(originX, originY);
			dMenu->setTag(101);
			addChild(dMenu,10);

			TouchListener = EventListenerTouchOneByOne::create();
			TouchListener->onTouchBegan = CC_CALLBACK_2(TrainScene::onTouchBegan, this);

			Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(TouchListener, 1);
		}
	}
	break;

	//Card Lib Area Was Pushed
	case 2:
	{
		auto dl = Sprite::create("Game/dl.png");
		dl->setPosition(originX + 185, visibleSizeH / 2);
		dl->setTag(100);

		TTFConfig ttfConfig("Fonts/Dengb.ttf", 26);
		auto l1 = Label::createWithTTF(ttfConfig, u8"AI剩余卡牌数：", TextHAlignment::CENTER);
		l1->setColor(Color3B(0, 0, 0));
		l1->setTag(101);
		auto l2 = Label::createWithTTF(ttfConfig, u8"玩家剩余卡牌数：", TextHAlignment::CENTER);
		l2->setColor(Color3B(0, 0, 0));
		l2->setTag(102);

		TTFConfig ttfConfig2("Fonts/ARLRDBD.ttf", 60);
		string s; stringstream ss; ss << AICardLib.size(); ss >> s; ss.clear();
		auto l11 = Label::createWithTTF(ttfConfig2, s.c_str(), TextHAlignment::CENTER);
		l11->setTag(103);
		if (AICardLib.size()>=10)
		{
			l11->setColor(Color3B(0, 0xFF, 0));
		}
		else
		{
			l11->setColor(Color3B(0xFF, 0, 0));
		}
		s.clear(); ss << myCardLib.size(); ss >> s; ss.clear();
		auto l22 = Label::createWithTTF(ttfConfig2, s.c_str(), TextHAlignment::CENTER);
		l22->setTag(104);
		if (myCardLib.size() >= 10)
		{
			l22->setColor(Color3B(0, 0xFF, 0));
		}
		else
		{
			l22->setColor(Color3B(0xFF, 0, 0));
		}

		l1->setPosition(originX + 200, visibleSizeH / 2 + 240);
		l11->setPosition(originX + 200, visibleSizeH / 2 + 200);
		l2->setPosition(originX + 220, visibleSizeH / 2 - 60);
		l22->setPosition(originX + 200, visibleSizeH / 2 - 100);

		addChild(dl);
		addChild(l1);
		addChild(l11);
		addChild(l2);
		addChild(l22);

		CB->setEnabled(false);

		scheduleOnce(schedule_selector(TrainScene::scheduleCallBack2), 1.5);
	}
	break;

	//No Was Pushed
	case 4:
	{
		removeChildByTag(3);
		removeChildByTag(6);
	}
	break;

	//Yes Was Pushed
	case 5:
	{
		endThisGame(false);
	}
	break;

	case 15:
	{
		ET->setEnabled(false);
		isHumanTurn = false;
		AITurn();
	}
	break;
	}
}

void TrainScene::cardSMenuCallback(cocos2d::Ref * sender)
{
	if (haveSound) { AudioEngine::play2d("Sounds/bi.mp3", false, 0.3); }

	auto selector = static_cast<MenuItemImage*>(sender);

	switch (selector->getTag())
	{
	case 0:
	{
		if (bSelectOne != nullptr) { removeChild(bSelectOne); }bSelectOne = nullptr;

		Director::getInstance()->getEventDispatcher()->removeEventListener(TouchListener);

		ET->setEnabled(true);
		CB->setEnabled(true);
		myEHandCard.clear();
		isHumanTurn = true;

		removeChildByTag(101);
		for (auto &s : ptrB)
		{
			removeChild(s);
		}
		ptrB.clear();

		if ((myAtt.size() < 8&& pkOne.number != 0)||((!pkOne.cardType)&& pkOne.number != 0))
		{
			myEJams -= pkOne.fee;

			for (auto iter = myHandCard.begin(); iter != myHandCard.end(); ++iter)
			{
				bool needMove = true;
				if (iter->number == pkOne.number)
				{
					removeChild(iter->cardSprite);
					needMove = false;
					myHandCard.erase(iter);
					break;
				}
				else
				{
					if (needMove)
					{
						Vec2 v2 = iter->cardSprite->getPosition();
						iter->cardSprite->setPosition(v2.x + 47, v2.y);
					}
				}
			}
			useMyDama();
		}

		for (int i = myEJams; i<myJams; ++i)
		{
			myJamSpr[i]->setOpacity(110);
		}
	}
	break;
	}
}

void TrainScene::menuCallbacktd(cocos2d::Ref * sender)
{
	MainMenu->setEnabled(true);

	if (haveSound) { AudioEngine::play2d("Sounds/bi.mp3", false, 0.3); }

	auto selector = static_cast<MenuItemImage*>(sender);

	switch (selector->getTag())
	{
	case 1:
	{
		removeChildByTag(55);
		removeChildByTag(56);

		string inTrainData("1 ");

		//1:敌方场攻 2:敌方随从总防御力 3:敌方随从数量 4:敌方血量 
		//5:玩家场攻 6:玩家随从总防御力 7:玩家随从数量 8:玩家血量 
		int td[8];
		td[0] = 0, td[1] = 0, td[2] = AIAtt.size(), td[3] = AIHeroBlood, td[4] = 0, td[5] = 0, td[6] = myAtt.size(), td[7] = myHeroBlood;
		for (auto &c : AIAtt)
		{
			td[1] += c.attackPower;
			td[2] += c.nowDefensePower;
		}

		for (auto &c : myAtt)
		{
			td[4] += c.attackPower;
			td[5] += c.nowDefensePower;
		}

		for (int i = 0; i < 8; ++i)
		{
			string w; stringstream ss;
			ss << (i + 1); ss >> w; ss.clear(); w += ":"; inTrainData += w; w.clear();
			ss << td[i]; ss >> w; ss.clear(); w += " "; inTrainData += w; w.clear();
		}

		inTrainData += "\n";
		ofstream of("traindata.txt", ofstream::app);
		of << inTrainData;
		of.close();
	}
	break;

	case 2:
	{
		removeChildByTag(55);
		removeChildByTag(56);

		string inTrainData("2 ");

		//1:敌方场攻 2:敌方随从总防御力 3:敌方随从数量 4:敌方血量 
		//5:玩家场攻 6:玩家随从总防御力 7:玩家随从数量 8:玩家血量 
		int td[8];
		td[0] = 0, td[1] = 0, td[2] = AIAtt.size(), td[3] = AIHeroBlood, td[4] = 0, td[5] = 0, td[6] = myAtt.size(), td[7] = myHeroBlood;
		for (auto &c : AIAtt)
		{
			td[1] += c.attackPower;
			td[2] += c.nowDefensePower;
		}

		for (auto &c : myAtt)
		{
			td[4] += c.attackPower;
			td[5] += c.nowDefensePower;
		}

		for (int i = 0; i < 8; ++i)
		{
			string w; stringstream ss;
			ss << (i + 1); ss >> w; ss.clear(); w += ":"; inTrainData += w; w.clear();
			ss << td[i]; ss >> w; ss.clear(); w += " "; inTrainData += w; w.clear();
		}

		inTrainData += "\n";
		ofstream of("traindata.txt", ofstream::app);
		of << inTrainData;
		of.close();
	}
	break;
	}
}

void TrainScene::DMmenuCallback(Ref * sender)
{
	MainMenu->setEnabled(true);

	if (haveSound) { AudioEngine::play2d("Sounds/bi.mp3", false, 0.3); }

	auto selector = static_cast<MenuItemImage*>(sender);

	switch (selector->getTag())
	{
	case 176:
	{
		changHeroBlood(true, DMScan);
	}
	break;

	case 177:
	{
		changHeroBlood(false, DMScan);
	}
	break;
	}

	removeChildByTag(107);

	DMScan = 0;
}

void TrainScene::DMmenuCallback2(Ref *sender)
{
	MainMenu->setEnabled(true);

	if (haveSound) { AudioEngine::play2d("Sounds/bi.mp3", false, 0.3); }

	auto selector = static_cast<MenuItemImage*>(sender);

	int cases = selector->getTag();

	attBeAttacked(AIAtt[cases].number, DMScan, false);

	removeChildByTag(107);
	DMScan = 0;
}

void TrainScene::DMmenuCallback3(Ref *sender)
{
	MainMenu->setEnabled(true);

	if (haveSound) { AudioEngine::play2d("Sounds/bi.mp3", false, 0.3); }

	auto selector = static_cast<MenuItemImage*>(sender);

	int cases = selector->getTag();

	attBeAttacked(myAtt[cases].number, DMScan, true);

	removeChildByTag(107);
	DMScan = 0;
}

void TrainScene::initCardGameData()
{
	//init Global Data first
	vector<string> s1;
	vector<bool> t1 = { false,false,false,false,false };
	Card c1(1,1,true,1,2,s1,t1);
	myCardLib.push_back(c1);

	vector<string> s2 = { "A1331" };
	vector<bool> t2 = { true,false,false,false,true };
	Card c2(2, 1, true, 1, 2, s2, t2);
	myCardLib.push_back(c2);

	vector<string> s3 = { "B0000" };
	vector<bool> t3 = { false,false,false,true,false };
	Card c3(3, 1, true, 2, 1, s3, t3);
	myCardLib.push_back(c3);

	vector<string> s4 = { "C0000" };
	vector<bool> t4 = { true,false,false,false,true };
	Card c4(4, 2, true, 2, 1, s4, t4);
	myCardLib.push_back(c4);

	vector<string> s5 = { "A1332" };
	vector<bool> t5 = { true,false,false,false,true };
	Card c5(5, 2, false, 0, 0, s5, t5);
	myCardLib.push_back(c5);

	vector<string> s6;
	vector<bool> t6 = { false,false,false,false,false };
	Card c6(6, 2, true, 2, 3, s6, t6);
	myCardLib.push_back(c6);

	vector<string> s7 = {"D1312"};
	vector<bool> t7 = { false,true,false,false,false };
	Card c7(7, 3, true, 3, 3, s7, t7);
	myCardLib.push_back(c7);

	vector<string> s8 = { "F0002" };
	vector<bool> t8 = { false,false,true,false,false };
	Card c8(8, 3, false, 0, 0, s8, t8);
	myCardLib.push_back(c8);

	vector<string> s9 = { "A1313" };
	vector<bool> t9 = { true,false,false,false,false };
	Card c9(9, 3, true, 1, 3, s9, t9);
	myCardLib.push_back(c9);

	vector<string> s10;
	vector<bool> t10 = { false,false,false,false,false };
	Card c10(10, 4, true, 2, 4, s10, t10);
	myCardLib.push_back(c10);

	vector<string> s11 = { "A2132" };
	vector<bool> t11 = { true,false,true,false,true };
	Card c11(11, 4, false, 0, 0, s11, t11);
	myCardLib.push_back(c11);

	vector<string> s12 = { "B0000" };
	vector<bool> t12 = { false,false,false,true,false };
	Card c12(12, 4, true, 3, 3, s12, t12);
	myCardLib.push_back(c12);

	vector<string> s13;
	vector<bool> t13 = { false,false,false,false,false };
	Card c13(13, 5, true, 5, 5, s13, t13);
	myCardLib.push_back(c13);

	vector<string> s14 = { "A2123" };
	vector<bool> t14 = { false,false,false,false,true };
	Card c14(14, 5, true, 4, 4, s14, t14);
	myCardLib.push_back(c14);

	vector<string> s15 = { "A2312" };
	vector<bool> t15 = { true,false,false,false,false };
	Card c15(15, 5, true, 3, 3, s15, t15);
	myCardLib.push_back(c15);

	vector<string> s16 = { "E1111" };
	vector<bool> t16 = { true,false,false,false,false };
	Card c16(16, 6, true, 3, 2, s16, t16);
	myCardLib.push_back(c16);

	vector<string> s17 = { "D2223" };
	vector<bool> t17 = { false,true,false,false,false };
	Card c17(17, 6, true, 4, 6, s17, t17);
	myCardLib.push_back(c17);

	vector<string> s18 = { "B0000" };
	vector<bool> t18 = { false,false,false,true,false };
	Card c18(18, 6, true, 3, 8, s18, t18);
	myCardLib.push_back(c18);

	vector<string> s19;
	vector<bool> t19 = { false,false,false,false,false };
	Card c19(19, 7, true, 7, 7, s19, t19);
	myCardLib.push_back(c19);

	vector<string> s20 = { "A2114" };
	vector<bool> t20 = { true,false,false,false,false };
	Card c20(20, 7, false, 0, 0, s20, t20);
	myCardLib.push_back(c20);

	vector<string> s21 = { "C0000" };
	vector<bool> t21 = { true,false,false,false,true };
	Card c21(21, 7, true, 6, 2, s21, t21);
	myCardLib.push_back(c21);

	vector<string> s22 = { "B0000" };
	vector<bool> t22 = { false,false,false,true,false };
	Card c22(22, 8, true, 8, 8, s22, t22);
	myCardLib.push_back(c22);

	vector<string> s23 = { "A1318" };
	vector<bool> t23 = { true,false,false,false,true };
	Card c23(23, 8, false, 0, 0, s23, t23);
	myCardLib.push_back(c23);

	vector<string> s24;
	vector<bool> t24 = { false,false,false,false,false };
	Card c24(24, 8, true, 10, 5, s24, t24);
	myCardLib.push_back(c24);

	vector<string> s25 = {"D1338","F0002"};
	vector<bool> t25 = { false,true,true,false,false };
	Card c25(25, 9, false, 0, 0, s25, t25);
	myCardLib.push_back(c25);

	vector<string> s26 = { "A2125" };
	vector<bool> t26 = { false,false,false,false,true };
	Card c26(26, 9, true, 7, 6, s26, t26);
	myCardLib.push_back(c26);

	vector<string> s27 = { "B0000" };
	vector<bool> t27 = { false,false,false,true,false };
	Card c27(27, 9, true, 4, 12, s27, t27);
	myCardLib.push_back(c27);

	vector<string> s28 = { "G2311" };
	vector<bool> t28 = { true,false,false,false,false };
	Card c28(28, 10, true, 3, 7, s28, t28);
	myCardLib.push_back(c28);

	vector<string> s29 = { "E2310" };
	vector<bool> t29 = { true,false,false,false,false };
	Card c29(29, 10, false, 0, 0, s29, t29);
	myCardLib.push_back(c29);
	
	vector<string> s30;
	vector<bool> t30 = { false,false,false,false,false };
	Card c30(30, 10, true, 10, 10, s30, t30);
	myCardLib.push_back(c30);

	//初始化AILogs;
	AILogs.push_back(u8"AI使用了 随从卡 大名");
	AILogs.push_back(u8"AI使用了 随从卡 圣战士，对指定随从造成了1点伤害");
	AILogs.push_back(u8"AI使用了 随从卡 吃瓜姐妹");
	AILogs.push_back(u8"AI使用了 随从卡 人鱼公主");
	AILogs.push_back(u8"AI使用了 法术卡 幻界，对指定角色造成了2点伤害");

	AILogs.push_back(u8"AI使用了 随从卡 C.VA");
	AILogs.push_back(u8"AI使用了 随从卡 爱的战士，对指定随从回复了2点生命");
	AILogs.push_back(u8"AI使用了 法术卡 初夏，抽了3张牌");
	AILogs.push_back(u8"AI使用了 随从卡 明星球员 ，对指定随从造成了3点伤害");
	AILogs.push_back(u8"AI使用了 随从卡 高中生漫画家");

	AILogs.push_back(u8"AI使用了 法术卡 衰老之地，对我方角色造成了2点伤害");
	AILogs.push_back(u8"AI使用了 随从卡 瓦尔基里");
	AILogs.push_back(u8"AI使用了 随从卡 校园偶像");
	AILogs.push_back(u8"AI使用了 随从卡 夜刀刺客，对指定英雄造成了3点伤害");
	AILogs.push_back(u8"AI使用了 随从卡 中庭之蛇，对所有随从造成了2点伤害");

	AILogs.push_back(u8"AI使用了 随从卡 钉宫香菜，消灭了一个指定随从");
	AILogs.push_back(u8"AI使用了 随从卡 不死鸟，回复了3点生命值");
	AILogs.push_back(u8"AI使用了 随从卡 原初守护者");
	AILogs.push_back(u8"AI使用了 随从卡 大剑士");
	AILogs.push_back(u8"AI使用了 法术卡 镜止世界，对我方随从造成了4点伤害");

	AILogs.push_back(u8"AI使用了 随从卡 云中仙人");
	AILogs.push_back(u8"AI使用了 随从卡 未来少女");
	AILogs.push_back(u8"AI使用了 法术卡 圆环之理，对指定角色造成了8点伤害");
	AILogs.push_back(u8"AI使用了 随从卡 少女杀手");
	AILogs.push_back(u8"AI使用了 法术卡 享乐园，对指定角色回复了8点生命值，抽了3张牌");

	AILogs.push_back(u8"AI使用了 随从卡 冰龙，对我方英雄造成了5点伤害");
	AILogs.push_back(u8"AI使用了 随从卡 皇家骑士");
	AILogs.push_back(u8"AI使用了 随从卡 爱神芙蕾雅，把所有随从攻击力变为了1");
	AILogs.push_back(u8"AI使用了 法术卡 诸神的黄昏，移除了所有随从");
	AILogs.push_back(u8"AI使用了 随从卡 灭世之龙");

	AICardLib = myCardLib;

	myHandCard.clear();AIHandCard.clear();
	myEHandCard.clear();AIEHandCard.clear();
	myAtt.clear();AIAtt.clear();
	myEAtt.clear();AIAtt.clear();
	myJams = 0;AIJams = 0;
	myEJams = 0;AIEJams = 0;
	myHeroBlood = 30;AIHeroBlood = 30;
	myJamSpr.clear(); AIJamSpr.clear();
	myHL = nullptr; AIHL = nullptr;
	myDrawWrong = 0; AIDrawWrong = 0;
	myTurnCount = 0; AITurnCount = 0;
    isHumanTurn = false;
	CB = nullptr;
	ET = nullptr;
	gameOver = false;
	ptrB.clear();
}

void TrainScene::gameStart()
{
	//区分先后手
	uniform_int_distribution<int> uni(0, 1);
	uniform_int_distribution<int> uni20(1, 20);
	default_random_engine e(time(0));

	int i = uni(e);

	bool b = false;
	if (i)
	{
		int beforeCid = 0;
		for (int i=0;i<3;++i)
		{
			while (1)
			{
				bool right = false;
				int cid = uni20(e);
				for (const auto &c : myCardLib)
				{
					if (cid == c.number&&cid!=beforeCid)
					{
						beforeCid = cid;
						right = true;break;
					}
				}

				if (right) { playerDrawCard(cid); break; }
				else { uni20(e); }
			}
		}
		
		beforeCid = 0;
		for (int i = 0; i<4; ++i)
		{
			while (1)
			{
				bool right = false;
				int cid = uni20(e);
				for (const auto &c : AICardLib)
				{
					if (cid == c.number&&cid != beforeCid)
					{
						beforeCid = cid;
						right = true; break;
					}
				}
				if (right) { AIDrawCard(cid); break; }
				else { uni20(e); }
			}
		}

		b = true;
		auto tips = Sprite::create("UI/firstHand.png");
		tips->setPosition(visibleSizeW / 2, visibleSizeH / 2);
		tips->setTag(7);
		addChild(tips,9);
	}
	else
	{
		int beforeCid = 0;
		for (int i = 0; i<3; ++i)
		{
			while (1)
			{
				bool right = false;
				int cid = uni20(e);
				for (const auto &c : AICardLib)
				{
					if (cid == c.number&&cid != beforeCid)
					{
						beforeCid = cid;
						right = true; break;
					}
				}
				if (right) { AIDrawCard(cid); break; }
				else { uni20(e); }
			}
		}
		beforeCid = 0;
		for (int i = 0; i<4; ++i)
		{
			while (1)
			{
				bool right = false;
				int cid = uni20(e);
				for (const auto &c : myCardLib)
				{
					if (cid == c.number&&cid != beforeCid)
					{
						beforeCid = cid;
						right = true; break;
					}
				}
				if (right) { playerDrawCard(cid); break; }
				else { uni20(e); }
			}
		}

		b = false;
		auto tips = Sprite::create("UI/secondHand.png");
		tips->setPosition(visibleSizeW / 2, visibleSizeH / 2);
		tips->setTag(7);
		addChild(tips,9);
	}
	scheduleOnce(schedule_selector(TrainScene::scheduleCallBack), 3.5);
	isHumanTurn = b;

	for (int i=0;i<10;++i)
	{
		auto jam = Sprite::create("Game/jam.png");
		jam->setPosition(originX + 85 + 50 * i,originY + 32);
		addChild(jam);
		myJamSpr.push_back(jam);
	}

	for (int i = 0; i<10; ++i)
	{
		auto jam = Sprite::create("Game/jam.png");
		jam->setPosition(originX + 85 + 50 * i, visibleSizeH - 32);
		addChild(jam);
		AIJamSpr.push_back(jam);
	}

	TTFConfig ttfConfig("Fonts/ARLRDBD.ttf", 110);
	auto Label1 = Label::createWithTTF(ttfConfig, "30", TextHAlignment::CENTER);
	auto Label2 = Label::createWithTTF(ttfConfig, "30", TextHAlignment::CENTER);
	Label1->setPosition(visibleSizeW / 2,originY + 50);
	Label2->setPosition(visibleSizeW / 2, visibleSizeH - 50);
	Label1->setColor(Color3B(0,0xFF,0));
	Label2->setColor(Color3B(0, 0xFF, 0));
	myHL = Label1; AIHL = Label2;
	addChild(Label1);
	addChild(Label2);

	for (const auto &s : myJamSpr)
	{
		s->setOpacity(0);
	}myJamSpr[0]->setOpacity(255);
	for (const auto &s : AIJamSpr)
	{
		s->setOpacity(0);
	}AIJamSpr[0]->setOpacity(255);

	//进行游戏主循环
	if (isHumanTurn)
	{
		humanTurn();
	}
	else
	{
		AITurn();
	}
}

void TrainScene::playerDrawCard(int id)
{
	if (haveSound) { AudioEngine::play2d("Sounds/drawCard.mp3", false, 1.0); }

	Card Xtemp;
	for (auto iter = myCardLib.begin();iter!=myCardLib.end();++iter)
	{
		if (iter->number==id)
		{
			Xtemp = *iter;
			myCardLib.erase(iter);
			break;
		}
	}

	if (myHandCard.size() == 10)
	{
		if (haveSound) { AudioEngine::play2d("Sounds/fullHandCard.mp3", false, 0.4); }
		return;
	}

	Xtemp.cardSprite = Sprite::create(Xtemp.getCardRes().c_str());

	if (Xtemp.cardSprite!=nullptr)
	{
		Xtemp.cardSprite->setPosition(originX + 200, visibleSizeH / 2);
		addChild(Xtemp.cardSprite, 8);

		myHandCard.insert(myHandCard.begin(), Xtemp);

		int targetX = visibleSizeW - (myHandCard.size() * 48);
		int targetY = originY + 70;

		auto acmov = MoveTo::create(1.0, Point(targetX, targetY));
		auto acsca = ScaleTo::create(1.0, 0.333);

		auto act = Spawn::create(acmov, acsca, nullptr);
		auto dt = EaseIn::create(act, 3.0);

		Xtemp.cardSprite->runAction(dt);
	}
}

void TrainScene::AIDrawCard(int id)
{
	if (haveSound) { AudioEngine::play2d("Sounds/drawCard.mp3", false, 1.0); }

	Card Xtemp;
	for (auto iter = AICardLib.begin(); iter != AICardLib.end(); ++iter)
	{
		if (iter->number == id)
		{
			Xtemp = *iter;
			AICardLib.erase(iter);
			break;
		}
	}

	if (AIHandCard.size() == 10)
	{
		return;
	}

	Xtemp.cardSprite = Sprite::create("Game/cardBack.png");
	Xtemp.cardSprite->setPosition(originX + 200, visibleSizeH / 2);
	addChild(Xtemp.cardSprite,8);

	AIHandCard.insert(AIHandCard.begin(), Xtemp);

	int targetX = visibleSizeW - (AIHandCard.size() * 48);
	int targetY = visibleSizeH - 70;

	auto acmov = MoveTo::create(1.0, Point(targetX, targetY));
	Xtemp.cardSprite->runAction(acmov);
}

void TrainScene::humanTurn()
{
	for (auto &a:myAtt)
	{
		if (!a.canUse)
		{
			a.canUse = true;
		}
	}

	++myTurnCount;
	if (myJams < 10)
	{
		++myJams; myEJams = myJams;
	}
	else
	{
		myJams = myEJams = 10;
	}

	for (const auto &s : myJamSpr)
	{
		s->setOpacity(0);
	}
	for (int i = 0; i<myJams; ++i)
	{
		if (myJamSpr.size()!=0)
		{
			myJamSpr[i]->setOpacity(255);
		}
	}

	//抽牌
	if (myCardLib.size()==0)
	{
		if (CB!=nullptr)
		{
			CB->setVisible(false);

			++myDrawWrong;
			changHeroBlood(true, myDrawWrong);
		}
	}
	else
	{
		if (myTurnCount<10)
		{
			int cid = 0;

			bool haveFeePointCard = false;
			for (auto &S : myCardLib)
			{
				if (S.fee == myTurnCount)
				{
					haveFeePointCard = true;
					break;
				}
			}

			if (haveFeePointCard)
			{
				int minNum = 1 + (myTurnCount - 1) * 3;
				int maxNum = 3 + (myTurnCount - 1) * 3;

				uniform_int_distribution<int> uniMM(minNum, maxNum);
				default_random_engine e(time(0));
				cid = uniMM(e);

				while (1)
				{
					bool illegal = false;
					for (const auto &c : myCardLib)
					{
						if (c.number == cid)
						{
							illegal = true; break;
						}
					}
					if (illegal)
					{
						break;
					}
					else
					{
						cid = uniMM(e);
					}
				}
			}
			else
			{
				if (myTurnCount == 10)
				{
					cid = myCardLib[0].number;
				}
				else
				{
					for (auto &S : myCardLib)
					{
						if (S.fee > myTurnCount)
						{
							break;
						}
					}
				}
			}

			playerDrawCard(cid);
		}
		else
		{
			uniform_int_distribution<int> uni30(1, 30);
			default_random_engine e(time(0));
			int cid = uni30(e);

			while (1)
			{
				bool illegal = false;
				for (const auto &c : myCardLib)
				{
					if (c.number == cid)
					{
						illegal = true; break;
					}
				}
				if (illegal)
				{
					break;
				}
				else
				{
					cid = uni30(e);
				}
			}
			playerDrawCard(cid);
		}
	}

	if (!gameOver)
	{
		MainMenu->setEnabled(false);

		auto dialog = Sprite::create("UI/cl.png");
		dialog->setPosition(visibleSizeW / 2, visibleSizeH / 2);
		dialog->setTag(55);

		auto AButton = MenuItemImage::create("UI/jcB.png", "UI/jcBP.png", CC_CALLBACK_1(TrainScene::menuCallbacktd, this));
		auto BButton = MenuItemImage::create("UI/zcB.png", "UI/zcBP.png", CC_CALLBACK_1(TrainScene::menuCallbacktd, this));

		AButton->setTag(1);
		BButton->setTag(2);

		AButton->setPosition(visibleSizeW / 2 - 300, originX + 280);
		BButton->setPosition(visibleSizeW / 2 + 300, originX + 280);

		auto Menu = Menu::create(AButton, BButton, nullptr);
		Menu->setPosition(originX, originY);
		Menu->setTag(56);

		addChild(dialog, 50);
		addChild(Menu, 51);

		ET->setEnabled(true);

		//标记自己的可用随从为高亮。
		for (auto &S:myAtt)
		{
			if (S.canUse)
			{
				Vec2 v = S.attSprite->getPosition();
				auto spr = Sprite::create("Game/attCanUsed.png");
				spr->setPosition(v);
				addChild(spr);

				canUseSpr.push_back(spr);
			}
		}

		//操控随从战斗
		MoveAttListener = EventListenerTouchOneByOne::create();
		MoveAttListener->onTouchBegan = CC_CALLBACK_2(TrainScene::onTouchBeganMATT, this);
		MoveAttListener->onTouchMoved = CC_CALLBACK_2(TrainScene::onTouchMovedMATT, this);
		MoveAttListener->onTouchEnded = CC_CALLBACK_2(TrainScene::onTouchEndedMATT, this);

		Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(MoveAttListener, 1);
	}
}

void TrainScene::AITurn()
{
	Director::getInstance()->getEventDispatcher()->removeEventListener(MoveAttListener);
	MoveAttListener = nullptr;

	for (auto &S:canUseSpr)
	{
		removeChild(S);
	}
	canUseSpr.clear();

	for (auto &a : AIAtt)
	{
		if (!a.canUse)
		{
			a.canUse = true;
		}
	}

	++AITurnCount;
	if (AIJams < 10)
	{
		++AIJams; AIEJams = AIJams;
	}
	else
	{
		AIJams = AIEJams = 10;
	}

	for (auto &s:AIJamSpr)
	{
		s->setOpacity(0);
	}

	for (int i=0;i<AIJams;++i)
	{
		if (AIJamSpr.size()!=0)
		{
			AIJamSpr[i]->setOpacity(255);
		}
	}

	//抽牌
	if (AICardLib.size() <= 0)
	{
		auto spr = Sprite::create("Game/heroBA.png");
		spr->setTag(7);
		spr->setPosition(visibleSizeW / 2, visibleSizeH -  70);
		addChild(spr);

		++AIDrawWrong;
		changHeroBlood(false, AIDrawWrong);

		scheduleOnce(schedule_selector(TrainScene::scheduleCallBack), 1.5);
	}
	else
	{
		if (AITurnCount<10)
		{
			int cid = 0;
			bool haveFeePointCard = false;
			for (auto &S:AICardLib)
			{
				if (S.fee==AITurnCount)
				{
					haveFeePointCard = true;
					break;
				}
			}

			if (haveFeePointCard)
			{
				int minNum = 1 + (AITurnCount - 1) * 3;
				int maxNum = 3 + (AITurnCount - 1) * 3;

				uniform_int_distribution<int> uniMM(minNum, maxNum);
				default_random_engine e(time(0));
				cid = uniMM(e);

				while (1)
				{
					bool illegal = false;
					for (const auto &c : AICardLib)
					{
						if (c.number == cid)
						{
							illegal = true; break;
						}
					}
					if (illegal)
					{
						break;
					}
					else
					{
						cid = uniMM(e);
					}
				}
			}
			else
			{
				if (AITurnCount==10)
				{
					cid = AICardLib[0].number;
				}
				else
				{
					for (auto &S : AICardLib)
					{
						if (S.fee > AITurnCount)
						{
							break;
						}
					}
				}
			}

			AIDrawCard(cid);
		}
		else
		{
			uniform_int_distribution<int> uni30(1, 30);
			default_random_engine e(time(0));
			int cid = uni30(e);

			while (1)
			{
				bool illegal = false;
				for (const auto &c : AICardLib)
				{
					if (c.number == cid)
					{
						illegal = true; break;
					}
				}
				if (illegal)
				{
					break;
				}
				else
				{
					cid = uni30(e);
				}
			}
			AIDrawCard(cid);
		}
	}

	if (!gameOver)
	{
		bool cuac = true;
		if (AIAtt.size() >= 8) { cuac = false; }

		//选卡出牌
		Card temp;
		vector<int> results=AISelectCard(cuac);

		int feeSum = 0;
		for (auto &s:results)
		{
			if (s!=0)
			{
				for (auto iter = AIHandCard.begin(); iter != AIHandCard.end(); ++iter)
				{
					bool needMove = true;
					if (iter->number == s)
					{
						feeSum += iter->fee;

						removeChild(iter->cardSprite);
						needMove = false;

						temp = *iter;
						AIHandCard.erase(iter);
						break;
					}
					else
					{
						if (needMove)
						{
							Vec2 v2 = iter->cardSprite->getPosition();
							iter->cardSprite->setPosition(v2.x + 47, v2.y);
						}
					}
				}
				useAIDama(temp);

				if (feeSum>0)
				{
					for (int i = AIJams - feeSum; i < AIJams; ++i)
					{
						AIJamSpr[i]->setOpacity(110);
					}
				}
			}
		}

		if (!gameOver)
		{
			bool AICanUseAtt = false;
			for (auto &S:AIAtt)
			{
				if (S.canUse)
				{
					AICanUseAtt = true;
					break;
				}
			}

			if (AICanUseAtt)
			{
				//使用场攻
				//统计嘲讽随从
				vector<int> ChaoFengAtt;
				bool haveChaoFeng = false;
				int i = 0;
				for (auto &S : myAtt)
				{
					if (S.isShield)
					{
						ChaoFengAtt.push_back(S.number);
						haveChaoFeng = true;
					}
					++i;
				}

				bool finishAttack = false;

				//计算斩杀
				//没有嘲讽的情况下尝试打脸
				if (!haveChaoFeng)
				{
					int temp = 0;
					for (auto &S : AIAtt)
					{
						if (S.canUse)
						{
							temp += S.attackPower;
						}
					}

					if (temp >= myHeroBlood)
					{
						TTFConfig AILogTtfConfig("Fonts/Dengb.ttf", 32);
						auto logLabel = Label::createWithTTF(AILogTtfConfig, u8"敌人的全部随从对我方英雄造成伤害", TextHAlignment::CENTER, visibleSizeW - 200);
						logLabel->setPosition(Vec2((originX + visibleSizeW) / 2, (originY + visibleSizeH) / 2 - 40));
						logLabel->setTag(90);
						logLabel->setColor(Color3B(105, 89, 205));

						addChild(logLabel);
						scheduleOnce(schedule_selector(TrainScene::scheduleCallBack3), 2.5);

						finishAttack = true;
						changHeroBlood(true, temp);
					}
				}
				//处理有嘲讽的情况
				else
				{
					finishAttack = true;

					int targetAttack = 0;
					int targetKB = 0;
					int targetNumber = 0;

					for (int i = 0; i<ChaoFengAtt.size(); ++i)
					{
						for (auto &S:myAtt)
						{
							if (S.number == ChaoFengAtt[i])
							{
								targetAttack = S.nowDefensePower;
								targetKB = S.attackPower;
								targetNumber = S.number;
							}
						}

						//尝试解决嘲讽随从
						int maxP = 100;
						//AI选择攻击的随从编号
						vector<int> numberOfAIChooseAtt(9,0);

						int m = AIAtt.size();
						for (int i = 0; i<m; ++i)
						{
							for (int j = i, k = m; j < k; --k)
							{
								int tempAttack = 0;
								vector<int> tempNOACA;
								for (int w = j; w < k; ++w)
								{
									if (AIAtt[w].canUse)
									{
										tempAttack += AIAtt[w].attackPower;
										tempNOACA.push_back(AIAtt[w].number);
									}
								}

								if (tempAttack>=targetAttack&&(tempAttack - targetAttack)<maxP&&tempNOACA.size()<numberOfAIChooseAtt.size())
								{
									maxP = tempAttack - targetAttack; 
									numberOfAIChooseAtt = tempNOACA;
								}
							}
						}

						bool haveChoose = true;
						for (auto &S:numberOfAIChooseAtt)
						{
							if (S==0)
							{
								haveChoose = false;
								break;
							}
						}

						if ((maxP==100||!haveChoose)&&(i == ChaoFengAtt.size()-1))
						{
							if (AIAtt.size()>=4)
							{
								AIAtt[0].canUse = false;
								AIAtt[1].canUse = false;
								int aia0 = AIAtt[0].number, aia1 = AIAtt[1].number;
								attBeAttacked(aia0, targetKB, false);
								attBeAttacked(aia1, targetKB, false);

								for (int i = 0; i<2; ++i)
								{
									attBeAttacked(targetNumber, AIAtt[i].attackPower, true);
								}
							}
						}
						else if(haveChoose)
						{
							attBeAttacked(targetNumber, 100, true);
							for (int l=0;l<numberOfAIChooseAtt.size();++l)
							{
								for (auto &S:AIAtt)
								{
									if (S.number== numberOfAIChooseAtt[l])
									{
										S.canUse = false; break;
									}
								}
								attBeAttacked(numberOfAIChooseAtt[l], targetKB, false);
							}
						}
					}

					//尝试解决其他的随从，如果有剩余的话
					int AllDefender = 0;
					for (auto &S:AIAtt)
					{
						AllDefender += S.nowDefensePower;
					}

					int myKB = 0;
					for (auto &S : myAtt)
					{
						myKB += S.attackPower;
					}

					if (AllDefender>=myKB||(AIAtt.size()-2>=myAtt.size()))
					{
						haveChaoFeng = false;
						for (auto &S : myAtt)
						{
							if (S.isShield)
							{
								haveChaoFeng = true; break;
							}
						}

						if (!haveChaoFeng)
						{
							int kbMyHero = 0;
							for (auto &S : AIAtt)
							{
								if (S.canUse)
								{
									kbMyHero += S.attackPower;
								}
							}
							changHeroBlood(true, kbMyHero);
						}						
					}
					else
					{
						for (int i = 0; i<myAtt.size(); ++i)
						{
							int targetAttack = myAtt[i].nowDefensePower;
							int targetKB = myAtt[i].attackPower;
							int targetNumber = myAtt[i].number;

							int maxP = 100;
							vector<int> numberOfAIChooseAtt(9, 0);

							int m = AIAtt.size();
							for (int i = 0; i<m; ++i)
							{
								for (int j = i, k = m; j < k; --k)
								{
									int tempAttack = 0;
									vector<int> tempNOACA;
									for (int w = j; w < k; ++w)
									{
										if (AIAtt[w].canUse)
										{
											tempAttack += AIAtt[w].attackPower;
											tempNOACA.push_back(AIAtt[w].number);
										}
									}

									if (tempAttack >= targetAttack && (tempAttack - targetAttack)<maxP&&tempNOACA.size()<numberOfAIChooseAtt.size())
									{
										maxP = tempAttack - targetAttack;
										numberOfAIChooseAtt = tempNOACA;
									}
								}
							}

							bool haveChoose = true;
							for (auto &S : numberOfAIChooseAtt)
							{
								if (S == 0)
								{
									haveChoose = false;
									break;
								}
							}

							if ((maxP == 100 || !haveChoose) && (i == myAtt.size() - 1))
							{
								if (AIAtt.size() >= 4)
								{
									AIAtt[0].canUse = false;
									AIAtt[1].canUse = false;
									int aia0 = AIAtt[0].number, aia1 = AIAtt[1].number;
									attBeAttacked(aia0, targetKB, false);
									attBeAttacked(aia1, targetKB, false);

									for (int i = 0; i<2; ++i)
									{
										AIAtt[numberOfAIChooseAtt[i]].canUse = false;
										attBeAttacked(targetNumber, AIAtt[i].attackPower, true);
									}
								}
							}
							else if (haveChoose)
							{
								attBeAttacked(targetNumber, 100, true);
								for (int l = 0; l<numberOfAIChooseAtt.size(); ++l)
								{
									for (auto &S : AIAtt)
									{
										if (S.number == numberOfAIChooseAtt[l])
										{
											S.canUse = false; break;
										}
									}
									attBeAttacked(numberOfAIChooseAtt[l], targetKB, false);
								}
							}
						}

						haveChaoFeng = false;
						for (auto &S : myAtt)
						{
							if (S.isShield)
							{
								haveChaoFeng = true; break;
							}
						}

						if (!haveChaoFeng)
						{
							for (auto &S : AIAtt)
							{
								if (S.canUse)
								{
									changHeroBlood(true, S.attackPower);
								}
							}
						}
					}
				}

				//处理没有嘲讽的情况
				if (!finishAttack)
				{
					for (int i = 0; i<myAtt.size(); ++i)
					{
						int targetAttack = myAtt[i].nowDefensePower;
						int targetKB = myAtt[i].attackPower;
						int targetNumber = myAtt[i].number;

						int maxP = 100;
						vector<int> numberOfAIChooseAtt(9, 0);

						int m = AIAtt.size();
						for (int i = 0; i<m; ++i)
						{
							for (int j = i, k = m; j < k; --k)
							{
								int tempAttack = 0;
								vector<int> tempNOACA;
								for (int w = j; w < k; ++w)
								{
									if (AIAtt[w].canUse)
									{
										tempAttack += AIAtt[w].attackPower;
										tempNOACA.push_back(AIAtt[w].number);
									}
								}

								if (tempAttack >= targetAttack && (tempAttack - targetAttack)<maxP&&tempNOACA.size()<numberOfAIChooseAtt.size())
								{
									maxP = tempAttack - targetAttack;
									numberOfAIChooseAtt = tempNOACA;
								}
							}
						}

						bool haveChoose = true;
						for (auto &S : numberOfAIChooseAtt)
						{
							if (S == 0)
							{
								haveChoose = false;
								break;
							}
						}

						if ((maxP == 100 || !haveChoose) && (i == myAtt.size() - 1))
						{
							if (AIAtt.size() >= 4)
							{
								AIAtt[0].canUse = false;
								AIAtt[1].canUse = false;
								int aia0 = AIAtt[0].number, aia1 = AIAtt[1].number;
								attBeAttacked(aia0, targetKB, false);
								attBeAttacked(aia1, targetKB, false);

								for (int i = 0; i<2; ++i)
								{
									for (auto &S : AIAtt)
									{
										if (S.number == numberOfAIChooseAtt[i])
										{
											S.canUse = false; break;
										}
									}
									attBeAttacked(targetNumber, AIAtt[i].attackPower, true);
								}
							}
						}
						else if (haveChoose)
						{
							attBeAttacked(targetNumber, 100, true);
							for (int l = 0; l<numberOfAIChooseAtt.size(); ++l)
							{
								for (auto &S : AIAtt)
								{
									if (S.number == numberOfAIChooseAtt[l])
									{
										S.canUse = false; break;
									}
								}
								attBeAttacked(numberOfAIChooseAtt[l], targetKB, false);
							}
						}
					}

					haveChaoFeng = false;
					for (auto &S : myAtt)
					{
						if (S.isShield)
						{
							haveChaoFeng = true; break;
						}
					}

					if (!haveChaoFeng)
					{
						for (auto &S : AIAtt)
						{
							if (S.canUse)
							{
								changHeroBlood(true, S.attackPower);
							}
						}
					}
				}
			}

			//交换控制权
			isHumanTurn = true;
			humanTurn();
		}
	}	
}

void TrainScene::changHeroBlood(bool isMyHero, int attNum)
{
	if (attNum<0)
	{
		if (haveSound) { AudioEngine::play2d("Sounds/addLife.mp3", false, 0.8); }
	}
	else
	{
		if (haveSound) { AudioEngine::play2d("Sounds/att.mp3", false, 1.0); }
	}

	if (isMyHero)
	{
		if (attNum>0)
		{
			auto spr = Sprite::create("Game/heroBA.png");
			spr->setTag(7);
			spr->setPosition(visibleSizeW / 2, originY + 70);
			addChild(spr);

			scheduleOnce(schedule_selector(TrainScene::scheduleCallBack), 1.5);
		}

		int newBlood = myHeroBlood - attNum;

		if (newBlood>30)
		{
			newBlood = 30;
		}

		myHeroBlood = newBlood;

		stringstream ss; string s; ss << newBlood; ss >> s; ss.clear();
		myHL->setString(s);

		if (newBlood<10)
		{
			myHL->setColor(Color3B(0xFF,0,0));
		}
		else
		{
			myHL->setColor(Color3B(0, 0xFF, 0));
		}

		if (newBlood<=0)
		{
			auto spr = Sprite::create("UI/fallDialog.png");
			spr->setPosition(visibleSizeW / 2 - 300, visibleSizeH / 2);
			addChild(spr);

			scheduleOnce(schedule_selector(TrainScene::ETGscheduleCallBack), 2.5);
		}
	}
	else
	{
		if (attNum > 0)
		{
			auto spr = Sprite::create("Game/heroBA.png");
			spr->setTag(7);
			spr->setPosition(visibleSizeW / 2, visibleSizeH - 70);
			addChild(spr);

			scheduleOnce(schedule_selector(TrainScene::scheduleCallBack), 1.5);
		}

		int newBlood = AIHeroBlood - attNum;

		if (newBlood>30)
		{
			newBlood = 30;
		}

		AIHeroBlood = newBlood;

		stringstream ss; string s; ss << newBlood; ss >> s; ss.clear();
		AIHL->setString(s);

		if (newBlood<10)
		{
			AIHL->setColor(Color3B(0xFF, 0, 0));
		}
		else
		{
			AIHL->setColor(Color3B(0, 0xFF, 0));
		}

		if (newBlood <= 0)
		{
			auto spr = Sprite::create("UI/winDialog.png");
			spr->setPosition(visibleSizeW / 2 - 300, visibleSizeH / 2);
			addChild(spr);

			scheduleOnce(schedule_selector(TrainScene::ETGscheduleCallBack), 2.5);
		}
	}
}

void TrainScene::endThisGame(bool isWin)
{
	gameOver = true;

	needTouchMove = false;
	chooseAtt = nullptr;
	canUseSpr.clear();
	myCardLib.clear();AICardLib.clear();
	myHandCard.clear();AIHandCard.clear();
	myEHandCard.clear();AIEHandCard.clear();
	myAtt.clear();AIAtt.clear();
	myEAtt.clear();AIAtt.clear();
	myJams = 0;AIJams = 0;
	myEJams = 0;AIEJams = 0;
	myHeroBlood = 30;AIHeroBlood = 30;
	myJamSpr.clear(); AIJamSpr.clear();
	AILogs.clear();
	myHL = nullptr; AIHL = nullptr;
	myDrawWrong = 0; AIDrawWrong = 0;
	myTurnCount = 0; AITurnCount = 0;
	isHumanTurn = false;
	CB = nullptr;
	ET = nullptr;
	MainMenu = nullptr;
	ptrB.clear();
	DMScan = 0;
	TouchListener = nullptr;
	MoveAttListener = nullptr;
	DMTouchListener = nullptr;
	bSelectOne = nullptr;
	unscheduleAllSelectors();

	Director::getInstance()->end();
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
    #endif
}

void TrainScene::scheduleCallBack(float delta)
{
	for (int i=0;i<15;++i)
	{
		removeChildByTag(7);
	}
}

void TrainScene::scheduleCallBack2(float delta)
{
	removeChildByTag(100);
	removeChildByTag(101);
	removeChildByTag(102);
	removeChildByTag(103);
	removeChildByTag(104);

	CB->setEnabled(true);
}

void TrainScene::scheduleCallBack3(float delta)
{
	removeChildByTag(90);
	removeChildByTag(90);
	removeChildByTag(90);
	removeChildByTag(90);
	removeChildByTag(90);
}

void TrainScene::useMyDama()
{
	if (pkOne.kotodama.size()!=0)
	{
		//先处理G。
		if (pkOne.kotodama[0][0] == 'G')
		{
			for (auto &s : myAtt)
			{
				s.attackPower = 1;
				s.lLabel->setString("1");
				s.lLabel->setColor(Color3B(0xFF, 0, 0));
			}

			for (auto &s : AIAtt)
			{
				s.attackPower = 1;
				s.lLabel->setString("1");
				s.lLabel->setColor(Color3B(0xFF, 0, 0));
			}
		}
		//再处理F
		else if (pkOne.kotodama[0][0] == 'F' || pkOne.kotodama.size() == 2)
		{
			uniform_int_distribution<int> uni30(1, 30);
			default_random_engine e(time(0));

			int cid = uni30(e);
			while (1)
			{
				bool illegal = false;
				for (const auto &c : myCardLib)
				{
					if (c.number == cid)
					{
						illegal = true; break;
					}
				}
				if (illegal)
				{
					break;
				}
				else
				{
					cid = uni30(e);
				}
			}
			playerDrawCard(cid);
			cid = uni30(e);
			while (1)
			{
				bool illegal = false;
				for (const auto &c : myCardLib)
				{
					if (c.number == cid)
					{
						illegal = true; break;
					}
				}
				if (illegal)
				{
					break;
				}
				else
				{
					cid = uni30(e);
				}
			}
			playerDrawCard(cid);
		}
		//再处理E
		if (!gameOver)
		{
			if (pkOne.kotodama[0][0] == 'E')
			{
				if (pkOne.kotodama[0] == "E1111")
				{
					DMSelectEvent(1, 1, 100);
				}
				else if (pkOne.kotodama[0] == "E2310")
				{
					for (auto &c : myAtt)
					{
						removeChild(c.attSprite);
						removeChild(c.lLabel);
						removeChild(c.rLabel);
					}
					myAtt.clear();

					for (auto &c : AIAtt)
					{
						removeChild(c.attSprite);
						removeChild(c.lLabel);
						removeChild(c.rLabel);
					}
					AIAtt.clear();
				}
			}
			//再处理 A和D
			else if (pkOne.kotodama[0][0] == 'D')
			{
				if (pkOne.kotodama[0] == "D1312")
				{
					DMSelectEvent(3, 1, -2);
				}
				else if (pkOne.kotodama[0] == "D2223")
				{
					changHeroBlood(true, -3);
				}
				else if (pkOne.kotodama[0] == "D1338")
				{
					DMSelectEvent(3, 3, -8);
				}
			}
			else if (pkOne.kotodama[0][0] == 'A')
			{
				if (pkOne.kotodama[0] == "A1313")
				{
					DMSelectEvent(3, 1, 3);
				}
				else if (pkOne.kotodama[0] == "A1318")
				{
					DMSelectEvent(3, 1, 8);
				}
				else if (pkOne.kotodama[0] == "A1331")
				{
					DMSelectEvent(3, 3, 1);
				}
				else if (pkOne.kotodama[0] == "A1332")
				{
					DMSelectEvent(3, 3, 2);
				}
				else if (pkOne.kotodama[0] == "A2132")
				{
					changHeroBlood(false, 2);

					AllAIAttBeAttacked(2);
				}
				else if (pkOne.kotodama[0] == "A2123")
				{
					changHeroBlood(false, 3);
				}
				else if (pkOne.kotodama[0] == "A2312")
				{
					AllAIAttBeAttacked(2);

					AllMyAttBeAttacked(2);
				}
				else if (pkOne.kotodama[0] == "A2114")
				{
					AllAIAttBeAttacked(4);
				}
				else if (pkOne.kotodama[0] == "A2125")
				{
					changHeroBlood(false, 5);
				}
			}

			if (!gameOver)
			{
				if (haveSound) { AudioEngine::play2d("Sounds/addLife.mp3", false, 0.3); }
				auto spr = Sprite::create("Game/magic.png");
				spr->setTag(7);
				spr->setPosition(visibleSizeW / 2, visibleSizeH / 2);
				addChild(spr);

				scheduleOnce(schedule_selector(TrainScene::scheduleCallBack), 1.5);
			}
		}
	}

	if (pkOne.cardType&&(!gameOver))
	{
		Attendant att(pkOne, myAtt.size());
		myAtt.push_back(att);
		addChild(att.attSprite);
		addChild(att.lLabel);
		addChild(att.rLabel);

		if (att.canUse)
		{
			Vec2 v = att.attSprite->getPosition();
			auto spr = Sprite::create("Game/attCanUsed.png");
			spr->setPosition(v);
			addChild(spr);
			canUseSpr.push_back(spr);
		}
	}
}

void TrainScene::useAIDama(Card &C)
{
	if (C.kotodama.size() != 0)
	{
		//先处理G。
		if (C.kotodama[0][0] == 'G')
		{
			for (auto &s : myAtt)
			{
				s.attackPower = 1;
				s.lLabel->setString("1");
				s.lLabel->setColor(Color3B(0xFF, 0, 0));
			}

			for (auto &s : AIAtt)
			{
				s.attackPower = 1;
				s.lLabel->setString("1");
				s.lLabel->setColor(Color3B(0xFF, 0, 0));
			}
		}
		//再处理F
		else if (C.kotodama[0][0] == 'F' || C.kotodama.size() == 2)
		{
			uniform_int_distribution<int> uni30(1, 30);
			default_random_engine e(time(0));

			int cid = uni30(e);
			while (1)
			{
				bool illegal = false;
				for (const auto &c : AICardLib)
				{
					if (c.number == cid)
					{
						illegal = true; break;
					}
				}
				if (illegal)
				{
					break;
				}
				else
				{
					cid = uni30(e);
				}
			}
			AIDrawCard(cid);
			cid = uni30(e);
			while (1)
			{
				bool illegal = false;
				for (const auto &c : AICardLib)
				{
					if (c.number == cid)
					{
						illegal = true; break;
					}
				}
				if (illegal)
				{
					break;
				}
				else
				{
					cid = uni30(e);
				}
			}
			AIDrawCard(cid);
		}
		//再处理E
		if (!gameOver)
		{
			if (C.kotodama[0][0] == 'E')
			{
				if (C.kotodama[0] == "E1111")
				{
					if (myAtt.size()!=0)
					{
						int numberofKill = -1;
						int maxDefender = -1;

						for (auto &s : myAtt)
						{
							if (s.nowDefensePower>=maxDefender)
							{
								numberofKill = s.number;
								maxDefender = s.nowDefensePower;
							}
						}
						attBeAttacked(numberofKill, 100, true);
					}
				}
				else if (C.kotodama[0] == "E2310")
				{
					for (auto &c : AIAtt)
					{
						removeChild(c.attSprite);
						removeChild(c.lLabel);
						removeChild(c.rLabel);
					}
					AIAtt.clear();

					for (auto &c : myAtt)
					{
						removeChild(c.attSprite);
						removeChild(c.lLabel);
						removeChild(c.rLabel);
					}
					myAtt.clear();
				}
			}
			//再处理 A和D
			else if (C.kotodama[0][0] == 'D')
			{
				if (C.kotodama[0] == "D1312")
				{
					for (int DM=2;DM>0;--DM)
					{
						bool needB = false;
						for (auto &s : AIAtt)
						{
							if ((s.maxDefensePower-s.nowDefensePower)>=DM)
							{
								needB = true;
								attBeAttacked(s.number,-2,false);
								break;
							}
						}

						if (needB) { break; }
					}
				}
				else if (C.kotodama[0] == "D2223")
				{
					changHeroBlood(false, -3);
				}
				else if (C.kotodama[0] == "D1338")
				{
					for (int DM = 8; DM>0; --DM)
					{
						bool needB = false;
						for (auto &s : AIAtt)
						{
							if ((s.maxDefensePower - s.nowDefensePower) >= DM)
							{
								needB = true;
								attBeAttacked(s.number, -8, false);
								break;
							}
						}
						if (needB){break;}

						if ((30-AIHeroBlood)>=DM)
						{
							changHeroBlood(false,-8);
							break;
						}
					}
				}
			}
			else if (C.kotodama[0][0] == 'A')
			{
				if (myAtt.size()!=0)
				{
					if (C.kotodama[0] == "A1313")
					{
						bool hasKilled = false;
						for (int i = 3; i>0; --i)
						{
							for (auto &s : myAtt)
							{
								if (s.nowDefensePower <= 3)
								{
									attBeAttacked(s.number, 3, true);
									hasKilled = true;
									break;
								}
							}
							if (hasKilled)
							{
								break;
							}
						}

						if (!hasKilled)
						{
							attBeAttacked(myAtt[0].number, 3, true);
						}
					}
					else if (C.kotodama[0] == "A1318")
					{
						bool hasKilled = false;
						for (int i = 8; i>0; --i)
						{
							for (auto &s : myAtt)
							{
								if (s.nowDefensePower <= 8)
								{
									attBeAttacked(s.number, 8, true);
									hasKilled = true;
									break;
								}
							}
							if (hasKilled)
							{
								break;
							}
						}

						if (!hasKilled)
						{
							attBeAttacked(myAtt[0].number, 8, true);
						}
					}
					else if (C.kotodama[0] == "A1331")
					{
						int CG = 0;
						for (auto &s : AIAtt)
						{
							CG += s.attackPower;
						}

						if (myAtt.size() == 0 || CG + 1 >= myHeroBlood)
						{
							changHeroBlood(true, 1);
						}
						else
						{
							bool needB = false;
							for (auto &s : myAtt)
							{
								if (s.nowDefensePower <= 1)
								{
									needB = true;
									attBeAttacked(s.number, 1, true);
									break;
								}
							}

							if (!needB)
							{
								attBeAttacked(myAtt[0].number, 1, true);
							}
						}
					}
					else if (C.kotodama[0] == "A1332")
					{
						int CG = 0;
						for (auto &s : AIAtt)
						{
							CG += s.attackPower;
						}

						if (myAtt.size() == 0 || CG + 2 >= myHeroBlood)
						{
							changHeroBlood(true, 2);
						}
						else
						{
							bool needB = false;
							for (auto &s : myAtt)
							{
								if (s.nowDefensePower <= 2)
								{
									needB = true;
									attBeAttacked(s.number, 2, true);
									break;
								}
							}

							if (!needB)
							{
								attBeAttacked(myAtt[0].number, 2, true);
							}
						}
					}
					else if (C.kotodama[0] == "A2132")
					{
						changHeroBlood(true, 2);

						AllMyAttBeAttacked(2);
					}
					else if (C.kotodama[0] == "A2123")
					{
						changHeroBlood(true, 3);
					}
					else if (C.kotodama[0] == "A2312")
					{
						AllMyAttBeAttacked(2);

						AllAIAttBeAttacked(2);
					}
					else if (C.kotodama[0] == "A2114")
					{
						AllMyAttBeAttacked(4);
					}
					else if (C.kotodama[0] == "A2125")
					{
						changHeroBlood(true, 5);
					}
				}
			}

			if (!gameOver)
			{
				if (haveSound) { AudioEngine::play2d("Sounds/aaddLife.mp3", false, 0.3); }
				auto spr = Sprite::create("Game/magic.png");
				spr->setTag(7);
				spr->setPosition(visibleSizeW / 2, visibleSizeH / 2);
				addChild(spr);

				scheduleOnce(schedule_selector(TrainScene::scheduleCallBack), 1.5);
			}
		}
	}

	if (C.cardType && (!gameOver))
	{
		Attendant att(C, AIAtt.size());
		AIAtt.push_back(att);

		Vec2 v2 = att.attSprite->getPosition();
		att.attSprite->setPosition(v2.x,v2.y+250);
		v2 = att.lLabel->getPosition();
		att.lLabel->setPosition(v2.x, v2.y + 250);
		v2 = att.rLabel->getPosition();
		att.rLabel->setPosition(v2.x, v2.y + 250);

		addChild(att.attSprite);
		addChild(att.lLabel);
		addChild(att.rLabel);
	}
}

void TrainScene::attBeAttacked(int attId, int blood, bool isMyAtt)
{
	if (blood<0)
	{
		if (haveSound) { AudioEngine::play2d("Sounds/addLife.mp3", false, 0.8); }
	}
	else
	{
		if (haveSound) { AudioEngine::play2d("Sounds/att.mp3", false, 1.0); }
	}

	if (isMyAtt)
	{
		bool needMove = false;
		for (auto &c : myAtt)
		{
			if (needMove)
			{
				Vec2 v2 = c.attSprite->getPosition();
				v2.x -= 146;
				c.attSprite->setPosition(v2);

				v2 = c.lLabel->getPosition();
				v2.x -= 146;
				c.lLabel->setPosition(v2);

				v2 = c.rLabel->getPosition();
				v2.x -= 146;
				c.rLabel->setPosition(v2);
			}

			if (c.number == attId)
			{
				if (blood>0)
				{
					c.rLabel->setColor(Color3B(0xFF, 0, 0));
				}

				c.nowDefensePower -= blood;

				if (c.nowDefensePower <= 0)
				{
					c.nowDefensePower = 0;
					removeChild(c.attSprite);
					removeChild(c.lLabel);
					removeChild(c.rLabel);

					needMove = true;
				}
				else if (c.nowDefensePower >= c.maxDefensePower)
				{
					c.nowDefensePower = c.maxDefensePower;
					c.rLabel->setColor(Color3B(0, 0xFF, 0));

					stringstream ss; string s; ss << c.nowDefensePower; ss >> s; ss.clear();
					c.rLabel->setString(s);
				}
				else
				{
					stringstream ss; string s; ss << c.nowDefensePower; ss >> s; ss.clear();
					c.rLabel->setString(s);
				}
			}
		}

		for (auto iter = myAtt.begin(); iter != myAtt.end(); ++iter)
		{
			if (iter->nowDefensePower <= 0)
			{
				myAtt.erase(iter);
				break;
			}
		}
	}
	else
	{
		bool needMove = false;
		for (auto &c : AIAtt)
		{
			if (needMove)
			{
				Vec2 v2 = c.attSprite->getPosition();
				v2.x -= 146;
				c.attSprite->setPosition(v2);

				v2 = c.lLabel->getPosition();
				v2.x -= 146;
				c.lLabel->setPosition(v2);

				v2 = c.rLabel->getPosition();
				v2.x -= 146;
				c.rLabel->setPosition(v2);
			}

			if (c.number == attId)
			{
				if (blood>0)
				{
					c.rLabel->setColor(Color3B(0xFF, 0, 0));
				}

				c.nowDefensePower -= blood;

				if (c.nowDefensePower <= 0)
				{
					c.nowDefensePower = 0;
					removeChild(c.attSprite);
					removeChild(c.lLabel);
					removeChild(c.rLabel);

					needMove = true;
				}
				else if (c.nowDefensePower >= c.maxDefensePower)
				{
					c.nowDefensePower = c.maxDefensePower;
					c.rLabel->setColor(Color3B(0, 0xFF, 0));

					stringstream ss; string s; ss << c.nowDefensePower; ss >> s; ss.clear();
					c.rLabel->setString(s);
				}
				else
				{
					stringstream ss; string s; ss << c.nowDefensePower; ss >> s; ss.clear();
					c.rLabel->setString(s);
				}
			}
		}

		for (auto iter = AIAtt.begin(); iter != AIAtt.end(); ++iter)
		{
			if (iter->nowDefensePower <= 0)
			{
				AIAtt.erase(iter);
				break;
			}
		}
	}
}

void TrainScene::AllMyAttBeAttacked(int blood)
{
	for (auto iter = myAtt.begin();iter!=myAtt.end();)
	{
		if (blood>0)
		{
			iter->rLabel->setColor(Color3B(0xFF, 0, 0));
		}

		iter->nowDefensePower -= blood;

		if (iter->nowDefensePower <= 0)
		{
			iter->nowDefensePower = 0;
			removeChild(iter->attSprite);
			removeChild(iter->lLabel);
			removeChild(iter->rLabel);
			iter = myAtt.erase(iter);

			for (auto it=iter;it!=myAtt.end();++it)
			{
				Vec2 v2 = it->attSprite->getPosition();
				v2.x -= 146;
				it->attSprite->setPosition(v2);

				v2 = it->lLabel->getPosition();
				v2.x -= 146;
				it->lLabel->setPosition(v2);

				v2 = it->rLabel->getPosition();
				v2.x -= 146;
				it->rLabel->setPosition(v2);
			}
		}
		else
		{
			if (iter->nowDefensePower >= iter->maxDefensePower)
			{
				iter->nowDefensePower = iter->maxDefensePower;
				iter->rLabel->setColor(Color3B(0, 0xFF, 0));

				stringstream ss; string s; ss << iter->nowDefensePower; ss >> s; ss.clear();
				iter->rLabel->setString(s);
			}
			else
			{
				stringstream ss; string s; ss << iter->nowDefensePower; ss >> s; ss.clear();
				iter->rLabel->setString(s);
			}
			++iter;
		}
	}
}

void TrainScene::AllAIAttBeAttacked(int blood)
{
	for (auto iter = AIAtt.begin(); iter != AIAtt.end();)
	{
		if (blood>0)
		{
			iter->rLabel->setColor(Color3B(0xFF, 0, 0));
		}

		iter->nowDefensePower -= blood;

		if (iter->nowDefensePower <= 0)
		{
			iter->nowDefensePower = 0;
			removeChild(iter->attSprite);
			removeChild(iter->lLabel);
			removeChild(iter->rLabel);

			iter = AIAtt.erase(iter);

			for (auto it = iter; it != AIAtt.end(); ++it)
			{
				Vec2 v2 = it->attSprite->getPosition();
				v2.x -= 146;
				it->attSprite->setPosition(v2);

				v2 = it->lLabel->getPosition();
				v2.x -= 146;
				it->lLabel->setPosition(v2);

				v2 = it->rLabel->getPosition();
				v2.x -= 146;
				it->rLabel->setPosition(v2);
			}
		}
		else
		{
			if (iter->nowDefensePower >= iter->maxDefensePower)
			{
				iter->nowDefensePower = iter->maxDefensePower;
				iter->rLabel->setColor(Color3B(0, 0xFF, 0));

				stringstream ss; string s; ss << iter->nowDefensePower; ss >> s; ss.clear();
				iter->rLabel->setString(s);
			}
			else
			{
				stringstream ss; string s; ss << iter->nowDefensePower; ss >> s; ss.clear();
				iter->rLabel->setString(s);
			}
			++iter;
		}
	}
}

bool TrainScene::onTouchBegan(Touch *touch, Event *unused_ecent)
{
	Point location = touch->getLocation();

	int i = 0;
	for (auto &c:ptrB)
	{
		Vec2 v2 = c->getPosition();
		Rect rect(v2.x-105,v2.y-153,210,307);
		
		if (rect.containsPoint(location))
		{
			bool d = true;
			if (bSelectOne!=nullptr)
			{
				for (auto &c : ptrB)
				{
					if (bSelectOne == c)
					{
						d = false;
						break;
					}
				}

				for (auto &c : myCardLib)
				{
					if (bSelectOne == c.cardSprite)
					{
						d = false;
						break;
					}
				}

				for (auto &c : myAtt)
				{
					if (bSelectOne == c.attSprite)
					{
						d = false;
						break;
					}
				}

				for (auto &c : AIAtt)
				{
					if (bSelectOne == c.attSprite)
					{
						d = false;
						break;
					}
				}

				if (d)
				{
					removeChild(bSelectOne);
					bSelectOne = nullptr;
				}
			}

			auto spr = Sprite::create("Game/cardCanUsed.png");
			
			if (spr!=nullptr)
			{
				spr->setScale(0.75);
				spr->setPosition(c->getPosition());
				addChild(spr, 11);
				bSelectOne = spr;
			}
			pkOne = myEHandCard[i];
			break;
		}
		++i;
	}

	return true;
}

//camp: 1-AI方，2-玩家方，3-所有  role:1-随从，2-英雄，3-所有
void TrainScene::DMSelectEvent(int camp, int role, int scanBlood)
{
	DMScan = scanBlood;

	if (camp==1 && role==2)
	{
		changHeroBlood(false,scanBlood);
	}
	else if (camp==2 && role==2)
	{
		changHeroBlood(true, scanBlood);
	}
	else if (camp == 3 && role == 2)
	{
		auto AButton = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback, this));
		auto BButton = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback, this));

		AButton->setTag(176);
		BButton->setTag(177);

		AButton->setPosition(visibleSizeW / 2, originY + 50);
		BButton->setPosition(visibleSizeW / 2, visibleSizeH - 50);

		auto Menu = Menu::create(AButton, BButton, nullptr);
		Menu->setTag(107);
		Menu->setPosition(originX, originY);
		addChild(Menu, 15);

		MainMenu->setEnabled(false);
	}
	else if (camp == 1 && role == 1)
	{
		auto Menu = Menu::create(nullptr);
		Menu->setTag(107);
		Menu->setPosition(originX, originY);

		int i = 0;
		for (const auto &S:AIAtt)
		{
			auto Button = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback2, this));
			Button->setPosition(S.attSprite->getPosition());
			Button->setTag(i);
			Menu->addChild(Button);

			++i;
		}

		addChild(Menu, 15);

		MainMenu->setEnabled(false);
	}
	else if (camp == 1 && role == 3)
	{
		for (const auto &A : AIAtt)
		{
			attBeAttacked(A.number, scanBlood, false);
		}
	}
	else if (camp == 2 && role == 1)
	{
		auto Menu = Menu::create(nullptr);
		Menu->setTag(107);
		Menu->setPosition(originX, originY);

		int i = 0;
		for (const auto &S : myAtt)
		{
			auto Button = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback3, this));
			Button->setPosition(S.attSprite->getPosition());
			Button->setTag(i);
			Menu->addChild(Button);

			++i;
		}

		addChild(Menu, 15);

		MainMenu->setEnabled(false);
	}
	else if (camp == 2 && role == 3)
	{
		for (const auto &A : myAtt)
		{
			attBeAttacked(A.number, scanBlood, true);
		}
	}
	else if (camp == 3 && role == 1)
	{
		auto Menu = Menu::create(nullptr);
		Menu->setTag(107);
		Menu->setPosition(originX, originY);

		int i = 0;
		for (const auto &S : AIAtt)
		{
			auto Button = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback2, this));
			Button->setPosition(S.attSprite->getPosition());
			Button->setTag(i);
			Menu->addChild(Button);

			++i;
		}

		i = 0;
		for (const auto &S : myAtt)
		{
			auto Button = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback3, this));
			Button->setPosition(S.attSprite->getPosition());
			Button->setTag(i);
			Menu->addChild(Button);

			++i;
		}

		addChild(Menu, 15);

		MainMenu->setEnabled(false);
	}
	else if (camp == 3 && role == 3)
	{
		auto AButton = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback, this));
		auto BButton = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback, this));

		AButton->setTag(176);
		BButton->setTag(177);

		AButton->setPosition(visibleSizeW / 2, originY + 50);
		BButton->setPosition(visibleSizeW / 2, visibleSizeH - 50);

		auto Menu = Menu::create(AButton, BButton,nullptr);
		Menu->setTag(107);
		Menu->setPosition(originX, originY);

		int i = 0;
		for (const auto &S : AIAtt)
		{
			auto Button = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback2, this));
			Button->setPosition(S.attSprite->getPosition());
			Button->setTag(i);
			Menu->addChild(Button);

			++i;
		}

		i = 0;
		for (const auto &S : myAtt)
		{
			auto Button = MenuItemImage::create("UI/SelectTarget.png", "UI/SelectTarget.png", CC_CALLBACK_1(TrainScene::DMmenuCallback3, this));
			Button->setPosition(S.attSprite->getPosition());
			Button->setTag(i);
			Menu->addChild(Button);

			++i;
		}

		addChild(Menu, 15);

		MainMenu->setEnabled(false);
	}
}

vector<int> TrainScene::AISelectCard(bool canUseAttCard)
{
	AIEHandCard.clear();
	//初始化AI可用的卡组
	for (auto &c : AIHandCard)
	{
		if (c.fee <= AIEJams)
		{
			AIEHandCard.push_back(c);
		}
	}

	//temp为返回的AI出牌id数组
	vector<int> temp;

	//当可以出牌时
	if (AIEHandCard.size() != 0)
	{
		//随机选择出牌模式为进攻或者防守
		uniform_int_distribution<int> uni(0, 1);
		default_random_engine e(time(0));
		int i = uni(e);

		//Power为AI卡牌的权值
		vector<double> power;

		//i为1，防守模式
		if (i)
		{
			//防守
			for (const auto &fAI : AIEHandCard)
			{
				double tempPower = 0;
				if (fAI.cardType)
				{
					tempPower = fAI.attackPower + fAI.defensePower*1.5;
				}

				if (fAI.kotodama.size() != 0)
				{
					if (fAI.kotodama[0][0] == 'G')
					{
						int t = 0;
						for (auto &S : myAtt)
						{
							t += S.attackPower - 1;
						}
						t *= 1.2; tempPower += t;

						t = 0;
						for (auto &S : AIAtt)
						{
							t += S.attackPower - 1;
						}
						tempPower -= t;
					}
					else if (fAI.kotodama[0][0] == 'F' || fAI.kotodama.size() == 2)
					{
						int t = 0;
						if (fAI.kotodama.size() == 1)
						{
							stringstream ss; ss << fAI.kotodama[0][4]; ss >> t; ss.clear();
						}
						else
						{
							stringstream ss; ss << fAI.kotodama[1][4]; ss >> t; ss.clear();
						}

						if (AICardLib.size() <= t || (AIHandCard.size() + t) >= 9)
						{
							tempPower -= t;
						}
						else
						{
							tempPower += t;
						}

					}
					else if (fAI.kotodama[0] == "E1111")
					{
						int max = 0;
						for (auto &S : myAtt)
						{
							if (S.attackPower>max)
							{
								max = S.attackPower;
							}
						}

						if (myAtt.size()>0)
						{
							tempPower += 1.2;
						}
						else
						{
							tempPower -= 1;
						}
					}
					else if (fAI.kotodama[0] == "E2310")
					{
						int t = 0;
						for (auto &S : myAtt)
						{
							t += S.nowDefensePower;
						}

						for (auto &S : AIAtt)
						{
							t -= S.nowDefensePower;
						}
						tempPower += t;
					}
					else if (fAI.kotodama[0] == "D1312")
					{
						if (AIAtt.size() != 0)
						{
							int max = -10;
							for (auto &S : AIAtt)
							{
								int t = 0;
								if ((S.maxDefensePower - S.nowDefensePower) >= 2)
								{
									t = 2 * 1.2;
								}
								else
								{
									t = -2;
								}

								if (t>max)
								{
									max = t;
								}
							}
							tempPower += max;
						}
						else
						{
							tempPower -= 2;
						}
					}
					else if (fAI.kotodama[0] == "D2223")
					{
						if (AIHeroBlood <= 10)
						{
							tempPower += 5 * 1.2;
						}
						else
						{
							tempPower += 3 * 1.2;
						}
					}
					else if (fAI.kotodama[0] == "D1338")
					{
						if (AIAtt.size() != 0)
						{
							int max = -10;
							for (auto &S : AIAtt)
							{
								int t = 0;
								if ((S.maxDefensePower - S.nowDefensePower) >= 8)
								{
									t = 8 * 1.2;
								}
								else
								{
									t = -8;
								}

								if (t>max)
								{
									max = t;
								}
							}
							tempPower += max;
						}
						else
						{
							tempPower -= 6 * 1.2;
						}
					}
					else if (fAI.kotodama[0][0] == 'B')
					{
						tempPower += fAI.defensePower*1.2;
						tempPower += AIAtt.size();

						if (AIHeroBlood<=10)
						{
							tempPower += (30 - AIHeroBlood);
						}
					}
					else if (fAI.kotodama[0][0] == 'A')
					{
						int t = 0;
						stringstream ss; ss << fAI.kotodama[0][4]; ss >> t; ss.clear();
						if (fAI.kotodama[0][1] == '1')
						{
							tempPower += t;
						}
						else if (fAI.kotodama[0][1] == '2')
						{
							tempPower += (t*myAtt.size());
						}
					}
				}
				
				power.push_back(tempPower);
			}
		}
		//i为0，进攻模式
		else
		{
			//进攻
			for (const auto &fAI : AIEHandCard)
			{
				double tempPower = 0;
				if (fAI.cardType)
				{
					tempPower = fAI.attackPower*1.5 + fAI.defensePower;
				}

				if (fAI.kotodama.size() != 0)
				{
					if (fAI.kotodama[0][0] == 'G')
					{
						int t = 0;
						for (auto &S : myAtt)
						{
							t += S.attackPower - 1;
						}
						t *= 1.2; tempPower += t;
					}
					else if (fAI.kotodama[0][0] == 'F' || fAI.kotodama.size() == 2)
					{
						int t = 0;
						if (fAI.kotodama.size() == 1)
						{
							stringstream ss; ss << fAI.kotodama[0][4]; ss >> t; ss.clear();
						}
						else
						{
							stringstream ss; ss << fAI.kotodama[1][4]; ss >> t; ss.clear();
						}

						if (AICardLib.size() <= t || (AIHandCard.size() + t) >= 9)
						{
							tempPower -= t;
						}
						else
						{
							tempPower += t;
						}

					}
					else if (fAI.kotodama[0] == "E1111")
					{
						int max = 0;
						for (auto &S : myAtt)
						{
							if (S.attackPower>max)
							{
								max = S.attackPower;
							}
						}

						if (myAtt.size()>0)
						{
							tempPower += 1.5;
						}
						else
						{
							tempPower -= 1;
						}
					}
					else if (fAI.kotodama[0] == "E2310")
					{
						int t = 0;
						for (auto &S : myAtt)
						{
							t += S.nowDefensePower;
						}

						for (auto &S : AIAtt)
						{
							t -= S.nowDefensePower;
						}

						if (t>0)
						{
							t *= 1.2;
						}
						tempPower += t;
					}
					else if (fAI.kotodama[0] == "D1312")
					{
						if (AIAtt.size() != 0)
						{
							int max = -10;
							for (auto &S : AIAtt)
							{
								int t = 0;
								if ((S.maxDefensePower - S.nowDefensePower) >= 2)
								{
									t = 2;
								}
								else
								{
									t = -2;
								}

								if (t>max)
								{
									max = t;
								}
							}
							tempPower += max;
						}
						else
						{
							tempPower -= 2;
						}
					}
					else if (fAI.kotodama[0] == "D2223")
					{
						if (AIHeroBlood <= 10)
						{
							tempPower += 5;
						}
						else
						{
							tempPower += 3;
						}
					}
					else if (fAI.kotodama[0] == "D1338")
					{
						if (AIAtt.size() != 0)
						{
							int max = -10;
							for (auto &S : AIAtt)
							{
								int t = 0;
								if ((S.maxDefensePower - S.nowDefensePower) >= 8)
								{
									t = 8;
								}
								else
								{
									t = -8;
								}

								if (t>max)
								{
									max = t;
								}
							}
							tempPower += max;
						}
						else
						{
							tempPower -= 6;
						}
					}
					else if (fAI.kotodama[0][0] == 'B')
					{
						tempPower += fAI.defensePower;
						tempPower += AIAtt.size();

						if (AIHeroBlood <= 10)
						{
							tempPower += (30 - AIHeroBlood);
						}
					}
					else if (fAI.kotodama[0][0] == 'A')
					{
						int t = 0;
						stringstream ss; ss << fAI.kotodama[0][4]; ss >> t; ss.clear();
						if (fAI.kotodama[0][1] == '1')
						{
							tempPower += t*1.2;
						}
						else if (fAI.kotodama[0][1] == '2')
						{
							tempPower += (t*myAtt.size()*1.2);
						}
					}
				}
				power.push_back(tempPower);
			}
		}

		//power_number为权值和卡牌id的数组
		vector<vector<double>> power_number;
		power_number.push_back(power);
		vector<double> Pnumber;
		for (auto &S : AIEHandCard)
		{
			Pnumber.push_back(S.number);
		}
		power_number.push_back(Pnumber);

		//tempPowerCP为当前组合的数字序列
		vector<int> tempPowerCP;
		//cpMaxPower为合起来强度的最大值
		int cpMaxPower = -3;

		int m = power_number[0].size();
		for (int i = 0; i<m; ++i)
		{
			for (int j = i,k = m; j < k; --k)
			{
				double maxP = 0.0;
				int fe = 0;
				vector<int> cps;
				for (int w = j; w < k; ++w)
				{
					maxP += power_number[0][w];
					fe += AIEHandCard[w].fee;

					if (!(canUseAttCard) && AIEHandCard[w].cardType)
					{
						maxP -= 1000;
					}

					tempPowerCP.push_back(power_number[1][w]);
				}

				if (fe<=AIEJams && maxP>cpMaxPower&&((AIAtt.size()+tempPowerCP.size())<=8))
				{
					cpMaxPower = maxP;
					temp = tempPowerCP;
				}
				tempPowerCP.clear();
			}
		}
	}

	//显示AILog;
	string textS;

	for (auto &S:temp)
	{
		textS += AILogs[S-1];
		textS += "\n";
	}

	TTFConfig AILogTtfConfig("Fonts/Dengb.ttf", 32);
	auto logLabel = Label::createWithTTF(AILogTtfConfig,textS,TextHAlignment::CENTER, visibleSizeW - 200);
	logLabel->setPosition(Vec2((originX + visibleSizeW) / 2, (originY + visibleSizeH) / 2 - 40));
	logLabel->setTag(90);
	logLabel->setColor(Color3B(255,0,255));

	addChild(logLabel);
	scheduleOnce(schedule_selector(TrainScene::scheduleCallBack3), 2.5);

	return temp;
}

bool TrainScene::onTouchBeganMATT(Touch *touch, Event *unused_ecent)
{
	if (isHumanTurn&&(myAtt.size()!=0))
	{
		Point location = touch->getLocation();
		double locationX = location.x;
		double locationY = location.y;

		for (auto &S : myAtt)
		{
			Vec2 v = S.attSprite->getPosition();
			Rect rect(v.x - 63.5, v.y - 110, 127, 220);

			if (rect.containsPoint(location) && S.canUse)
			{
				YVec = v;
				needTouchMove = true;
				chooseAtt = &S;

				break;
			}
		}
	}
	return true;
}

void TrainScene::onTouchMovedMATT(Touch *touch, Event *unused_ecent)
{
	if (needTouchMove && chooseAtt!=nullptr)
	{
		Point location = touch->getLocation();
		double locationX = location.x;
		double locationY = location.y;

		chooseAtt->attSprite->setPosition(location);
	}
}

void TrainScene::onTouchEndedMATT(Touch *touch, Event *unused_ecent)
{
	if (chooseAtt!=nullptr&&needTouchMove)
	{
		needTouchMove = false;
		chooseAtt->attSprite->setPosition(YVec);

		Point location = touch->getLocation();
		double locationX = location.x;
		double locationY = location.y;

		Rect rect1(610, 675, 670, 720);

		bool haveChaoFeng = false;
		for (auto &S : AIAtt)
		{
			if (S.isShield)
			{
				haveChaoFeng = true;
				break;
			}
		}

		if (rect1.containsPoint(location))
		{
			if (!haveChaoFeng)
			{
				changHeroBlood(false, chooseAtt->attackPower);
				chooseAtt->canUse = false;
			}
		}
		else
		{
			for (auto &S : AIAtt)
			{
				Vec2 v = S.attSprite->getPosition();
				Rect rect(v.x - 63.5, v.y - 110, 127, 220);

				if (rect.containsPoint(location))
				{
					if ((!haveChaoFeng)||(haveChaoFeng&&S.isShield))
					{
						Attendant temp = S;
						attBeAttacked(temp.number, chooseAtt->attackPower, false);

						attBeAttacked(chooseAtt->number, temp.attackPower, true);
						chooseAtt->canUse = false;
						break;
					}
				}
			}
		}
	}
	chooseAtt = nullptr;
}

void TrainScene::ETGscheduleCallBack(float delta)
{
	if (myHeroBlood>0)
	{
		endThisGame(true);
	}
	else
	{
		endThisGame(false);
	}
}