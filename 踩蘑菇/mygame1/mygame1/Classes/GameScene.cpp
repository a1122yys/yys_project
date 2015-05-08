#include "GameScene.h"
#include"shop.h"
#include"help.h"
#include"gameOverScene.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;
using namespace CocosDenshion;
Scene* GameScene::createScene(){
	auto scene = Scene::createWithPhysics();
	auto layer = GameScene::create();
	layer->setPhyWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}
GameScene::GameScene(){
	_bear = NULL;
	_heroAction = NULL;
	_moveAction = NULL;
}
bool GameScene::init(){
	bool bRet = false;
	do
	{
		if (!Layer::init())
		{
			return false;
		}
		Size winSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();
		addMusic();
		this->setTouchEnabled(true);
		isOnTouch = false;
		levelNum = UserDefault::getInstance()->getFloatForKey("Level");
		Sprite* s = Sprite::create("jishu_2.png");
		s->setPosition(-100, -100);
		addChild(s, 2, 95);
		jinbis.pushBack(s);
		mogus.pushBack(s);
		gamedaojus.pushBack(s);
		moguNum = 0;
		jinbiNum = 1400;
		addjishu();
		addCaotiao();
		readTexture();
		addListence();
		isOnTouch = false;
		addDaoJuBao();
		start();
		schedule(schedule_selector(GameScene::addCrash));
		bRet = true;
	} while (0);
	return bRet;
}

void GameScene::readTexture(){                     //读取.plist图片
	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("heros.plist", "heros.png");
	Texture2D *texture = TextureCache::sharedTextureCache()->textureForKey("heros.png");
	SpriteBatchNode *node = SpriteBatchNode::createWithTexture(texture);
	for (int i = 5; i <= 8; i++){
		heroAnimFrames.pushBack(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(String::createWithFormat("hero_0%d.png", i)->getCString()));
	}
	Animation *heroAnim = Animation::createWithSpriteFrames(heroAnimFrames, 0.1f);
	Size winSize = Director::getInstance()->getWinSize();
	//this->setBear(Sprite::createWithSpriteFrameName("hero_05.png"));
	hero = Sprite::createWithSpriteFrameName("hero_05.png");
	hero->setPosition(Point(100, (winSize.height * 3) / 5 + 40));
	this->setHeroAction(RepeatForever::create(Animate::create(heroAnim)));
	PhysicsBody* hero1 = PhysicsBody::createCircle(hero->getContentSize().width / 2, PHYSICSBODY_MATERIAL_DEFAULT);
	//是否设置物体为静态
	//ballBody->setDynamic(false);
	//设置物体的恢复力
	hero1->getShape(0)->setRestitution(0.0f);
	//设置物体的摩擦力
	hero1->getShape(0)->setFriction(0.0f);
	//设置物体密度
	hero1->getShape(0)->setDensity(0.0f);
	//设置质量
	hero1->getShape(0)->setMass(0);
	//设置物体是否受重力系数影响
	hero1->setGravityEnable(true);
	//设置物体的冲力
	Vect force = Vect(0.0f, 0.0f);
	hero1->applyImpulse(force);
	//把物体添加到精灵中
	hero->setPhysicsBody(hero1);
	hero->getPhysicsBody()->setCategoryBitmask(0x01);
	hero->getPhysicsBody()->setContactTestBitmask(0x01);
	hero->getPhysicsBody()->setCollisionBitmask(0x01);
	//node->addChild(this->getBear());
	addChild(hero, 2);
	//Sprite* he = Sprite::create("help.png");
	//hero->addChild(he);
}
void GameScene::start(){
	/* 创建监听器对象 */
	auto contactListener = EventListenerPhysicsContact::create();
	/* 设置onContactBegin回调函数为onContactBegin 开始碰撞调用 */
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
	/* 设置onContactSeperate回调函数为onContactSeperate 结束碰撞调用 */
	contactListener->onContactSeperate = CC_CALLBACK_1(GameScene::onContactSeperate, this);
	/* 设置监听 最后在释放函数中不要忘记取消注册 */
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
}
void GameScene::addMusic(){
	Size winsize = Director::getInstance()->getWinSize();
	MenuItemImage* _turnOn = MenuItemImage::create(
		"menu_sound_on.png",
		"menu_sound_on.png");
	MenuItemImage* _turnOff = MenuItemImage::create(
		"menu_sound_off.png",
		"menu_sound_off.png");
	MenuItemToggle *toggleItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(GameScene::menuMusicCallback, this), _turnOn, _turnOff, NULL);
	toggleItem->setScale(0.8f);
	toggleItem->setPosition(Point(winsize.width - 12, winsize.height - 12));
	MenuItemImage* help = MenuItemImage::create(
		"help.png",
		"help.png",
		CC_CALLBACK_1(GameScene::switchToHelpLayer, this));
	help->setScale(0.8f);
	help->setPosition(12.5f, winsize.height - 12);
	auto menu = Menu::create(toggleItem, help, NULL);
	menu->setPosition(Point::ZERO);
	addChild(menu, 2);
	
	// 预加载背景音乐和音效
	SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("bg_music.mp3");
	SimpleAudioEngine::sharedEngine()->preloadEffect("tiao.mp3");
	SimpleAudioEngine::sharedEngine()->preloadEffect("chimogu.mp3");
	// 先默认播放背景音乐
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bg_music.mp3", true);
	SimpleAudioEngine::sharedEngine()->setEffectsVolume(1.0f);
	isPause = false;
}
void GameScene::menuMusicCallback(cocos2d::Ref* pSender){
	if (isPause == false)
	{
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
		SimpleAudioEngine::sharedEngine()->pauseAllEffects();
		isPause = true;
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
		SimpleAudioEngine::sharedEngine()->resumeAllEffects();
		isPause = false;
	}
}
void GameScene::addListence(){                                      //点击事件
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = [this](Touch* t, Event* e){
		Point beginPoint = t->getLocation();
		beginX = beginPoint.x;
		beginY = beginPoint.y;
		return true;
	};
	touchListener->onTouchEnded = [this](Touch* t, Event* e){
		if (isOnTouch == true||hero->getChildByTag(1)!=NULL){
			Point touchlocation = t->getLocation();
			Size winsize = Director::getInstance()->getWinSize();
			float speet = winsize.width / 3.0f;                   //移动速度
			Point movepoint = ccpSub(touchlocation, hero->getPosition());
			float distancemove = ccpLength(movepoint);
			float moveduration = distancemove / speet;
			if (movepoint.x < 0){
				hero->setFlippedX(true);
			}
			else{
				hero->setFlippedX(false);
			}
			hero->stopAction(this->getMoveAction());
			if (touchlocation.y - beginY != 0){
				if (touchlocation.x - beginX  < 0){
					hero->setFlippedX(true);
				}
				else{
					hero->setFlippedX(false);
				}
			}
			if (touchlocation.y - beginY == 0){
				int isOnTouchDaoju = 1;
				if (touchlocation.x - beginX == 0){
					Vector<Sprite*>::iterator j = gamedaojus.end();
					Sprite* p;
					while (j != gamedaojus.begin())
					{
						j--;
						p = *j;
						if (p->getBoundingBox().containsPoint(touchlocation)){
							isOnTouchDaoju = 0;
							Point s = hero->getPosition();
							switch (p->getTag())
							{
							case 202:
								isOnHuJia = true;
								hero->setScale(1.5f);
								scheduleOnce(schedule_selector(GameScene::OnTouchHuJia), 3.0f);
								break;
							case 203:
								hero->setPosition(s.x, s.y - 32);
								break;
							case 204:
								hero->setPosition(s.x, s.y + winsize.height / 5 + 32);
								break;
							case 205:{
										 int j = 0;
										 Vector<Sprite*>::iterator i = jinbis.end();
										 Sprite* p1;
										 while (i != jinbis.begin())
										 {
											 i--;
											 p1 = *i;
											 if (Rect(0, 0, 480, 320).containsPoint(p1->getPosition())){
												 jinbiNum = jinbiNum + 200;
												 UserDefault::getInstance()->setIntegerForKey("jinbiNum", jinbiNum);
												 Sequence* s=Sequence::create(
													 MoveTo::create(0.5f, Point(35 + winsize.width / 2, winsize.height - 15)),
													 CallFunc::create(this, callfunc_selector(GameScene::jinbiMoveEnd)),
													 NULL);
												 p1->runAction(s);
												 jinbishuzu[j] = p1->getTag();
												 j++;
											 }
										 }
							}
								break;
							case 206:
								adddaojuxiaoguo();
								break;
							default:
								break;
							}
							j = gamedaojus.erase(j);
							this->removeChildByTag(p->getTag());
							this->removeChild(p, true);
						}
						hero->stopAction(this->getHeroAction());
						tagMove = 0;

					}
					if (isOnTouchDaoju != 0){
						tagMove = 1;
						hero->runAction(this->getHeroAction());
						this->setMoveAction(Sequence::create(
							MoveTo::create(moveduration, Vec2(touchlocation.x, hero->getPosition().y)),
							CallFunc::create(this, callfunc_selector(GameScene::bearMoveEnd)),
							NULL));
					}
				}
			}
			else{
				if (touchlocation.y - beginY < 0){
				}
				tagMove = 0;
				hero->stopAction(this->getHeroAction());
				if (touchlocation.x - beginX  > 0){
					SimpleAudioEngine::sharedEngine()->playEffect("tiao.wav");
					if (hero->getPosition().y >= 4 * winsize.height / 5){
						this->setMoveAction(Sequence::create(
							JumpTo::create(1.0f, Point(hero->getPosition().x + 80, hero->getPosition().y), 30, 1),
							CallFunc::create(this, callfunc_selector(GameScene::bearMoveEnd)),
							NULL));
					}
					else{
						this->setMoveAction(Sequence::create(
							JumpTo::create(1.0f, Point(hero->getPosition().x + 80, hero->getPosition().y), winsize.height / 5 + 15, 1),
							CallFunc::create(this, callfunc_selector(GameScene::bearMoveEnd)),
							NULL));
					}
				}
				else{
					SimpleAudioEngine::sharedEngine()->playEffect("tiao.wav");
					if (hero->getPosition().y >= 4 * winsize.height / 5){
						this->setMoveAction(Sequence::create(
							JumpTo::create(1.0f, Point(hero->getPosition().x - 80, hero->getPosition().y), 30, 1),
							CallFunc::create(this, callfunc_selector(GameScene::bearMoveEnd)),
							NULL));
					}
					else{
						this->setMoveAction(Sequence::create(
							JumpTo::create(1.0f, Point(hero->getPosition().x - 80, hero->getPosition().y), winsize.height / 5 + 15, 1),
							CallFunc::create(this, callfunc_selector(GameScene::bearMoveEnd)),
							NULL));
					}
				}
			}
			if (this->getMoveAction() != NULL){
				hero->stopAction(this->getMoveAction());
			}
			hero->runAction(this->getMoveAction());
		}
	};
	dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}
void GameScene::adddaojuxiaoguo(){
	Sprite* wudi = Sprite::create("wudi.png");
	wudi->setCascadeOpacityEnabled(true);
	wudi->setOpacity(200);
	wudi->setPosition(hero->getContentSize().width / 2, hero->getContentSize().height / 2);
	hero->addChild(wudi,1,1);
	scheduleOnce(schedule_selector(GameScene::xiaoguoshixian),5.0f);
}
void GameScene::xiaoguoshixian(float t){
	hero->removeChildByTag(1);
}
void GameScene::stopAllAction(float t){
	schedule(schedule_selector(GameScene::pangduanbianjie),15.0f-levelNum);
}
void GameScene::OnTouchHuJia(float t){
	isOnHuJia = false;
	hero->setScale(1.0f);
}
void GameScene::jinbiMoveEnd(){
	for (int k = 0; k < 20;k++){
		Vector<Sprite*>::iterator i = jinbis.end();
		Sprite* p1;
		while (i != jinbis.begin())
		{
			i--;
			p1 = *i;
			if (jinbishuzu[k]==p1->getTag()){
				i = jinbis.erase(i);
				this->removeChildByTag(p1->getTag());
				this->removeChild(p1, true);
			}
		}

	}
}
bool GameScene::onContactBegin(PhysicsContact& contact){

	hero->getPhysicsBody()->resetForces();
	isOnTouch = true;
	hero->getPhysicsBody()->setGravityEnable(false);
	Size winsize = Director::getInstance()->getWinSize();
	Sprite* spriteA = (Sprite*)contact.getShapeA()->getBody()->getNode();
	Sprite* spriteB = (Sprite*)contact.getShapeB()->getBody()->getNode();
	int tagA = spriteA->getTag();
	int tagB = spriteB->getTag();
	if (tagB >= 1 && tagB <= 10){
		Vector<Sprite*>::iterator j = mogus.end();
		Sprite* p;
		while (j != mogus.begin())
		{
			j--;
			p = *j;
			if (p->getTag() == tagB){
				if (p->getTag() % 3 == 0){
					j = mogus.erase(j);
					hero->setScale(1.0f);
					this->removeChildByTag(p->getTag());
					this->removeChild(p, true);
					if (isOnHuJia == false){
						switchScene();
					}
				}
				else{
					SimpleAudioEngine::sharedEngine()->playEffect("chimogu.wav");
					moguNum = moguNum + 1;
					scoreNum = scoreNum + 20;
					j = mogus.erase(j);
					this->removeChildByTag(p->getTag());
					this->removeChild(p, true);
				}
				Vect force = Vect(0.0f, 0.0f);
				hero->getPhysicsBody()->setVelocity(force);
				hero->getPhysicsBody()->setGravityEnable(true);
			}

		}
		
	}
	if (tagB>30 && tagB<95){
		Vector<Sprite*>::iterator i = jinbis.end();
		Sprite* p1;
		while (i != jinbis.begin())
		{
			i--;
			p1 = *i;
			if (p1->getTag() == tagB){
				SimpleAudioEngine::sharedEngine()->playEffect("coin.wav");
				jinbiNum = jinbiNum + 100;
				scoreNum = scoreNum + 5;
				UserDefault::getInstance()->setIntegerForKey("jinbiNum", jinbiNum);
				i = jinbis.erase(i);
				this->removeChildByTag(p1->getTag());
				this->removeChild(p1, true);
				Vect force = Vect(0.0f, 0.0f);
				hero->getPhysicsBody()->setVelocity(force);
				hero->getPhysicsBody()->setGravityEnable(true);
			}
		}
	}
	if (tagB >= 100 && tagB<200){
		if (this->getMoveAction() != NULL){
			hero->stopAction(this->getMoveAction());
		}
		Vect force = Vect(0.0f, 0.0f);
		hero->getPhysicsBody()->setVelocity(force);
		Point caotiaoPoint = spriteB->getPosition();

		if (caotiaoPoint.y>hero->getPositionY()){
			hero->getPhysicsBody()->setGravityEnable(true);
		}
		else{
			if ((int)(hero->getPositionY() - caotiaoPoint.y) >= 15){
				hero->getPhysicsBody()->setGravityEnable(false);
				if ((int)((hero->getPositionY()) / (winsize.height / 5)) % 2 == 0){
					MoveTo* moveTo = MoveTo::create((15.0f - levelNum), Point(hero->getPositionX() - 480, hero->getPositionY()));
					this->setMoveAction(moveTo);
					hero->runAction(this->getMoveAction());
				}
				else{
					MoveTo* moveTo = MoveTo::create((15.0f - levelNum), Point(hero->getPositionX() + 480, hero->getPositionY()));
					this->setMoveAction(moveTo);
					hero->runAction(this->getMoveAction());
				}
			}
			else{
				hero->getPhysicsBody()->setGravityEnable(true);
			}
		}
	}
	return true;
}
void GameScene::onContactSeperate(PhysicsContact& contact){
	hero->getPhysicsBody()->setGravityEnable(true);
	if (tagMove == 1){
		hero->stopAction(this->getMoveAction());
	}
	hero->stopAction(this->getHeroAction());
	_bearMoving = false;
	isOnTouch = false;
}
void GameScene::addMogu(){
	time_t t;
	std::srand((unsigned int)time(&t));              //随机数种子
	Size winSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	bg_1 = Sprite::create("bg_01.png");
	bg_1->setPosition(winSize.width/2,winSize.height/2);
	addChild(bg_1, -1);
	bg_2 = Sprite::create("bg_02.png");
	bg_2->setPosition(winSize.width * 3/ 2, winSize.height / 2);
	addChild(bg_2, -1);


	Sprite* youdumogu;
	Sprite* wudumogu;
	for (int i = 1; i <= 10; i++){
		int suijiV = 1 + rand() % 4;
		if (suijiV % 2 == 0){
			if (i % 3 == 0){
				youdumogu = Sprite::create("mogu_02.png");
				youdumogu->setAnchorPoint(Point(0, 0));
				youdumogu->setPosition(Point(origin.x + 1440 - (i * 120) - 100, origin.y + (winSize.height * suijiV) / 5 + 8));
				PhysicsBody* caotiaophysics = PhysicsBody::createBox(Size(16, 16));
				caotiaophysics->setDynamic(false);
				youdumogu->setPhysicsBody(caotiaophysics);
				youdumogu->getPhysicsBody()->setCategoryBitmask(0x01);
				youdumogu->getPhysicsBody()->setContactTestBitmask(0x01);
				youdumogu->getPhysicsBody()->setCollisionBitmask(0x01);
				addChild(youdumogu, 2, i);
				mogus.pushBack(youdumogu);
			}
			else{
				wudumogu = Sprite::create("mogu_01.png");
				wudumogu->setAnchorPoint(Point(0, 0));
				wudumogu->setPosition(Point(origin.x + 1440 - (i * 120) - 100, origin.y + (winSize.height * suijiV) / 5 + 8));
				PhysicsBody* caotiaophysics = PhysicsBody::createBox(Size(16, 16));
				caotiaophysics->setDynamic(false);
				wudumogu->setPhysicsBody(caotiaophysics);
				wudumogu->getPhysicsBody()->setCategoryBitmask(0x01);
				wudumogu->getPhysicsBody()->setContactTestBitmask(0x01);
				wudumogu->getPhysicsBody()->setCollisionBitmask(0x01);
				addChild(wudumogu, 2, i);
				mogus.pushBack(wudumogu);
			}
		}
		else{
			if (i % 3 == 0){
				youdumogu = Sprite::create("mogu_02.png");
				youdumogu->setAnchorPoint(Point(0, 0));
				youdumogu->setPosition(Point(origin.x - 960 + (i * 120) + 55, origin.y + (winSize.height * suijiV) / 5 + 8));
				PhysicsBody* caotiaophysics = PhysicsBody::createBox(Size(16, 16));
				caotiaophysics->setDynamic(false);
				caotiaophysics->setGravityEnable(true);
				youdumogu->setPhysicsBody(caotiaophysics);
				youdumogu->getPhysicsBody()->setCategoryBitmask(0x01);
				youdumogu->getPhysicsBody()->setContactTestBitmask(0x01);
				youdumogu->getPhysicsBody()->setCollisionBitmask(0x01);
				addChild(youdumogu, 2, i);
				mogus.pushBack(youdumogu);
			}
			else{
				wudumogu = Sprite::create("mogu_01.png");
				wudumogu->setAnchorPoint(Point(0, 0));
				wudumogu->setPosition(Point(origin.x - 960 + (i * 120) + 55, origin.y + (winSize.height * suijiV) / 5 + 8));
				PhysicsBody* caotiaophysics = PhysicsBody::createBox(Size(16, 16));
				caotiaophysics->setDynamic(false);
				wudumogu->setPhysicsBody(caotiaophysics);
				wudumogu->getPhysicsBody()->setCategoryBitmask(0x01);
				wudumogu->getPhysicsBody()->setContactTestBitmask(0x01);
				wudumogu->getPhysicsBody()->setCollisionBitmask(0x01);
				addChild(wudumogu, 2, i);
				mogus.pushBack(wudumogu);
			}
		}
	}
}
void GameScene::bearMoveEnd(){
	Size winSize = Director::getInstance()->getVisibleSize();
	hero->stopAction(this->getHeroAction());
	
	if (tagMove == 1){
		if ((int)((hero->getPositionY()) / (winSize.height / 5)) % 2 == 0){
			MoveTo* moveTo = MoveTo::create((15.0f - levelNum), Point(hero->getPositionX() - 480, hero->getPositionY()));
			this->setMoveAction(moveTo);
			hero->runAction(this->getMoveAction());
		}
		else{
			MoveTo* moveTo = MoveTo::create((15.0f - levelNum), Point(hero->getPositionX() + 480, hero->getPositionY()));
			this->setMoveAction(moveTo);
			hero->runAction(this->getMoveAction());
		}
	}
	
	tagMove = 0;
}

void GameScene::addCaotiao(){
	time_t t;
	std::srand((unsigned int)time(&t));              //随机数种子
	Size winSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Sprite* caotiao;
	int num = 0;
	for (int j = 1; j <= 4; j++){
		m = j;
		int k = 1 + rand() % 4;
		int n = 1 + rand() % 10;
		for (int i = 0; i < 8; i++){
			caotiao = Sprite::create("caotiao.png");
			caotiao->setAnchorPoint(Point(0, 0));
			switch (j)
			{
			case 1:caotiao->setPosition(Vec2(origin.x + winSize.width - 76 - (i * 120), origin.y + (winSize.height * j) / 5));
				caotiaos1.pushBack(caotiao);
				caotiaos.pushBack(caotiao);
				break;
			case 2:caotiao->setPosition(Vec2(origin.x + (i * 120), origin.y + (winSize.height * j) / 5));
				caotiaos2.pushBack(caotiao);
				caotiaos.pushBack(caotiao);
				break;
			case 3:caotiao->setPosition(Vec2(origin.x + winSize.width - 76 - (i * 120), origin.y + (winSize.height * j) / 5));
				caotiaos3.pushBack(caotiao);
				caotiaos.pushBack(caotiao);
				break;
			case 4:caotiao->setPosition(Vec2(origin.x + (i * 120), origin.y + (winSize.height * j) / 5));
				caotiaos4.pushBack(caotiao);
				caotiaos.pushBack(caotiao);
				break;
			default:
				break;
			}
			//加入物体
			PhysicsBody* caotiaophysics = PhysicsBody::createBox(caotiao->getContentSize());
			caotiaophysics->setDynamic(false);
			caotiao->setPhysicsBody(caotiaophysics);
			caotiao->getPhysicsBody()->setCategoryBitmask(0x01);
			caotiao->getPhysicsBody()->setContactTestBitmask(0x01);
			caotiao->getPhysicsBody()->setCollisionBitmask(0x01);
			addChild(caotiao, 2, 100 + num);
			num++;
		}
	}
	addMogu();
	addJinBi();
	moguGundong();
	gundong();
	schedule(schedule_selector(GameScene::pangduanbianjie), (15.0f - levelNum));
}
void GameScene::gundong(){
	runRand = 1 + rand() % 4;
	Size winSize = Director::getInstance()->getVisibleSize();
	Vector<Sprite*>::iterator j;
	Sprite* p;
	for (j = caotiaos1.begin(); j != caotiaos1.end(); j++){
		p = *j;
		Point caotiaoPoint = p->getPosition();
		MoveTo* moveTo = MoveTo::create((15.0f - levelNum), Vec2(caotiaoPoint.x + 480, caotiaoPoint.y));
		p->runAction(moveTo);
	}
	for (j = caotiaos2.begin(); j != caotiaos2.end(); j++){
		p = *j;
		Point caotiaoPoint = p->getPosition();
		MoveTo* moveTo = MoveTo::create((15.0f - levelNum), Vec2(caotiaoPoint.x - 480, caotiaoPoint.y));
		p->runAction(moveTo);
	}
	for (j = caotiaos3.begin(); j != caotiaos3.end(); j++){
		p = *j;
		Point caotiaoPoint = p->getPosition();
		MoveTo* moveTo = MoveTo::create((15.0f - levelNum), Vec2(caotiaoPoint.x + 480, caotiaoPoint.y));
		p->runAction(moveTo);
	}
	for (j = caotiaos4.begin(); j != caotiaos4.end(); j++){
		p = *j;
		Point caotiaoPoint = p->getPosition();
		MoveTo* moveTo = MoveTo::create((15.0f - levelNum), Vec2(caotiaoPoint.x - 480, caotiaoPoint.y));
		p->runAction(moveTo);
	}
	MoveTo* moveTo1 = MoveTo::create((15.0f - levelNum) , Vec2(bg_1->getPositionX() - 240, bg_1->getPositionY()));
	MoveTo* moveTo2 = MoveTo::create((15.0f - levelNum) , Vec2(bg_2->getPositionX() - 240, bg_2->getPositionY()));
	bg_1->runAction(moveTo1);
	bg_2->runAction(moveTo2);
}
void GameScene::pangduanbianjie(float t){
	Size winSize = Director::getInstance()->getVisibleSize();
	float levelNum1 = UserDefault::getInstance()->getFloatForKey("Level");
	
	switch ((int)levelNum1)
	{
	case 1:{
			   if (moguNum >= 2){
				   UserDefault::getInstance()->setFloatForKey("Level", 2.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 2:{
			   if (moguNum >= 15){
				   UserDefault::getInstance()->setFloatForKey("Level", 3.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 3:{
			   if (moguNum >= 21){
				   UserDefault::getInstance()->setFloatForKey("Level", 4.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 4:{
			   if (moguNum >= 26){
				   UserDefault::getInstance()->setFloatForKey("Level", 5.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 5:{
			   if (moguNum >= 31){
				   UserDefault::getInstance()->setFloatForKey("Level", 6.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 6:{
			   if (moguNum >= 35){
				   UserDefault::getInstance()->setFloatForKey("Level", 7.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 7:{
			   if (moguNum >= 39){
				   UserDefault::getInstance()->setFloatForKey("Level", 8.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 8:{
			   if (moguNum >= 42){
				   UserDefault::getInstance()->setFloatForKey("Level", 9.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 9:{
			   if (moguNum >= 44){
				   UserDefault::getInstance()->setFloatForKey("Level", 10.0f);
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	case 10:{
			   if (moguNum >= 50){
				   deleteVector();
				   addJinBi();
				   addMogu();
				   unschedule(schedule_selector(GameScene::pangduanbianjie));
			   }
	}
		break;
	default:
		break;
	}
	levelNum = UserDefault::getInstance()->getFloatForKey("Level");
	if (levelNum > levelNum1){
		char string[20] = { 0 };
		sprintf(string, "Level:%d ", (int)levelNum);
		auto label1 = Label::create(string, "Arial", 50);
		label1->setAnchorPoint(Point(0.5, 0.5));
		label1->setColor(cocos2d::Color3B::YELLOW);
		label1->setPosition(Point(winSize.width/2, winSize.height/2));
		SimpleAudioEngine::sharedEngine()->playEffect("NextGameRound.wav");
		Sprite* guoguan = Sprite::create("guoguan.png");
		guoguan->setPosition(Point(-guoguan->getContentSize().width / 2, winSize.height / 2));
		MoveTo* moveTo = MoveTo::create(2.0f, Point(winSize.width + guoguan->getContentSize().width / 2,winSize.height/2));
		guoguan->runAction(moveTo);
		addChild(guoguan, 2, 333);
		addChild(label1,2,520);
		UserDefault::getInstance()->setIntegerForKey("jinbiNum",(jinbiNum + (levelNum *200)));
		scheduleOnce(schedule_selector(GameScene::removelabel),2.0f);
		schedule(schedule_selector(GameScene::pangduanbianjie), (15.0f - levelNum));
	}
	gundong();
	moguGundong();
}
void GameScene::removelabel(float t){
	this->removeChildByTag(520);
	this->removeChildByTag(333);
}
void GameScene::moguGundong(){
	Size winSize = Director::getInstance()->getVisibleSize();
	Vector<Sprite*>::iterator j;
	Sprite* p;
	for (j = mogus.begin(); j != mogus.end(); j++){
		p = *j;
		Point moguPoint = p->getPosition();
		if (p->getTag() != 95){
			if ((int)(moguPoint.y / (winSize.height / 5)) % 2 == 0){
				moguMoveTo = MoveTo::create((15.0f - levelNum), Vec2(moguPoint.x - 480, moguPoint.y));
				p->runAction(moguMoveTo);
			}
			else{
				moguMoveTo = MoveTo::create((15.0f - levelNum), Vec2(moguPoint.x + 480, moguPoint.y));
				p->runAction(moguMoveTo);
			}
		}
	}
	addjinbigundong();
}
void GameScene::addjinbigundong(){
	Size winSize = Director::getInstance()->getVisibleSize();
	Vector<Sprite*>::iterator j;
	Sprite* p;
	for (j = jinbis.begin(); j != jinbis.end(); j++){
		p = *j;
		Point jinbiPoint = p->getPosition();
		if (p->getTag() != 95){
			if ((int)(jinbiPoint.y / (winSize.height / 5)) % 2 == 0){
				jinbiMoveTo = MoveTo::create((15.0f - levelNum), Vec2(jinbiPoint.x - 480, jinbiPoint.y));
				p->runAction(jinbiMoveTo);
			}
			else{
				jinbiMoveTo = MoveTo::create((15.0f - levelNum), Vec2(jinbiPoint.x + 480, jinbiPoint.y));
				p->runAction(jinbiMoveTo);
			}
		}
	}
}
void GameScene::addCrash(float t){
	Size winSize = Director::getInstance()->getVisibleSize();
	char string[10] = {};
	sprintf(string, "%d", moguNum);
	moguNumLabel->setString(string);
	
	char string1[10] = {};
	jinbiNum = UserDefault::getInstance()->getIntegerForKey("jinbiNum");
	sprintf(string1, "%d", jinbiNum);
	jinbiNumLabel->setString(string1);
	
	char string2[10] = {};
	sprintf(string2, "%d", scoreNum);
	scoreLabel->setString(string2);

	addDaoju();
	if (hero->getPositionX()<11 || hero->getPositionX() > winSize.width - 11 || hero->getPositionY()<16){
		switchScene();
	}
	if (bg_1->getPositionX() == (-winSize.width / 2)){
		bg_1->setPosition(winSize.width * 3 / 2, winSize.height / 2);
	}
	if (bg_2->getPositionX() == (-winSize.width / 2)){
		bg_2->setPosition(winSize.width * 3 / 2, winSize.height / 2);
	}
	Vector<Sprite*>::iterator j;
	Sprite* p;
	for (j = caotiaos1.begin(); j != caotiaos1.end(); j++){
		p = *j;
		Point caotiaoPoint = p->getPosition();
		if (caotiaoPoint.x > winSize.width + 38){
			p->setPosition(caotiaoPoint.x - winSize.width * 2, caotiaoPoint.y);
		}
	}
	for (j = caotiaos2.begin(); j != caotiaos2.end(); j++){
		p = *j;
		Point caotiaoPoint = p->getPosition();
		if (caotiaoPoint.x < -38){
			p->setPosition(caotiaoPoint.x + winSize.width * 2, caotiaoPoint.y);
		}
	}for (j = caotiaos3.begin(); j != caotiaos3.end(); j++){
		p = *j;
		Point caotiaoPoint = p->getPosition();
		if (caotiaoPoint.x > winSize.width + 38){
			p->setPosition(caotiaoPoint.x - winSize.width * 2, caotiaoPoint.y);
		}
	}
	for (j = caotiaos4.begin(); j != caotiaos4.end(); j++){
		p = *j;
		Point caotiaoPoint = p->getPosition();
		if (caotiaoPoint.x < -38){
			p->setPosition(caotiaoPoint.x + winSize.width * 2, caotiaoPoint.y);
		}
	}
	for (j = mogus.begin(); j != mogus.end(); j++){
		p = *j;
		Point moguPoint = p->getPosition();
		if (p->getTag() != 95){
			if ((int)(moguPoint.y / (winSize.height / 5)) % 2 == 0){
				if (moguPoint.x < -8){
					p->setPosition(moguPoint.x + winSize.width * 5 /2 , moguPoint.y);
				}
			}
			else{
				if (moguPoint.x > winSize.width + 8){
					p->setPosition(moguPoint.x - winSize.width * 5 / 2, moguPoint.y);
				}
			}
		}

	}
	for (j = jinbis.begin(); j != jinbis.end(); j++){
		p = *j;
		Point jinbiPoint = p->getPosition();
		if (p->getTag() != 95){
			if ((int)(jinbiPoint.y / (winSize.height / 5)) % 2 == 0){
				if (jinbiPoint.x < -8){
					p->setPosition(jinbiPoint.x + (120 * (10 + (int)levelNum * 2)), jinbiPoint.y);
				}
			}
			else{
				if (jinbiPoint.x > winSize.width + 8){
					p->setPosition(jinbiPoint.x - (120 * (10 + (int)levelNum * 2)), jinbiPoint.y);
				}
			}
		}
	}
}
void GameScene::addDaoJuBao(){
	Size winSize = Director::getInstance()->getVisibleSize();
	auto bao = MenuItemImage::create(
		"bao.png",
		"bao1.png",
		CC_CALLBACK_1(GameScene::switchToLayer, this));
	bao->setAnchorPoint(Point(0.5, 0.5));
	bao->setPosition(35, 35);
	auto menu = Menu::create(bao, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, 2);
	Sprite* daoju;
	for (int i = 12; i <= 17; i++){
		daoju = Sprite::create("gezi.png");
		daoju->setPosition(Point(100 + (((i % 10) - 2) * 60), 30));
		daojubaos.pushBack(daoju);
		addChild(daoju, 2, i);
	}
}
void GameScene::addDaoju(){
	Vector<Sprite*>::iterator j;
	Sprite* p;
	int tag = UserDefault::getInstance()->getIntegerForKey("daojuNum");
	if (tag != NULL&&tag != 0){
		for (j = daojubaos.begin(); j != daojubaos.end(); j++){
			p = *j;
			if (p->getTag() == tag + 10){
				if (tag==7){
					HavaDaHuaiDan = true;
				}
				char daju1[20] = { 0 };
				sprintf(daju1, "daoju_%d.png", tag);
				Sprite* daoju = Sprite::create(daju1);
				daoju->setPosition(p->getPositionX(), p->getPositionY());
				if (this->getChildByTag(tag + 200)==NULL){
					addChild(daoju, 3, tag + 200);
					gamedaojus.pushBack(daoju);
				}
				UserDefault::getInstance()->setIntegerForKey("daojuNum", 0);
			}
		}
	}
}
void GameScene::switchToHelpLayer(cocos2d::Ref* pSender){
	Scene* scene = help::createScene();
	Director::getInstance()->pushScene(scene);
}
void GameScene::switchToLayer(cocos2d::Ref* pSender){
	UserDefault::getInstance()->setIntegerForKey("jinbiNum", jinbiNum);
	UserDefault::getInstance()->flush();
	Scene* scene = shop::createScene();
	Director::getInstance()->pushScene(scene);
}
void GameScene::addJinBi(){
	time_t t;
	std::srand((unsigned int)time(&t));              //随机数种子
	Size winSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Sprite* jinbi;
	int oldsuijiV = 0;
	int oldsuijiH = 0;
	for (int i = 1; i <= 10 + ((int)levelNum * 2); i++){
		int suijiV = 1 + rand() % 3;
		jinbi = Sprite::create("jishu_2.png");
		if (suijiV % 2 == 0){
			jinbi->setPosition(Point(origin.x + 1440 - (i * 120) + 55, origin.y + (winSize.height * suijiV) / 5 + 50));
		}
		else{
			jinbi->setPosition(Point(origin.x - 960 + (i * 120) + 55, origin.y + (winSize.height * suijiV) / 5 + 50));
		}
		jinbis.pushBack(jinbi);
		PhysicsBody* caotiaophysics = PhysicsBody::createBox(Size(15, 15));
		caotiaophysics->setDynamic(false);
		jinbi->setPhysicsBody(caotiaophysics);
		jinbi->getPhysicsBody()->setCategoryBitmask(0x01);
		jinbi->getPhysicsBody()->setContactTestBitmask(0x01);
		jinbi->getPhysicsBody()->setCollisionBitmask(0x01);
		addChild(jinbi, 2, i + 30);
	}
}
void GameScene::addjishu(){
	Sprite* s;
	Size winSize = Director::getInstance()->getVisibleSize();
	for (int i = 21; i <= 23; i++){
		char jishu[20] = { 0 };
		sprintf(jishu, "jishu_%d.png", (i - 20));
		s = Sprite::create(jishu);
		s->setPosition(((i - 20)*winSize.width /4), winSize.height - 15);
		addChild(s, 0, i);
	}
	char temStr[20] = { 0 };
	sprintf(temStr, "%d", moguNum);
	moguNumLabel = LabelTTF::create(temStr, "Arial", 16);
	moguNumLabel->setPosition(35 + winSize.width / 4, winSize.height - 15);
	moguNumLabel->setColor(Color3B::BLACK);
	addChild(moguNumLabel, 1);

	char moguString[20] = { 0 };
	sprintf(moguString, "%d", jinbiNum);
	jinbiNumLabel = LabelTTF::create(moguString, "Arial", 16);
	jinbiNumLabel->setPosition(35 + winSize.width / 2, winSize.height - 15);
	jinbiNumLabel->setColor(Color3B::BLACK);
	addChild(jinbiNumLabel, 1);

	char score[20] = { 0 };
	sprintf(score, "%d", scoreNum);
	scoreLabel = LabelTTF::create(score, "Arial", 16);
	scoreLabel->setPosition(35 + winSize.width * 3 / 4, winSize.height - 15);
	scoreLabel->setColor(Color3B::BLACK);
	addChild(scoreLabel, 1);

}

void GameScene::switchScene(){
	Size winSize = Director::getInstance()->getVisibleSize();
	if (HavaDaHuaiDan==false){
		unschedule(schedule_selector(GameScene::addCrash));
		unschedule(schedule_selector(GameScene::pangduanbianjie));
		UserDefault::getInstance()->setIntegerForKey("moguNum", moguNum);
		UserDefault::getInstance()->setIntegerForKey("jinbiNum", jinbiNum);
		UserDefault::getInstance()->setIntegerForKey("scoreNum", scoreNum);
		UserDefault::getInstance()->flush();
		Scene* scene = gameOver::createScene();
		auto transion=TransitionMoveInT::create(1.0f, scene);
		Director::getInstance()->replaceScene(transion);
		SimpleAudioEngine::sharedEngine()->end();
	}
	else{
		hero->setPosition(Point(100, (winSize.height * 3) / 5 + 40));
		CCFadeIn* fadein = CCFadeIn::create(2);
		hero->runAction(fadein);
		Vector<Sprite*>::iterator j = gamedaojus.end();
		Sprite* p;
		while (j != gamedaojus.begin())
		{
			j--;
			p = *j;
			if (p->getTag() == 207){
				j = gamedaojus.erase(j);
				this->removeChildByTag(p->getTag());
				this->removeChild(p, true);
			}
		}
		HavaDaHuaiDan = false;
	}
}
void GameScene::deleteVector(){
	Vector<Sprite*>::iterator i = jinbis.end();
	Sprite* p1;
	while (i != jinbis.begin())
	{
		i--;
		p1 = *i;
			i = jinbis.erase(i);
			this->removeChildByTag(p1->getTag());
			this->removeChild(p1, true);
	}
	Vector<Sprite*>::iterator j = mogus.end();
	Sprite* p;
	while (j != mogus.begin())
	{
		j--;
		p = *j;

		j = mogus.erase(j);
		this->removeChildByTag(p->getTag());
		this->removeChild(p, true);
	}
	jinbis.clear();
	mogus.clear();
}

