#ifndef kafkaproducer_h

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <vector>
using namespace std;

#include "rdkafka.h"
class CKafkaProducer
{
public:
    CKafkaProducer();
    ~CKafkaProducer();
public:
    //初始化Kafka资源
    int InitKafkaProducer(const char * pKafkaAddress);
    //增加主题
    int AddTopics(const char * pTopic);
    //销毁Kafka资源
    int Uninit();
    //发送消息
    int SendProducerMessage(char * pMsg, int nLen, int nOrder = 0);
    //发送消息结果回调, 消息是否正确发送
    static void MessageCallback(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque);
private:
    rd_kafka_t *rk;            /*Producer instance handle*/
    rd_kafka_conf_t * m_pConf;     /*临时配置对象*/
    char errstr[512];
    char buf[512];
    char m_pBrokers[128];

    vector<rd_kafka_topic_t *> m_vTopic;
};

#define kafkaproducer_h
#endif