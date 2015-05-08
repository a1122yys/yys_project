#include"cocos2d.h"
using namespace cocos2d;
class help :public cocos2d::Layer{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	void swithchScene(cocos2d::Ref* pSender);
	CREATE_FUNC(help);

};