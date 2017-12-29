#include "LoginScene.h"
#include "ChatScene.h"
#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>

USING_NS_CC;
USING_NS_CC_EXT;

#define GATE_HOST "114.115.136.73"
#define GATE_PORT 7110
//#define GATE_HOST "127.0.0.1"
//#define GATE_PORT 3014

#include <stdlib.h>
#include <json/stringbuffer.h>
#include <json/writer.h>

#define PC_TEST_ASSERT(expr)                                          \
    do {                                                         \
        if (!(expr)) {                                           \
            fprintf(stderr,                                      \
                    "Assertion failed in %s on line %d: %s\n",   \
                    __FILE__,                                    \
                    __LINE__,                                    \
                    #expr);                                      \
            abort();                                             \
        }                                                        \
    } while (0)

#define EV_HANDLER_EX ((void*)0x44)
#define EV_HANDLER_EX1 ((void*)0x55)

#define REQ_EX ((void*)0x22)

#define REQ_TIMEOUT 3

static string connectorHost = "";
static int connectorPort = 0;
static std::string username = "";
static std::string channel = "";
static pc_client_t* pomelo_client = NULL;
static CCArray* messageQueue = NULL;
static CCArray* userQueue = NULL;
static int error = 0;
static json_t* userList = NULL;
int Login::synHandlerTag = 0;
static void on_close(pc_client_t *client, const char *event, void *data);

vector<int > Login::mRoomID;
vector<int > Login::mGameID;
vector<int > Login::mCellID;
vector<int > Login::mScoreMin;
vector<int > Login::mScoreMax;


vector<int > Login::mUserID;
vector<string > Login::mName;
vector<string > Login::mSex;
vector<string > Login::mHeadUrl;
vector<int > Login::mState;



void Login::onChatCallback(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2)
{
//    json_t* json = (json_t* )data;
//    const char* msg = json_dumps(json, 0);
    CCLOG("onChatCallback ev_type[%d] ex_data[%s] arg1[%s]arg2[%s]", ev_type,ex_data, arg1,arg2);
//    messageQueue->addObject(CCString::create(msg));
    return;
}

void Login::onAddCallback(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2)
{
    CCLOG("onAddCallback ev_type[%d] ex_data[%s] arg1[%s]arg2[%s]", ev_type,ex_data, arg1,arg2);
    if (!userQueue) {
        return;
    }
//    json_t* json = (json_t* )data;
//    json_t* user = json_object_get(json, "user");
//    const char* msg = json_string_value(user);
//    CCLOG("%s %s", event, msg);
//    userQueue->addObject(CCString::create(msg));
    return;
}

void Login::onLeaveCallback(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2)
{
    CCLOG("onLeaveCallback ev_type[%d] ex_data[%s] arg1[%s]arg2[%s]", ev_type,ex_data, arg1,arg2);
    if (!userQueue) {
        return;
    }
//    json_t* json = (json_t* )data;
//    json_t* user = json_object_get(json, "user");
//    const char* msg = json_string_value(user);
//    CCLOG("%s %s", event,msg);
//    for (unsigned int i=0; i<userQueue->count(); i++) {
//        CCString* cstring = (CCString* )userQueue->objectAtIndex(i);
//        if (strcmp(cstring->getCString(),msg) == 0) {
//            userQueue->removeObjectAtIndex(i);
//            break;
//        }
//    }
    return;
}

void Login::onDisconnectCallback(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2)
{
    CCLOG("onDisconnectCallback ev_type[%d] ex_data[%s] arg1[%s]arg2[%s]", ev_type,ex_data, arg1,arg2);
//    CCLOG("%s", event);
    return;
}

void Login::requstGateCallback(const pc_request_t *req, int status, const char* resp)
{
    CCLOG("===>pomelo 请求入口回调 status[%d] resp[%s]",status,resp);

    if(status == -1) {
        CCLOG("Fail to send request to server.\n");
    } else if(status == 0) {
        rapidjson::Document m_doc;
        m_doc.Parse<0>(resp);
        if (m_doc.HasParseError())
        {
            log("GetParaseError:%s\n", m_doc.GetParseError());
        }
        if(m_doc.HasMember("data"))
        {

            auto statValue = m_doc["data"].GetObject();
            if(statValue.HasMember("host"))
            {
                connectorHost = statValue["host"].GetString();
            }
            if(statValue.HasMember("port"))
            {
                connectorPort = statValue["port"].GetInt();
            }
            synHandlerTag = SYN_TAG_GATE_REQUEST_CB;
        }

        CCLOG("requstGateCallback server response: %s %d\n", connectorHost.c_str(), connectorPort);
//        connectorHost = json_string_value(json_object_get(resp, "host"));
//        connectorPort = json_number_value(json_object_get(resp, "port"));
//        CCLOG("%s %d", connectorHost,connectorPort);

        return;
    }

    // release relative resource with pc_request_t
//    json_t *pc_msg = req->msg;
//    pc_client_t *pc_client = req->client;
//    json_decref(pc_msg);
//    pc_request_destroy(req);
//
//    pc_client_stop(pc_client);

#if 0
    pc_client_disconnect(pc_request_client(req));

    pc_client_cleanup(pc_request_client(req));

//    PC_TEST_ASSERT(pc_client_state(pc_request_client(req)) == PC_ST_NOT_INITED);

    free(pc_request_client(req));

    pc_lib_cleanup();
#endif
}

void Login::requstLoginCallback(const pc_request_t *req, int status, const char* resp)
{
    CCLOG("===>pomelo 请求登录游戏回调 status[%d] resp[%s]",status,resp);

    if(status == -1) {
        CCLOG("Fail to send request to server.\n");
    } else if(status == 0) {
        rapidjson::Document m_doc;
        m_doc.Parse<0>(resp);
        if (m_doc.HasParseError())
        {
            log("GetParaseError:%s\n", m_doc.GetParseError());
        }
        if(m_doc.HasMember("data"))
        {

            int isNew = -1;
            auto dataValue = m_doc["data"].GetObject();
            if(dataValue.HasMember("isNew"))
            {
                isNew = dataValue["isNew"].GetInt();
            }

            if(dataValue.HasMember("player"))
            {

                auto player = dataValue["player"].GetObject();

                int userId = -1;
                string userName = "";
                string sex = "";
                string headUrl = "";

                if(player.HasMember("userId"))
                {
                    userId = player["userId"].GetInt();
                }
                if(player.HasMember("name"))
                {
                    userName = player["name"].GetString();
                }
                if(player.HasMember("sex"))
                {
                    sex = player["sex"].GetString();
                }
                if(player.HasMember("headurl"))
                {
                    headUrl = player["headurl"].GetString();
                }
            }

            synHandlerTag = SYN_TAG_LOGIN_REQUEST_CB;
//            CCLOG("requstGateCallback server response: %s %d\n", connectorHost.c_str(), connectorPort);
        }

//        connectorHost = json_string_value(json_object_get(resp, "host"));
//        connectorPort = json_number_value(json_object_get(resp, "port"));
//        CCLOG("%s %d", connectorHost,connectorPort);

        return;
    }

    // release relative resource with pc_request_t
//    json_t *pc_msg = req->msg;
//    pc_client_t *pc_client = req->client;
//    json_decref(pc_msg);
//    pc_request_destroy(req);
//
//    pc_client_stop(pc_client);

#if 0
    pc_client_disconnect(pc_request_client(req));

    pc_client_cleanup(pc_request_client(req));

//    PC_TEST_ASSERT(pc_client_state(pc_request_client(req)) == PC_ST_NOT_INITED);

    free(pc_request_client(req));

    pc_lib_cleanup();
#endif
}

void Login::requstRoomListCallback(const pc_request_t *req, int status, const char* resp)
{
    CCLOG("===>pomelo 请求房间列表回调 status[%d] resp[%s]",status,resp);
    error = 0;
    if(status == -1) {
        CCLOG("Fail to send request to server.\n");
    } else if(status == 0) {
        rapidjson::Document m_doc;
        m_doc.Parse<0>(resp);
        if (m_doc.HasParseError())
        {
            log("GetParaseError:%s\n", m_doc.GetParseError());
        }

        char * errorStr;
        if(m_doc.HasMember("error"))
        {
            errorStr = (char *)m_doc["error"].GetString();
            error = 1;
            CCLOG("connect error %s", resp);
            return;
        }

//        pomelo_client = req->client;

        pomelo_client = pc_request_client(req);

//        CCLOG("requstRoomListCallback resp [%s]",resp);

        if(m_doc.HasMember("data"))
        {
            rapidjson::Value & data = m_doc["data"];

            int roomId = -1;
            int gameId = -1;
            int cell = -1;
            int min = -1;
            int max = -1;

            if(data.IsArray())
            {
                for (rapidjson::SizeType i = 0; i < data.Size(); i++)
                {
                    const rapidjson::Value &p = data[i];

                    if(p.HasMember("roomId"))
                    {
                        roomId = p["roomId"].GetInt();
                    }

                    if(p.HasMember("gameId"))// 游戏ID，10001 表示牛牛
                    {
                        gameId = p["gameId"].GetInt();
                    }

                    if(p.HasMember("cell"))// 底分
                    {
                        cell = p["cell"].GetInt();
                    }

                    if(p.HasMember("min"))
                    {
                        min = p["min"].GetInt();
                    }

                    if(p.HasMember("max"))
                    {
                        max = p["max"].GetInt();
                    }
                    mRoomID.push_back(roomId);
                    mGameID.push_back(gameId);
                    mCellID.push_back(cell);
                    mScoreMin.push_back(min);
                    mScoreMax.push_back(max);
                }

                synHandlerTag = SYN_TAG_ROOM_LIST_REQUEST_CB;
            }

        }

//        for (unsigned int i=0; i<json_array_size(users); i++) {
//            json_t* val = json_array_get(users,i);
//            userQueue->addObject(CCString::create(json_string_value(val)));
//        }
    }

    // release relative resource with pc_request_t
//    json_t *msg = req->msg;
//    pc_client_t *client = req->client;
//    json_decref(msg);
//    pc_request_destroy(req);

#if 0
    pc_client_disconnect(pc_request_client(req));

    pc_client_cleanup(pc_request_client(req));

    PC_TEST_ASSERT(pc_client_state(pc_request_client(req)) == PC_ST_NOT_INITED);

    free(pc_request_client(req));

    pc_lib_cleanup();
#endif
}

void Login::requstJoinRoomCallback(const pc_request_t *req, int status, const char* resp)
{
    CCLOG("===>pomelo 请求进入房间回调 status[%d] resp[%s]",status,resp);
    error = 0;
    if(status == -1) {
        CCLOG("Fail to send request to server.\n");
    } else if(status == 0) {
        rapidjson::Document m_doc;
        m_doc.Parse<0>(resp);
        if (m_doc.HasParseError())
        {
            log("GetParaseError:%s\n", m_doc.GetParseError());
        }

        char * errorStr;
        if(m_doc.HasMember("error"))
        {
            errorStr = (char *)m_doc["error"].GetString();
            error = 1;
            CCLOG("connect error %s", resp);
            return;
        }

//        pomelo_client = req->client;

        pomelo_client = pc_request_client(req);

//        CCLOG("requstRoomListCallback resp [%s]",resp);

        if(m_doc.HasMember("data"))
        {
            rapidjson::Value & data = m_doc["data"];

            if(data.HasMember("table"))
            {
                rapidjson::Value & dataTable = m_doc["table"];
                int state = -1;
                if(dataTable.HasMember("state"))
                {
                    state = dataTable["state"].GetInt();// 桌子状态 1空闲
                }
                int ds_time = -1;
                if(dataTable.HasMember("ds_time"))
                {
                    ds_time = dataTable["ds_time"].GetInt();// 距离开始毫秒数 -1表示无法开始
                }
            }

            if(data.HasMember("users"))
            {
                rapidjson::Value & dataUsers = m_doc["users"];
                int userID;
                string name;
                string sex;
                string headUrl;
                int state;

                if(dataUsers.IsArray())
                {
                    for (rapidjson::SizeType i = 0; i < dataUsers.Size(); i++)
                    {
                        const rapidjson::Value &p = dataUsers[i];
                        if(p.HasMember("userId"))
                        {
                            userID = p["userId"].GetInt();
                        }
                        if(p.HasMember("name"))// 游戏ID，10001 表示牛牛
                        {
                            name = p["name"].GetString();
                        }
                        if(p.HasMember("sex"))// 底分
                        {
                            sex = p["sex"].GetString();
                        }
                        if(p.HasMember("headurl"))
                        {
                            headUrl = p["headurl"].GetString();
                        }
                        if(p.HasMember("state"))
                        {
                            state = p["state"].GetInt();// 玩家状态 1空闲 2准备
                        }
                        mUserID.push_back(userID);
                        mName.push_back(name);
                        mSex.push_back(sex);
                        mHeadUrl.push_back(headUrl);
                        mState.push_back(state);
                    }
                    synHandlerTag = SYN_TAG_JOIN_ROOM_REQUEST_CB;
                }
            }

            int roomId = -1;
            int gameId = -1;
            int cell = -1;
            int min = -1;
            int max = -1;

            if(data.IsArray())
            {
                for (rapidjson::SizeType i = 0; i < data.Size(); i++)
                {
                    const rapidjson::Value &p = data[i];

                    if(p.HasMember("roomId"))
                    {
                        roomId = p["roomId"].GetInt();
                    }

                    if(p.HasMember("gameId"))// 游戏ID，10001 表示牛牛
                    {
                        gameId = p["gameId"].GetInt();
                    }

                    if(p.HasMember("cell"))// 底分
                    {
                        cell = p["cell"].GetInt();
                    }

                    if(p.HasMember("min"))
                    {
                        min = p["min"].GetInt();
                    }

                    if(p.HasMember("max"))
                    {
                        max = p["max"].GetInt();
                    }
                    mRoomID.push_back(roomId);
                    mGameID.push_back(gameId);
                    mCellID.push_back(cell);
                    mScoreMin.push_back(min);
                    mScoreMax.push_back(max);
                }

                synHandlerTag = SYN_TAG_ROOM_LIST_REQUEST_CB;
            }

        }

//        for (unsigned int i=0; i<json_array_size(users); i++) {
//            json_t* val = json_array_get(users,i);
//            userQueue->addObject(CCString::create(json_string_value(val)));
//        }
    }

    // release relative resource with pc_request_t
//    json_t *msg = req->msg;
//    pc_client_t *client = req->client;
//    json_decref(msg);
//    pc_request_destroy(req);

#if 0
    pc_client_disconnect(pc_request_client(req));

    pc_client_cleanup(pc_request_client(req));

    PC_TEST_ASSERT(pc_client_state(pc_request_client(req)) == PC_ST_NOT_INITED);

    free(pc_request_client(req));

    pc_lib_cleanup();
#endif
}
// disconnect event callback.
void on_close(pc_client_t *client, const char *event, void *data)
{
    CCLOG("client closed: %d.\n", pc_client_state(client) );
}

void Login::doLogin()
{	
    const char *ip = GATE_HOST;
    int port = GATE_PORT;


    //const char *route = "gate.gateHandler.queryEntry";
    const char *route = "gate.gateHandler.getConnector";

    StringBuffer s;
    rapidjson::Writer<StringBuffer> writer(s);

    writer.StartObject();               // Between StartObject()/EndObject(),
    writer.Key("uid");                // output a key,
    writer.String(username.c_str());             // follow by a value.
    writer.EndObject();
    const char *  reqMsg = s.GetString();

    /////
    pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
    int handler_id;
    pc_lib_init(NULL, NULL, NULL, NULL);

//    config.transport_name = PC_TR_NAME_UV_TLS;
    //config.local_storage_cb = local_storage_cb;
    config.local_storage_cb = nullptr;

    pc_client_t* client;
    client = (pc_client_t*)malloc(pc_client_size());
    PC_TEST_ASSERT(client);
    pc_client_init(client, (void*)0x11, &config);
    PC_TEST_ASSERT(pc_client_ex_data(client) == (void*)0x11);
    PC_TEST_ASSERT(pc_client_state(client) == PC_ST_INITED);

    mClient = client;

    handler_id = pc_client_add_ev_handler(client, Login::event_cb, EV_HANDLER_EX, NULL);
    int ret = pc_client_connect(client, GATE_HOST, GATE_PORT, NULL);
    if(ret != PC_RC_OK)
    {
        CCLOG("===>pomelo doLogin 连接入口 GATE_HOST[%s] GATE_PORT[%d],ret[%d]",GATE_HOST,GATE_PORT,ret);
        return;
    }
    CCLOG("===>pomelo doLogin 连接入口 GATE_HOST[%s] GATE_PORT[%d]",GATE_HOST,GATE_PORT);
//    SLEEP(1);
//    void (*on_request_gate_cb)(const pc_request_t *req, int status, const char *resp) = &Login::requstGateCallback;
//
//    CCLOG("route [%s] reqMsg [%s]",route,reqMsg);
//    pc_request_with_timeout(client, route, "", REQ_EX, REQ_TIMEOUT, on_request_gate_cb);

//    pc_notify_with_timeout(client, NOTI_ROUTE, NOTI_MSG, NOTI_EX, NOTI_TIMEOUT, notify_cb);
//    SLEEP(50);

#if 0
    pc_client_disconnect(client);

    pc_client_rm_ev_handler(client, handler_id);

    pc_client_cleanup(client);

    PC_TEST_ASSERT(pc_client_state(client) == PC_ST_NOT_INITED);

    free(client);

    pc_lib_cleanup();
#endif
}

Scene *Login::scene()
{
    Scene *scene = NULL;
    do {
        // 'scene' is an autorelease object
        scene = Scene::create();
        CC_BREAK_IF(! scene);
        // 'layer' is an autorelease object
        Login *layer = Login::create();
        CC_BREAK_IF(! layer);
        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);
    // return the scene
    return scene;
}

void Login::onEnter()
{
    Layer::onEnter();
    CCLOG("Login onEnter");
    pomelo_client = NULL;
    messageQueue = new __Array();
    messageQueue->init();

    userQueue = new __Array();
    userQueue->init();

    Director::getInstance()->getScheduler()->schedule(
        schedule_selector(Login::dispatchLoginCallbacks), this, 0, false);
}

// on "init" you need to initialize your instance
bool Login::init()
{
    bool bRet = false;
    pomeloClient = nullptr;
    isConnected = false;

    synHandlerTag = 0;
    mClient = nullptr;
//    mCallQueue = new CallQueue();
//    PomeloClient::initLib(NULL, NULL, NULL, NULL);
//    pomeloClient = PomeloClient::createClient();
    do {
        CC_BREAK_IF(! Layer::init());
//        CCPoint visibleOrigin = CCEGLView::sharedOpenGLView()->getVisibleOrigin();
//        CCSize visibleSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();
        // top
        Size editBoxSize = CCSizeMake(visibleSize.width - 100, 60);
        m_pEditName = EditBox::create(editBoxSize , Scale9Sprite::create("green_edit.png"));
        m_pEditName->setPosition(ccp(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height * 3 / 4));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        m_pEditName->setFont("Paint Boy", 25);
#else
        m_pEditName->setFont("Arial", 25);
#endif
        m_pEditName->setFontColor(ccBLACK);
        m_pEditName->setPlaceHolder("Name:");
        m_pEditName->setPlaceholderFontColor(ccWHITE);
        m_pEditName->setMaxLength(8);
        m_pEditName->setReturnType(EditBox::KeyboardReturnType::DONE);
        m_pEditName->setDelegate(this);
        this->addChild(m_pEditName, 1);
        // middle
        m_pEditChannel = EditBox::create(editBoxSize, Scale9Sprite::create("green_edit.png"));
        m_pEditChannel->setPosition(ccp(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 2));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        m_pEditChannel->setFont("American Typewriter", 30);
#else
        m_pEditChannel->setFont("Arial", 25);
#endif
        m_pEditChannel->setFontColor(ccBLACK);
        m_pEditChannel->setPlaceHolder("Channel:");
        m_pEditChannel->setPlaceholderFontColor(ccWHITE);
        m_pEditChannel->setMaxLength(8);
        m_pEditChannel->setReturnType(EditBox::KeyboardReturnType::DONE);
        m_pEditChannel->setDelegate(this);
        this->addChild(m_pEditChannel, 1);
        // 1. Add a menu item with "X" image, which is clicked to quit the program.
        CCLabelTTF *label = CCLabelTTF::create("Login", "Arial", 50);
        //#endif
        MenuItemLabel *pMenuItem = MenuItemLabel::create(label, this, menu_selector(Login::onLogin));
        Menu *pMenu = Menu::create(pMenuItem, NULL);
        pMenu->setPosition( Vec2() );
        pMenuItem->setPosition( Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 4) );
        //m_pEditEmail->setAnchorPoint(ccp(0.5, 1.0f));
        this->addChild(pMenu, 1);
        CCLabelTTF *pLabel = CCLabelTTF::create("pomelo-cocos2dchat", "Arial", 30);
        CC_BREAK_IF(! pLabel);
        // Get window size and place the label upper.
        Size size = Director::getInstance()->getWinSize();
        pLabel->setPosition(Vec2(size.width / 2, size.height - 30));
        // Add the label to HelloWorld layer as a child layer.
        this->addChild(pLabel, 1);
        // 3. Add add a splash screen, show the cocos2d splash image.
        Sprite *pSprite = Sprite::create("HelloWorld.png");
        CC_BREAK_IF(! pSprite);
        // Place the sprite on the center of the screen
        pSprite->setPosition(Vec2(size.width / 2, size.height / 2));
        // Add the sprite to HelloWorld layer as a child layer.
        this->addChild(pSprite, 0);

        bRet = true;
    } while (0);

    return bRet;
}

void Login::dispatchLoginCallbacks(float delta)
{
    // wait for pomelo_client init from connector callback
    if(pomeloClient != nullptr)
    {
        pomeloClient->update(delta);
    }
    if(synHandlerTag == SYN_TAG_GATE_CONNECT_CB)
    {
        void (*on_request_gate_cb)(const pc_request_t *req, int status, const char *resp) = &Login::requstGateCallback;

        const char *route = "gate.gateHandler.getConnector";
        const char *msg = "{}";
        CCLOG("route [%s]",route);
        int ret = pc_request_with_timeout(mClient, route, msg, REQ_EX, REQ_TIMEOUT, on_request_gate_cb);
        if(ret != PC_RC_OK)
        {

            CCLOG("===>pomelo dispatchLoginCallbacks 发起请求入口ret[%d]",ret);
        }
        synHandlerTag = SYN_TAG_NONE;


        CCLOG("===>pomelo dispatchLoginCallbacks 发起请求入口 route[%s] msg[%s]",route,msg);
    }

    if(synHandlerTag == SYN_TAG_GATE_REQUEST_CB)
    {
        synHandlerTag = SYN_TAG_NONE;

        if(connectorPort != 0 && isConnected == false)
        {
            isConnected = true;
            pc_client_t *client = (pc_client_t*)malloc(pc_client_size());

            pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;

            pc_client_init(client, (void*)0x11, &config);

            mClient  = client;
            int handler_id = pc_client_add_ev_handler(client, Login::event_cb, EV_HANDLER_EX1, NULL);

            // try to connect to server.
            CCLOG("fail to connect server connectorHost[%s] connectorPort[%d].\n",connectorHost.c_str(),connectorPort);
            int ret = pc_client_connect(client, connectorHost.c_str(), connectorPort, NULL);
            if(ret != 0) {
                CCLOG("fail to connect server.\n");
//            pc_client_destroy(client);

                pc_client_cleanup(client);
                free(client);
                pc_lib_cleanup();
                return ;
            }

            CCLOG("===>pomelo dispatchLoginCallbacks 连接连接器 connectorHost[%s] connectorPort[%d]"
            ,connectorHost.c_str(),connectorPort);
        }
    }

    if(synHandlerTag == SYN_TAG_LOGIN_CONNECT_CB)
    {
        synHandlerTag = SYN_TAG_NONE;
        const char *route = "connector.entryHandler.login";


//{ _id: 'xxxxxx', name:'xxxx', headurl: 'xxxxxxxxxxxxxxxx' }
        StringBuffer s;
        rapidjson::Writer<StringBuffer> writer(s);

        writer.StartObject();               // Between StartObject()/EndObject(),
        writer.Key("_id");                // output a key,
        writer.String(channel.c_str());             // follow by a value.

        writer.Key("name");                // output a key,
        writer.String(username.c_str());             // follow by a value.

        writer.Key("headurl");                // output a key,
        writer.String(username.c_str());             // follow by a value.
        //writer.Key("rid");                // output a key,

        writer.EndObject();

        const char *  msg = s.GetString();

//        pc_request_t *request = pc_request_new();
        void (*connect_cb)(const pc_request_t *req, int status, const char *resp )= &Login::requstLoginCallback;

        int ret = pc_request_with_timeout(mClient, route, msg, REQ_EX, REQ_TIMEOUT, connect_cb);
        if(ret != PC_RC_OK)
        {
            CCLOG("===>pomelo dispatchLoginCallbacks 发起请求连接器 ret[%d]",ret);
        }
        CCLOG("===>pomelo dispatchLoginCallbacks 发起请求连接器 route[%s] msg[%s]",route,msg);


//        pc_request(client, request, route, msg, connect_cb);

        /*
        char *json_str = json_dumps(resp, 0);
        if(json_str != NULL) {
            CCLOG("server response: %s %d\n", connectorHost, connectorPort);
            free(json_str);
        }
        */


        // release relative resource with pc_request_t
//    json_t *pc_msg = req->msg;
//    pc_client_t *pc_client = req->client;
//    json_decref(pc_msg);
//    pc_request_destroy(req);
//
//    pc_client_stop(pc_client);

#if 0
        pc_client_disconnect(pc_request_client(req));

    pc_client_cleanup(pc_request_client(req));

//    PC_TEST_ASSERT(pc_client_state(pc_request_client(req)) == PC_ST_NOT_INITED);

    free(pc_request_client(req));

    pc_lib_cleanup();
#endif
    }


    if(synHandlerTag == SYN_TAG_LOGIN_REQUEST_CB)
    {
        synHandlerTag = SYN_TAG_NONE;
        const char *route = "hall.hallHandler.roomList";

        const char *  msg = "{}";

//        pc_request_t *request = pc_request_new();
        void (*connect_cb)(const pc_request_t *req, int status, const char *resp )= &Login::requstRoomListCallback;

        int ret = pc_request_with_timeout(mClient, route, msg, REQ_EX, REQ_TIMEOUT, connect_cb);
        if(ret != PC_RC_OK)
        {
            CCLOG("===>pomelo dispatchLoginCallbacks 发起请求房间列表 ret[%d]",ret);
        }
        CCLOG("===>pomelo dispatchLoginCallbacks 发起请求连房间列表 route[%s] msg[%s]",route,msg);

    }

    if(synHandlerTag == SYN_TAG_ROOM_LIST_REQUEST_CB)
    {
        synHandlerTag = SYN_TAG_NONE;
        const char *route = "game.niuniuHandler.joinRoom";

        int roomID = mRoomID[0];

        StringBuffer s;
        rapidjson::Writer<StringBuffer> writer(s);

        writer.StartObject();               // Between StartObject()/EndObject(),
        writer.Key("roomId");                // output a key,
        writer.Int(roomID);             // follow by a value.

        writer.EndObject();

        const char *  msg = s.GetString();


//        pc_request_t *request = pc_request_new();
        void (*connect_cb)(const pc_request_t *req, int status, const char *resp )= &Login::requstJoinRoomCallback;

        int ret = pc_request_with_timeout(mClient, route, msg, REQ_EX, REQ_TIMEOUT, connect_cb);
        if(ret != PC_RC_OK)
        {
            CCLOG("===>pomelo dispatchLoginCallbacks 发起请求进入房间 ret[%d]",ret);
        }
        CCLOG("===>pomelo dispatchLoginCallbacks 发起请求进入房间 route[%s] msg[%s]",route,msg);

    }
    return;


    if(pomelo_client == NULL || error == 1) {
        return;
    }

    Director::getInstance()->getScheduler()->pauseTarget(this);

    Scene* pScene = Scene::create();
    Chat* pLayer = new Chat();
    pLayer->setChannel(channel);
    pLayer->setUser(username);
    pLayer->setClient(pomelo_client);
    pLayer->setUserQueue(userQueue);
    pLayer->setMessageQueue(messageQueue);

    CCLOG("init player");
    if(pLayer && pLayer->init()) {
        //pLayer->autorelease();
        pScene->addChild(pLayer);
        CCLOG("director replaceScene");
        pomeloClient->destroy();
        Director::getInstance()->replaceScene(TransitionFade::create(1, pScene));
    } else {
        delete pLayer;
        pLayer = NULL;
    }
}

void Login::onLogin(Ref *pSender)
{
    doLogin();
}


void Login::event_cb(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2)
{
//    PC_TEST_ASSERT(ex_data == EV_HANDLER_EX);
    printf("test: get event %s, arg1: %s, arg2: %s\n", pc_client_ev_str(ev_type),
           arg1 ? arg1 : "", arg2 ? arg2 : "");

    char errorMsg[128];
    memset(errorMsg,0,sizeof(errorMsg));
    sprintf(errorMsg,"===>pomelo event_cb test:ex_data [%d] get event %s, arg1: %s, arg2: %s\n", (int )ex_data, pc_client_ev_str(ev_type),
            arg1 ? arg1 : "", arg2 ? arg2 : "");

    CCLOG("%s",errorMsg);

    if(ev_type != 1)
    {
        CCLOG("===>pomelo event_cb ev_type error [%d]",ev_type);
        return;
    }

    int a = (int )ex_data;
    int b = (int)EV_HANDLER_EX;
    int c = (int)EV_HANDLER_EX1;

    if(a == b)
    {
        synHandlerTag = SYN_TAG_GATE_CONNECT_CB;
    }
    if(a == c)
    {
        synHandlerTag = SYN_TAG_LOGIN_CONNECT_CB;
    }
    int aa =  10;

}
void Login::editBoxEditingDidBegin(ui::EditBox *editBox)
{
    CCLog("editBox %p DidBegin !", editBox);
}

void Login::editBoxEditingDidEnd(ui::EditBox *editBox)
{
    CCLog("editBox %p DidEnd !", editBox);
}

void Login::editBoxTextChanged(ui::EditBox *editBox, const std::string &text)
{
    if (editBox == m_pEditName) {
        username = text;
        CCLog("name editBox %p TextChanged, text: %s ", editBox, text.c_str());
    } else {
        channel = text;
        CCLog("channel editBox %p TextChanged, text: %s ", editBox, text.c_str());
    }
}

void Login::editBoxReturn(ui::EditBox *editBox)
{
    CCLog("editBox %p was returned !");
}