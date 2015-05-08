#include"cocos2d.h"
using namespace cocos2d;
class shop :public cocos2d::Layer{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	void addListense();
	Sprite* kuan;
	int tag=2;
	Sprite* s;
	Sprite* zi;
	Label* label1;
	void swithchScene();
	//Label* tip;
	void switchToLayer(cocos2d::Ref* pSender);
	CREATE_FUNC(shop);

};