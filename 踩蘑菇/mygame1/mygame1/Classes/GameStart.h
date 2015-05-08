#include "cocos2d.h"
using namespace cocos2d;
class  GameStart : public cocos2d::Layer{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	void switchScene(float t);
	void readTexture();
	void removeMogu(float t);
	Vector <SpriteFrame*> actionAnimFrames;
	Sprite* s;
	Action* action;
	void bearMoveEnd();
	CREATE_FUNC(GameStart);
};