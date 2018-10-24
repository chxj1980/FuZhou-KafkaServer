#ifndef kafkaproducer_h

#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "rdkafka.h"
class CKafkaProducer
{
public:
    CKafkaProducer();
    ~CKafkaProducer();
public:
    //��ʼ��Kafka��Դ
    int InitKafkaProducer(const char * pLWKafkaAddress, const char * pSnapTopic, const char * pAlarmTopic, const char * pLayoutLibTopic);
    //����Kafka��Դ
    int Uninit();
    //������Ϣ
    int SendProducerMessage(char * pMsg, int nLen);
    //������Ϣ����ص�, ��Ϣ�Ƿ���ȷ����
    static void MessageCallback(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque);
private:
    rd_kafka_t *rk;            /*Producer instance handle*/
    rd_kafka_topic_t *rkt;     /*topic����*/
    rd_kafka_conf_t * m_pConf;     /*��ʱ���ö���*/
    char errstr[512];
    char buf[512];
    char m_pBrokers[128];
    char m_pTopic[128];
};

#define kafkaproducer_h
#endif