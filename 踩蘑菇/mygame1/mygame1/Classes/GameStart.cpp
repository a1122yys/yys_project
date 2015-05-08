#include "GameStart.h"
#include "ChooseLevelScene.h"
USING_NS_CC;

Scene* GameStart::createScene(){
	auto scene = Scene::create();
    auto layer = GameStart::create();
	scene->addChild(layer);
	return scene;
}
bool GameStart::init(){
	if (!Layer::init())
	{
		return false;
	}
	Size winSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Sprite * background = Sprite::create("start_bg.png");
	background->setPosition(Vec2(origin.x + winSize.width / 2, origin.y + winSize.height/2));
	addChild(background);
	Sprite* xiaomogu = Sprite::create("mogu_01.png");
	xiaomogu->setPosition(Point(winSize.width * 2 / 3, winSize.height / 4));
	addChild(xiaomogu, 2, 2);
	Sprite* damogu = Sprite::create("moguscore.png");
	damogu->setPosition(Point(winSize.width * 5 / 6, winSize.height / 4 + xiaomogu->getContentSize().height/2));
	addChild(damogu, 2, 1);

	Sprite* mo = Sprite::create("mo.png");
	mo->setPosition(Point(winSize.width / 2, winSize.height + mo->getContentSize().height / 2));
	addChild(mo,2);
	Sprite* cai = Sprite::create("cai.png");
	cai->setPosition(Point((winSize.width / 2 - mo->getContentSize().width), winSize.height + cai->getContentSize().height / 2));
	addChild(cai, 2);
	Sprite* gu = Sprite::create("gu.png");
	gu->setPosition(Point((winSize.width / 2 + mo->getContentSize().width), winSize.height+gu->getContentSize().height/2));
	addChild(gu, 2);

	Sequence* sequence1 = Sequence::create(
		MoveTo::create(0.5f, Point((winSize.width / 2 + mo->getContentSize().width), winSize.height * 3 / 4)),
		JumpTo::create(1.0f, Point((winSize.width / 2 + mo->getContentSize().width), winSize.height * 3 / 4), winSize.height / 10 ,1),
		JumpTo::create(1.0f, Point((winSize.width / 2 + mo->getContentSize().width), winSize.height * 3 / 4), winSize.height / 16, 1),
		NULL);
	Sequence* sequence2 = Sequence::create(
		MoveTo::create(0.5f, Point((winSize.width / 2 - mo->getContentSize().width), winSize.height * 3 / 4)),
		JumpTo::create(1.0f, Point((winSize.width / 2 - mo->getContentSize().width), winSize.height * 3 / 4), winSize.height / 10, 1),
		JumpTo::create(1.0f, Point((winSize.width / 2 - mo->getContentSize().width), winSize.height * 3 / 4), winSize.height / 16, 1),
		NULL);
	Sequence* sequence3 = Sequence::create(
		MoveTo::create(2.0f, Point(winSize.width / 2, winSize.height * 3 / 4)),
		JumpTo::create(1.0f, Point(winSize.width / 2, winSize.height * 3 / 4), winSize.height / 10, 1),
		JumpTo::create(1.0f, Point(winSize.width / 2, winSize.height * 3 / 4), winSize.height / 16, 1),
		NULL);
	gu->runAction(sequence1);
	cai->runAction(sequence2);
	mo->runAction(sequence3);


	readTexture();
	scheduleOnce(schedule_selector(GameStart::removeMogu), 3.0f);
	scheduleOnce(schedule_selector(GameStart::switchScene), 4.0f);
	return true;
}
void GameStart::readTexture(){                     //¶ÁÈ¡.plistÍ¼Æ¬
	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("heros.plist", "heros.png");
	Texture2D *texture = TextureCache::sharedTextureCache()->textureForKey("heros.png");
	SpriteBatchNode *node = SpriteBatchNode::createWithTexture(texture);
	for (int i = 5; i <= 8; i++){
		actionAnimFrames.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(String::createWithFormat("hero_0%d.png", i)->getCString()));
	}
	Animation *heroAnim = Animation::createWithSpriteFrames(actionAnimFrames, 0.1f);
	Size winSize = Director::getInstance()->getWinSize();
	
	
	s = Sprite::createWithSpriteFrameName("hero_05.png");
	s->setPosition(Point(winSize.width/4,winSize.height/4));
	s->setAnchorPoint(Point(0.5, 0.2));
	action = RepeatForever::create(Animate::create(heroAnim));
	s->runAction(action);


	Sequence* sequence = Sequence::create(
		MoveTo::create(2.0f,Point(winSize.width/2,winSize.height/4)),
		CallFunc::create(this, callfunc_selector(GameStart::bearMoveEnd)),
		JumpTo::create(2.0f,Point(winSize.width*5/6,winSize.height/4),winSize.height/5,2),
		NULL);
	s->runAction(sequence);
	addChild(s,2);
	
}
void GameStart::removeMogu(float t){
	this->removeChildByTag(2);
}
void GameStart::bearMoveEnd(){
	s->stopAction(action);
}
void GameStart::switchScene(float t){
	this->removeChildByTag(1);
	auto scene = ChooseLevelScene::createScene();
	auto transition=TransitionProgressRadialCW::create(1.0f, scene);
	Director::getInstance()->replaceScene(transition);
}