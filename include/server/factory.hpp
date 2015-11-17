//------------------------------------------------------------------------------
// File: factory.hpp
//
// Desc: Camera factory - Manage IP Camera.
//
// Copyright (c) 2014-2018 opencvr.com. All rights reserved.
//------------------------------------------------------------------------------

#ifndef __VSC_FACTORY_H_
#define __VSC_FACTORY_H_
#include "confdb.hpp"
#include "camera.hpp"
#include "vdb.hpp"
#include "vhdfsdb.hpp"
#include "vplay.hpp"
#include "sysdb.hpp"
#include "hddcamera.hpp"
#include <QThread>
#include <qdebug.h>
#include "Poco/Path.h"
#include "Poco/File.h"
#include "vidconf.pb.h"

typedef enum
{
    FACTORY_CAMERA_ADD = 1,
    FACTORY_CAMERA_DEL,
    FACTORY_CAMERA_ONLINE,
    FACTORY_CAMERA_OFFLINE,
    FACTORY_CAMERA_RECORDING_ON,
    FACTORY_CAMERA_RECORDING_OFF,

    FACTORY_CAMERA_LAST
} FactoryCameraChangeType;


class FactoryCameraChangeData
{
public:
	FactoryCameraChangeType type;
	int id;
};

typedef BOOL (*FactoryCameraChangeNotify)(void* pParam, 
		FactoryCameraChangeData data);

typedef std::list<LPCamera> CameraList;
typedef std::list<CameraParam> CameraParamList;
typedef std::map<astring, LPCamera> CameraMap;
typedef std::map<astring, CameraParam> CameraParamMap;
typedef std::map<astring, bool> CameraOnlineMap;
typedef std::map<void *, FactoryCameraChangeNotify> CameraChangeNofityMap;

class Factory;
class FactoryHddTask:public QThread
{
	Q_OBJECT
public:
	FactoryHddTask(Factory &pFactory);
	~FactoryHddTask();
public:
	void run();
private:
	Factory &m_Factory;
};

/* Fatory is Qthread for callback in Qt GUI */
class Factory: public QThread
{
    Q_OBJECT
public:
    Factory();
    ~Factory();
public:
	/* Init function */
	BOOL Init();
	s32 InitAddCamera(CameraParam & pParam, astring strCamId);
	
public:
	BOOL RegCameraChangeNotify(void * pData, FactoryCameraChangeNotify callback);
	BOOL CallCameraChange(FactoryCameraChangeData data);
	
public:
	BOOL GetLicense(astring &strLicense, astring &strHostId, 
							int &ch, astring &type);
	BOOL SetLicense(astring &strLicense);
	BOOL InitLicense();

	BOOL GetExportPath(astring &strPath);
	BOOL SetExportPath(astring &strPath);

public:
	BOOL GetAutoLogin();
	BOOL AuthUser(astring &strUser, astring &strPasswd);
	BOOL GetUserData(VSCUserData &pData);
	BOOL SetUserData(VSCUserData &pData);

public:
	BOOL GetHdfsRecordConf(VSCHdfsRecordData &pData);
	BOOL SetHdfsRecordConf(VSCHdfsRecordData &pData);

	/* OpenCVR API port */
	BOOL GetOAPIPort(u16 &pPort);
	BOOL SetOAPIPort(u16 &pPort);

public:
	/* UI can use this for display camera tree */
	BOOL GetCameraParamMap(CameraParamMap &pMap);
	BOOL GetCameraOnlineMap(CameraOnlineMap &pMap);

       /* Camera function */
	s32 AddCamera(CameraParam & pParam);
	s32 GetCameraParamById(CameraParam & pParam, astring strCamId);
	BOOL GetCameraRtspUrl(astring & strUrl, astring strCamId);
	s32 GetCameraParamByIdTryLock(CameraParam & pParam, astring strCamId);
	BOOL DelCamera(astring strCamId);
	
	BOOL PtzAction(astring strCamId, FPtzAction action, float speed);

public:
	BOOL StartRecord(astring strCamId);
	BOOL StopRecord(astring strCamId);
	BOOL StartHdfsRecord(astring strCamId);
	BOOL StopHdfsRecord(astring strCamId);
	BOOL StartRecordAll();
	BOOL StopRecordAll();
	BOOL StartHdfsRecordAll();
	BOOL StopHdfsRecordAll();
public:
	BOOL GetRecordStatus(s32 nIndex, BOOL &bStatus);

public:
	/* Disk function */
	BOOL AddHdd(astring &strHdd, astring & strPath, s64 nSize);
	BOOL DelHdd(astring & strHdd);
	BOOL HddUpdateSize(astring &strHdd, s64 nSize);
	BOOL GetDiskMap(VDBDiskMap &pMap);
	BOOL GetDiskStatus(VDBDiskStatus &pStatus);
	BOOL UpdateDiskStatusMap(VDBDiskStatus &pStatus);

	/* Search function */
	BOOL SearchItems(astring strCamId, u32 startTime, u32 endTime, u32 recordType, 
					RecordItemMap &map);
	BOOL SearchHasItems(astring strCamId, u32 startTime, u32 endTime, 
					u32 recordType);
	BOOL SearchAItem(astring strCamId, u32 Time, VdbRecordItem &pItem);
	BOOL SearchAItemNear(astring strCamId, u32 Time, VdbRecordItem &pItem);
	BOOL SearchNextItem(astring strCamId, s64 LastId, VdbRecordItem &pItem);
	BOOL RequestAMFRead(VdbRecordItem &pItem, astring & strPath);
	BOOL FinishedAMFRead(VdbRecordItem &pItem, astring & strPath);

	VDB & GetVdb();


public:
	BOOL GetStreamInfo(astring strCamId, VideoStreamInfo &pInfo);
	/* Data */
	BOOL RegDataCallback(astring strCamId, CameraDataCallbackFunctionPtr pCallback, void * pParam);
	BOOL UnRegDataCallback(astring strCamId, void * pParam);
	BOOL GetInfoFrame(astring strCamId, InfoFrame &pFrame);
	BOOL RegSubDataCallback(astring strCamId, CameraDataCallbackFunctionPtr pCallback, void * pParam);
	BOOL UnRegSubDataCallback(astring strCamId, void * pParam);
	BOOL GetSubInfoFrame(astring strCamId, InfoFrame &pFrame);

	/* Raw data */
	BOOL RegRawCallback(astring strCamId, CameraRawCallbackFunctionPtr pCallback, void * pParam);
	BOOL UnRegRawCallback(astring strCamId, void * pParam);
	BOOL RegSubRawCallback(astring strCamId, CameraRawCallbackFunctionPtr pCallback, void * pParam);
	BOOL UnRegSubRawCallback(astring strCamId, void * pParam);

	/* Seq data  */
	BOOL RegSeqCallback(astring strCamId, CameraSeqCallbackFunctionPtr pCallback, void * pParam);
	BOOL UnRegSeqCallback(astring strCamId, void * pParam);
	
	BOOL RegDelCallback(astring strCamId, CameraDelCallbackFunctionPtr pCallback, void * pParam);
	BOOL UnRegDelCallback(astring strCamId, void * pParam);

	BOOL GetCameraOnline(astring strCamId, BOOL &bStatus);
	BOOL GetUrl(astring strCamId, std::string &url);
	BOOL SetSystemPath(astring &strPath);
public:
	void Lock(){m_Lock.lock();}
	bool TryLock(){return m_Lock.try_lock();}
	void UnLock(){m_Lock.unlock();}
public:
	static void Run(void * pParam);
	void run();

private:
	CameraMap m_CameraMap;
	CameraParamMap m_CameraParamMap;
	CameraOnlineMap m_CameraOnlineMap;

	/* Virtual IP camera param */
	VIPCCameraParamMap m_VIPCCameraParamMap;
	fast_mutex m_Lock;
	tthread::thread *m_pThread;

private:
	CameraChangeNofityMap m_CameraChange;

private:
	VDB *m_pVdb;
	VHdfsDB *m_pVHdfsdb;
	FactoryHddTask *m_HddTask;

private:
	ConfDB m_Conf;
	SysDB m_SysPath;
private:
	//CmnHttpServer *m_pHttpServer;
};

typedef Factory* LPFactory;


#include "factoryimpl.hpp"

#endif // __VSC_FACTORY_H_
