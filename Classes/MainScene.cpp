#include "AudioEngine.h"
#include "MainScene.h"
#include "SetScene.h"
#include "TrainScene.h"
#include "GameScene.h"

USING_NS_CC;
using namespace cocos2d::experimental;

//Extern Records Data
extern double visibleSizeW;
extern double visibleSizeH;

extern double originX;
extern double originY;

extern int audioId;
extern bool haveSound;

Scene* MainScene::createScene()
{
    auto scene = Scene::create();
	
	auto backGroundLayer = LayerGradient::create(Color4B(0xBF, 0xEF, 0xFF, 255), Color4B(0xAB, 0x82, 0xFF, 255));
	auto layer = MainScene::create();

    scene->addChild(backGroundLayer);
	scene->addChild(layer);

    return scene;
}

bool MainScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	//Load Title Label
	TTFConfig ttfConfig("Fonts/Dengb.ttf", 60);
	auto titleLabel = Label::createWithTTF(ttfConfig, u8"基于机器学习的卡牌", TextHAlignment::CENTER, visibleSizeW);
	titleLabel->setPosition(Vec2((originX + visibleSizeW) / 2, (originY + visibleSizeH - 50)));
	titleLabel->setColor(Color3B(0,0,0));
	addChild(titleLabel);

	//Load Main Menu
	auto trainButton = MenuItemImage::create("UI/trainB.png","UI/trainBP.png",CC_CALLBACK_1(MainScene::menuCallback,this));
	auto gameButton = MenuItemImage::create("UI/gameB.png", "UI/gameBP.png", CC_CALLBACK_1(MainScene::menuCallback, this));
	auto BGMButton = MenuItemImage::create("UI/BGMB.png", "UI/BGMBP.png",CC_CALLBACK_1(MainScene::menuCallback, this));
	auto setButton = MenuItemImage::create("UI/setB.png", "UI/setBP.png", CC_CALLBACK_1(MainScene::menuCallback, this));

	trainButton->setTag(0);
	gameButton->setTag(1);
	BGMButton->setTag(2);
	setButton->setTag(3);

	trainButton->setPosition((originX + visibleSizeW) /4+15, (originY + visibleSizeH) / 2-40);
	gameButton->setPosition((originX + visibleSizeW) /4*3-15, (originY + visibleSizeH) / 2-40);
	BGMButton->setPosition((originX + 50), (originY + visibleSizeH - 50));
	setButton->setPosition((originX + visibleSizeW - 50), (originY + visibleSizeH - 50));

	auto Menu = Menu::create(trainButton, gameButton, BGMButton, setButton, nullptr);
	Menu->setPosition(originX,originY);
	addChild(Menu);

	if (!haveSound)
	{
		auto No = Sprite::create("UI/N.png");
		No->setPosition((originX + 50), (originY + visibleSizeH - 50));
		No->setTag(4);
		addChild(No);
	}

    return true;
}

void MainScene::menuCallback(Ref *sender)
{
	if (haveSound) { AudioEngine::play2d("Sounds/bi.mp3", false, 0.5); }

	auto selector = static_cast<MenuItemImage*>(sender);

	switch (selector->getTag())
	{
	//Train Button Was Pushed
	case 0:
	{
		//Train Scene
		Director::getInstance()->replaceScene(TrainScene::createScene());
	}
	break;

	//Game Button Was Pushed
	case 1:
	{
		//Game Scene
		Director::getInstance()->replaceScene(GameScene::createScene());
	}
	break;

	//BGM Button Was Pushed
	case 2:
	{
		if (haveSound)
		{
			haveSound = false;
			AudioEngine::pause(audioId);

			auto No = Sprite::create("UI/N.png");
			No->setPosition((originX + 50), (originY + visibleSizeH - 50));
			No->setTag(4);
			addChild(No);
		}
		else
		{
			haveSound = true;
			AudioEngine::resume(audioId);
			removeChildByTag(4);
		}
	}
	break;

	//Set Button Was Pushed
	case 3:
	{
		//Set Scene
		auto NextScene = SetScene::createScene();
		Director::getInstance()->pushScene(TransitionFadeUp::create(0.7, NextScene));
	}
	break;
	}
}