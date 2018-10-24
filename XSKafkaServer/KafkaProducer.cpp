#include "KafkaProducer.h"

CKafkaProducer::CKafkaProducer()
{
}


CKafkaProducer::~CKafkaProducer()
{
}
//初始化Kafka资源
int CKafkaProducer::InitKafkaProducer(const char * pKafkaAddress)
{
    //35.24.22.158，35.24.22.159，35.24.22.142
    //sprintf(m_pBrokers, "%s", "35.24.22.158:9092");
    //sprintf(m_pTopic, "%s", "XINYI_IMAGE_INFO_TOPIC");

    strcpy(m_pBrokers, pKafkaAddress);

    /* 创建一个kafka配置占位 */
    m_pConf = rd_kafka_conf_new();

    /*创建broker集群*/
    if (rd_kafka_conf_set(m_pConf, "bootstrap.servers", m_pBrokers, errstr,
        sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        fprintf(stderr, "%s\n", errstr);
        return -1;
    }

    /*设置发送报告回调函数，rd_kafka_produce()接收的每条消息都会调用一次该回调函数
    *应用程序需要定期调用rd_kafka_poll()来服务排队的发送报告回调函数*/
    rd_kafka_conf_set_dr_msg_cb(m_pConf, MessageCallback);

    /*创建producer实例
    rd_kafka_new()获取conf对象的所有权,应用程序在此调用之后不得再次引用它*/
    rk = rd_kafka_new(RD_KAFKA_PRODUCER, m_pConf, errstr, sizeof(errstr));
    if (!rk)
    {
        fprintf(stderr, "%% Failed to create new producer:%s\n", errstr);
        return -1;
    }

    printf("Init kafka[%s] producer success.\n", pKafkaAddress);
    return 0;
}
//销毁Kafka资源
int CKafkaProducer::Uninit()
{
    fprintf(stderr, "%% Flushing final message.. \n");
    /*rd_kafka_flush是rd_kafka_poll()的抽象化，
    等待所有未完成的produce请求完成，通常在销毁producer实例前完成
    以确保所有排列中和正在传输的produce请求在销毁前完成*/
    rd_kafka_flush(rk, 10 * 1000);

    /* Destroy topic object */
    for (int i = 0; i < m_vTopic.size(); i++)
    {
        rd_kafka_topic_destroy(m_vTopic[i]);
    }

    /* Destroy the producer instance */
    rd_kafka_destroy(rk);

    return 0;
}
int CKafkaProducer::AddTopics(const char * pTopic)
{
    /*实例化一个或多个topics(`rd_kafka_topic_t`)来提供生产或消费，topic
    对象保存topic特定的配置，并在内部填充所有可用分区和leader brokers，*/
    rd_kafka_topic_t *rkt = rd_kafka_topic_new(rk, pTopic, NULL);
    if (!rkt)
    {
        fprintf(stderr, "%% Failed to create topic object: %s\n",
            rd_kafka_err2str(rd_kafka_last_error()));
        rd_kafka_destroy(rk);
        return -1;
    }
    //保存
    m_vTopic.push_back(rkt);

    return 0;
}
//发送消息
int CKafkaProducer::SendProducerMessage(char * pMsg, int nLen, int nOrder)
{
    const void * p = NULL;
    do 
    {
        if (rd_kafka_produce(
            /* Topic object */
            m_vTopic[nOrder],
            /*使用内置的分区来选择分区*/
            RD_KAFKA_PARTITION_UA,
            /*生成payload的副本*/
            RD_KAFKA_MSG_F_COPY,
            /*消息体和长度*/
            pMsg, nLen,
            /*可选键及其长度*/
            NULL, 0, NULL) == -1)
        {
            fprintf(stderr,
                "%% Failed to produce to topic %s: %s\n",
                rd_kafka_topic_name(m_vTopic[nOrder]),
                rd_kafka_err2str(rd_kafka_last_error()));

            if (rd_kafka_last_error() == RD_KAFKA_RESP_ERR__QUEUE_FULL)
            {
                /*如果内部队列满，等待消息传输完成并retry,
                内部队列表示要发送的消息和已发送或失败的消息，
                内部队列受限于queue.buffering.max.messages配置项*/
                rd_kafka_poll(rk, 1000);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    } while (true);
    
    /*producer应用程序应不断地通过以频繁的间隔调用rd_kafka_poll()来为
    传送报告队列提供服务。在没有生成消息以确定先前生成的消息已发送了其
    发送报告回调函数(和其他注册过的回调函数)期间，要确保rd_kafka_poll()
    仍然被调用*/
    rd_kafka_poll(rk, 0);

    return 0;
}
//发送消息结果回调, 消息是否正确发送
void CKafkaProducer::MessageCallback(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque)
{
    if (rkmessage->err)
        fprintf(stderr, "%% Message delivery failed: %s\n",
            rd_kafka_err2str(rkmessage->err));
    else
        fprintf(stderr,
            "%% Message delivered (%zd bytes, "
            "partition %"PRId32")\n",
            rkmessage->len, rkmessage->partition);
    /* rkmessage被librdkafka自动销毁*/

    return;
}