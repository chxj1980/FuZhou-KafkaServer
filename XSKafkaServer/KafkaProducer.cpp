#include "KafkaProducer.h"

CKafkaProducer::CKafkaProducer()
{
}


CKafkaProducer::~CKafkaProducer()
{
}
//��ʼ��Kafka��Դ
int CKafkaProducer::InitKafkaProducer(const char * pKafkaAddress)
{
    //35.24.22.158��35.24.22.159��35.24.22.142
    //sprintf(m_pBrokers, "%s", "35.24.22.158:9092");
    //sprintf(m_pTopic, "%s", "XINYI_IMAGE_INFO_TOPIC");

    strcpy(m_pBrokers, pKafkaAddress);

    /* ����һ��kafka����ռλ */
    m_pConf = rd_kafka_conf_new();

    /*����broker��Ⱥ*/
    if (rd_kafka_conf_set(m_pConf, "bootstrap.servers", m_pBrokers, errstr,
        sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        fprintf(stderr, "%s\n", errstr);
        return -1;
    }

    /*���÷��ͱ���ص�������rd_kafka_produce()���յ�ÿ����Ϣ�������һ�θûص�����
    *Ӧ�ó�����Ҫ���ڵ���rd_kafka_poll()�������Ŷӵķ��ͱ���ص�����*/
    rd_kafka_conf_set_dr_msg_cb(m_pConf, MessageCallback);

    /*����producerʵ��
    rd_kafka_new()��ȡconf���������Ȩ,Ӧ�ó����ڴ˵���֮�󲻵��ٴ�������*/
    rk = rd_kafka_new(RD_KAFKA_PRODUCER, m_pConf, errstr, sizeof(errstr));
    if (!rk)
    {
        fprintf(stderr, "%% Failed to create new producer:%s\n", errstr);
        return -1;
    }

    printf("Init kafka[%s] producer success.\n", pKafkaAddress);
    return 0;
}
//����Kafka��Դ
int CKafkaProducer::Uninit()
{
    fprintf(stderr, "%% Flushing final message.. \n");
    /*rd_kafka_flush��rd_kafka_poll()�ĳ��󻯣�
    �ȴ�����δ��ɵ�produce������ɣ�ͨ��������producerʵ��ǰ���
    ��ȷ�����������к����ڴ����produce����������ǰ���*/
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
    /*ʵ����һ������topics(`rd_kafka_topic_t`)���ṩ���������ѣ�topic
    ���󱣴�topic�ض������ã������ڲ�������п��÷�����leader brokers��*/
    rd_kafka_topic_t *rkt = rd_kafka_topic_new(rk, pTopic, NULL);
    if (!rkt)
    {
        fprintf(stderr, "%% Failed to create topic object: %s\n",
            rd_kafka_err2str(rd_kafka_last_error()));
        rd_kafka_destroy(rk);
        return -1;
    }
    //����
    m_vTopic.push_back(rkt);

    return 0;
}
//������Ϣ
int CKafkaProducer::SendProducerMessage(char * pMsg, int nLen, int nOrder)
{
    const void * p = NULL;
    do 
    {
        if (rd_kafka_produce(
            /* Topic object */
            m_vTopic[nOrder],
            /*ʹ�����õķ�����ѡ�����*/
            RD_KAFKA_PARTITION_UA,
            /*����payload�ĸ���*/
            RD_KAFKA_MSG_F_COPY,
            /*��Ϣ��ͳ���*/
            pMsg, nLen,
            /*��ѡ�����䳤��*/
            NULL, 0, NULL) == -1)
        {
            fprintf(stderr,
                "%% Failed to produce to topic %s: %s\n",
                rd_kafka_topic_name(m_vTopic[nOrder]),
                rd_kafka_err2str(rd_kafka_last_error()));

            if (rd_kafka_last_error() == RD_KAFKA_RESP_ERR__QUEUE_FULL)
            {
                /*����ڲ����������ȴ���Ϣ������ɲ�retry,
                �ڲ����б�ʾҪ���͵���Ϣ���ѷ��ͻ�ʧ�ܵ���Ϣ��
                �ڲ�����������queue.buffering.max.messages������*/
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
    
    /*producerӦ�ó���Ӧ���ϵ�ͨ����Ƶ���ļ������rd_kafka_poll()��Ϊ
    ���ͱ�������ṩ������û��������Ϣ��ȷ����ǰ���ɵ���Ϣ�ѷ�������
    ���ͱ���ص�����(������ע����Ļص�����)�ڼ䣬Ҫȷ��rd_kafka_poll()
    ��Ȼ������*/
    rd_kafka_poll(rk, 0);

    return 0;
}
//������Ϣ����ص�, ��Ϣ�Ƿ���ȷ����
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
    /* rkmessage��librdkafka�Զ�����*/

    return;
}