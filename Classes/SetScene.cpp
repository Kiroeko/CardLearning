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
	auto titleLabel = Label::createWithTTF(ttfConfig, u8"����ҳ��", TextHAlignment::CENTER, visibleSizeW);
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
	auto resaiLabel = Label::createWithTTF(ttfConfig2, u8"����AI", TextHAlignment::RIGHT, visibleSizeW);
	resaiLabel->setPosition(Vec2((originX + visibleSizeW - 50), (originY + visibleSizeH - 120)));
	resaiLabel->setColor(Color3B(0xFF, 0, 0));
	addChild(resaiLabel);

	//Load Rule Label
	TTFConfig ttfConfig3("Fonts/Dengb.ttf", 32);
	auto ruleLabel = Label::createWithTTF(ttfConfig3, 
		u8"��Ϸ����\n\
        ��Ϸ��ÿ�˶���30�ſ��ƣ�ÿ���غϿ�ʼ��һ�����ƽ׶Σ�ÿ�γ��ƶ�����ƿ������ȡһ�ſ��Ƶ������С�\n\
		��Ϸ��һ�������ֿ��ƣ���ӿ��ƺͷ������ơ�\n\
		����������һ���Կ��ƣ��������д���������ƣ��ͻᴥ���������ƶ�Ӧ��һ����Ч����\n\
		��ӿ��Ʊ��������д����ᱻ�ƶ����������ϡ��������ϵ����ֻ�����������Է�����ӻ�Է�Ӣ�ۡ�\n\
        ת�Ƶ����ϵ�����������Ӽ��ܣ��ͻᴥ������Ч����\n\
        ÿ�����ƶ��д򵽳�����Ҫ�ķ��ã������Ҫ����ˮ��ʹ�����ơ���Ϸ���������ˮ�������᲻�����ӡ�\n\
        ����ƿ���ˣ���ҳ��Գ��ƻ��ܵ��˺���\n\
        ÿ����ҿ��Ƶ�Ӣ�۶���30������ֵ����������֮�󣬲ٿ����Ӣ�۵���Ҿ�������һ����Ϸ��",
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