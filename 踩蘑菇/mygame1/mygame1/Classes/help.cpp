#include"help.h"
#include"GameScene.h"
USING_NS_CC;
Scene* help::createScene(){
	auto scene = Scene::create();
	auto layer = help::create();
	scene->addChild(layer);
	return scene;
}
bool help::init(){
	if (!Layer::init())
	{
		return false;
	}
	Size winSize = Director::getInstance()->getVisibleSize();
	auto background = cocos2d::LayerColor::create(Color4B(0, 255, 255, 255));
	Sprite* bg = Sprite::create("help_bg.png");
	bg->setPosition(Point(winSize.width/2,winSize.height/2));
	addChild(bg,2,1);

	auto close = MenuItemImage::create(
		"fanhui_1.png",
		"fanhui.png",
		CC_CALLBACK_1(help::swithchScene, this));
	close->setAnchorPoint(Point(0.5, 0.5));
	close->setPosition(winSize.width - close->getContentSize().width / 2, close->getContentSize().height/2);
	auto menu = Menu::create(close, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, 2);
}

void help::swithchScene(cocos2d::Ref* pSender){
	Director::getInstance()->popScene();
}