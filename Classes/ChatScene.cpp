#include <json/stringbuffer.h>
#include <json/writer.h>
#include "ChatScene.h"
#include "LoginScene.h"
#include "UserScene.h"

USING_NS_CC;
using namespace std;

#define LINE_SPACE          50
#define TESTS_COUNT         5
#define FONT_NAME           "Thonburi"
#define FONT_SIZE           36
#define CHAT_LIMIT			20

#define REQ_TIMEOUT 10

std::string global_text[] =
{
    "Welcome To Pomelo Cocos2d-x Chat Demo",
    "This is a simple chat demo based on libpomelo and cocos2d-x",
    "You can send broadcast messages directly",
    "You can also send private message by @user at the head of your messages",
    "Enjoy with it"
};

static CCPoint s_tCurPos = CCPointZero;
static int cur = 0;

void onSendCallback(const pc_request_t *req, int status, const char *resp)
{

}

// on "init" you need to initialize your instance
bool Chat::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    CCRect visibleRect = VisibleRect::getVisibleRect();
    m_tBeginPos = ccp(0, VisibleRect::top().y - 50);
    CCLabelTTF *backLabel = CCLabelTTF::create("Back", "Arial", 25);
    CCMenuItemLabel *pBackMenuItem = CCMenuItemLabel::create(backLabel, this, menu_selector(Chat::backCallback));
    CCMenu *pBackMenu = CCMenu::create(pBackMenuItem, NULL);
    pBackMenu->setAnchorPoint(CCPointZero);
    pBackMenu->setPosition(ccp(VisibleRect::left().x + 35, VisibleRect::top().y - 20));
    addChild(pBackMenu, 10);

    CCLabelTTF *userLabel = CCLabelTTF::create("Users", "Arial", 25);
    CCMenuItemLabel *pUserMenuItem = CCMenuItemLabel::create(userLabel, this, menu_selector(Chat::userCallback));
    CCMenu *pUserMenu = CCMenu::create(pUserMenuItem, NULL);
    pUserMenu->setAnchorPoint(CCPointZero);
    pUserMenu->setPosition(ccp(VisibleRect::right().x - 40, VisibleRect::top().y - 20));
    addChild(pUserMenu, 10);

    CCLabelTTF *sendLabel = CCLabelTTF::create("Send", "Arial", 25);
    CCMenuItemLabel *pSendMenuItem = CCMenuItemLabel::create(sendLabel, this, menu_selector(Chat::sendCallback));
    CCMenu *pSendMenu = CCMenu::create(pSendMenuItem, NULL);
    pSendMenu->setAnchorPoint(CCPointZero);
    pSendMenu->setPosition(ccp(VisibleRect::right().x - 45, 25));
    addChild(pSendMenu, 10);

    channelText = CCLabelTTF::create(s_channel.c_str(), "Arial", 25);
    channelText->setPosition(ccp(VisibleRect::center().x, VisibleRect::top().y - 20));
    addChild(channelText, 10);

    m_layer = CCLayer::create();

    for (int i = 0; i < visibleRect.size.height / LINE_SPACE; ++i)
    {
        CCLabelTTF *label;
        if (i < TESTS_COUNT)
        {
            label = CCLabelTTF::create(global_text[i].c_str(), "Arial", 23, CCSizeMake(visibleRect.size.width, 50), kCCTextAlignmentLeft);
        }
        else
        {
            label = CCLabelTTF::create("", "Arial", 23, CCSizeMake(visibleRect.size.width, 50), kCCTextAlignmentLeft);
        }
        label->setColor(ccc3(32, 32, 32));
        label->setAnchorPoint(ccp(0, 0));
        label->setPosition(ccp(VisibleRect::left().x + 10, (m_tBeginPos.y - (i + 1) * LINE_SPACE) ));
        m_layer->addChild(label, 10, i);
    }

    m_layer->setContentSize(CCSizeMake(VisibleRect::getVisibleRect().size.width - 30, (TESTS_COUNT + 1) * (LINE_SPACE)));
    m_layer->setPosition(s_tCurPos);
    addChild(m_layer, 2);

    setTouchEnabled(true);

    CCLayerColor *layer1 = CCLayerColor::create(ccc4(255, 255, 255, 255), visibleRect.size.width, visibleRect.size.height);
    layer1->setCascadeColorEnabled(false);
    layer1->setPosition(ccp(0, 0));
    addChild(layer1, 1);

    CCLayerColor *layer2 = CCLayerColor::create(ccc4(0, 0, 0, 255), visibleRect.size.width, 40);
    layer2->setCascadeColorEnabled(false);
    layer2->setPosition(ccp(0, VisibleRect::top().y - 40));
    addChild(layer2, 8);

    CCLayerColor *layer3 = CCLayerColor::create(ccc4(72, 72, 72, 255), visibleRect.size.width, 50);
    layer3->setCascadeColorEnabled(false);
    layer3->setPosition(ccp(0, 0));
    addChild(layer3, 8);

    m_pTextField = CCTextFieldTTF::textFieldWithPlaceHolder("",
                   FONT_NAME,
                   FONT_SIZE);
    m_pTextField->setAnchorPoint(CCPointZero);
    m_pTextField->setPosition(ccp(VisibleRect::left().x + 10, 5));
    addChild(m_pTextField, 12);

    m_pTextField->setDelegate(this);



    //允许接收触摸事件
    this->setTouchEnabled(true);

    EventDispatcher* eventDispatcher = Director::getInstance()->getEventDispatcher();

    auto listen = EventListenerTouchAllAtOnce::create();
    listen->onTouchesBegan = CC_CALLBACK_2(Chat::onTouchesBegan,this);
    listen->onTouchesMoved = CC_CALLBACK_2(Chat::onTouchesMoved,this);
    listen->onTouchesEnded = CC_CALLBACK_2(Chat::onTouchesEnded,this);
    listen->onTouchesCancelled = CC_CALLBACK_2(Chat::onTouchesCancelled,this);
    eventDispatcher->addEventListenerWithSceneGraphPriority(listen,this);

    return true;
}

void Chat::onEnter()
{
    Layer::onEnter();
    CCLOG("ChatScene onEnter");
    Director::getInstance()->getScheduler()->scheduleSelector(
        schedule_selector(Chat::updateUICallback), this, 0, false);
}

void Chat::updateUICallback(float delta)
{
    if (!messageQueue)
    {
        return;
    }
    unsigned int count = 0;
    bool flag = 0;
    for (unsigned int i = cur; i < messageQueue->count(); i++, cur++)
    {
        __String *cstring = (__String * )messageQueue->objectAtIndex(i);



        rapidjson::Document m_doc;
        m_doc.Parse<0>(cstring->getCString());
        if (m_doc.HasParseError())
        {
            log("GetParaseError:%s\n", m_doc.GetParseError());
        }



        std::string msg = "";
        std::string from = "";
        std::string target = "";

        char * msgStr;
        if(m_doc.HasMember("msg"))
        {
            msgStr = (char *)m_doc["msg"].GetString();
            std::string msg = msgStr;
        }

        char * fromStr;
        if(m_doc.HasMember("from"))
        {
            fromStr = (char *)m_doc["from"].GetString();
            from = fromStr;
        }
        if (from == s_user)
        {
            continue;
        }

        char * targetStr;
        if(m_doc.HasMember("target"))
        {
            targetStr = (char *)m_doc["target"].GetString();
            target = targetStr;
        }
//        json_error_t error;
//        json_t *json = json_loads(cstring->getCString(), JSON_DECODE_ANY, &error);
//        std::string msg = json_string_value(json_object_get(json, "msg"));
//        std::string from = json_string_value(json_object_get(json, "from"));
//        if (from == s_user)
//        {
//            continue;
//        }
//        std::string target = json_string_value(json_object_get(json, "target"));
        if (target == "*")
        {
            target = "all";
        }
        std::string content = from + std::string(" says to ") + target + std::string(" :") + msg;

        CCLabelTTF *label = CCLabelTTF::create(content.c_str(), "Arial", 25, CCSizeMake(VisibleRect::getVisibleRect().size.width, 50), kCCTextAlignmentLeft);
        label->setColor(ccc3(32, 32, 32));
        label->setAnchorPoint(ccp(0, 0));
        count = m_layer->getChildrenCount();
        label->setPosition(ccp(VisibleRect::left().x + 10, (VisibleRect::top().y - (count + 2) * LINE_SPACE) ));
        m_layer->addChild(label, 10);
        flag = 1;
    }
    if (flag)
    {
        m_layer->setPosition(ccp(0, ((count + 2)* LINE_SPACE - VisibleRect::getVisibleRect().size.height + 50)));
        m_layer->setContentSize(CCSizeMake(VisibleRect::getVisibleRect().size.width - 20, (count + 1) * (LINE_SPACE)));
    }
}

void Chat::menuCallback(cocos2d::CCObject *sender)
{
    CCDirector *pDirector = CCDirector::sharedDirector();
    CCScene *pScene = Login::scene();
    pDirector->replaceScene(pScene);
}

void Chat::backCallback(cocos2d::CCObject *sender)
{
    // release relative resource with pc_request_t
    CCDirector::sharedDirector()->getScheduler()->pauseTarget(this);
//    pc_client_destroy(client);

    pc_client_cleanup(client);

//    PC_TEST_ASSERT(pc_client_state(client) == PC_ST_NOT_INITED);

    free(client);

    pc_lib_cleanup();

    messageQueue->release();
    userQueue->release();
    cur = 0;
    Director *pDirector = Director::getInstance();
    Scene *pScene = Login::scene();
    pDirector->replaceScene(pScene);
}

void Chat::userCallback(cocos2d::Ref *sender)
{
    Director *pDirector = Director::getInstance();
    pDirector->getScheduler()->pauseTarget(this);
    Scene *pScene = Scene::create();
    Users *pLayer = new Users();
    pLayer->setUserQueue(userQueue);
    if (pLayer && pLayer->init())
    {
        pScene->addChild(pLayer);
        CCLOG("director pushScene");
        pDirector->pushScene(pScene);
    }
    else
    {
        delete pLayer;
        pLayer = NULL;
    }
}

void Chat::sendCallback(cocos2d::CCObject *sender)
{
    CCLog("%d", m_layer->getChildrenCount());
    unsigned int count = m_layer->getChildrenCount();
    std::string text = m_pTextField->getString();
    std::string target = "";
    std::string content = "";
    if (text.length() > 1 && text[0] == '@')
    {
        for (unsigned int i = 1; i < text.length(); i++)
        {
            if (text[i] != ' ')
            {
                target += text[i];
            }
            else
            {
                content = text.substr(i);
                break;
            }
        }
    }

    if (target == "")
    {
        target = "*";
        content = s_user + " says to all: " + text;
    }
    else
    {
        content = s_user + " says to " + target + ": " + content;
    }

    CCLabelTTF *label = CCLabelTTF::create(content.c_str(), "Arial", 25, CCSizeMake(VisibleRect::getVisibleRect().size.width, 50), kCCTextAlignmentLeft);
    label->setColor(ccc3(32, 32, 32));
    label->setAnchorPoint(ccp(0, 0));
    label->setPosition(ccp(VisibleRect::left().x + 10, (VisibleRect::top().y - (count + 2) * LINE_SPACE) ));

    m_layer->addChild(label, 10, count);
    m_layer->setPosition(ccp(0, ((count + 2)* LINE_SPACE - VisibleRect::getVisibleRect().size.height + 50)));
    m_layer->setContentSize(CCSizeMake(VisibleRect::getVisibleRect().size.width - 20, (count + 1) * (LINE_SPACE)));

    const char *route = "chat.chatHandler.send";

#if 0
    json_t *msg = json_object();
    json_t *str = json_string(content.c_str());
    json_object_set(msg, "content", str);
    json_object_set(msg, "rid", json_string(s_channel.c_str()));
    json_object_set(msg, "from", json_string(s_user.c_str()));
    json_object_set(msg, "target", json_string(target.c_str()));
#else

//    rapidjson::Document writedoc;
//    writedoc.SetObject();
//    rapidjson::Document::AllocatorType& allocator = writedoc.GetAllocator();
//    rapidjson::Value object(rapidjson::kObjectType);

    // json object 格式添加 “名称/值” 对
//    writedoc.AddMember("json", "json string", allocator);
//    writedoc.AddMember("content",  content.c_str() ,allocator);
//    writedoc.AddMember("rid",  s_channel.c_str() ,allocator);
//    writedoc.AddMember("from",  s_user.c_str() ,allocator);
//    writedoc.AddMember("target",  target.c_str() ,allocator);


    StringBuffer s;
    rapidjson::Writer<StringBuffer> writer(s);

    writer.StartObject();               // Between StartObject()/EndObject(),
    writer.Key("content");                // output a key,
    writer.String(content.c_str());             // follow by a value.
    writer.Key("rid");                // output a key,
    writer.String(s_channel.c_str());             // follow by a value.
    writer.Key("from");                // output a key,
    writer.String(s_user.c_str());             // follow by a value.

    writer.Key("target");                // output a key,
    writer.String(target.c_str());             // follow by a value.
    writer.EndObject();

    const char *  reqMsg = s.GetString();
#endif


    m_pTextField->setString("");

//    pc_request_t *request = pc_request_new();
    void (*on_send_cb)(const pc_request_t * req, int status, const char * resp) = onSendCallback;
//    pc_request(client, request, route, msg, on_send_cb);



    void (*on_request_gate_cb)(const pc_request_t *req, int status, const char *resp) = &Login::requstGateCallback;
    pc_request_with_timeout(client, route, reqMsg, 0, REQ_TIMEOUT, on_send_cb);

//    pc_request_with_timeout(client, REQ_ROUTE, REQ_MSG, REQ_EX, REQ_TIMEOUT, request_cb);

}

void Chat::onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
    //CCLOG("ccTouchesBegan");
//    CCSetIterator it = pTouches->begin();
//    CCTouch *touch = (CCTouch *)(*it);
    m_tBeginPos = touches[0]->getLocation();
}

void Chat::onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
    //CCLOG("ccTouchesMoved");
    unsigned int count = m_layer->getChildrenCount();
//    CCSetIterator it = pTouches->begin();
//    CCTouch *touch = (CCTouch *)(*it);
    Point touchLocation = touches[0]->getLocation();
    float nMoveY = touchLocation.y - m_tBeginPos.y;
    Point curPos  = m_layer->getPosition();
    Point nextPos = ccp(curPos.x, curPos.y + nMoveY);
    if (nextPos.y < 0.0f)
    {
        m_layer->setPosition(ccp(0, 0));
        return;
    }
    if (nextPos.y > ((count + 1)* LINE_SPACE - VisibleRect::getVisibleRect().size.height))
    {
        m_layer->setPosition(ccp(0, ((count + 1)* LINE_SPACE - VisibleRect::getVisibleRect().size.height + 50)));
        return;
    }
    m_layer->setPosition(nextPos);
    m_tBeginPos = touchLocation;
    s_tCurPos   = nextPos;
}

void Chat::onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
    //CCLOG("ccTouchesEnd");
//    CCSetIterator it = pTouches->begin();
//    CCTouch *touch = (CCTouch *)(*it);
    Point endPos = touches[0]->getLocation();
    float delta = 0.0f;
    if (endPos.x > delta && endPos.x - VisibleRect::getVisibleRect().size.width < delta && endPos.y > delta && endPos.y - 50 < delta)
    {
        m_pTextField->attachWithIME();
    }
    else
    {
        m_pTextField->detachWithIME();
    }
}

void Chat::editBoxEditingDidBegin(cocos2d::extension::EditBox *editBox)
{
    CCLog("editBox %p DidBegin !", editBox);
}

void Chat::editBoxEditingDidEnd(cocos2d::extension::EditBox *editBox)
{
    CCLog("editBox %p DidEnd !", editBox);
}

void Chat::editBoxTextChanged(cocos2d::extension::EditBox *editBox, const std::string &text)
{
    s_text = text;
    CCLog("editBox %p TextChanged, text: %s ", editBox, text.c_str());
}

void Chat::editBoxReturn(cocos2d::extension::EditBox *editBox)
{
    CCLog("editBox %p was returned !");
}

void Chat::setChannel(string channel)
{
    s_channel = channel;
}

void Chat::setUser(string user)
{
    s_user = user;
}

void Chat::setClient(pc_client_t *_client)
{
    client = _client;
}

void Chat::setMessageQueue(CCArray *queue)
{
    messageQueue = queue;
}

void Chat::setUserQueue(CCArray *queue)
{
    userQueue = queue;
}

bool Chat::onTextFieldAttachWithIME(TextFieldTTF *pSender)
{
    //CCLOG("onTextFieldAttachWithIME");
    return false;
}

bool Chat::onTextFieldDetachWithIME(TextFieldTTF *pSender)
{
    //CCLOG("onTextFieldDetachWithIME");
    return false;
}

bool Chat::onTextFieldInsertText(TextFieldTTF *pSender, const char *text, int nLen)
{
    // if insert enter, treat as default to detach with ime
    if ('\n' == *text)
    {
        this->sendCallback(NULL);
        return false;
    }

    // if the textfield's char count more than m_nCharLimit, doesn't insert text anymore.
    if (pSender->getCharCount() >= CHAT_LIMIT)
    {
        return true;
    }

    return false;
}

bool Chat::onTextFieldDeleteBackward(TextFieldTTF *pSender, const char *delText, int nLen)
{
    return false;
}

