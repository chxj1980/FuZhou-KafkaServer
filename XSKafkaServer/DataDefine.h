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
//数据库表名
#define LIBINFOTABLE        "checklibinfo"
#define CHECKPOINTPREFIX    "checkpoint"
#define ONEDAYTIME      (24 * 3600 * 1000)

#define COMMANDADDLIB   "addlib"    //增加关联卡口或重点库
#define COMMANDDELLIB   "dellib"    //删除关联卡口或重点库
#define COMMANDADD      "add"       //增加特征值命令
#define COMMANDDEL      "del"       //删除特征值命令
#define COMMANDCLEAR    "clear"     //清空特征值库命令
#define COMMANDSEARCH   "search"    //搜索特征值命令
#define COMMANDCAPTURE  "capture"   //获取抓拍图片
#define COMMANDBUSINESSSUMMARY "business.summary"   //频繁出入等统计

//Redis Hash表字段定义
#define REDISDEVICEID   "libid"     //卡口ID或重点库ID
#define REDISFACEUUID   "faceuuid"  //faceuuid
#define REDISSCORE      "score"     //分数
#define REDISTIME       "time"      //时间
#define REDISDRIVE      "drive"     //盘符
#define REDISIMAGEIP    "imageip"   //图片保存服务器IP
#define REDISHITCOUNT   "hitcount"  //图片命中次数
#define REDISFACERECT   "facerect"  //人脸坐标
#define HASHOVERTIME    1800        //数据保存时间

#define SUBALLCHECKPOINT    "allcheckpoints"    //订阅所有卡口
#define SUBALLLIB           "allfocuslib"       //订阅所有重点库

#define DEVICESTATUS    "libstatus"  //卡口状态发布

//Json串字段定义
#define JSONDLIBID      "libid"         //卡口或重点库ID
#define JSONLIBTYPE     "type"          //重点库类型, 2: 核查库, 3: 布控库
//卡口
#define JSONDEVICEID    "checkpoint"    //卡口ID
#define JSONFACEUUID    "faceuuid"      //特征值FaceUUID
#define JSONFACERECT    "facerect"      //人脸图片坐标
#define JSONFEATURE     "feature"       //特征值
#define JSONTIME        "imagetime"     //抓拍时间
#define JSONDRIVE       "imagedisk"     //图片保存在采集服务上的驱动盘符
#define JSONSERVERIP    "imageip"       //采集服务IP
#define JSONFACEURL     "face_url"      //人脸url
#define JSONBKGURL     "bkg_url"      //背景图url

#define JSONTASKID      "taskid"        //检索任务ID
#define JSONFACE        "faces"         //检索特征值数组
#define JSONFACENO      "faceno"
#define JSONFACEFEATURE "feature"
#define JSONBEGINTIME   "starttime"     //检索开始时间
#define JSONENDTIME     "endtime"       //检索结束时间
#define JSONSIMILAR     "similar"       //检索阈值
#define JSONASYNCRETURN "asyncreturn"   //是否立即返回搜索结果(团伙分析服务)
#define JSONSCORE       "score"         //检索分数

#define JSONSEARCHCOUNT     "count"         //检索结果数
#define JSONSEARCHHIGHEST   "highest"       //检索最高分数
#define JSONSEARCHLATESTTIME "lasttime"     //检索最新图片时间
#define JSONCAPTURESIZE     "size"          //获取抓拍图片最大数
#define JSONRETURNFEATURE   "feature"       //抓拍是否返回特征值
#define JSONSEARCHTWOMORECOUNT      "hittwomorecount"         //检索命中2个或以上结果数
#define JSONSEARCHTHREEMORECOUNT    "hitthreemorecount"       //检索命中3个或以上结果数
#define JSONSEARCHFOURMORECOUNT     "hitfourmorecount"        //检索命中4个或以上结果数
#define JSONSEARCHFIVEMORECOUNT     "hitfivemorecount"        //检索命中5个结果数

//频繁出入等统计
#define JSONNIGHTSTARTTIME  "nightstarttime"    //夜晚开始时间
#define JSONNIGHTENDTIME    "nightendtime"      //夜晚结束时间
#define JSONFREQUENTLY      "frequency"         //统计次数阈值
#define JSONPERSONID        "person"            //统计人员ID
#define JSONPROGRESS        "progress"          //业务统计进度返回

//kafka定义字段
#define KAFKADEVICEID   "device_id"
#define KAFKAFACEUUID   "faceuuid"
#define KAFKAFACETIME   "face_time"
#define KAFKAFACERECT   "face_rect"
#define KAFKAFACEURL    "face_url"
#define KAFKABKGURL     "bkg_url"
#define KAFKAFEATURE    "feature"
#define KAFKAFEATURELEN "feature_len"
#define KAFKAGAFACEURL  "ga_face_url"        //Kafka推送图片公安网地址
#define KAFKAGABKGURL   "ga_bkg_url"
#define KAFKALAYOUTLIBID      "layoutlib_id"         
#define KAFKALAYOUTLIBNAME    "layoutlib_name"         
#define KAFKALAYOUTFACEUUID "layoutfaceuuid"
#define KAFKASCORE      "score"
#define KAFKALAYOUTFACEURL   "layoutface_url"
#define KAFKALAYOUTFACEGAURL   "ga_layoutface_url"
#define KAFKALAYOUTFACEINFO  "layoutface_info"

#define KAFKALAYOUTSERVER "kafkalayoutserver"   //向公安网代理服务订阅字段, 等待卡口分析服务返回抓拍图片对应url信息

#define SQLMAXLEN       1024 * 4    //SQL语句最大长度
#define MAXIPLEN        20          //IP, FaceRect最大长度
#define MAXLEN          72          //FaceUUID, LibName, DeviceID最大长度
#define FEATURELEN      1024 * 4    //Feature最大长度
#define FEATUREMIXLEN   500         //Feature最短长度, 小于此长度定为不合法
#define RESOURCEMAXNUM  150         //最大HTTP消息队列数, 超过后将无法塞入队列处理, 直接返回失败
#define THREADNUM       8           //线程数
#define THREADWAITTIME  5           //线程等待时间(ms)
#define FEATURESEARCHWAIT   2000    //多线程搜索等待超时时间

enum ErrorCode
{
    INVALIDERROR = 0,       //无错误
    ServerNotInit = 12001,  //服务尚未初始化完成
    DBAleadyExist,          //库己存在
    DBNotExist,             //库不存在
    FaceUUIDAleadyExist,    //FaceUUID己存在
    FaceUUIDNotExist,       //FaceUUID不存在
    ParamIllegal,           //参数非法
    NewFailed,              //new申请内存失败
    JsonFormatError,        //json串格式错误
    CommandNotFound,        //不支持的命令
    HttpMsgUpperLimit,      //http消息待处理数量己达上限
    PthreadMutexInitFailed, //临界区初始化失败
    FeatureNumOverMax,      //批量增加特征值数量超标
    JsonParamIllegal,       //Json串有值非法
    MysqlQueryFailed,       //Mysql操作执行失败.
    ParamLenOverMax,        //参数长度太长
    LibAleadyExist,         //卡口己存在
    LibNotExist,            //卡口不存在
    CheckpointInitFailed,   //卡口类初始化失败
    VerifyFailed,           //特征值比对失败, 失败一次后即不再比对
    HttpSocketBindFailed,   //http端口绑定失败
    CreateTableFailed,      //在数据库创建表失败
    SearchTimeWrong,        //错误的时间段(开始时间大于结束时间)
    SearchNoDataOnTime,      //指定时间段内没有数据
    AddFeatureToCheckpointFailed,    //增加特征值到卡口DB错误
    SocketInitFailed,        //网络初始化失败
    InsertRedisFailed       //数据插入Redis错误
};

//任务类型
enum TASKTYPE
{
    INVALIDTASK,
    ADDLIB,              //增加重点库或卡口
    DELLIB,              //删除重点库或卡口
    LIBADDFEATURE,       //向卡口增加特征值
    LIBDELFEATURE,       //从卡口删除特征值
    LIBCLEARFEATURE,     //从卡口清空特征值
    LIBSEARCH,           //按时间, 卡口搜索特征值
    LIBCAPTURE,          //查询指定时间段内抓拍图片
    LIBBUSINESS          //频繁出入, 深夜出入, 昼伏夜出等业务统计
};

typedef struct _SubMessage
{
    char pHead[MAXLEN];         //订阅消息头
    char pOperationType[MAXLEN];//订阅消息操作类型
    char pSource[MAXLEN];       //订阅消息源
    char pSubJsonValue[FEATURELEN * 10];       //订阅消息Json串
    string sPubJsonValue;       //发布消息Json串

    char pDeviceID[MAXLEN];     //卡口ID
    char pFaceUUID[MAXLEN];     //特征值FaceUUID
    char pFeature[FEATURELEN];  //特征值
    int nFeatureLen;            //特征值长度

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

    list<LPLAYOUTFACEINFO> listLayoutFaceInfo;  //一张抓拍图片可能命中多张布控图片
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


