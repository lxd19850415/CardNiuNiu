//#include <pomelo.h>
//#include <PomeloClient.h>
//#include "CallQueue.h"
//
//void CallQueue::call()
//{
//    if(eventCbQueue.size() <= 0 && requestCbQueue.size() <= 0 && notifyCbQueue.size() <= 0){
//        return ;
//    }
//    eventLock.lock();
//    for(int i=0;i<eventCbQueue.size();i++){
//        auto eventCbData = eventCbQueue[i];
//        if(eventCbData.ev_type == PC_EV_USER_DEFINED_PUSH){
//            functionMap[eventCbData.arg1](eventCbData.arg2.c_str());
//        }
//        else if (eventCbData.ev_type == PC_EV_CONNECTED){
//            functionMap[PC_EV_CONNECTED_NAME]("");
//        }
//    }
//    for(int i=0;i<requestCbQueue.size();i++){
//        auto requestCbData = requestCbQueue[i];
//        if(requestCbData.rc == PC_RC_OK){
//            functionMap[requestCbData.route](requestCbData.resp.c_str());
//        }
//    }
//    for(int i=0;i<notifyCbQueue.size();i++){
//        auto notifyCbData = notifyCbQueue[i];
//        if(notifyCbData.rc == PC_RC_OK){
//            functionMap[notifyCbData.route]("{}");
//        }
//    }
//    eventCbQueue.clear();
//    requestCbQueue.clear();
//    notifyCbQueue.clear();
//    eventLock.unlock();
//}