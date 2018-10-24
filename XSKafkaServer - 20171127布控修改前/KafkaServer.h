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

    //��ʼ��Zeromq����
    bool InitZeromq();
    //zeromq������Ϣ�ص�
    static void ZeromqSubMsg(LPSUBMESSAGE pSubMessage, void * pUser);
    //����Zeromq json
    bool ParseZeromqJson(LPSUBMESSAGE pSubMessage);

    //��ʼ��KafkaProducer
    bool InitKafkaProducer();
    //��ʼ��KafkaConsumer
    bool InitKafkaConsumer();
    //Consumer ��Ϣ�ص�
    static void KafkaMessageCallback(char * pMsg, int nLen, void * pUser);
    //��Ϣ�����߳�
    static void * MessageThread(void * pParam);
    void MessageAction();
    //�������ץ����Ϣ(��ͬ�ڲɼ�����)
    void PubSnapMessage(char * pMsg, int nLen, LPSUBMESSAGE pPubMessage);
private:
    CConfigRead m_ConfigRead;   //�����ļ���ȡ

    CZeromqManage * m_pZeromqManage;    //Zeromq����
    CKafkaProducer * m_pKafkaProducer;  //kafka producer
    CKafkaConsumer * m_pKafkaConsumer;  //kafka consumer


    list<string> m_listKafkaMsg;
    pthread_mutex_t m_mutex;
    int m_nPipe[2];
    pthread_t m_hThreadID[128];
};

#define kafkaserver_h
#endif