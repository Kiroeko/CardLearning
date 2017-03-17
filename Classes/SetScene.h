#ifndef _SET_SCENE_H_
#define _SET_SCENE_H_

#include "cocos2d.h"

class SetScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuCallback(cocos2d::Ref* sender);

	void scheduleCallBack(float delta);

	CREATE_FUNC(SetScene);
};

#endif // _MAIN_SCENE_H_