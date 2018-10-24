#ifndef kafkaserver_h
#include <map>
#include <string>
#include <set>
#include "DataDefine.h"
#include "ConfigRead.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include <mysql/mysql.h>
#include "ZeromqManage.h"
#include "KafkaProducer.h"
#include "KafkaConsumer.h"
#include "RedisManage.h"
#include <unistd.h>
using namespace std;

class CKafkaServer
{
public:
    CKafkaServer();
    ~CKafkaServer();
public:
    bool StartKafkaServer(int nServerType);
    bool StopServer();
private:
    bool InitServer();
    int Uninit();

    //初始化Zeromq服务
    bool InitZeromq(const char * pZmqIP, int nPort);
    //初始化Redis
    bool InitRedis(const char * pRedisIP, int nPort);
    //初始化连接数据库
    bool InitDB();
    //kafka server4 从公安网DB获取重点库信息
    bool GetStoreLibInfoFromDB();
    //zeromq订阅消息回调
    static void ZeromqSubMsg(LPSUBMESSAGE pSubMessage, void * pUser);
    //解析Zeromq json
    bool ParseZeromqJson(LPSUBMESSAGE pSubMessage);

    //初始化Kafka
    bool InitKafkaServer();
    //Consumer 消息回调
    static void KafkaMessageCallback(char * pMsg, int nLen, void * pUser);
    //消息处理线程
    static void * MessageThread(void * pParam);
    void MessageAction();

    //预警消息处理线程
    static void * AlarmThread(void * pParam);
    void AlarmAction();
    //向代理发布抓拍信息(等同于采集服务)
    void PubSnapMessage(const char * pMsg, int nLen, LPSUBMESSAGE pPubMessage);
    //保存布控预警信息, 等待卡口分析服务返回抓拍图片url
    void SaveAlarmMessage(const char * pMsg, int nLen);
    //处理布控预警信息
    void HandleAlarmMessage(LPSUBMESSAGE pPubMessage);
    //向数据库写入布控图片信息
    void InsertLayoutFaceInfoToDB(const char * pMsg, int nLen);
    string ChangeSecondToTime(unsigned long long nSecond);
private:
    //kakfa服务类型, 1: Snap Producer(专网推送抓拍信息), 2: Snap Consumer(公安网接收抓拍信息), 
    //3: Layout Producer(专网推送布控报警和布控图片信息), 4: Layout Consumer(公安网接收布控报警和布控图片信息)
    int m_nServerType;          
    CConfigRead m_ConfigRead;   //配置文件读取
    set<int> m_setLibID;

    CZeromqManage * m_pZeromqManage;    //Zeromq管理
    CKafkaProducer * m_pKafkaProducer;  //kafka producer
    CKafkaConsumer * m_pKafkaConsumer;  //kafka consumer

    CRedisManage * m_pRedisManage;
    MYSQL m_mysql;                      //连接MySQL数据库

    list<string> m_listKafkaMsg;
    pthread_mutex_t m_mutex;
    pthread_mutex_t m_mutexDB;
    pthread_mutex_t m_mutexAlarm;
    int m_nPipe[2];
    pthread_t m_hThreadID[128];
    pthread_t m_hAlarmThreadID;

    //收到预警消息后， pub消息保存到链表， 单独线程发送， 直到收到回应(预警消息可能比抓拍图片先到公安网， 此时查不到抓拍图片对应信息， 反复发送直到收到为止）
    map<string, LPSUBMESSAGE> m_mapPubMessage;
    MAPLAYOUTLIBALARMINFO m_mapLayoutLibAlarmInfo;
};

#define kafkaserver_h
#endif