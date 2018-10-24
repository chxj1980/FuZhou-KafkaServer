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

    int m_nKafkaType;           //kafka����, 1: Producer, 2: Consumer

    string m_sLWKafkaAddress;   //����kafka��ַ
    string m_sSnapTopic;        //ץ������
    string m_sAlarmTopic;       //��������
    string m_sLayoutLibTopic;   //������Ϣ����

    string m_sProxyIP;          //�������IP
    int m_nProxyPubPort;        //������񷢲��˿�
    int m_nProxySubPort;        //��������Ķ˿�

    int m_nThreadCount;         //�����߳���

    string m_sRedisIP;          //Redis IP
    int m_nRedisPort;           //Redis Port

    string m_sGASnapUrl;            //��������ץ��ͼƬ  ������URLǰ׺
    string m_sGALayoutURL;      //�������Ͳ��ؿ�ͼƬ������URLǰ׺

    //���ݿ������Ϣ, ����ʱ��ȡipcamera���ȡҪ�����豸��Ϣ
    string m_sDBIP;
    int m_nDBPort;
    string m_sDBName;
    string m_sDBUser;
    string m_sDBPd;
    
};

#define configread_h
#endif