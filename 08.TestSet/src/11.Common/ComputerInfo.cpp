
#include "stdafx.h"
#include "ComputerInfo.h"
#include <Iphlpapi.h> 


#pragma comment(lib,"Iphlpapi")


ComputerInfo::ComputerInfo()
	:upBytes_(0),
	downBytes_(0),
	upTick_(0),
	downTick_(0)
{
}

ComputerInfo::~ComputerInfo()
{
	ipAddr_.clear();
	macAddr_.clear();
}

/**
 *\fn         Init()
 *\brief      ��ʹ����
 */
void ComputerInfo::Init()
{
	ULONG dataLen = 0;
    char *buff = NULL;

	// Make an initial call to GetAdaptersInfo to get    
	// the necessary size into the ulOutBufLen variable    
	if (GetAdaptersInfo(NULL, &dataLen) == ERROR_BUFFER_OVERFLOW)
    {   
        buff = new char[dataLen];
        TRACE("GetAdaptersInfo:ERROR_BUFFER_OVERFLOW:%d\n", dataLen);
	}

    PIP_ADAPTER_INFO adapterInfo = (IP_ADAPTER_INFO*)buff;
	int ret = GetAdaptersInfo(adapterInfo, &dataLen);

    if (0 == ret)
    {
        for (; adapterInfo != NULL; adapterInfo = adapterInfo->Next)
        {
            ipAddr_.push_back(adapterInfo->IpAddressList.IpAddress.String);

            char mac[MAX_PATH] = "";
            sprintf(mac, "%02X-%02X-%02X-%02X-%02X-%02X", 
                adapterInfo->Address[0],
                adapterInfo->Address[1],
                adapterInfo->Address[2],
                adapterInfo->Address[3],
                adapterInfo->Address[4],
                adapterInfo->Address[5]);

            macAddr_.push_back(mac);

            TRACE("mac:%s ip:%s\n", mac, adapterInfo->IpAddressList.IpAddress.String);
        }
    }

	delete[] buff;

    // �������
	char name[MAX_PATH] = "";
	int len = sizeof(name);
	GetComputerName(name, (LPDWORD)&len);
	computerName_ = name;
    TRACE("computerName:%s\n", computerName_.c_str());

    // ��ǰ�Ѿ����շ��͵����ݴ�С
	if (upBytes_ == 0)
    {
		upBytes_ = GetUpBytes();
		upTick_ = GetTickCount();
	}

	if (downBytes_ == 0)
    {
		downBytes_ = GetDownBytes();
		downTick_ = GetTickCount();
	}
}

/**
*\fn         GetUpSpeed()
*\brief      �õ��������������ٶ�
*\return     �������������ٶ�
*/
int ComputerInfo::GetUpSpeed()
{
	if (upBytes_ == 0)
    {
		upBytes_ = GetUpBytes();
		upTick_ = GetTickCount();
	}

	int nowUpBytes = GetUpBytes();
	int nowUpTick = GetTickCount();

#pragma warning(disable : 4244)
	int speed = (nowUpBytes-upBytes_)/(float)(nowUpTick-upTick_);
#pragma warning(default : 4244)

	upBytes_ = nowUpBytes;
	upTick_ = nowUpTick;

	return speed;
}

/**
*\fn         GetDownSpeed()
*\brief      �õ��������������ٶ�
*\return     �������������ٶ�
*/
int ComputerInfo::GetDownSpeed()
{
	if (downBytes_ == 0)
    {
		downBytes_ = GetDownBytes();
		downTick_ = GetTickCount();
	}

	int nowDownBytes = GetDownBytes();
	int nowDownTick = GetTickCount();

#pragma warning(disable : 4244)
	int speed = (nowDownBytes-downBytes_)/(float)(nowDownTick-downTick_);
#pragma warning(default : 4244)

	downBytes_ = nowDownBytes;
	downTick_ = nowDownTick;

	return speed;
}

/**
*\fn         GetUpBytes()
*\brief      �õ��������������ֽ���
*\return     �������������ֽ���
*/
int ComputerInfo::GetUpBytes()
{
	int speed = -1;	// Ҳ�ɷ��ش����
	DWORD retVal = 0;
	ULONG buffLen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO adapterInfo = (IP_ADAPTER_INFO *)malloc(buffLen);
	MIB_IFROW ifRow = {0}; 

	memset(adapterInfo, 0, buffLen);

	// Make an initial call to GetAdaptersInfo to get    
	// the necessary size into the buffLen variable    
	if (GetAdaptersInfo(adapterInfo, &buffLen) == ERROR_BUFFER_OVERFLOW)
    {   
		delete adapterInfo;   
		adapterInfo = (IP_ADAPTER_INFO *)malloc(buffLen);
	}

	if (GetAdaptersInfo(adapterInfo, &buffLen) == NO_ERROR)
    {
		ATLTRACE("The number of your netadapters is %d \n", buffLen/sizeof(IP_ADAPTER_INFO));

		ifRow.dwIndex = adapterInfo->Index;

		if((retVal = GetIfEntry(&ifRow)) != NO_ERROR)
        {
			ATLTRACE("GetIfEntry Error: %d\n", retVal);
			speed = -2;	// ���ش����
		}
        else
        {
			speed = ifRow.dwOutOctets;	// �����ֽ���
		}
	}

	delete adapterInfo;
	return speed;
}

/**
*\fn         GetDownBytes()
*\brief      �õ��������������ֽ���
*\return     �������������ֽ���
*/
int ComputerInfo::GetDownBytes()
{
	int speed = -1;	// Ҳ�ɷ��ش����
	DWORD retVal = 0;
	ULONG buffLen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO adapterInfo = (IP_ADAPTER_INFO *)malloc(buffLen);
	MIB_IFROW ifRow = {0}; 

	memset(adapterInfo, 0, buffLen);

	// Make an initial call to GetAdaptersInfo to get    
	// the necessary size into the buffLen variable    
	if (GetAdaptersInfo(adapterInfo, &buffLen) == ERROR_BUFFER_OVERFLOW)
    {   
		delete adapterInfo;   
		adapterInfo = (IP_ADAPTER_INFO *)malloc(buffLen);
	}

	if (GetAdaptersInfo(adapterInfo, &buffLen) == NO_ERROR)
    {
		ATLTRACE("The number of your netadapters is %d \n", buffLen/sizeof(IP_ADAPTER_INFO));

		ifRow.dwIndex = adapterInfo->Index;

		if((retVal = GetIfEntry(&ifRow)) != NO_ERROR)
        {
			ATLTRACE("GetIfEntry Error: %d\n", retVal);
			speed = -2;	// ���ش����
		}
        else
        {
			speed = ifRow.dwInOctets;	// �����ֽ���
		}
	}

	delete adapterInfo;
	return speed;
}