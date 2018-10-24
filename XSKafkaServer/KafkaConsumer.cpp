#include "KafkaConsumer.h"

CKafkaConsumer::CKafkaConsumer()
{
    m_pGroup = NULL;
}


CKafkaConsumer::~CKafkaConsumer()
{
}
//��ʼ��Kafka��Դ
int CKafkaConsumer::InitKafkaConsumer(const char * pKafkaAddress)
{
    strcpy(m_pBrokers, pKafkaAddress);

    if (initKafka(m_pBrokers, m_pGroup, m_pTopic) < 0)
    {
        fprintf(stderr, "kafka server initialize error\n");
    }

    printf("Init kafka[%s] consumer success.\n", pKafkaAddress);
    return 0;
}
//����Kafka��Դ
int CKafkaConsumer::Uninit()
{
    /*�˵��ý���������ֱ��consumer��������䣬����rebalance_cb(���������)��
    commit offset��broker,���뿪consumer group
    �������ʱ�䱻����Ϊsession.timeout.ms
    */
    rd_kafka_resp_err_t err = rd_kafka_consumer_close(rk);
    if (err)
    {
        fprintf(stderr, "%% Failed to close consumer: %s\n", rd_kafka_err2str(err));
    }
    else
    {
        fprintf(stderr, "%% Consumer closed\n");
    }

    //�ͷ�topics listʹ�õ�������Դ�����Լ�
    rd_kafka_topic_partition_list_destroy(topics);

    //destroy kafka handle
    rd_kafka_destroy(rk);

    int run = 5;
    //�ȴ�����rd_kafka_t�������٣�����kafka�������٣�����0����ʱ����-1
    while (run-- > 0 && rd_kafka_wait_destroyed(1000) == -1)
    {
        printf("Waiting for librdkafka to decommission\n");
    }
    if (run <= 0)
    {
        //dump rdkafka�ڲ�״̬��stdout��
        rd_kafka_dump(stdout, rk);
    }
    return 0;

}
//������Ϣ�ص�
int CKafkaConsumer::SetMessageCallback(LPKafkaMessageCallback pCallback, void * pUser)
{
    m_pMsgCallback = pCallback;
    m_pUser = pUser;
    return 0;
}
int CKafkaConsumer::initKafka(char * pBrokers, char * pGroup, char * pTopic)
{
    rd_kafka_conf_t *conf;
    rd_kafka_topic_conf_t *topic_conf;
    char tmp[16];
    char errstr[512];

    /* Kafka configuration */
    conf = rd_kafka_conf_new();

    //quick termination
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);

    //pTopic configuration
    topic_conf = rd_kafka_topic_conf_new();

    /* Consumer groups require a pGroup id */
    if (!pGroup)
    {
        pGroup = new char[1028];
        strcpy(pGroup, "rdkafka_consumer_example");
    }
    if (rd_kafka_conf_set(conf, "group.id", pGroup,
        errstr, sizeof(errstr)) !=
        RD_KAFKA_CONF_OK)
    {
        fprintf(stderr, "%% %s\n", errstr);
        return -1;
    }

    /* Consumer groups always use broker based offset storage */
    if (rd_kafka_topic_conf_set(topic_conf, "offset.store.method",
        "broker",
        errstr, sizeof(errstr)) !=
        RD_KAFKA_CONF_OK)
    {
        fprintf(stderr, "%% %s\n", errstr);
        return -1;
    }

    /* Set default pTopic config for pattern-matched topics. */
    rd_kafka_conf_set_default_topic_conf(conf, topic_conf);

    //ʵ����һ����������rd_kafka_t��Ϊ�����������ṩȫ�����ú͹���״̬
    rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    if (!rk)
    {
        fprintf(stderr, "%% Failed to create new consumer:%s\n", errstr);
        return -1;
    }

    //Librdkafka��Ҫ����һ��pBrokers�ĳ�ʼ��list
    if (rd_kafka_brokers_add(rk, pBrokers) == 0)
    {
        fprintf(stderr, "%% No valid pBrokers specified\n");
        return -1;
    }

    //�ض��� rd_kafka_poll()���е�consumer_poll()����
    rd_kafka_poll_set_consumer(rk);

    //����һ��Topic+Partition�Ĵ洢�ռ�(list/vector)
    topics = rd_kafka_topic_partition_list_new(1);
    
    return 1;
}
//���Ӷ���
int CKafkaConsumer::AddTopics(const char * pTopic)
{
    //��Topic+Partition����list
    rd_kafka_topic_partition_list_add(topics, pTopic, -1);
    return 0;
}
//��ʼ����, ׼����������
int CKafkaConsumer::StartSubscribe()
{
    rd_kafka_resp_err_t err;
    //����consumer���ģ�ƥ���pTopic������ӵ������б���
    if ((err = rd_kafka_subscribe(rk, topics)))
    {
        fprintf(stderr, "%% Failed to start consuming topics: %s\n", rd_kafka_err2str(err));
        return -1;
    }

    //��������Kafka��Ϣ�߳�
    pthread_create(&m_hThreadID, NULL, RecvKafkaMessageThread, (void*)this);

    return 0;
}
void * CKafkaConsumer::RecvKafkaMessageThread(void * pParam)
{
    CKafkaConsumer * pThis = (CKafkaConsumer *)pParam;
    pThis->RecvKafkaMessageAction();
    return NULL;
}
void CKafkaConsumer::RecvKafkaMessageAction()
{
    while (true)
    {
        rd_kafka_message_t *rkmessage;
        /*-��ѯ�����ߵ���Ϣ���¼����������timeout_ms
        -Ӧ�ó���Ӧ�ö��ڵ���consumer_poll()����ʹû��Ԥ�ڵ���Ϣ���Է���
        �����Ŷӵȴ��Ļص���������ע���rebalance_cb���ò�����Ϊ��Ҫ��
        ��Ϊ����Ҫ����ȷ�ص��úʹ�����ͬ���ڲ�������״̬ */
        rkmessage = rd_kafka_consumer_poll(rk, 1000);
        if (rkmessage)
        {
            msg_consume(rkmessage, NULL);
            /*�ͷ�rkmessage����Դ����������Ȩ����rdkafka*/
            rd_kafka_message_destroy(rkmessage);
        }
    }

    return;
}
void CKafkaConsumer::msg_consume(rd_kafka_message_t * rkmessage, void * opaque)
{
    if (rkmessage->err)
    {
        if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
        {
            fprintf(stderr,
                "%% Consumer reached end of %s [%"PRId32"] "
                "message queue at offset %"PRId64"\n",
                rd_kafka_topic_name(rkmessage->rkt),
                rkmessage->partition, rkmessage->offset);

            return;
        }

        if (rkmessage->rkt)
            fprintf(stderr, "%% Consume error for "
                "topic \"%s\" [%"PRId32"] "
                "offset %"PRId64": %s\n",
                rd_kafka_topic_name(rkmessage->rkt),
                rkmessage->partition,
                rkmessage->offset,
                rd_kafka_message_errstr(rkmessage));
        else
            fprintf(stderr, "%% Consumer error: %s: %s\n",
                rd_kafka_err2str(rkmessage->err),
                rd_kafka_message_errstr(rkmessage));

        if (rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
            rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)
        {
            //����, ֹͣ����
        }
        return;
    }

    fprintf(stdout, "%% Message (topic %s [%"PRId32"], "
        "offset %"PRId64", %zd bytes):\n",
        rd_kafka_topic_name(rkmessage->rkt),
        rkmessage->partition,
        rkmessage->offset, rkmessage->len);

    if (rkmessage->key_len)
    {
        printf("Key: %.*s\n", (int)rkmessage->key_len, (char *)rkmessage->key);
    }

    //printf("%.*s\n", (int)rkmessage->len, (char *)rkmessage->payload);
    m_pMsgCallback((char *)rkmessage->payload, (int)rkmessage->len, m_pUser);
}