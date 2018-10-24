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

    //��ʼ��Zeromq����
    bool InitZeromq(const char * pZmqIP, int nPort);
    //��ʼ��Redis
    bool InitRedis(const char * pRedisIP, int nPort);
    //��ʼ���������ݿ�
    bool InitDB();
    //kafka server4 �ӹ�����DB��ȡ�ص����Ϣ
    bool GetStoreLibInfoFromDB();
    //zeromq������Ϣ�ص�
    static void ZeromqSubMsg(LPSUBMESSAGE pSubMessage, void * pUser);
    //����Zeromq json
    bool ParseZeromqJson(LPSUBMESSAGE pSubMessage);

    //��ʼ��Kafka
    bool InitKafkaServer();
    //Consumer ��Ϣ�ص�
    static void KafkaMessageCallback(char * pMsg, int nLen, void * pUser);
    //��Ϣ�����߳�
    static void * MessageThread(void * pParam);
    void MessageAction();

    //Ԥ����Ϣ�����߳�
    static void * AlarmThread(void * pParam);
    void AlarmAction();
    //�������ץ����Ϣ(��ͬ�ڲɼ�����)
    void PubSnapMessage(const char * pMsg, int nLen, LPSUBMESSAGE pPubMessage);
    //���沼��Ԥ����Ϣ, �ȴ����ڷ������񷵻�ץ��ͼƬurl
    void SaveAlarmMessage(const char * pMsg, int nLen);
    //������Ԥ����Ϣ
    void HandleAlarmMessage(LPSUBMESSAGE pPubMessage);
    //�����ݿ�д�벼��ͼƬ��Ϣ
    void InsertLayoutFaceInfoToDB(const char * pMsg, int nLen);
    string ChangeSecondToTime(unsigned long long nSecond);
private:
    //kakfa��������, 1: Snap Producer(ר������ץ����Ϣ), 2: Snap Consumer(����������ץ����Ϣ), 
    //3: Layout Producer(ר�����Ͳ��ر����Ͳ���ͼƬ��Ϣ), 4: Layout Consumer(���������ղ��ر����Ͳ���ͼƬ��Ϣ)
    int m_nServerType;          
    CConfigRead m_ConfigRead;   //�����ļ���ȡ
    set<int> m_setLibID;

    CZeromqManage * m_pZeromqManage;    //Zeromq����
    CKafkaProducer * m_pKafkaProducer;  //kafka producer
    CKafkaConsumer * m_pKafkaConsumer;  //kafka consumer

    CRedisManage * m_pRedisManage;
    MYSQL m_mysql;                      //����MySQL���ݿ�

    list<string> m_listKafkaMsg;
    pthread_mutex_t m_mutex;
    pthread_mutex_t m_mutexDB;
    pthread_mutex_t m_mutexAlarm;
    int m_nPipe[2];
    pthread_t m_hThreadID[128];
    pthread_t m_hAlarmThreadID;

    //�յ�Ԥ����Ϣ�� pub��Ϣ���浽���� �����̷߳��ͣ� ֱ���յ���Ӧ(Ԥ����Ϣ���ܱ�ץ��ͼƬ�ȵ��������� ��ʱ�鲻��ץ��ͼƬ��Ӧ��Ϣ�� ��������ֱ���յ�Ϊֹ��
    map<string, LPSUBMESSAGE> m_mapPubMessage;
    MAPLAYOUTLIBALARMINFO m_mapLayoutLibAlarmInfo;
};

#define kafkaserver_h
#endif