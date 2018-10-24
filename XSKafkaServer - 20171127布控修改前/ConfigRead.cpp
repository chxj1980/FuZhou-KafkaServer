#include "ConfigRead.h"


CConfigRead::CConfigRead(void)
{
    m_sLWKafkaAddress = "";
    m_sSnapTopic = "";
    m_sAlarmTopic = "";
    m_sLayoutLibTopic = "";
}
#ifdef __WINDOWS__
string CConfigRead::GetCurrentPath()
{
    DWORD nBufferLenth = MAX_PATH;
    char szBuffer[MAX_PATH] = { 0 };
    DWORD dwRet = GetModuleFileNameA(NULL, szBuffer, nBufferLenth);
    char *sPath = strrchr(szBuffer, '\\');
    memset(sPath, 0, strlen(sPath));
    m_sCurrentPath = szBuffer;
    return m_sCurrentPath;
}
#endif
CConfigRead::~CConfigRead(void)
{
}
bool CConfigRead::ReadConfig()
{
#ifdef __WINDOWS__
    GetCurrentPath();
    m_sConfigFile = m_sCurrentPath + "/Config/config.txt";
#ifdef _DEBUG
    m_sConfigFile = "./Config/config.txt";
#endif
#else
    m_sConfigFile = "config.txt";
#endif
    fstream cfgFile;
    cfgFile.open(m_sConfigFile.c_str()); //打开文件      
    if(!cfgFile.is_open())  
    {  
        printf("can not open file[%s]!\n",  m_sConfigFile.c_str()); 
        return false;  
    }  

    char tmp[1000];  
    while(!cfgFile.eof())//循环读取每一行  
    {  
        cfgFile.getline(tmp, 1000);//每行读取前1000个字符，1000个应该足够了  
        string sLine(tmp);  
        size_t pos = sLine.find('=');//找到每行的“=”号位置，之前是key之后是value  
        if(pos == string::npos) 
        {
            printf("****Error: Config File Format Wrong: [%s]!\n", sLine.c_str());
            return false;  
        }
        string tmpKey = sLine.substr(0, pos); //取=号之前  
        if(sLine[sLine.size() - 1] == 13)
        {
            sLine.erase(sLine.size() -1, 1);
        }

        if ("KafkaType" == tmpKey)
        {
            string sType = sLine.substr(pos + 1);//取=号之后  
            m_nKafkaType = atoi(sType.c_str());
        }
        else if("LWKafka" == tmpKey)  
        {  
            m_sLWKafkaAddress.assign(sLine, pos + 1, sLine.size() - 1 - pos);
        }  
        else if("SnapTopic" == tmpKey)  
        {  
            m_sSnapTopic.assign(sLine, pos + 1, sLine.size() - 1 - pos);
        }  
        else if("AlarmTopic" == tmpKey)  
        {  
            m_sAlarmTopic.assign(sLine, pos + 1, sLine.size() - 1 - pos);
        } 
        else if("LayoutLibTopic" == tmpKey)  
        {  
            m_sLayoutLibTopic.assign(sLine, pos + 1, sLine.size() - 1 - pos);
        } 
        else if ("ProxyServerIP" == tmpKey)
        {
            m_sProxyIP.assign(sLine, pos + 1, sLine.size() - 1 - pos);
        }
        else if ("ProxyServerPubPort" == tmpKey)
        {
            string sPort = sLine.substr(pos + 1);
            m_nProxyPubPort = atoi(sPort.c_str());
            if (1 == m_nKafkaType)
            {
                break;
            }
        }
        else if ("ProxyServerSubPort" == tmpKey)
        {
            string sPort = sLine.substr(pos + 1);
            m_nProxySubPort = atoi(sPort.c_str());
        }
        else if ("ThreadCount" == tmpKey)
        {
            string sCount = sLine.substr(pos + 1);
            m_nThreadCount = atoi(sCount.c_str());
        }
        else if ("RedisIP" == tmpKey)
        {
            m_sRedisIP.assign(sLine, pos + 1, sLine.size() - 1 - pos);
        }
        else if ("RedisPort" == tmpKey)
        {
            string sPort = sLine.substr(pos + 1);
            m_nRedisPort = atoi(sPort.c_str());
        }
        else if ("GASnapURL" == tmpKey)
        {
            m_sGASnapUrl.assign(sLine, pos + 1, sLine.size() - 1 - pos);
        }
        else if ("GALayoutURL" == tmpKey)
        {
            m_sGALayoutURL.assign(sLine, pos + 1, sLine.size() - 1 - pos);
            break;
        }
    }  
    printf("KafkaInfo: Type: %d, Address: %s\n", m_nKafkaType, m_sLWKafkaAddress.c_str());
    if("" == m_sLWKafkaAddress)
    {
        printf("****Error: Get Kafka Info Failed!\n");
        return false;
    }

    return true;
}
