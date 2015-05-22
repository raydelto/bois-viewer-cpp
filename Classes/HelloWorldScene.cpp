#include "HelloWorldScene.h"
#include "ShipCreatedDto.h"

USING_NS_CC;

auto logger = (Stormancer::FileLogger*)Stormancer::ILogger::instance(new Stormancer::FileLogger);

void HelloWorld::shipUpdate(Stormancer::uint16 id, float x, float y, float rot)
{
	wstringstream ss;
	ss << id << L" " << x << L" " << y << L" " << rot << endl;
	logger->log(ss.str());
	if (Stormancer::Helpers::mapContains(ships, id))
	{
		Ship& ship = ships[id];
		ship.x = x;
		ship.y = y;
		ship.rot = rot;
		ship.sprite->setPosition(x, y);
	}
	else
	{
		shipAdd(id, x, y, rot);
	}
}

void HelloWorld::shipAdd(Stormancer::uint16 id, float x, float y, float rot)
{
	if (Stormancer::Helpers::mapContains(ships, id))
	{
		shipRemove(id);
	}
	auto sprite = Sprite::create("ship.png");
	ships[id] = { id, x, y, rot, sprite };
	sprite->setPosition(x, y);
	this->addChild(sprite, 0);
}

void HelloWorld::shipRemove(Stormancer::uint16 id)
{
	if (Stormancer::Helpers::mapContains(ships, id))
	{
		this->removeChild(ships[id].sprite);
		ships.erase(id);
	}
}

Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	//auto menu = Menu::create(closeItem, NULL);
	//menu->setPosition(Vec2::ZERO);
	//this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label

	//auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);

	// position the label on the center of the screen
	//label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	//this->addChild(label, 1);

	// add "HelloWorld" splash screen"
	//auto sprite = Sprite::create("HelloWorld.png");

	// position the sprite on the center of the screen
	//sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	// add the sprite as a child to this layer
	//this->addChild(sprite, 0);

	auto sprite = Sprite::create("ship.png");
	sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(sprite, 0);
	
	auto klistener = EventListenerKeyboard::create();

	klistener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			this->left = true;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			this->right = true;
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
			this->up = true;
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			this->down = true;
			break;
		default:
			break;
		}
	};

	klistener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			this->left = false;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			this->right = false;
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
			this->up = false;
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			this->down = false;
			break;
		default:
			break;
		}
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(klistener, this);
	
	config = shared_ptr<Stormancer::Configuration>(new Stormancer::Configuration(L"d81fc876-6094-3d92-a3d0-86d42d866b96", L"boids-demo"));
	client = shared_ptr<Stormancer::Client>(new Stormancer::Client(config.get()));

	task = client->getPublicScene(L"main-session").then([this](shared_ptr<Stormancer::Scene> sc) {
		scene = sc;

		scene->addRoute(L"position.update", [this](shared_ptr<Stormancer::Packet<Stormancer::IScenePeer>> p) {
			while (p->stream->rdbuf()->in_avail())
			{
				Stormancer::uint16 id;
				float x, y, rot;
				p->stream->read((char*)&id, sizeof(id));
				p->stream->read((char*)&x, sizeof(x));
				p->stream->read((char*)&y, sizeof(y));
				p->stream->read((char*)&rot, sizeof(rot));
				this->shipUpdate(id, x, y, rot);
			}
		});
		scene->addRoute(L"ship.add", [this](shared_ptr<Stormancer::Packet<Stormancer::IScenePeer>> p) {
			ShipCreatedDto shipCreatedDto(p->stream);
			this->shipAdd(shipCreatedDto.id, shipCreatedDto.x, shipCreatedDto.y, shipCreatedDto.rot);
		});
		scene->addRoute(L"ship.remove", [this](shared_ptr<Stormancer::Packet<Stormancer::IScenePeer>> p) {
			Stormancer::uint16 id;
			*p->stream >> id;
			this->shipRemove(id);
		});

		return scene->connect().then([]() {
			logger->log(L"Connected");
		});
	});

	try
	{
		task.wait();
	}
	catch (const exception &e)
	{
		logger->log(e);
	}

	this->scheduleUpdate();

	return true;
}

void HelloWorld::update(float delta)
{
	if (left)
	{
		this->setPositionX(this->getPositionX() + cameraScrollSpeed * delta);
	}
	if (right)
	{
		this->setPositionX(this->getPositionX() - cameraScrollSpeed * delta);
	}
	if (up)
	{
		this->setPositionY(this->getPositionY() - cameraScrollSpeed * delta);
	}
	if (down)
	{
		this->setPositionY(this->getPositionY() + cameraScrollSpeed * delta);
	}
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
