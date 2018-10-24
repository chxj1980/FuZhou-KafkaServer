#ifndef datadefine_h

#include <map>
#include <string>
#include <string.h>
#include <list>
#include <vector>
#include <set>
#include <stdint.h>
using namespace std;


#define UNIXBEGINTIME   946656000000    //2000-01-01 00:00:00
#define UNIXENDTIME   32503651200000    //3000-01-01 00:00:00
//���ݿ����
#define LIBINFOTABLE        "checklibinfo"
#define CHECKPOINTPREFIX    "checkpoint"
#define ONEDAYTIME      (24 * 3600 * 1000)

#define COMMANDADDLIB   "addlib"    //���ӹ������ڻ��ص��
#define COMMANDDELLIB   "dellib"    //ɾ���������ڻ��ص��
#define COMMANDADD      "add"       //��������ֵ����
#define COMMANDDEL      "del"       //ɾ������ֵ����
#define COMMANDCLEAR    "clear"     //�������ֵ������
#define COMMANDSEARCH   "search"    //��������ֵ����
#define COMMANDCAPTURE  "capture"   //��ȡץ��ͼƬ
#define COMMANDBUSINESSSUMMARY "business.summary"   //Ƶ�������ͳ��

//Redis Hash���ֶζ���
#define REDISDEVICEID   "libid"     //����ID���ص��ID
#define REDISFACEUUID   "faceuuid"  //faceuuid
#define REDISSCORE      "score"     //����
#define REDISTIME       "time"      //ʱ��
#define REDISDRIVE      "drive"     //�̷�
#define REDISIMAGEIP    "imageip"   //ͼƬ���������IP
#define REDISHITCOUNT   "hitcount"  //ͼƬ���д���
#define REDISFACERECT   "facerect"  //��������
#define HASHOVERTIME    1800        //���ݱ���ʱ��

#define SUBALLCHECKPOINT    "allcheckpoints"    //�������п���
#define SUBALLLIB           "allfocuslib"       //���������ص��

#define DEVICESTATUS    "libstatus"  //����״̬����

//Json���ֶζ���
#define JSONDLIBID      "libid"         //���ڻ��ص��ID
#define JSONLIBTYPE     "type"          //�ص������, 2: �˲��, 3: ���ؿ�
//����
#define JSONDEVICEID    "checkpoint"    //����ID
#define JSONFACEUUID    "faceuuid"      //����ֵFaceUUID
#define JSONFACERECT    "facerect"      //����ͼƬ����
#define JSONFEATURE     "feature"       //����ֵ
#define JSONTIME        "imagetime"     //ץ��ʱ��
#define JSONDRIVE       "imagedisk"     //ͼƬ�����ڲɼ������ϵ������̷�
#define JSONSERVERIP    "imageip"       //�ɼ�����IP
#define JSONFACEURL     "face_url"      //����url
#define JSONBKGURL     "bkg_url"      //����ͼurl

#define JSONTASKID      "taskid"        //��������ID
#define JSONFACE        "faces"         //��������ֵ����
#define JSONFACENO      "faceno"
#define JSONFACEFEATURE "feature"
#define JSONBEGINTIME   "starttime"     //������ʼʱ��
#define JSONENDTIME     "endtime"       //��������ʱ��
#define JSONSIMILAR     "similar"       //������ֵ
#define JSONASYNCRETURN "asyncreturn"   //�Ƿ����������������(�Ż��������)
#define JSONSCORE       "score"         //��������

#define JSONSEARCHCOUNT     "count"         //���������
#define JSONSEARCHHIGHEST   "highest"       //������߷���
#define JSONSEARCHLATESTTIME "lasttime"     //��������ͼƬʱ��
#define JSONCAPTURESIZE     "size"          //��ȡץ��ͼƬ�����
#define JSONRETURNFEATURE   "feature"       //ץ���Ƿ񷵻�����ֵ
#define JSONSEARCHTWOMORECOUNT      "hittwomorecount"         //��������2�������Ͻ����
#define JSONSEARCHTHREEMORECOUNT    "hitthreemorecount"       //��������3�������Ͻ����
#define JSONSEARCHFOURMORECOUNT     "hitfourmorecount"        //��������4�������Ͻ����
#define JSONSEARCHFIVEMORECOUNT     "hitfivemorecount"        //��������5�������

//Ƶ�������ͳ��
#define JSONNIGHTSTARTTIME  "nightstarttime"    //ҹ��ʼʱ��
#define JSONNIGHTENDTIME    "nightendtime"      //ҹ�����ʱ��
#define JSONFREQUENTLY      "frequency"         //ͳ�ƴ�����ֵ
#define JSONPERSONID        "person"            //ͳ����ԱID
#define JSONPROGRESS        "progress"          //ҵ��ͳ�ƽ��ȷ���

//kafka�����ֶ�
#define KAFKADEVICEID   "device_id"
#define KAFKAFACEUUID   "faceuuid"
#define KAFKAFACETIME   "face_time"
#define KAFKAFACERECT   "face_rect"
#define KAFKAFACEURL    "face_url"
#define KAFKABKGURL     "bkg_url"
#define KAFKAFEATURE    "feature"
#define KAFKAFEATURELEN "feature_len"
#define KAFKAGAFACEURL  "ga_face_url"        //Kafka����ͼƬ��������ַ
#define KAFKAGABKGURL   "ga_bkg_url"
#define KAFKALAYOUTLIBID      "layoutlib_id"         
#define KAFKALAYOUTLIBNAME    "layoutlib_name"         
#define KAFKALAYOUTFACEUUID "layoutfaceuuid"
#define KAFKASCORE      "score"
#define KAFKALAYOUTFACEURL   "layoutface_url"
#define KAFKALAYOUTFACEGAURL   "ga_layoutface_url"
#define KAFKALAYOUTFACEINFO  "layoutface_info"

#define KAFKALAYOUTSERVER "kafkalayoutserver"   //�򹫰�������������ֶ�, �ȴ����ڷ������񷵻�ץ��ͼƬ��Ӧurl��Ϣ

#define SQLMAXLEN       1024 * 4    //SQL�����󳤶�
#define MAXIPLEN        20          //IP, FaceRect��󳤶�
#define MAXLEN          72          //FaceUUID, LibName, DeviceID��󳤶�
#define FEATURELEN      1024 * 4    //Feature��󳤶�
#define FEATUREMIXLEN   500         //Feature��̳���, С�ڴ˳��ȶ�Ϊ���Ϸ�
#define RESOURCEMAXNUM  150         //���HTTP��Ϣ������, �������޷�������д���, ֱ�ӷ���ʧ��
#define THREADNUM       8           //�߳���
#define THREADWAITTIME  5           //�̵߳ȴ�ʱ��(ms)
#define FEATURESEARCHWAIT   2000    //���߳������ȴ���ʱʱ��

enum ErrorCode
{
    INVALIDERROR = 0,       //�޴���
    ServerNotInit = 12001,  //������δ��ʼ�����
    DBAleadyExist,          //�⼺����
    DBNotExist,             //�ⲻ����
    FaceUUIDAleadyExist,    //FaceUUID������
    FaceUUIDNotExist,       //FaceUUID������
    ParamIllegal,           //�����Ƿ�
    NewFailed,              //new�����ڴ�ʧ��
    JsonFormatError,        //json����ʽ����
    CommandNotFound,        //��֧�ֵ�����
    HttpMsgUpperLimit,      //http��Ϣ������������������
    PthreadMutexInitFailed, //�ٽ�����ʼ��ʧ��
    FeatureNumOverMax,      //������������ֵ��������
    JsonParamIllegal,       //Json����ֵ�Ƿ�
    MysqlQueryFailed,       //Mysql����ִ��ʧ��.
    ParamLenOverMax,        //��������̫��
    LibAleadyExist,         //���ڼ�����
    LibNotExist,            //���ڲ�����
    CheckpointInitFailed,   //�������ʼ��ʧ��
    VerifyFailed,           //����ֵ�ȶ�ʧ��, ʧ��һ�κ󼴲��ٱȶ�
    HttpSocketBindFailed,   //http�˿ڰ�ʧ��
    CreateTableFailed,      //�����ݿⴴ����ʧ��
    SearchTimeWrong,        //�����ʱ���(��ʼʱ����ڽ���ʱ��)
    SearchNoDataOnTime,      //ָ��ʱ�����û������
    AddFeatureToCheckpointFailed,    //��������ֵ������DB����
    SocketInitFailed,        //�����ʼ��ʧ��
    InsertRedisFailed       //���ݲ���Redis����
};

//��������
enum TASKTYPE
{
    INVALIDTASK,
    ADDLIB,              //�����ص��򿨿�
    DELLIB,              //ɾ���ص��򿨿�
    LIBADDFEATURE,       //�򿨿���������ֵ
    LIBDELFEATURE,       //�ӿ���ɾ������ֵ
    LIBCLEARFEATURE,     //�ӿ����������ֵ
    LIBSEARCH,           //��ʱ��, ������������ֵ
    LIBCAPTURE,          //��ѯָ��ʱ�����ץ��ͼƬ
    LIBBUSINESS          //Ƶ������, ��ҹ����, ���ҹ����ҵ��ͳ��
};

typedef struct _SubMessage
{
    char pHead[MAXLEN];         //������Ϣͷ
    char pOperationType[MAXLEN];//������Ϣ��������
    char pSource[MAXLEN];       //������ϢԴ
    char pSubJsonValue[FEATURELEN * 10];       //������ϢJson��
    string sPubJsonValue;       //������ϢJson��

    char pDeviceID[MAXLEN];     //����ID
    char pFaceUUID[MAXLEN];     //����ֵFaceUUID
    char pFeature[FEATURELEN];  //����ֵ
    int nFeatureLen;            //����ֵ����

    char pTime[MAXLEN];
    int64_t nTime;
    char pFaceRect[MAXLEN];
    char pFaceURL[2048];
    char pBkgURL[2048];
    _SubMessage()
    {
        memset(pHead, 0, sizeof(pHead));
        memset(pOperationType, 0, sizeof(pOperationType));
        memset(pSource, 0, sizeof(pSource));
        memset(pSubJsonValue, 0, sizeof(pSubJsonValue));
        sPubJsonValue = "";
        memset(pDeviceID, 0, sizeof(pDeviceID));
        memset(pFaceUUID, 0, sizeof(pFaceUUID));
        memset(pFeature, 0, sizeof(pFeature));
        nFeatureLen = 0;

        memset(pTime, 0, sizeof(pTime));
        nTime = 0;
        memset(pFaceRect, 0, sizeof(pFaceRect));
        memset(pFaceURL, 0, sizeof(pFaceURL));
        memset(pBkgURL, 0, sizeof(pBkgURL));
    }
    void Init()
    {
        memset(pHead, 0, sizeof(pHead));
        memset(pOperationType, 0, sizeof(pOperationType));
        memset(pSource, 0, sizeof(pSource));
        memset(pSubJsonValue, 0, sizeof(pSubJsonValue));
        sPubJsonValue = "";
        memset(pDeviceID, 0, sizeof(pDeviceID));
        memset(pFaceUUID, 0, sizeof(pFaceUUID));
        memset(pFeature, 0, sizeof(pFeature));
        nFeatureLen = 0;

        memset(pTime, 0, sizeof(pTime));
        nTime = 0;
        memset(pFaceRect, 0, sizeof(pFaceRect));
        memset(pFaceURL, 0, sizeof(pFaceURL));
        memset(pBkgURL, 0, sizeof(pBkgURL));
    }
}SUBMESSAGE, *LPSUBMESSAGE;
typedef std::list<LPSUBMESSAGE> LISTSUBMESSAGE;
typedef void(*LPSubMessageCallback)(LPSUBMESSAGE pSubMessage, void * pUser);

typedef void(*LPKafkaMessageCallback)(char * pMsg, int nLen, void * pUser);

typedef struct _LayoutFaceInfo
{
    int nLayoutLibID;
    char pLayoutFaceUUID[MAXLEN];
    int nScore;
    _LayoutFaceInfo()
    {
        memset(pLayoutFaceUUID, 0, sizeof(pLayoutFaceUUID));
    }
}LAYOUTFACEINFO, *LPLAYOUTFACEINFO;

typedef struct _LayoutLibAlarmInfo
{
    char pFaceUUID[MAXLEN];
    char pDeviceID[MAXLEN];
    int64_t nTime;

    list<LPLAYOUTFACEINFO> listLayoutFaceInfo;  //һ��ץ��ͼƬ�������ж��Ų���ͼƬ
    _LayoutLibAlarmInfo()
    {
        memset(pFaceUUID, 0, sizeof(pFaceUUID));
        memset(pDeviceID, 0, sizeof(pDeviceID));
    }
    ~_LayoutLibAlarmInfo()
    {
        while (listLayoutFaceInfo.size() > 0)
        {
            delete listLayoutFaceInfo.front();
            listLayoutFaceInfo.pop_front();
        }
    }
}LAYOUTLIBALARMINFO, *LPLAYOUTLIBALARMINFO;

typedef std::map<string, LPLAYOUTLIBALARMINFO> MAPLAYOUTLIBALARMINFO;

#define datadefine_h
#endif


