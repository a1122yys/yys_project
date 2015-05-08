#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
using namespace cocos2d;
class ChooseLevelScene : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();
	//cocos2d::Sprite* sprite1;
	//cocos2d::Sprite* sprite2;
	void addBg();
	bool onTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	void onTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	// a selector callback
	void menuCloseCallback1(cocos2d::Ref* pSender);
	void menuCloseCallback2(cocos2d::Ref* pSender);
	// implement the "static create()" method manually
	void switchScene();
	float beginX;
	float beginY;
	void leftLive();
	void rigthLive();
	CREATE_FUNC(ChooseLevelScene);
};

#endif // __HELLOWORLD_SCENE_H__
