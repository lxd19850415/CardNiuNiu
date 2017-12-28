#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#include ""

using namespace cocos2d;
using namespace cocos2d::ui;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);


    void sendMsg(Ref *pSender, Widget::TouchEventType type);


    //Uitls


    const char *connectorHost = "";
    int connectorPort = 0;
    const char *user = "";
    const char *channel = "";
    pc_client_t *pomelo_client;

    void login(const char *username, const char *Channel);
};

#endif // __HELLOWORLD_SCENE_H__
