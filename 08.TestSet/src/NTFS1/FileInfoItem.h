#ifndef BASICUTILS_FILEINFOITEM_H_
#define BASICUTILS_FILEINFOITEM_H_

/**
 *\class  FileInfoItem
 *\brief  ɨ����õ��ļ���Ϣ�ڵ��ͨ������Ķ���ָ�룬�ɹ��������ļ���Ϣ��ģ�͡�
 *\date   2012-07-10
 *\author ����
 *\par    Revision History:
 */

#include <list>

// #include <BasicUtils.h>
// #include <LWString.h>

// LSTP_BASICUTILS_NS_BEGIN
// USE_LSTP_BASICLIBRARY_NS
// USE_STD_NS

enum ITEMTYPE {
	IT_MYCOMPUTER = 0,
	IT_DRIVE,
	IT_DIRECTORY,
	IT_FILE,
};
class FileInfoItem;

typedef std::list<FileInfoItem*> FileInfoItems;

class /*BASICUTILS_API*/ FileInfoItem
{
private:
	/**
	 *\fn         FileInfoItem(FileInfoItem* parent, const wchar_t* path, ITEMTYPE type,FILETIME& ft)
	 *\brief      ���캯�������иù��캯������������Ŀ¼�����������ļ��ڵ��
	 *\param[in]  parent ���ڵ㣬�����ֵΪNULL����ʾ��ǰ�ڵ�Ϊ���ڵ㡣
	 *\param[in]  path �ýڵ��ʾ��Ŀ¼����������������Ŀ¼����������ǰĿ¼����������ȫ·������
	 *\param[in]  type �ڵ�����͡�
	 *\param[in]  ft ��ǰ�ڵ�������޸�ʱ�䡣
	 */	
	void FileInfoItem(FileInfoItem* parent, const wchar_t* path, ITEMTYPE type,FILETIME& ft);
	/**
	 *\fn         FileInfoItem(FileInfoItem* parent, const wchar_t* filename, const uint64 filesize,FILETIME& ft)
	 *\brief      ���캯�������иù��캯��������������ͨ�ļ����ļ��ڵ��
	 *\param[in]  parent ���ڵ㡣
	 *\param[in]  filename ʵ�ʵ��ļ�����������ȫ·���ļ�����
	 *\param[in]  filesize �ļ��ĳ��ȡ�
	 *\param[in]  ft ��ǰ�ڵ�������޸�ʱ�䡣
	 */	
	void FileInfoItem(FileInfoItem* parent, const wchar_t* filename, const uint64 filesize,FILETIME& ft);
	/**
	 *\fn         ~FileInfoItem()
	 *\brief      ����������
	 */
	void ~FileInfoItem();

public:
	/**
	 *\fn         const LWString GetFullname() const
	 *\brief      ��ȡ��ǰ�ڵ��ȫĿ¼����ȫ�ļ�����
	 *\return     ���ص�ǰ�ڵ��ȫĿ¼����ȫ�ļ�����
	 */	
	const std::wstring GetFullname() const;
	/**
	 *\fn         const LWString GetName() const
	 *\brief      ����ȡ��ǰ�ڵ��Ŀ¼�����ļ�����
	 *\return     �����ص�ǰ�ڵ��Ŀ¼�����ļ�����
	 */	
	inline const std::wstring GetName() const {
		return name_;
	}
	/**
	 *\fn         ITEMTYPE GetType() const
	 *\brief      ��ȡ��ǰ�ڵ�����͡�
	 *\return     ���ص�ǰ�ڵ�����͡�
	 */	
	inline ITEMTYPE GetType() const {
		return type_;
	}
	/**
	 *\fn         size_t SubdirCount() const
	 *\brief      ��ȡ��ǰ�ڵ�����������Ŀ¼������
	 *\return     ���ص�ǰ�ڵ�����������Ŀ¼������
	 */	
	inline size_t SubdirCount() const {
		return subdirCount_;
	}
	/**
	 *\fn         size_t FileCount() const
	 *\brief      ��ȡ��ǰ�ڵ����������ļ�������
	 *\return     ���ص�ǰ�ڵ����������ļ�������
	 */	
	inline size_t FileCount() const {
		return filesCount_;
	}
	/**
	 *\fn         uint64 GetTotalSize() const
	 *\brief      ��ȡ��ǰ�ڵ���ռ�õĴ��̿ռ��ֽ�����
	 *\return     �����ǰ�ڵ�Ϊ�ļ��������ļ�����ĳ��ȣ����򷵻�����Ŀ¼�������ļ��ĳ����ܺ͡�
	 */	
	inline uint64 GetTotalSize() const {
		return totalSize_;
	}
	/**
	 *\fn         const FileInfoItems& ChildrenSubdirs() const
	 *\brief      ��ȡ��ǰ�ڵ������һ����Ŀ¼�ڵ㡣
	 *\return     ���ص�ǰ�ڵ������һ����Ŀ¼�ڵ㡣
	 */	
	inline const FileInfoItems& ChildrenSubdirs() const {
		return subdirItems_;
	}
	/**
	 *\fn         const FileInfoItems& ChildrenFiles() const
	 *\brief      ��ȡ��ǰ�ڵ������һ�����ļ��ڵ㡣
	 *\return     ���ص�ǰ�ڵ������һ�����ļ��ڵ㡣
	 */	
	inline const FileInfoItems& ChildrenFiles() const {
		return fileItems_;
	}
	/**
	 *\fn         FileInfoItem* GetParent() const
	 *\brief      ��ȡ��ǰ�ڵ�ĸ��ڵ㡣
	 *\return     ���ص�ǰ�ڵ�ĸ��ڵ㣬�����ֵΪNULL�����ʾ��ǰ�ڵ�Ϊ���ڵ㡣
	 */	
	inline FileInfoItem* GetParent() const {
		return parent_;
	}
	/**
	 *\fn         bool IsRootItem() const
	 *\brief      �жϵ�ǰ�ڵ��Ƿ�Ϊ���ڵ㡣
	 *\return     �Ƿ���true������false��
	 */	
	inline bool IsRootItem() const {
		return NULL == parent_;
	}
	
private:
	void AddChildDirectory(FileInfoItem* childItem);
	void AddChildFile(FileInfoItem* childItem);
	void UpdateAllData(uint64 updateSize, uint32 updateFileCount, uint32 updateDirCount, FILETIME& lastChangeTime);

private:
	ITEMTYPE type_;
	uint64 totalSize_;
	uint32 filesCount_;
	uint32 subdirCount_;
	FILETIME lastChangedTime_;
	std::wstring name_;
	FileInfoItems subdirItems_;
	FileInfoItems fileItems_;
	FileInfoItem* parent_;
	friend class FileScanner;
	friend class FileScanRunnable;
	friend class NtfsDiskScanRunnable;
};

inline bool operator< (const FILETIME& t1, const FILETIME& t2)
{
	if (t1.dwHighDateTime < t2.dwHighDateTime) {
		return true;
	} else if (t1.dwHighDateTime == t2.dwHighDateTime) {
		return t1.dwLowDateTime < t2.dwLowDateTime;
	} else {
		return false;
	}
}

inline bool operator == (const FILETIME& t1, const FILETIME& t2)
{
	return t1.dwLowDateTime == t2.dwLowDateTime && t1.dwHighDateTime == t2.dwHighDateTime;
}

//LSTP_BASICUTILS_NS_END
#endif /*BASICUTILS_FILEINFOITEM_H_*/
