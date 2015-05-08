#include "ChooseLevelScene.h"
#include "gameScene.h"
USING_NS_CC;
Vector<Sprite*> bgs;
Vector<Sprite*> daojus;
Scene* ChooseLevelScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ChooseLevelScene::create();
	scene->addChild(layer);

	return scene;
}
bool ChooseLevelScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
	auto background = cocos2d::LayerColor::create(Color4B(0, 255, 255, 255));
	background->setAnchorPoint(Point(0, 0));
	background->setPosition(0, 0);
	addChild(background, -1);
	auto closeItem1 = MenuItemImage::create(
		"startleft.png",
		"pressedleft.png",
		CC_CALLBACK_1(ChooseLevelScene::menuCloseCallback1, this));

	closeItem1->setPosition(Point(origin.x + closeItem1->getContentSize().width / 2,
		origin.y + visibleSize.height / 2));

	auto closeItem2 = MenuItemImage::create(
		"start.png",
		"pressed.png",
		CC_CALLBACK_1(ChooseLevelScene::menuCloseCallback2, this));

	closeItem2->setPosition(Point(origin.x + visibleSize.width - closeItem2->getContentSize().width / 2,
		origin.y + visibleSize.height / 2));
	auto menu = Menu::create(closeItem1, closeItem2, NULL);
	menu->setPosition(Point::ZERO);
	addBg();
	this->addChild(menu, 2);
	
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(ChooseLevelScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(ChooseLevelScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(ChooseLevelScene::onTouchEnded, this);
	
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}
void ChooseLevelScene::addBg(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Sprite* s;
	Sprite* daoju;

	for (int i = 1; i <= 10; i++){
		s = Sprite::create("bg.png");
		s->setPosition(Vec2(visibleSize.width / 2 + ((i - 1) * visibleSize.width), visibleSize.height / 2));
		bgs.pushBack(s);
		addChild(s, 2, i);
		char string[20] = { 0 };
		sprintf(string, "Level:%d ", i);
		auto label1 = Label::create(string, "Arial", 24);
		label1->setAnchorPoint(Point(0.5, 0.5));
		label1->setPosition(Point(83, 150));
		s->addChild(label1);
		if (i <= 7){
		char daju1[20] = { 0 };
		sprintf(daju1, "daoju_1%d.png", i);
		daoju = Sprite::create(daju1);
		daoju->setPosition(Point(83, 80));
		daojus.pushBack(daoju);
		s->addChild(daoju);
	    }
	}
}
bool ChooseLevelScene::onTouchBegan(CCTouch* touch, CCEvent* event)
{
	beginX = touch->getLocation().x;
	beginY = touch->getLocation().y;
	return true;
}

void ChooseLevelScene::onTouchEnded(CCTouch* touch, CCEvent* event)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	if (touch->getLocation().x-beginX > 0){
		rigthLive();
	}
	else{
		if (touch->getLocation().x - beginX < 0){
			leftLive();
		}
	}
	if (touch->getLocation() == Point(beginX, beginY)){
		Vector<Sprite*>::iterator j;
		Sprite* p;
		for (j = bgs.begin(); j != bgs.end(); j++){
			p = *j;
			
			if (Rect(p->getPosition().x - visibleSize.width / 2, p->getPosition().y - visibleSize.height/2,480,320).containsPoint(touch->getLocation())){
				UserDefault::getInstance()->setFloatForKey("Level", p->getTag());
				switchScene();
			}
			
		}
		
	}
}
void ChooseLevelScene::menuCloseCallback1(cocos2d::Ref* pSender)
{
	leftLive();
}
void ChooseLevelScene::menuCloseCallback2(Ref* pSender)
{
	rigthLive();
}
void ChooseLevelScene::switchScene(){
	auto scene = GameScene::createScene();
	Director::getInstance()->replaceScene(scene);
}
void ChooseLevelScene::leftLive(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
	Vector<Sprite*>::iterator j;
	Sprite* p;
	for (j = bgs.begin(); j != bgs.end(); j++){
		p = *j;
		MoveTo* moveTo = MoveTo::create(1.0f, Point(p->getPosition().x - visibleSize.width, p->getPosition().y));
		p->runAction(moveTo);
		if (p->getPosition().x < -visibleSize.width){
			p->setPosition(Point(p->getPosition().x + 10 * visibleSize.width, p->getPosition().y));
		}
	}
}
void ChooseLevelScene::rigthLive(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
	Vector<Sprite*>::iterator j;
	Sprite* p;
	for (j = bgs.begin(); j != bgs.end(); j++){
		p = *j;
		if (p->getPosition().x > visibleSize.width){
			p->setPosition(Point(p->getPosition().x - 10 * visibleSize.width, p->getPosition().y));
		}
		MoveTo* moveTo = MoveTo::create(1.0f, Point(p->getPosition().x + visibleSize.width, p->getPosition().y));
		p->runAction(moveTo);
	}
}