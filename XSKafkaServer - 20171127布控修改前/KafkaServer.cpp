#include "KafkaServer.h"



CKafkaServer::CKafkaServer()
{
    m_pZeromqManage = NULL;
    m_pKafkaProducer = NULL;
    m_pKafkaConsumer = NULL;

    pthread_mutex_init(&m_mutex, NULL);
    pipe(m_nPipe);
    for (int i = 0; i < 128; i++)
    {
        m_hThreadID[i] = -1;
    }
}


CKafkaServer::~CKafkaServer()
{
    if (1 == m_ConfigRead.m_nKafkaType)
    {
        delete m_pKafkaProducer;
    }
    else if (2 == m_ConfigRead.m_nKafkaType)
    {
        delete m_pKafkaConsumer;
    }
    delete m_pZeromqManage;

    pthread_mutex_destroy(&m_mutex);
    close(m_nPipe[0]);
    close(m_nPipe[1]);
}
bool CKafkaServer::StartKafkaServer()
{
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
            printf("KafkaAddress: %s\nTopic:\n\t%s\n\t%s\n\t%s\n",
                m_ConfigRead.m_sLWKafkaAddress.c_str(), m_ConfigRead.m_sSnapTopic.c_str(),
                m_ConfigRead.m_sAlarmTopic.c_str(), m_ConfigRead.m_sLayoutLibTopic.c_str());
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
    if (1 == m_ConfigRead.m_nKafkaType)
    {
        if (!InitKafkaProducer())
        {
            return false;
        }
        if (!InitZeromq())
        {
            return false;
        }
    }
    else if (2 == m_ConfigRead.m_nKafkaType)
    {
        if (!InitZeromq())
        {
            return false;
        }
        for (int i = 0; i < m_ConfigRead.m_nThreadCount; i++)
        {
            pthread_create(&m_hThreadID[i], NULL, MessageThread, (void*)this);
        }

        if (!InitKafkaConsumer())
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
    if (1 == m_ConfigRead.m_nKafkaType)
    {
        m_pKafkaProducer->Uninit();
    }
    else if (2 == m_ConfigRead.m_nKafkaType)
    {
        m_pKafkaConsumer->Uninit();
    }
    m_pZeromqManage->UnInit();
    return 0;
}
bool CKafkaServer::InitKafkaProducer()
{
    if (NULL == m_pKafkaProducer)
    {
        m_pKafkaProducer = new CKafkaProducer;
    }
    if (m_pKafkaProducer->InitKafkaProducer(m_ConfigRead.m_sLWKafkaAddress.c_str(), m_ConfigRead.m_sSnapTopic.c_str(),
                                            m_ConfigRead.m_sAlarmTopic.c_str(), m_ConfigRead.m_sLayoutLibTopic.c_str()) < 0)
    {
        printf("Init KafkaProducer Failed.\n");
        return false;
    }
    return true;
}
bool CKafkaServer::InitKafkaConsumer()
{
    if (NULL == m_pKafkaConsumer)
    {
        m_pKafkaConsumer = new CKafkaConsumer;
    }
    m_pKafkaConsumer->SetMessageCallback(KafkaMessageCallback, this);
    if (m_pKafkaConsumer->InitKafkaConsumer(m_ConfigRead.m_sLWKafkaAddress.c_str(), m_ConfigRead.m_sSnapTopic.c_str(),
        m_ConfigRead.m_sAlarmTopic.c_str(), m_ConfigRead.m_sLayoutLibTopic.c_str()) < 0)
    {
        printf("Init KafkaConsumer Failed.\n");
        return false;
    }
    return true;
}


//初始化Zeromq
bool CKafkaServer::InitZeromq()
{
    if (NULL == m_pZeromqManage)
    {
        m_pZeromqManage = new CZeromqManage;
    }
    if (1 == m_ConfigRead.m_nKafkaType)
    {
        if (!m_pZeromqManage->InitSub(NULL, 0, (char*)m_ConfigRead.m_sProxyIP.c_str(), m_ConfigRead.m_nProxyPubPort, ZeromqSubMsg, this, 1))
        {
            printf("****Error: init sub zeromq[%s:%d]失败!", m_ConfigRead.m_sProxyIP.c_str(), m_ConfigRead.m_nProxyPubPort);
            return false;
        }
        m_pZeromqManage->AddSubMessage("ZPJ");
    }
    else if (2 == m_ConfigRead.m_nKafkaType)
    {
        if (!m_pZeromqManage->InitPub(NULL, 0, (char*)m_ConfigRead.m_sProxyIP.c_str(), m_ConfigRead.m_nProxySubPort))
        {
            printf("****Error: init pub zeromq[%s:%d]失败!", m_ConfigRead.m_sProxyIP.c_str(), m_ConfigRead.m_nProxySubPort);
            return false;
        }
    }
    else
    {
        return false;
    }
    
    return true;
}
//Zeromq消息回调
void CKafkaServer::ZeromqSubMsg(LPSUBMESSAGE pSubMessage, void * pUser)
{
    CKafkaServer * pThis = (CKafkaServer *)pUser;
    pThis->ParseZeromqJson(pSubMessage);
}
//Zeromq回调消息解析处理
bool CKafkaServer::ParseZeromqJson(LPSUBMESSAGE pSubMessage)
{
    int nRet = 0;
    string sCommand(pSubMessage->pOperationType);
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


    if (sCommand == COMMANDADD)        //增加图片
    {
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
            //1. 保存FaceUUID
            strcpy(pSubMessage->pFaceUUID, document[JSONFACEUUID].GetString());
            //2. 保存特征值及长度
            strcpy(pSubMessage->pFeature, document[JSONFEATURE].GetString());
            pSubMessage->nFeatureLen = strlen(document[JSONFEATURE].GetString());
            //3. 保存特征值时间
            pSubMessage->nTime = document[JSONTIME].GetInt64();
            //4. 保存盘符
            //5. 保存图片保存服务器IP
            //6. 保存人脸图片坐标
            strcpy(pSubMessage->pFaceRect, document[JSONFACERECT].GetString());
            //7. 保存人脸url
            strcpy(pSubMessage->pFaceURL, document[JSONFACEURL].GetString());
            //8. 保存背景url
            strcpy(pSubMessage->pBkgURL, document[JSONBKGURL].GetString());

            //生成Kakfa消息格式串
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

                printf("%s\n", sProduceMsg.c_str());

                m_pKafkaProducer->SendProducerMessage((char*)sProduceMsg.c_str(), sProduceMsg.size());
            }
        }
        else
        {
            nRet = JsonFormatError;
        }
    }

    return true;
}
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
                printf("-----Thread[%d] :\n%s\n", nSeq, sMsg.c_str());
                PubSnapMessage((char*)sMsg.c_str(), sMsg.size(), pPubMessage);
            }

        } while ("" != sMsg);
    }
}
//向代理发布抓拍信息(等同于采集服务)
void CKafkaServer::PubSnapMessage(char * pMsg, int nLen, LPSUBMESSAGE pPubMessage)
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
        //1. 保存DeviceID
        strcpy(pPubMessage->pHead, document[KAFKADEVICEID].GetString());
        //2. 保存FaceUUID
        strcpy(pPubMessage->pFaceUUID, document[KAFKAFACEUUID].GetString());
        //3. 保存特征值时间
        strcpy(pPubMessage->pTime, document[KAFKAFACETIME].GetString());
        pPubMessage->nTime = strtol(pPubMessage->pTime, NULL, 10);//document[KAFKAFACETIME].GetInt64();
        //4. 保存人脸图片坐标
        strcpy(pPubMessage->pFaceRect, document[KAFKAFACERECT].GetString());
        //5. 保存人脸公安网url
        sprintf(pPubMessage->pFaceURL,"http://10.131.15.106:8888/bucketWeb/imageServlet?path=%s", document[KAFKAGAFACEURL].GetString());
        //6. 保存背景公安网url
        sprintf(pPubMessage->pBkgURL, "http://10.131.15.106:8888/bucketWeb/imageServlet?path=%s", document[KAFKAGABKGURL].GetString());
        //7. 保存特征值及长度
        strcpy(pPubMessage->pFeature, document[KAFKAFEATURE].GetString());


        //生成采集推送消息格式串
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