#include "gameOverScene.h"
#include "ChooseLevelScene.h"
USING_NS_CC;
Vector <SpriteFrame*> timoAnimFrames;
Scene* gameOver::createScene(){
	auto gameOverScene = Scene::create();
	auto layer = gameOver::create();
	gameOverScene->addChild(layer);
	return gameOverScene;
}
bool gameOver::init(){
	if (!Layer::init())
	{
		return false;
	}
	Size winSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto background = cocos2d::LayerColor::create(Color4B(0, 255, 255, 255));
	background->setAnchorPoint(Point(0, 0));
	background->setPosition(0, 0);
	addChild(background, -1);

	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("timo.plist", "timo.png");
	Texture2D *texture = TextureCache::sharedTextureCache()->textureForKey("timo.png");
	SpriteBatchNode *node = SpriteBatchNode::createWithTexture(texture);
	for (int i = 1; i <= 4; i++){
		timoAnimFrames.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(String::createWithFormat("timo_%d.png", i)->getCString()));
	}
	Animation *heroAnim = Animation::createWithSpriteFrames(timoAnimFrames, 0.5f);
	Sprite* s = Sprite::createWithSpriteFrameName("timo_1.png");
	s->runAction(RepeatForever::create(Animate::create(heroAnim)));
	s->setAnchorPoint(Point(0.5, 0));
	s->setPosition(winSize.width / 2, winSize.height / 4);
	addChild(s);

	Sprite* mogu = Sprite::create("moguscore.png");
	int moguNum = UserDefault::getInstance()->getIntegerForKey("moguNum");
	mogu->setAnchorPoint(Point(1, 0.5));
	mogu->setPosition(origin.x + winSize.width * 2 / 3, origin.y + winSize.height * 3 / 4);
	addChild(mogu);

	char string[20] = { 0 };
	sprintf(string, ":%d ", moguNum);
	auto label1 = Label::create(string, "Arial", 32);
	label1->setPosition(Vec2(origin.x + mogu->getPositionX() + 15, origin.y + mogu->getPositionY()));
	label1->setAnchorPoint(Point(0, 0.5));
	addChild(label1);

	char string1[20] = { 0 };
	int scoreNum = UserDefault::getInstance()->getIntegerForKey("scoreNum");
	sprintf(string1, "score :%d ", scoreNum);
	auto label2 = Label::create(string1, "Arial", 32);
	label2->setPosition(origin.x + winSize.width / 3, origin.y + winSize.height * 3 / 4);
	addChild(label2);


	auto restart = MenuItemImage::create(
		"restart.png",
		"restart.png",
		CC_CALLBACK_1(gameOver::restartGame, this));
	restart->setAnchorPoint(Point(0.5, 0.5));
	restart->setPosition(origin.x + winSize.width / 3, origin.y + winSize.height / 4);

	auto exit = MenuItemImage::create(
		"exit.png",
		"exit.png",
		CC_CALLBACK_1(gameOver::exitGame, this));
	exit->setAnchorPoint(Point(0.5, 0.5));
	exit->setPosition(origin.x + winSize.width * 2 / 3, origin.y + winSize.height / 4);

	auto menu = Menu::create(restart, exit, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, 2);

}

void gameOver::restartGame(cocos2d::Ref* pSender){
		UserDefault::getInstance()->setIntegerForKey("moguNum", 0);
		auto scene = ChooseLevelScene::createScene();
		auto transition = TransitionProgressRadialCW::create(1.0f, scene);
		Director::getInstance()->replaceScene(transition);
	}

void gameOver::exitGame(cocos2d::Ref* pSender){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
		MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
		return;
#endif

		Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
#endif
	}


