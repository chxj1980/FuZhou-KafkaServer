#ifndef kafkaserver_h
#include <map>
#include <string>
#include "DataDefine.h"
#include "ConfigRead.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "ZeromqManage.h"
#include "KafkaProducer.h"
#include "KafkaConsumer.h"
#include <unistd.h>
using namespace std;

class CKafkaServer
{
public:
    CKafkaServer();
    ~CKafkaServer();
public:
    bool StartKafkaServer();
    bool StopServer();
private:
    bool InitServer();
    int Uninit();

    //初始化Zeromq服务
    bool InitZeromq();
    //zeromq订阅消息回调
    static void ZeromqSubMsg(LPSUBMESSAGE pSubMessage, void * pUser);
    //解析Zeromq json
    bool ParseZeromqJson(LPSUBMESSAGE pSubMessage);

    //初始化KafkaProducer
    bool InitKafkaProducer();
    //初始化KafkaConsumer
    bool InitKafkaConsumer();
    //Consumer 消息回调
    static void KafkaMessageCallback(char * pMsg, int nLen, void * pUser);
    //消息处理线程
    static void * MessageThread(void * pParam);
    void MessageAction();
    //向代理发布抓拍信息(等同于采集服务)
    void PubSnapMessage(char * pMsg, int nLen, LPSUBMESSAGE pPubMessage);
private:
    CConfigRead m_ConfigRead;   //配置文件读取

    CZeromqManage * m_pZeromqManage;    //Zeromq管理
    CKafkaProducer * m_pKafkaProducer;  //kafka producer
    CKafkaConsumer * m_pKafkaConsumer;  //kafka consumer


    list<string> m_listKafkaMsg;
    pthread_mutex_t m_mutex;
    int m_nPipe[2];
    pthread_t m_hThreadID[128];
};

#define kafkaserver_h
#endif