#include "KafkaServer.h"



CKafkaServer::CKafkaServer()
{
    m_pZeromqManage = NULL;
    m_pKafkaProducer = NULL;
    m_pKafkaConsumer = NULL;
    m_pRedisManage = NULL;

    pthread_mutex_init(&m_mutex, NULL);
    pthread_mutex_init(&m_mutexDB, NULL);
    pthread_mutex_init(&m_mutexAlarm, NULL);
    pipe(m_nPipe);
    for (int i = 0; i < 128; i++)
    {
        m_hThreadID[i] = -1;
    }
}


CKafkaServer::~CKafkaServer()
{
    if (1 == m_nServerType || 3 == m_nServerType)
    {
        delete m_pKafkaProducer;
    }
    if (2 == m_nServerType || 4 == m_nServerType)
    {
        delete m_pKafkaConsumer;
    }
    delete m_pZeromqManage;

    pthread_mutex_destroy(&m_mutex);
    pthread_mutex_destroy(&m_mutexDB);
    pthread_mutex_destroy(&m_mutexAlarm);
    close(m_nPipe[0]);
    close(m_nPipe[1]);
}
bool CKafkaServer::StartKafkaServer(int nServerType)
{
    m_nServerType = nServerType;
    if (!InitServer())
    {
        printf("***Warning: StartAnalyseServer::Init Failed.\n");
        return false;
    }

    unsigned char pIn;
    while (true)
    {
        pIn = getchar();
        switch (pIn)
        {
        case 'a':
        {
            if (1 == m_nServerType)
            {
                printf("[ServerType: %d]KafkaAddress: %s\nTopic:\n\t%s\n",
                    m_nServerType, m_ConfigRead.m_sLWSpecialKafkaAddress.c_str(), m_ConfigRead.m_sSnapTopic.c_str());
            }
            else if (3 == m_nServerType)
            {
                printf("[ServerType: %d]KafkaAddress: %s\nTopic:\n\t%s\n\t%s\n",
                    m_nServerType, m_ConfigRead.m_sLWSpecialKafkaAddress.c_str(), 
                    m_ConfigRead.m_sAlarmTopic.c_str(), m_ConfigRead.m_sLayoutLibTopic.c_str());
            }
            if (2 == m_nServerType)
            {
                printf("[ServerType: %d]KafkaAddress: %s\nTopic:\n\t%s\n",
                    m_nServerType, m_ConfigRead.m_sLWGAKafkaAddress.c_str(), m_ConfigRead.m_sSnapTopic.c_str());
            }
            else if (4 == m_nServerType)
            {
                printf("[ServerType: %d]KafkaAddress: %s\nTopic:\n\t%s\n\t%s\n",
                    m_nServerType, m_ConfigRead.m_sLWGAKafkaAddress.c_str(),
                    m_ConfigRead.m_sAlarmTopic.c_str(), m_ConfigRead.m_sLayoutLibTopic.c_str());
            }
        }
            
        }
        sleep(1);
    }

    return true;
}
bool CKafkaServer::StopServer()
{

    return true;
}
bool CKafkaServer::InitServer()
{
    if (!m_ConfigRead.ReadConfig())
    {
        printf("Read Config Failed.\n");
        return false;
    }
    if (1 == m_nServerType || 3 == m_nServerType)
    {
        if (!InitKafkaServer())
        {
            return false;
        }
        if (!InitZeromq(m_ConfigRead.m_sSpecialProxyIP.c_str(), m_ConfigRead.m_nProxyPubPort))
        {
            return false;
        }
    }
    else if (2 == m_nServerType || 4 == m_nServerType)
    {
        if (!InitZeromq(m_ConfigRead.m_sGAProxyIP.c_str(), m_ConfigRead.m_nProxySubPort))
        {
            return false;
        }
        
        if (4 == m_nServerType)
        {
            //��ʼ��Redis
            if (!InitRedis(m_ConfigRead.m_sRedisIP.c_str(), m_ConfigRead.m_nRedisPort))
            {
                return false;
            }
            //��ʼ���������ݿ�
            if(!InitDB())
            {
                return false;
            }
            if (!GetStoreLibInfoFromDB())
            {
                return false;
            }
            pthread_create(&m_hAlarmThreadID, NULL, AlarmThread, (void*)this);  //Ԥ����Ϣ���������߳�
        }

        for (int i = 0; i < m_ConfigRead.m_nThreadCount; i++)
        {
            pthread_create(&m_hThreadID[i], NULL, MessageThread, (void*)this);
        }

        if (!InitKafkaServer())
        {
            return false;
        }
    }
    else
    {
        printf("Kafka Type not define!\n");
        return false;
    }
    

    return true;
}
int CKafkaServer::Uninit()
{
    if (1 == m_nServerType || 3 == m_nServerType)
    {
        m_pKafkaProducer->Uninit();
    }
    else if (2 == m_nServerType || 4 == m_nServerType)
    {
        m_pKafkaConsumer->Uninit();
    }
    m_pZeromqManage->UnInit();
    return 0;
}
bool CKafkaServer::InitKafkaServer()
{
    if (1 == m_nServerType || 3 == m_nServerType)   //ר��kafka����
    {
        if (NULL == m_pKafkaProducer)
        {
            m_pKafkaProducer = new CKafkaProducer;
        }
        if (m_pKafkaProducer->InitKafkaProducer(m_ConfigRead.m_sLWSpecialKafkaAddress.c_str()) < 0) //��ʼ��ר��kafka����
        {
            printf("Init KafkaProducer Failed.\n");
            return false;
        }
        if (1 == m_nServerType)
        {
            m_pKafkaProducer->AddTopics(m_ConfigRead.m_sSnapTopic.c_str());         //����: ����kafkaץ������
        }
        else if (3 == m_nServerType)
        {
            m_pKafkaProducer->AddTopics(m_ConfigRead.m_sAlarmTopic.c_str());        //����: ����kafka���ر�������
            m_pKafkaProducer->AddTopics(m_ConfigRead.m_sLayoutLibTopic.c_str());    //����: ����kafka���ؿ�ͼƬ��Ϣ����
        }
    }
    else if (2 == m_nServerType || 4 == m_nServerType)  //������kafka����
    {
        if (NULL == m_pKafkaConsumer)
        {
            m_pKafkaConsumer = new CKafkaConsumer;
        }
        m_pKafkaConsumer->SetMessageCallback(KafkaMessageCallback, this);           //���ö�����Ϣ�ص�����
        if (m_pKafkaConsumer->InitKafkaConsumer(m_ConfigRead.m_sLWGAKafkaAddress.c_str()) < 0)  //��ʼ��������kafka����
        {
            printf("Init KafkaConsumer Failed.\n");
            return false;
        }
        if (2 == m_nServerType)
        {
            m_pKafkaConsumer->AddTopics(m_ConfigRead.m_sSnapTopic.c_str());         //����: ����kafkaץ������
        }
        else if (4 == m_nServerType)
        {
            m_pKafkaConsumer->AddTopics(m_ConfigRead.m_sAlarmTopic.c_str());        //����: ����kafka���ر�������
            m_pKafkaConsumer->AddTopics(m_ConfigRead.m_sLayoutLibTopic.c_str());    //����: ����kafka���ؿ�ͼƬ��Ϣ����
        }
        m_pKafkaConsumer->StartSubscribe();
    }
    return true;
}

//��ʼ��Zeromq
bool CKafkaServer::InitZeromq(const char * pZmqIP, int nPort)
{
    if (NULL == m_pZeromqManage)
    {
        m_pZeromqManage = new CZeromqManage;
    }
    if (1 == m_nServerType || 3 == m_nServerType)
    {
        if (!m_pZeromqManage->InitSub(NULL, 0, (char*)pZmqIP, nPort, ZeromqSubMsg, this, 1))
        {
            printf("****Error: init sub zeromq[%s:%d]ʧ��!", pZmqIP, nPort);
            return false;
        }
        if (1 == m_nServerType)
        {
            m_pZeromqManage->AddSubMessage(m_ConfigRead.m_sZmqSnapTopic.c_str());    //����1 ����ץ������
            m_pZeromqManage->AddSubMessage("3");    //����1 ����ץ������
        }
        else
        {
            m_pZeromqManage->AddSubMessage(m_ConfigRead.m_sZmqAlarmTopic.c_str());    //����3 �����ر�������
            m_pZeromqManage->AddSubMessage(m_ConfigRead.m_sZmqLayoutTopic.c_str());   //����3 ���Ĳ��ؿ�ͼƬ��Ϣ����
        }
    }
    else if (2 == m_nServerType || 4 == m_nServerType)
    {
        if (!m_pZeromqManage->InitPub(NULL, 0, (char*)pZmqIP, nPort))
        {
            printf("****Error: init pub zeromq[%s:%d]ʧ��!", pZmqIP, nPort);
            return false;
        }

        if (4 == m_nServerType)
        {
            if (!m_pZeromqManage->InitSub(NULL, 0, (char*)pZmqIP, nPort - 1, ZeromqSubMsg, this, 1))
            {
                printf("****Error: init sub zeromq[%s:%d]ʧ��!", pZmqIP, nPort);
                return false;
            }
            m_pZeromqManage->AddSubMessage(KAFKALAYOUTSERVER);
        }
    }
    else
    {
        return false;
    }
    
    return true;
}
//��ʼ��Redis
bool CKafkaServer::InitRedis(const char * pRedisIP, int nPort)
{
    if (NULL == m_pRedisManage)
    {
        m_pRedisManage = new CRedisManage;
        if (!m_pRedisManage->InitRedis(pRedisIP, nPort))
        {
            return false;
        }
    }
    return true;
}
//��ʼ���������ݿ�
bool CKafkaServer::InitDB()
{
    mysql_init(&m_mysql);
    char value = 1;
    mysql_options(&m_mysql, MYSQL_OPT_RECONNECT, (char *)&value);
    if (!mysql_real_connect(&m_mysql, m_ConfigRead.m_sDBIP.c_str(), m_ConfigRead.m_sDBUser.c_str(),
        m_ConfigRead.m_sDBPd.c_str(), m_ConfigRead.m_sDBName.c_str(), m_ConfigRead.m_nDBPort, NULL, 0))
    {
        const char * pErrorMsg = mysql_error(&m_mysql);
        printf("%s\n", pErrorMsg);
        printf("***Warning: CLibInfo::mysql_real_connect Failed, Please Check MySQL Service is start!\n");
        printf("DBInfo: %s:%s:%s:%s:%d\n", m_ConfigRead.m_sDBIP.c_str(), m_ConfigRead.m_sDBUser.c_str(),
            m_ConfigRead.m_sDBPd.c_str(), m_ConfigRead.m_sDBName.c_str(), m_ConfigRead.m_nDBPort);
        return false;
    }
    else
    {
        printf("CLibInfo::Connect MySQL Success!\n");
    }
    return true;

}
//kafka server4 �ӹ�����DB��ȡ�ص����Ϣ
bool CKafkaServer::GetStoreLibInfoFromDB()
{
    char pSQL[SQLMAXLEN] = { 0 };
    int nRet = INVALIDERROR;
    //��ȡ��������ֵ��Ϣ
    int nRead = 0;
    sprintf(pSQL, "select id from storelib_special");
    nRet = mysql_query(&m_mysql, pSQL);
    if (nRet == 1)
    {
        const char * pErrorMsg = mysql_error(&m_mysql);
        printf("***Warning: Excu SQL Failed[\"%s\"], SQL:\n%s\n", pErrorMsg, pSQL);
        return false;
    }
    MYSQL_RES *result = mysql_store_result(&m_mysql);
    if (NULL == result)
    {
        printf("Excu SQL Failed, SQL:\n%s\n", pSQL);
        return false;
    }
    int nRowCount = mysql_num_rows(result);
    
    int nLibID = 0;
    if (nRowCount > 0)
    {
        MYSQL_ROW row = NULL;
        row = mysql_fetch_row(result);
        while (NULL != row)
        {
            if (NULL != row[0])
            {
                nLibID = strtol(row[0], NULL, 10);
                m_setLibID.insert(nLibID);
            }
            row = mysql_fetch_row(result);
        }
    }

    return true;
}

/***********************ר���ص�zmq��Ϣ****************************/
//ר��Zeromq��Ϣ�ص�
void CKafkaServer::ZeromqSubMsg(LPSUBMESSAGE pSubMessage, void * pUser)
{
    CKafkaServer * pThis = (CKafkaServer *)pUser;
    pThis->ParseZeromqJson(pSubMessage);
}
//ר��Zeromq�ص���Ϣ��������
bool CKafkaServer::ParseZeromqJson(LPSUBMESSAGE pSubMessage)
{
    int nRet = 0;
    string sCommand(pSubMessage->pOperationType);
    if (sCommand == COMMANDADD)        //����ͼƬ
    {
        rapidjson::Document document;
        if (string(pSubMessage->pSubJsonValue) != "" && pSubMessage->pSubJsonValue[0] == '{')
        {
            document.Parse(pSubMessage->pSubJsonValue);
            if (document.HasParseError())
            {
                printf("***Warning: Parse Json Format Failed[%s].\n", pSubMessage->pSubJsonValue);
                return false;
            }
        }
        else
        {
            printf("***Warning: Parse Json Format Failed[%s].\n", pSubMessage->pSubJsonValue);
            return false;
        }

        if (document.HasMember(JSONFACEUUID) && document[JSONFACEUUID].IsString() && strlen(document[JSONFACEUUID].GetString()) < MAXLEN       &&
            document.HasMember(JSONFEATURE) && document[JSONFEATURE].IsString() && strlen(document[JSONFEATURE].GetString()) < FEATURELEN    &&
            document.HasMember(JSONTIME) && document[JSONTIME].IsInt64() && strlen(document[JSONFEATURE].GetString()) > FEATUREMIXLEN &&
            document.HasMember(JSONDRIVE) && document[JSONDRIVE].IsString() && strlen(document[JSONDRIVE].GetString()) == 1 &&
            document.HasMember(JSONSERVERIP) && document[JSONSERVERIP].IsString() && strlen(document[JSONSERVERIP].GetString()) < MAXIPLEN     &&
            document.HasMember(JSONFACERECT) && document[JSONFACERECT].IsString() && strlen(document[JSONFACERECT].GetString()) < MAXIPLEN      &&
            document.HasMember(JSONFACEURL) && document[JSONFACEURL].IsString() && strlen(document[JSONFACEURL].GetString()) < 2048     &&
            document.HasMember(JSONBKGURL) && document[JSONBKGURL].IsString() && strlen(document[JSONBKGURL].GetString()) < 2048
            )
        {
            //1. ����FaceUUID
            strcpy(pSubMessage->pFaceUUID, document[JSONFACEUUID].GetString());
            //2. ��������ֵ������
            strcpy(pSubMessage->pFeature, document[JSONFEATURE].GetString());
            pSubMessage->nFeatureLen = strlen(document[JSONFEATURE].GetString());
            //3. ��������ֵʱ��
            pSubMessage->nTime = document[JSONTIME].GetInt64();
            //4. �����̷�
            //5. ����ͼƬ���������IP
            //6. ��������ͼƬ����
            strcpy(pSubMessage->pFaceRect, document[JSONFACERECT].GetString());
            //7. ��������url
            strcpy(pSubMessage->pFaceURL, document[JSONFACEURL].GetString());
            //8. ���汳��url
            strcpy(pSubMessage->pBkgURL, document[JSONBKGURL].GetString());

            //����Kakfa��Ϣ��ʽ��
            {
                rapidjson::Document document;
                document.SetObject();
                rapidjson::Document::AllocatorType&allocator = document.GetAllocator();

                 document.AddMember(KAFKADEVICEID, rapidjson::StringRef(pSubMessage->pHead), allocator);
                 document.AddMember(KAFKAFACEUUID, rapidjson::StringRef(pSubMessage->pFaceUUID), allocator);
                 document.AddMember(KAFKAFACETIME, pSubMessage->nTime, allocator);
                 document.AddMember(KAFKAFACERECT, rapidjson::StringRef(pSubMessage->pFaceRect), allocator);
                 document.AddMember(KAFKAFACEURL, rapidjson::StringRef(pSubMessage->pFaceURL), allocator);
                 document.AddMember(KAFKABKGURL, rapidjson::StringRef(pSubMessage->pBkgURL), allocator);
                 document.AddMember(KAFKAFEATURE, rapidjson::StringRef(pSubMessage->pFeature), allocator);
                 document.AddMember(KAFKAFEATURELEN, pSubMessage->nFeatureLen, allocator);


                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                document.Accept(writer);
                string sProduceMsg = buffer.GetString();

                printf("[DeviceID: %s][faceUUID: %s]\n", pSubMessage->pHead, pSubMessage->pFaceUUID);

                m_pKafkaProducer->SendProducerMessage((char*)sProduceMsg.c_str(), sProduceMsg.size());
            }
        }
        else
        {
            nRet = JsonFormatError;
        }
    }
    else if (sCommand == "AlarmInfo")       //ר�����ر�����Ϣ
    {
        printf("AlarmInfo:\n%s\n", pSubMessage->pSubJsonValue);
        m_pKafkaProducer->SendProducerMessage(pSubMessage->pSubJsonValue, strlen(pSubMessage->pSubJsonValue), 0);
    }
    else if (sCommand == "LayoutLibImageInfo")  //ר������ͼƬ��Ϣ
    {
        //printf("LayoutLibImageInfo:\n%s\n%s\n%s\n%s\n", pSubMessage->pHead, pSubMessage->pOperationType, pSubMessage->pSource, pSubMessage->pSubJsonValue);
        printf("LayoutLibImageInfo:\n%s\n", pSubMessage->pSubJsonValue);
        m_pKafkaProducer->SendProducerMessage(pSubMessage->pSubJsonValue, strlen(pSubMessage->pSubJsonValue), 1);
    }
    else if (sCommand == "getfaceurl")  //�������ӿ��ڷ������񷵻�ץ��ͼƬurl��Ϣ
    {
        printf("Recv XSAnalyse_Checkpoint face URL Info:\n%s\n", pSubMessage->pSubJsonValue);
        HandleAlarmMessage(pSubMessage);
    }
    else
    {
        printf("Command Not Found.\n", sCommand.c_str());
        return false;
    }

    return true;
}

/***********************�������ص�kafka��Ϣ****************************/
//������Kafka�ص���Ϣ����
void CKafkaServer::KafkaMessageCallback(char * pMsg, int nLen, void * pUser)
{
    CKafkaServer * pThis = (CKafkaServer *)pUser;
    pthread_mutex_lock(&pThis->m_mutex);
    pThis->m_listKafkaMsg.push_back(string(pMsg, nLen));
    write(pThis->m_nPipe[1], "1", 1);
    pthread_mutex_unlock(&pThis->m_mutex);
}
void * CKafkaServer::MessageThread(void * pParam)
{
    CKafkaServer * pThis = (CKafkaServer *)pParam;
    pThis->MessageAction();
    return NULL;
}
int nCount = 0;
void CKafkaServer::MessageAction()
{
    int nSeq = nCount++;
    char pPipeRead[10] = { 0 };
    string sMsg = "";
    LPSUBMESSAGE pPubMessage = new SUBMESSAGE;
    while (true)
    {
        read(m_nPipe[0], pPipeRead, 1);
        do 
        {
            pthread_mutex_lock(&m_mutex);
            if (m_listKafkaMsg.size() > 0)
            {
                sMsg = m_listKafkaMsg.front();
                m_listKafkaMsg.pop_front();
            }
            else
            {
                sMsg = "";
            }
            pthread_mutex_unlock(&m_mutex);

            if ("" != sMsg)
            {
                pPubMessage->Init();
                printf("-----Thread[%d] :\n%s\n", nSeq, sMsg.c_str());
                if (2 == m_nServerType) //kafka server2, ����ץ��ͼƬ��Ϣ��zmq�������
                {
                    PubSnapMessage(sMsg.c_str(), sMsg.size(), pPubMessage);
                }
                if (4 == m_nServerType) //kafka server4, ������ͼƬ��Ϣ�Ͳ��ر�����Ϣ
                {
                    if (sMsg.find("layoutface_url") != string::npos)    //����ͼƬ��Ϣ
                    {
                        //����ͼƬ��Ϣд�����ݿ�
                        InsertLayoutFaceInfoToDB(sMsg.c_str(), sMsg.size());
                    }
                    else                                                //����Ԥ����Ϣ
                    {
                        //������Ԥ����Ϣ, �򿨿ڷ��������ѯץ��ͼƬ��Ӧurl, �ȴ����غ�д��DB, ����Redis���뱨����Ϣ
                        SaveAlarmMessage(sMsg.c_str(), sMsg.size());
                    }
                }
            }

        } while ("" != sMsg);
    }
}
//�������ץ����Ϣ(��ͬ�ڲɼ�����)
void CKafkaServer::PubSnapMessage(const char * pMsg, int nLen, LPSUBMESSAGE pPubMessage)
{
    rapidjson::Document document;
    if (string(pMsg, nLen) != "" && pMsg[0] == '{')
    {
        document.Parse(pMsg);
        if (document.HasParseError())
        {
            printf("***Warning: Parse Json Format Failed, %d\n%s\n", nLen, pMsg);
            return;
        }
    }
    else
    {
        printf("***Warning: Json Format Wrong!, %d\n%s\n", nLen,  pMsg);
        return;
    }
    if (document.HasMember(KAFKADEVICEID) && document[KAFKADEVICEID].IsString() && strlen(document[KAFKADEVICEID].GetString()) < MAXLEN     &&
        document.HasMember(KAFKAFACEUUID) && document[KAFKAFACEUUID].IsString() && strlen(document[KAFKAFACEUUID].GetString()) < MAXLEN     &&
        document.HasMember(KAFKAFACETIME) && document[KAFKAFACETIME].IsString()  && 
        document.HasMember(KAFKAFACERECT) && document[KAFKAFACERECT].IsString() && strlen(document[KAFKAFACERECT].GetString()) < MAXLEN     &&
        document.HasMember(KAFKAGAFACEURL)  && document[KAFKAGAFACEURL].IsString()  && strlen(document[KAFKAGAFACEURL].GetString()) < 2048        &&
        document.HasMember(KAFKAGABKGURL)   && document[KAFKAGABKGURL].IsString()   && strlen(document[KAFKAGABKGURL].GetString()) < 2048         &&
        document.HasMember(KAFKAFEATURE)  && document[KAFKAFEATURE].IsString()  && strlen(document[KAFKAFEATURE].GetString()) < FEATURELEN 
        )
    {
        //1. ����DeviceID
        strcpy(pPubMessage->pHead, document[KAFKADEVICEID].GetString());
        //2. ����FaceUUID
        strcpy(pPubMessage->pFaceUUID, document[KAFKAFACEUUID].GetString());
        //3. ��������ֵʱ��
        strcpy(pPubMessage->pTime, document[KAFKAFACETIME].GetString());
        pPubMessage->nTime = strtol(pPubMessage->pTime, NULL, 10);//document[KAFKAFACETIME].GetInt64();
        //4. ��������ͼƬ����
        strcpy(pPubMessage->pFaceRect, document[KAFKAFACERECT].GetString());
        //5. ��������������url
        sprintf(pPubMessage->pFaceURL,"%s%s", m_ConfigRead.m_sGASnapUrl.c_str(), document[KAFKAGAFACEURL].GetString());
        //6. ���汳��������url
        sprintf(pPubMessage->pBkgURL, "%s%s", m_ConfigRead.m_sGASnapUrl.c_str(), document[KAFKAGABKGURL].GetString());
        //7. ��������ֵ������
        strcpy(pPubMessage->pFeature, document[KAFKAFEATURE].GetString());


        //���ɲɼ�������Ϣ��ʽ��
        {
            rapidjson::Document document;
            document.SetObject();
            rapidjson::Document::AllocatorType&allocator = document.GetAllocator();

            document.AddMember(JSONFACEUUID,    rapidjson::StringRef(pPubMessage->pFaceUUID), allocator);
            document.AddMember(JSONFEATURE,     rapidjson::StringRef(pPubMessage->pFeature), allocator);
            document.AddMember(JSONTIME,        pPubMessage->nTime, allocator);
            document.AddMember(JSONDRIVE,       rapidjson::StringRef("D"), allocator);
            document.AddMember(JSONSERVERIP,    rapidjson::StringRef("0.0.0.0"), allocator);
            document.AddMember(JSONFACERECT,    rapidjson::StringRef(pPubMessage->pFaceRect), allocator);
            document.AddMember(JSONFACEURL,     rapidjson::StringRef(pPubMessage->pFaceURL), allocator);
            document.AddMember(JSONBKGURL,      rapidjson::StringRef(pPubMessage->pBkgURL), allocator);


            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);
            pPubMessage->sPubJsonValue = buffer.GetString();

            printf("%s\n", pPubMessage->pHead);
            printf("%s\n", pPubMessage->sPubJsonValue.c_str());

            strcpy(pPubMessage->pOperationType, COMMANDADD);
            strcpy(pPubMessage->pSource, "XSKafkaConsumer");

            m_pZeromqManage->PubMessage(pPubMessage);
        }
    }
    else
    {
        printf("***Warning: Json Get Data Failed.\n");
    }

    return;
}
void CKafkaServer::SaveAlarmMessage(const char * pMsg, int nLen)
{
    rapidjson::Document document;
    if (string(pMsg, nLen) != "" && pMsg[0] == '{')
    {
        document.Parse(pMsg);
        if (document.HasParseError())
        {
            printf("***Warning: Parse Json Format Failed, %d\n%s\n", nLen, pMsg);
            return;
        }
    }
    else
    {   
        printf("***Warning: Json Format Wrong!, %d\n%s\n", nLen, pMsg);
        return;
    }
    if (document.HasMember(KAFKADEVICEID) && document[KAFKADEVICEID].IsString() && strlen(document[KAFKADEVICEID].GetString()) < MAXLEN     &&
        document.HasMember(KAFKAFACEUUID) && document[KAFKAFACEUUID].IsString() && strlen(document[KAFKAFACEUUID].GetString()) < MAXLEN     &&
        document.HasMember(KAFKALAYOUTLIBID) && document[KAFKALAYOUTLIBID].IsString() && strlen(document[KAFKALAYOUTLIBID].GetString()) < MAXLEN     &&
        document.HasMember(KAFKALAYOUTFACEUUID) && document[KAFKALAYOUTFACEUUID].IsString() && strlen(document[KAFKALAYOUTFACEUUID].GetString()) < MAXLEN     &&
        document.HasMember(KAFKAFACETIME) && document[KAFKAFACETIME].IsString() && strlen(document[KAFKAFACETIME].GetString()) < MAXLEN     &&
        document.HasMember(KAFKASCORE) && document[KAFKASCORE].IsString() && strlen(document[KAFKASCORE].GetString()) < 10
        )
    {
        string sFaceUUID = document[KAFKAFACEUUID].GetString();
        pthread_mutex_lock(&m_mutexAlarm);
        MAPLAYOUTLIBALARMINFO::iterator it = m_mapLayoutLibAlarmInfo.find(sFaceUUID);
        if (it == m_mapLayoutLibAlarmInfo.end())
        {
            //����ץ��ͼƬ��Ϣ
            LPLAYOUTLIBALARMINFO pLayoutAlarmInfo = new LAYOUTLIBALARMINFO;
            strcpy(pLayoutAlarmInfo->pDeviceID, document[KAFKADEVICEID].GetString());
            strcpy(pLayoutAlarmInfo->pFaceUUID, document[KAFKAFACEUUID].GetString());
            string sFaceTime = document[KAFKAFACETIME].GetString();
            pLayoutAlarmInfo->nTime = strtol(sFaceTime.c_str(), NULL, 10);

            //���沼�ؿ�����ͼƬ��Ϣ
            LPLAYOUTFACEINFO pLayoutFaceInfo = new LAYOUTFACEINFO;
            strcpy(pLayoutFaceInfo->pLayoutFaceUUID, document[KAFKALAYOUTFACEUUID].GetString());
            pLayoutFaceInfo->nLayoutLibID = atoi(document[KAFKALAYOUTLIBID].GetString());
            pLayoutFaceInfo->nScore = atoi(document[KAFKASCORE].GetString());
            pLayoutAlarmInfo->listLayoutFaceInfo.push_back(pLayoutFaceInfo);

            //ץ��ͼƬ��Ϣ����map����            
            m_mapLayoutLibAlarmInfo.insert(make_pair(pLayoutAlarmInfo->pFaceUUID, pLayoutAlarmInfo));

            //��zmq������ͼƬurl��ѯ��Ϣ, �ڶ��ķ����߳��д������
            {
                LPSUBMESSAGE pPubMessage = new SUBMESSAGE;
                strcpy(pPubMessage->pHead, pLayoutAlarmInfo->pDeviceID);
                strcpy(pPubMessage->pOperationType, "getfaceurl");
                strcpy(pPubMessage->pSource, KAFKALAYOUTSERVER);

                rapidjson::Document document;
                document.SetObject();
                rapidjson::Document::AllocatorType&allocator = document.GetAllocator();

                document.AddMember(JSONFACEUUID, rapidjson::StringRef(pLayoutAlarmInfo->pFaceUUID), allocator);
                document.AddMember(JSONTIME, pLayoutAlarmInfo->nTime, allocator);

                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                document.Accept(writer);
                pPubMessage->sPubJsonValue = buffer.GetString();

                printf("send zmq msg to Checkpoint[%s]:\n%s\n", pLayoutAlarmInfo->pDeviceID, pPubMessage->sPubJsonValue.c_str());
                m_pZeromqManage->PubMessage(pPubMessage);

                m_mapPubMessage.insert(make_pair(pLayoutAlarmInfo->pFaceUUID, pPubMessage));
            }
        }
        else
        {
            //���沼�ؿ�����ͼƬ��Ϣ
            LPLAYOUTFACEINFO pLayoutFaceInfo = new LAYOUTFACEINFO;
            strcpy(pLayoutFaceInfo->pLayoutFaceUUID, document[KAFKALAYOUTFACEUUID].GetString());
            pLayoutFaceInfo->nLayoutLibID = atoi(document[KAFKALAYOUTLIBID].GetString());
            pLayoutFaceInfo->nScore = atoi(document[KAFKASCORE].GetString());
            it->second->listLayoutFaceInfo.push_back(pLayoutFaceInfo);
        }
        pthread_mutex_unlock(&m_mutexAlarm);
    }
    return;
}
void * CKafkaServer::AlarmThread(void * pParam)
{
    CKafkaServer * pThis = (CKafkaServer *)pParam;
    pThis->AlarmAction();
    return NULL;
}
void CKafkaServer::AlarmAction()
{
    while (true)
    {
        pthread_mutex_lock(&m_mutexAlarm);
        map<string, LPSUBMESSAGE>::iterator it = m_mapPubMessage.begin();
        for (; it != m_mapPubMessage.end(); it ++)
        {
            m_pZeromqManage->PubMessage(it->second);
        }
        pthread_mutex_unlock(&m_mutexAlarm);

        sleep(3);
    }


    return;
}
//���������ڷ������񷵻�ץ��ͼƬ��Ӧurl��Ϣ, ��������, ����layout_special, ����reids������Ϣlayout_special
void CKafkaServer::HandleAlarmMessage(LPSUBMESSAGE pPubMessage)
{
    rapidjson::Document document;
    if (string(pPubMessage->pSubJsonValue) != "")
    {
        document.Parse(pPubMessage->pSubJsonValue);
        if (document.HasParseError())
        {
            printf("***Warning: Parse Json Format Failed\n%s\n", pPubMessage->pSubJsonValue);
            return;
        }
    }
    else
    {
        printf("***Warning: Json Format Wrong!\n%s\n", pPubMessage->pSubJsonValue);
        return;
    }
    if (document.HasMember(JSONFACEUUID)    && document[JSONFACEUUID].IsString()    && strlen(document[JSONFACEUUID].GetString()) < MAXLEN      &&
        document.HasMember(JSONFACERECT)    && document[JSONFACERECT].IsString()    && strlen(document[JSONFACERECT].GetString()) < MAXLEN      &&
        document.HasMember(JSONFACEURL)     && document[JSONFACEURL].IsString()     && strlen(document[JSONFACEURL].GetString()) < 2048         &&
        document.HasMember(JSONBKGURL)      && document[JSONBKGURL].IsString()      && strlen(document[JSONBKGURL].GetString()) < 2048
        )
    {
        string sFaceUUID = document[JSONFACEUUID].GetString();
        pthread_mutex_lock(&m_mutexAlarm);
        MAPLAYOUTLIBALARMINFO::iterator it = m_mapLayoutLibAlarmInfo.find(sFaceUUID);
        if (it == m_mapLayoutLibAlarmInfo.end())
        {
            printf("***Warning: checkpoint analyse server return faceurl info not found alarm info![%s]\n", sFaceUUID.c_str());
            pthread_mutex_unlock(&m_mutexAlarm);
            return;
        }
        else
        {
            string sFaceRect = document[JSONFACERECT].GetString();
            string sFaceURL  = document[JSONFACEURL].GetString();
            string sBkgURL   = document[JSONBKGURL].GetString();

            char pSQL[4096] = { 0 };
            list<LPLAYOUTFACEINFO>::iterator itLayout = it->second->listLayoutFaceInfo.begin();
            for (; itLayout != it->second->listLayoutFaceInfo.end(); itLayout++)
            {
                string sImageTime = ChangeSecondToTime(it->second->nTime / 1000);
                //������Ϣд�����ݿ�
                sprintf(pSQL,
                    "insert into layoutResult_special ( LayoutFaceUUID, checkpoint, FaceUUID, imagedisk, imageip, facerect, Time, Score, LayoutLibID, face_url, bkg_url) "
                    "values('%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s')",
                    (*itLayout)->pLayoutFaceUUID, it->second->pDeviceID, it->second->pFaceUUID, "D", "",
                    sFaceRect.c_str(), sImageTime.c_str(), (*itLayout)->nScore, (*itLayout)->nLayoutLibID, sFaceURL.c_str(), sBkgURL.c_str());
                pthread_mutex_lock(&m_mutexDB);
                mysql_ping(&m_mysql);
                int nRet = mysql_query(&m_mysql, pSQL);
                if (nRet == 1)
                {
                    const char * pErrorMsg = mysql_error(&m_mysql);
                    printf("Excu SQL Failed[\"%s\"]\n", pErrorMsg);
                    pthread_mutex_unlock(&m_mutexDB);
                    return;
                }
                pthread_mutex_unlock(&m_mutexDB);

                //���ͱ�����Redis(��webҳ��ʵʱ��ʾԤ��)
                char pPublish[64] = { 0 };
                char pPublishMessage[128] = { 0 };
                sprintf(pPublish, "Layout_special.%s", (*itLayout)->pLayoutFaceUUID);

                sImageTime.replace(10, 1, "_");
                sprintf(pPublishMessage, "%s#%s#%s",
                    it->second->pFaceUUID, it->second->pDeviceID, sImageTime.c_str());
                m_pRedisManage->PublishMessage(pPublish, pPublishMessage);
            }

            delete it->second;
            m_mapLayoutLibAlarmInfo.erase(it);

            map<string, LPSUBMESSAGE>::iterator it = m_mapPubMessage.find(sFaceUUID);
            if (it != m_mapPubMessage.end())
            {
                delete it->second;
                m_mapPubMessage.erase(it);
            }
        }
        pthread_mutex_unlock(&m_mutexAlarm);
    }
    return;
}
//�����ݿ�д�벼��ͼƬ��Ϣ
void CKafkaServer::InsertLayoutFaceInfoToDB(const char * pMsg, int nLen)
{
    rapidjson::Document document;
    if (string(pMsg, nLen) != "" && pMsg[0] == '{')
    {
        document.Parse(pMsg);
        if (document.HasParseError())
        {
            printf("***Warning: Parse Json Format Failed, %d\n%s\n", nLen, pMsg);
            return;
        }
    }
    else
    {
        printf("***Warning: Json Format Wrong!, %d\n%s\n", nLen, pMsg);
        return;
    }
    if (document.HasMember(KAFKALAYOUTLIBID)    && document[KAFKALAYOUTLIBID].IsString()    && strlen(document[KAFKALAYOUTLIBID].GetString()) < MAXLEN     &&
        document.HasMember(KAFKALAYOUTLIBNAME)  && document[KAFKALAYOUTLIBNAME].IsString()  && strlen(document[KAFKALAYOUTLIBNAME].GetString()) < 1024     &&
        document.HasMember(KAFKALAYOUTFACEUUID) && document[KAFKALAYOUTFACEUUID].IsString() && strlen(document[KAFKALAYOUTFACEUUID].GetString()) < MAXLEN  &&
        document.HasMember(KAFKALAYOUTFACEGAURL)&& document[KAFKALAYOUTFACEGAURL].IsString()&& strlen(document[KAFKALAYOUTFACEGAURL].GetString()) < 2048 &&
        document.HasMember(KAFKALAYOUTFACEINFO) && document[KAFKALAYOUTFACEINFO].IsString() && strlen(document[KAFKALAYOUTFACEINFO].GetString()) < 2048
        )
    {
        string sLayoutLibID     = document[KAFKALAYOUTLIBID].GetString();
        string sLayoutLibName   = document[KAFKALAYOUTLIBNAME].GetString();
        string sLayoutFaceUUID  = document[KAFKALAYOUTFACEUUID].GetString();
        string sURL             = document[KAFKALAYOUTFACEGAURL].GetString();
        string sInfo            = document[KAFKALAYOUTFACEINFO].GetString();

        sURL = m_ConfigRead.m_sGASnapUrl.c_str() + sURL;

        char pSQL[4096] = { 0 };
        //�Ȳ��Ҵ��ص���Ƿ񼺼�¼��storelib_special
        int nLibID = strtol(sLayoutLibID.c_str(), NULL, 10);
        if (m_setLibID.find(nLibID) == m_setLibID.end())    //δ�ҵ���¼, �����Ӽ�¼, ����storelib_special���в������ص����Ϣ
        {
            m_setLibID.insert(nLibID);

            //��storelib_special���в������ص����Ϣ
            sprintf(pSQL, 
                "insert into storelib_special (ID, Name) values(%s, '%s')", 
                sLayoutLibID.c_str(), sLayoutLibName.c_str());
            pthread_mutex_lock(&m_mutexDB);
            mysql_ping(&m_mysql);
            int nRet = mysql_query(&m_mysql, pSQL);
            if (nRet == 1)
            {
                const char * pErrorMsg = mysql_error(&m_mysql);
                printf("Excu SQL Failed[\"%s\"]\n", pErrorMsg);
                pthread_mutex_unlock(&m_mutexDB);
                return;
            }
            pthread_mutex_unlock(&m_mutexDB);
        }

        //�ص��ͼƬ��Ϣд�����ݿ��storefaceinfo_special
        sprintf(pSQL,
            "insert into storefaceinfo_special ( FaceUUID, UserName, LayoutLibID, face_url) "
            "values('%s', '%s', %s, '%s')",
            sLayoutFaceUUID.c_str(), sInfo.c_str(), sLayoutLibID.c_str(), sURL.c_str());
        pthread_mutex_lock(&m_mutexDB);
        mysql_ping(&m_mysql);
        int nRet = mysql_query(&m_mysql, pSQL);
        if (nRet == 1)
        {
            const char * pErrorMsg = mysql_error(&m_mysql);
            printf("Excu SQL Failed[\"%s\"]\n[%s]\n", pErrorMsg, pSQL);
            pthread_mutex_unlock(&m_mutexDB);
            return;
        }
        pthread_mutex_unlock(&m_mutexDB);
    }
    else
    {
        printf("Parse LayoutLib Face Info Msg Failed!\n[%s]n", pMsg);
    }
    return;
}
string CKafkaServer::ChangeSecondToTime(unsigned long long nSecond)
{
    time_t ctime = nSecond;
    tm *tTime = localtime(&ctime);
    char sTime[20];
    sprintf(sTime, "%04d-%02d-%02d %02d:%02d:%02d", tTime->tm_year + 1900, tTime->tm_mon + 1, tTime->tm_mday,
        tTime->tm_hour, tTime->tm_min, tTime->tm_sec);
    string RetTime = sTime;
    return sTime;
}