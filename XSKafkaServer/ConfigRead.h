#ifndef configread_h

#include <iostream>  
#include <string>  
#include <fstream> 
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class CConfigRead
{
public:
    CConfigRead(void);
    ~CConfigRead(void);
public:
#ifdef __WINDOWS__
    string GetCurrentPath();
#endif
    bool ReadConfig();
public:
    string m_sConfigFile;
    string m_sCurrentPath;

    string m_sLWSpecialKafkaAddress;   //����ר��kafka��ַ
    string m_sLWGAKafkaAddress; //����������kafka��ַ
    string m_sSnapTopic;        //ץ������
    string m_sAlarmTopic;       //��������
    string m_sLayoutLibTopic;   //������Ϣ����

    string m_sSpecialProxyIP;   //ר���������IP
    string m_sGAProxyIP;        //�������������IP
    int m_nProxyPubPort;        //������񷢲��˿�
    int m_nProxySubPort;        //��������Ķ˿�

    string m_sZmqSnapTopic;     //zmqץ��ͼƬ��������
    string m_sZmqAlarmTopic;    //zmq��������
    string m_sZmqLayoutTopic;   //zmq����ͼƬ��Ϣ����
    int m_nThreadCount;         //�������������kafka��Ϣ�߳���

    string m_sRedisIP;          //������Redis IP, kafka server����ר������Ԥ����Ϣ, ����redis֪ͨweb
    int m_nRedisPort;           //Redis Port

    string m_sGASnapUrl;        //��������ץ��ͼƬ  ������URLǰ׺
    string m_sGALayoutURL;      //�������Ͳ��ؿ�ͼƬ������URLǰ׺

    //���ݿ���Ϣ, ����ʱ��ȡipcamera���ȡҪ�����豸��Ϣ, ������
    string m_sDBIP;
    int m_nDBPort;
    string m_sDBName;
    string m_sDBUser;
    string m_sDBPd;
    
};

#define configread_h
#endif