#ifndef kafkaconsumer_h

#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <error.h>
#include <getopt.h>
#include "rdkafka.h"
#include "DataDefine.h"

class CKafkaConsumer
{
public:
    CKafkaConsumer();
    ~CKafkaConsumer();
public:
    //��ʼ��Kafka��Դ
    int InitKafkaConsumer(const char * pKafkaAddress);
    //����Kafka��Դ
    int Uninit();
    //���Ӷ���
    int AddTopics(const char * pTopic);
    //��ʼ����, ׼����������
    int StartSubscribe();
    //������Ϣ�ص�
    int SetMessageCallback(LPKafkaMessageCallback pCallback, void * pUser);
private:
    int initKafka(char * pBrokers, char * pGroup, char * pTopic);
    void msg_consume(rd_kafka_message_t * rkmessage, void * opaque);

    //����kafka��Ϣ�߳�
    static void * RecvKafkaMessageThread(void * pParam);
    void RecvKafkaMessageAction();
private:
    rd_kafka_t *rk;            /*Producer instance handle*/
    rd_kafka_conf_t * m_pConf;     /*��ʱ���ö���*/
    rd_kafka_topic_partition_list_t *topics;
    char errstr[512];
    char buf[512];
    char m_pBrokers[128];
    char m_pTopic[128];
    char * m_pGroup;

    pthread_t m_hThreadID;

    LPKafkaMessageCallback m_pMsgCallback;
    void * m_pUser;
};


#define kafkaconsumer_h
#endif
