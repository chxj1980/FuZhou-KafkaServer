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
    //初始化Kafka资源
    int InitKafkaConsumer(const char * pKafkaAddress);
    //销毁Kafka资源
    int Uninit();
    //增加订阅
    int AddTopics(const char * pTopic);
    //开始订阅, 准备接收数据
    int StartSubscribe();
    //设置消息回调
    int SetMessageCallback(LPKafkaMessageCallback pCallback, void * pUser);
private:
    int initKafka(char * pBrokers, char * pGroup, char * pTopic);
    void msg_consume(rd_kafka_message_t * rkmessage, void * opaque);

    //接收kafka消息线程
    static void * RecvKafkaMessageThread(void * pParam);
    void RecvKafkaMessageAction();
private:
    rd_kafka_t *rk;            /*Producer instance handle*/
    rd_kafka_conf_t * m_pConf;     /*临时配置对象*/
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
