#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <stormancer.h>

struct Ship
{
	Stormancer::uint16 id;
	float x;
	float y;
	float rot;
	cocos2d::Sprite* sprite;
};

class HelloWorld : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(HelloWorld);

	void update(float delta) override;

public:
	void shipUpdate(Stormancer::uint16 id, float x, float y, float rot);
	void shipAdd(Stormancer::uint16 id, float x, float y, float rot);
	void shipRemove(Stormancer::uint16 id);

public:
	shared_ptr<Stormancer::Configuration> config = nullptr;
	shared_ptr<Stormancer::Client> client = nullptr;
	shared_ptr<Stormancer::Scene> scene = nullptr;
	pplx::task<void> task;
	map<Stormancer::uint16, Ship> ships;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool mouseHold = false;
	float cameraScrollSpeed = 100;
};

#endif // __HELLOWORLD_SCENE_H__
