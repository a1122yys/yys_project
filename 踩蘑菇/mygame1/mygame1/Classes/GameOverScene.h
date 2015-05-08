#include "cocos2d.h"
using namespace cocos2d;
class gameOver :public cocos2d::Layer{
public:
	static cocos2d::Scene* createScene();
	//char label[];
	bool init();
    void restartGame(cocos2d::Ref* pSender);
	void exitGame(cocos2d::Ref* pSender);
	//float score;
	
	CREATE_FUNC(gameOver);
};