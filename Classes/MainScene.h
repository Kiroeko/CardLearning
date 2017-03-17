#ifndef _MAIN_SCENE_H_
#define _MAIN_SCENE_H_

#include "cocos2d.h"

class MainScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
	void menuCallback(cocos2d::Ref* sender);

    CREATE_FUNC(MainScene);
};

#endif // _MAIN_SCENE_H_