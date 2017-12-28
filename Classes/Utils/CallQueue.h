//
//#ifndef __CALL_QUEUE_H
//#define __CALL_QUEUE_H
//
//#include <string>
//#include <mutex>
//#include <vector>
//#include <functional>
//#include <map>
//
//struct EventCbData1{
//    int ev_type;
//    void * ex_data;
//    std::string arg1;
//    std::string arg2;
//};
//struct RequestCbData1{
//    int rc;
//    std::string resp;
//    std::string route;
//};
//struct NotifyCbData1{
//    int rc;
//    std::string route;
//};
//
//
//class CallQueue
//{
//public:
//    void call();
//
//private:
//    static std::mutex eventLock;
//    std::vector<EventCbData1> eventCbQueue;
//    std::vector<RequestCbData1> requestCbQueue;
//    std::vector<NotifyCbData1> notifyCbQueue;
//    std::map<std::string, std::function<void(const char * msg)>> functionMap;
//};
//
//#endif