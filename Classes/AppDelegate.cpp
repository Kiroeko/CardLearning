//Engine Version cocos2d-x 3.13.1
#include "AppDelegate.h"
#include "AudioEngine.h"
#include "MainScene.h"

USING_NS_CC;
using namespace cocos2d::experimental;

static cocos2d::Size designResolutionSize = cocos2d::Size(1280, 720);
static cocos2d::Size smallResolutionSize = cocos2d::Size(960, 540);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1280, 720);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1920, 1080);

//Extern Records Data
extern double visibleSizeW;
extern double visibleSizeH;

extern double originX;
extern double originY;

extern int audioId;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}


void AppDelegate::initGLContextAttrs()
{
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };
    GLView::setGLContextAttrs(glContextAttrs);
}

static int register_all_packages()
{
    return 0;
}

bool AppDelegate::applicationDidFinishLaunching() 
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) 
	{
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
          glview = GLViewImpl::createWithRect("Card Learning", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        #else
          glview = GLViewImpl::create("Card Learning");
        #endif

        director->setOpenGLView(glview);
    }

    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 60);

    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
 
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

    register_all_packages();

	//Initialize Visible Size and Site of Origin Point
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	visibleSizeH = visibleSize.height;
	visibleSizeW = visibleSize.width;

	originX = origin.x;
	originY = origin.y;

	//Initialize Background Music
	audioId = AudioEngine::play2d("Sounds/song.mp3", true, 0.5);

	//Start FirstScene
    auto FirstScene = MainScene::createScene();
    director->runWithScene(FirstScene);

    return true;
}

void AppDelegate::applicationDidEnterBackground() 
{
    Director::getInstance()->stopAnimation();
	AudioEngine::pause(audioId);
}

void AppDelegate::applicationWillEnterForeground() 
{
    Director::getInstance()->startAnimation();
	AudioEngine::resume(audioId);
}