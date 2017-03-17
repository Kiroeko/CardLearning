#include <fstream>

#include "AudioEngine.h"
#include "SetScene.h"

USING_NS_CC;
using namespace cocos2d::experimental;

using std::ofstream;

//Extern Records Data
extern double visibleSizeW;
extern double visibleSizeH;

extern double originX;
extern double originY;

extern int audioId;
extern bool haveSound;

Scene* SetScene::createScene()
{
	auto scene = Scene::create();

	auto backGroundLayer = LayerGradient::create(Color4B(0xAB, 0x82, 0xFF, 255), Color4B(0xBF, 0xEF, 0xFF, 255));
	auto layer = SetScene::create();

	scene->addChild(backGroundLayer);
	scene->addChild(layer);

	return scene;
}

bool SetScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	//Load Title Label
	TTFConfig ttfConfig("Fonts/Dengb.ttf", 60);
	auto titleLabel = Label::createWithTTF(ttfConfig, u8"设置页面", TextHAlignment::CENTER, visibleSizeW);
	titleLabel->setPosition(Vec2((originX + visibleSizeW) / 2, (originY + visibleSizeH - 50)));
	addChild(titleLabel);

	//Load Main Menu
	auto backButton = MenuItemImage::create("UI/backB.png", "UI/backBP.png", CC_CALLBACK_1(SetScene::menuCallback, this));
	auto resetButton = MenuItemImage::create("UI/resetB.png", "UI/resetBP.png", CC_CALLBACK_1(SetScene::menuCallback, this));

	backButton->setTag(0);
	resetButton->setTag(1);

	backButton->setPosition((originX + 50), (originY + visibleSizeH - 50));
	resetButton->setPosition((originX + visibleSizeW - 50), (originY + visibleSizeH - 50));

	auto Menu = Menu::create(backButton, resetButton, nullptr);
	Menu->setPosition(originX, originY);
	addChild(Menu);

	//Load BackGround
	auto BG = Sprite::create("UI/bb.png");
	BG->setPosition(visibleSizeW / 2, visibleSizeH / 2 - 40);
	addChild(BG);

	//Reset AI Label
	TTFConfig ttfConfig2("Fonts/Dengb.ttf", 30);
	auto resaiLabel = Label::createWithTTF(ttfConfig2, u8"重置AI", TextHAlignment::RIGHT, visibleSizeW);
	resaiLabel->setPosition(Vec2((originX + visibleSizeW - 50), (originY + visibleSizeH - 120)));
	resaiLabel->setColor(Color3B(0xFF, 0, 0));
	addChild(resaiLabel);

	//Load Rule Label
	TTFConfig ttfConfig3("Fonts/Dengb.ttf", 32);
	auto ruleLabel = Label::createWithTTF(ttfConfig3, 
		u8"游戏规则：\n\
        游戏中每人都有30张卡牌，每个回合开始有一个抽牌阶段，每次抽牌都会从牌库里随机取一张卡牌到手牌中。\n\
		游戏中一共有两种卡牌，随从卡牌和法术卡牌。\n\
		法术卡牌是一次性卡牌，从手牌中打出法术卡牌，就会触发法术卡牌对应的一次性效果。\n\
		随从卡牌被从手牌中打出后会被移动到己方场上。己方场上的随从只能用来攻击对方的随从或对方英雄。\n\
        转移到场上的随从如果有随从技能，就会触发技能效果。\n\
        每个卡牌都有打到场上需要的费用，玩家需要消耗水晶使用手牌。游戏过程中玩家水晶数量会不断增加。\n\
        如果牌库空了，玩家尝试抽牌会受到伤害。\n\
        每个玩家控制的英雄都有30点生命值，当其死亡之后，操控这个英雄的玩家就输了这一局游戏。",
		TextHAlignment::LEFT, visibleSizeW-200);

	ruleLabel->setPosition(Vec2((originX+visibleSizeW)/2, (originY + visibleSizeH)/2-40));
	addChild(ruleLabel);

	return true;
}

void SetScene::menuCallback(Ref *sender)
{
	if (haveSound) { AudioEngine::play2d("Sounds/bi.mp3", false, 0.5); }

	auto selector = static_cast<MenuItemImage*>(sender);

	switch (selector->getTag())
	{
	//Back Button Was Pushed
	case 0:
	{
		unscheduleAllSelectors();
		Director::getInstance()->popScene();
	}
	break;

	//Reset Button Was Pushed
	case 1:
	{
		ofstream out("traindata.txt");
		out.close();
		out.open("model.txt");
		out.close();

		auto tips = Sprite::create("UI/tips.png");
		tips->setPosition(visibleSizeW/2,visibleSizeH/2);
		tips->setTag(2);
		addChild(tips);

		scheduleOnce(schedule_selector(SetScene::scheduleCallBack), 1.5);
	}
	break;
	}
}

void SetScene::scheduleCallBack(float delta)
{
	removeChildByTag(2);
}