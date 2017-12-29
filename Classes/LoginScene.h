#ifndef __LOGIN_SCENE_H__
#define __LOGIN_SCENE_H__

#include <stdio.h>
#include <Utils/CallQueue.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "VisibleRect.h"
#include "pomelo.h"
#include "jansson.h"
#include "PomeloClient.h"

#define SYN_TAG_NONE (-1)
#define SYN_TAG_GATE_CONNECT_CB (1)
#define SYN_TAG_GATE_REQUEST_CB (2)
#define SYN_TAG_LOGIN_CONNECT_CB (3)
#define SYN_TAG_LOGIN_REQUEST_CB (4)
#define SYN_TAG_ROOM_LIST_REQUEST_CB (5)
#define SYN_TAG_JOIN_ROOM_REQUEST_CB (6)

using namespace std;
class Login : public cocos2d::Layer, public cocos2d::extension::EditBoxDelegate
{
public:
    virtual bool init();  
    static cocos2d::Scene* scene();
	CREATE_FUNC(Login);
    
	// cocos2d-x schedule handle callback
	void dispatchLoginCallbacks(float delta);
	
	// cocos2d-x events callback
	void onLogin(Ref* pSender);
	virtual void onEnter();
	virtual void editBoxEditingDidBegin(cocos2d::extension::EditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::extension::EditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::extension::EditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::extension::EditBox* editBox);

	// pomelo handle
	void doLogin();
	// pomelo events callback
	static void onChatCallback(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2);
	static void onAddCallback(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2);
	static void onLeaveCallback(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2);
	static void onDisconnectCallback(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2);

	static void requstGateCallback(const pc_request_t *req, int status, const char* resp);
	static void requstLoginCallback(const pc_request_t *req, int status, const char* resp);
    static void requstRoomListCallback(const pc_request_t *req, int status, const char* resp);
    static void requstJoinRoomCallback(const pc_request_t *req, int status, const char* resp);



    static void event_cb(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2);

private:
	std::string s_EditName;
	std::string s_EditChannel;
	cocos2d::extension::EditBox* m_pEditName;
    cocos2d::extension::EditBox* m_pEditChannel;
// pomelo
	PomeloClient *pomeloClient;
	int port;
    bool isConnected;

	static int synHandlerTag;
	pc_client_t* mClient;

    static vector<int > mRoomID;
    static vector<int > mGameID;
    static vector<int > mCellID;
    static vector<int > mScoreMin;
    static vector<int > mScoreMax;

    static vector<int > mUserID;
    static vector<string > mName;
    static vector<string > mSex;
    static vector<string > mHeadUrl;
    static vector<int > mState;

//    CallQueue* mCallQueue;
};

#endif  // __LOGIN_SCENE_H__