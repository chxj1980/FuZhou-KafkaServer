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

    string m_sLWSpecialKafkaAddress;   //南威专网kafka地址
    string m_sLWGAKafkaAddress; //南威公安网kafka地址
    string m_sSnapTopic;        //抓拍主题
    string m_sAlarmTopic;       //报警主题
    string m_sLayoutLibTopic;   //布控信息主题

    string m_sSpecialProxyIP;   //专网代理服务IP
    string m_sGAProxyIP;        //公安网代理服务IP
    int m_nProxyPubPort;        //代理服务发布端口
    int m_nProxySubPort;        //代理服务订阅端口

    string m_sZmqSnapTopic;     //zmq抓拍图片推送主题
    string m_sZmqAlarmTopic;    //zmq报警主题
    string m_sZmqLayoutTopic;   //zmq布控图片信息主题
    int m_nThreadCount;         //公安网处理接收kafka消息线程数

    string m_sRedisIP;          //公安网Redis IP, kafka server接收专网布控预警信息, 发往redis通知web
    int m_nRedisPort;           //Redis Port

    string m_sGASnapUrl;        //南威推送抓拍图片  公安网URL前缀
    string m_sGALayoutURL;      //南威推送布控库图片公安网URL前缀

    //数据库信息, 启动时读取ipcamera表获取要订阅设备信息, 暂无用
    string m_sDBIP;
    int m_nDBPort;
    string m_sDBName;
    string m_sDBUser;
    string m_sDBPd;
    
};

#define configread_h
#endif