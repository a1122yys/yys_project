#include"shop.h"
#include"GameScene.h"
USING_NS_CC;
Vector<Sprite*> daojus1;
Scene* shop::createScene(){
	auto scene = Scene::create();
	auto layer = shop::create();
	scene->addChild(layer);
	return scene;
}
bool shop::init(){
	if (!Layer::init())
	{
		return false;
	}
	Size winSize = Director::getInstance()->getVisibleSize();
	auto background = cocos2d::LayerColor::create(Color4B(0, 255, 255, 255));
	background->setAnchorPoint(Point(0, 0));
	background->setPosition(0, 0);
	addChild(background, -1);
	Sprite* bg = Sprite::create("bg1.png");
	bg->setPosition(winSize.width/2,winSize.height/2);
	this->addChild(bg,0);

	s = Sprite::create("close.png");
	s->setPosition(winSize.width / 2 + bg->getContentSize().width/2, winSize.height / 2 + bg->getContentSize().height/2);
	s->setAnchorPoint(Point(1, 1));
	addChild(s, 2, 0);

	Sprite* daoju;
	for (int i = 2; i <= 7;i++){
		char daju1[20] = { 0 };
		sprintf(daju1, "daoju_%d.png", i);
		daoju = Sprite::create(daju1);
		if (i<=4){
			daoju->setPosition(185 + (i - 2) * 55, winSize.height / 2 + 85);
		}
		else{
			daoju->setPosition(185 + (i - 5) * 55, winSize.height / 2 + 25);
		}
		daojus1.pushBack(daoju);
		this->addChild(daoju,1,i);
	}
	auto bao = MenuItemImage::create(
		"goumai.png",
		"goumai1.png",
		CC_CALLBACK_1(shop::switchToLayer, this));
	bao->setAnchorPoint(Point(0.5, 0.5));
	bao->setPosition(winSize.width/2, winSize.height/2 - 90);
	auto menu = Menu::create(bao, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, 2);
	tag = 1;

	char string[20] = { 0 };
	sprintf(string, "$:%d", 200);
	label1 = Label::create(string, "Arial", 50);
	label1->setAnchorPoint(Point(0.5, 0.5));
	label1->setColor(cocos2d::Color3B::YELLOW);
	label1->setPosition(Point(winSize.width / 2, winSize.height / 2 - 40));
	addChild(label1);


	addListense();
}
void shop::addListense(){
	Size winSize = Director::getInstance()->getVisibleSize();
	kuan = Sprite::create("kuan.png");
	kuan->setPosition(185, winSize.height / 2 + 85);
	addChild(kuan);
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = [this](Touch* t, Event* e){
		return true;
	};
	touchListener->onTouchEnded = [this](Touch* t, Event* e){
		Size winsize = Director::getInstance()->getVisibleSize();

		Vector<Sprite*>::iterator j;
		Sprite* p;
		for (j = daojus1.begin(); j != daojus1.end(); j++){
			p = *j;
			if (p->getBoundingBox().containsPoint(t->getLocation())){
				kuan->setPosition(p->getPosition());
				char string[20] = { 0 };
				sprintf(string, "$:%d", (p->getTag() - 1) * 200);
				label1->setString(string);
			}
		}
		if (this->getChildByTag(1)!=NULL){
			this->removeChildByTag(1);
		}
		if (s->getBoundingBox().containsPoint(t->getLocation())){
			swithchScene();
		}
	};
	dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}
void shop::switchToLayer(cocos2d::Ref* pSender){
	Size winSize = Director::getInstance()->getVisibleSize();
	char string[20] = { 0 };
	Vector<Sprite*>::iterator j;
	Sprite* p;
	for (j = daojus1.begin(); j != daojus1.end(); j++){
		p = *j;
		if (kuan->getPosition() == p->getPosition()){
			tag = p->getTag();
		}
	}
	int jinbiNum = UserDefault::getInstance()->getIntegerForKey("jinbiNum");
	if (jinbiNum < (tag - 1) * 200){
		zi = Sprite::create("jinbibuzu.png");
		zi->setPosition(winSize.width / 2, winSize.height / 2 - 40);
		addChild(zi,2,1);
	}
	else{
		jinbiNum = jinbiNum - (tag - 1) * 200;
		UserDefault::getInstance()->setIntegerForKey("jinbiNum", jinbiNum);
		UserDefault::getInstance()->setIntegerForKey("daojuNum", tag);
		UserDefault::getInstance()->flush();
		swithchScene();
	}
}
void shop::swithchScene(){
	
	Director::getInstance()->popScene();
}