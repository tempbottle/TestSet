
#pragma once
#include <map>
#include <RichOle.h>


class _AFX_RICHEDITEX_STATE
{
public:
	_AFX_RICHEDITEX_STATE();
	virtual ~_AFX_RICHEDITEX_STATE();
	HINSTANCE m_hInstRichEdit20;
};


//------------------------------------------------------
// ����OLE�ǵ��̶߳��󣬲���ֱ���������߳��е��ã�����ֻ�ܷ���Ϣ
#ifdef _DEBUG
#import "..\Debug\ShowImageOle.dll" no_namespace
#else
#import "..\Release\ShowImageOle.dll" no_namespace
#endif


enum enTransFile
{
	TRANS_LINK_ACCEPT  = 0x01,
	TRANS_LINK_SAVEAS  = 0x02,
	TRANS_LINK_REFUSE  = 0x03,
	TRANS_PROCCESSBAR  = 0x04,
	TRANS_ERROR		   = 0x05,
};

enum enKeyAreaType
{
	KEY_TYPE_TEXT	   = 0x00,
	KEY_TYPE_LINK	   = 0x01,
	KEY_TYPE_GIF	   = 0x02,
	KEY_TYPE_IMAGE	   = 0x03,
	KEY_TYPE_TRANSFILE = 0x04,
};

struct KEY_AREA
{
	int					type;
	int					id;
	CHARRANGE			pos;
	LPVOID				data;
	CComPtr<IShowImage>	ctrl;
};

typedef std::map<int, KEY_AREA*> KEY_AREA_MAP;


// �ڴ������ǰӦ�ȵ��� AfxInitRichEdit(); ���򽫲���ʾ����
class CXTRichEdit : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CXTRichEdit)

public:
	CXTRichEdit();
	virtual ~CXTRichEdit();

protected:
	HWND					parentWnd_; //��������
	CHARFORMAT2				charfmt_;	// Ĭ���ַ���ʽ
	CComPtr<IRichEditOle>	richEditOle_;

public:
	/**
	 *\fn         Init()
	 *\brief      ��ʹ����
	 */
	void Init();

	/**
	 *\fn         AddText(const char *text)
	 *\brief      ����ı���
	 *\param[in]  text  �ı����ݡ�
	 */
	void AddText(const char *text);

	/**
	 *\fn         AddText(const char *text, CHARFORMAT2 &format)
	 *\brief      ��Ӹ�ʽ�ı���
	 *\param[in]  text  �ı����ݡ�
	 */
	void AddText(const char *text, CHARFORMAT2 &format);
	
	/**
	 *\fn         AddText(const char *text, COLORREF color, int weight)
	 *\brief      ����ı���
	 *\param[in]  text   �ı����ݡ�
	 *\param[in]  color  �ı���ɫ��
	 *\param[in]  weight �ı��ֺš�
	 */
	void AddText(const char *text, COLORREF color, int weight);

	/**
	 *\fn         AddLink(const char *text, COLORREF color, int weight)
	 *\brief      ��ӳ������ı���
	 *\param[in]  text   �ı����ݡ�
	 *\param[in]  data   �������ݡ�
	 */
	KEY_AREA* AddLink(const char *text, void *data);

	/**
	 *\fn         SetLink(int id, const char *text)
	 *\brief      ��ӳ������ı���
	 *\param[in]  id     ����ID��
	 *\param[in]  text   �ı����ݡ�
	 */
	KEY_AREA* SetLink(int id, const char *text);

	/**
	 *\fn         SetLink(KEY_AREA *key, const char *text)
	 *\brief      ��ӳ������ı���
	 *\param[in]  key    �ؼ��㡣
	 *\param[in]  text   �ı����ݡ�
	 */
	KEY_AREA* SetLink(KEY_AREA *key, const char *text);

	/**
	 *\fn         AddImage(HBITMAP bitmap, void *data)
	 *\brief      ���ͼƬ�ؼ��㡣
	 *\param[in]  bitmap    BITMAP�����
	 *\param[in]  data      �������ݡ�
	 *\return     ��ָ�롣
	 */
	KEY_AREA* AddImage(HBITMAP bitmap, void *data);

	/**
	 *\fn         AddImage(HBITMAP bitmap, void *data)
	 *\brief      ���ͼƬ�ؼ��㡣
	 *\param[in]  filename  BITMAP�ļ�·������
	 *\param[in]  data      �������ݡ�
	 *\return     ��ָ�롣
	 */
	KEY_AREA* AddImage(const char *filename, void *data);
	
	/**
	 *\fn         AddImage(const char *filename, CHARRANGE range, void *data)
	 *\brief      ���ͼƬ�ؼ��㡣
	 *\param[in]  filename  BITMAP�ļ�·������
	 *\param[in]  range     λ�á�
	 *\param[in]  data      �������ݡ�
	 *\return     ��ָ�롣
	 */
	KEY_AREA* AddImage(const char *filename, CHARRANGE range, void *data);

	/**
	 *\fn         SetImage(KEY_AREA *key, HBITMAP bitmap)
	 *\brief      ����ͼƬ�ؼ��㡣
	 *\param[in]  key		�ؼ��㡣
	 *\param[in]  bitmap    λͼ��
	 *\return     ��ָ�롣
	 */
	KEY_AREA* SetImage(KEY_AREA *key, HBITMAP bitmap);

	/**
	 *\fn         SetImage(KEY_AREA *key, HBITMAP bitmap)
	 *\brief      ����ͼƬ�ؼ��㡣
	 *\param[in]  id		�ؼ���ID��
	 *\param[in]  bitmap    λͼ��
	 *\return     ��ָ�롣
	 */
	KEY_AREA* SetImage(int id, HBITMAP bitmap);

	/**
	 *\fn         SetImage(KEY_AREA *key, const char *filename)
	 *\brief      ����ͼƬ�ؼ��㡣
	 *\param[in]  key		�ؼ��㡣
	 *\param[in]  filename  λͼ�ļ�����
	 *\return     ��ָ�롣
	 */
	KEY_AREA* SetImage(KEY_AREA *key, const char *filename);

	/**
	 *\fn         SetImage(int id, const char *filename)
	 *\brief      ����ͼƬ�ؼ��㡣
	 *\param[in]  id		�ؼ���ID��
	 *\param[in]  filename  λͼ�ļ�����
	 *\return     ��ָ�롣
	 */
	KEY_AREA* SetImage(int id, const char *filename);

	/**
	 *\fn         AddGif(const char *filename, void data)
	 *\brief      ���GIF�ļ���
	 *\param[in]  filename	�ļ�����
	 *\param[in]  data      �������ݡ�
	 *\return     ��ָ�롣
	 */
	KEY_AREA* AddGif(const char *filename, void *data);


	/**
	 *\fn         SetGif(KEY_AREA *key, const char *filename)
	 *\brief      ����GIF�ļ���
	 *\param[in]  key	    �ؼ��㡣
	 *\param[in]  filename	�ļ�����
	 *\return     ��ָ�롣
	 */
	KEY_AREA* SetGif(KEY_AREA *key, const char *filename);

	/**
	 *\fn         SetGif(int id, const char *filename)
	 *\brief      ����GIF�ļ���
	 *\param[in]  id	    �ؼ���ID��
	 *\param[in]  filename	�ļ�����
	 *\return     ��ָ�롣
	 */
	KEY_AREA* SetGif(int id, const char *filename);

	/**
	 *\fn         AddTransFile(const char *filename,
	 *\fn					  UINT64  fileLen,
	 *\fn        			  const char *iconFilename,
	 *\fn        			  const char *text,
	 *\fn        			  void* data)
	 *\brief      ��Ӵ����ļ��ؼ���
	 *\param[in]  filename		�ļ�����
	 *\param[in]  fileLen		�ļ�����
	 *\param[in]  iconFilename	ͼ���ļ�����
	 *\param[in]  text			�ı���Ϣ��
	 *\param[in]  data			�������ݡ�
	 *\return     ��ָ�롣
	 */
	KEY_AREA * CXTRichEdit::AddTransFile(const char *filename,
		UINT64  fileLen,
		const char *iconFilename,
		const char *text,
		void* data);

	/**
	 *\fn         SetTransFile(KEY_AREA *key, const char *text, int state, UINT64 param)
	 *\brief      ���ô����ļ��ؼ���
	 *\param[in]  key		�ؼ��㡣
	 *\param[in]  text		����Ϣ��
	 *\param[in]  state		״̬��(0-��ʾ�ļ������ļ���С,1-��ʾ����,2-��ʾ�ļ�������ɺ���ʱ)
	 *\param[in]  param		������
	 */
	void SetTransFile(KEY_AREA *key, const char *text, int state, UINT64 param);

	/**
	 *\fn         GetCtrlInfo(LONG index, int &id, LONG &pos)
	 *\brief      �õ��ؼ���Ϣ��
	 *\param[in]  index		��š�
	 *\param[out] id		�ؼ�ID��
	 *\param[out] pos		�ؼ�λ�á�
	 *\return     �Ƿ�ɹ���
	 */
	bool GetCtrlInfo(LONG index, int &id, LONG &pos);
	
	/**
	 *\fn         getCtrlCount()
	 *\brief      �õ�ole��������
	 *\return     ����ole��������
	 */
	LONG getCtrlCount() { richEditOle_->GetObjectCount(); }
	
protected:
    KEY_AREA_MAP keyMap_;

	/**
	 *\fn         MakeID()
	 *\brief      ����һ��Ψһ��ID��
	 *\return     Ψһ��ID��
	 */
	DWORD MakeID();

	/**
	 *\fn         getKey(int id)
	 *\brief      �õ���ָ�롣
	 *\return     ��ָ�롣
	 */
	KEY_AREA* getKey(int id);

	/**
	 *\fn         getKey(CHARRANGE range)
	 *\brief      �õ���ָ�롣
	 *\param[in]  range    �ַ�λ�á�
	 *\return     ��ָ�롣
	 */
	KEY_AREA* getKey(CHARRANGE range);

	/**
	 *\fn         DeleteKey(KEY_AREA* key)
	 *\brief      ɾ������
	 *\param[in]  key    ��ָ�롣
	 */
	void DeleteKey(KEY_AREA* key);

	/**
	 *\fn         DeleteAll()
	 *\brief      ɾ�����ڵ㡣
	 */
	void DeleteAll();

	/**
	 *\fn         KeyOffset(KEY_AREA *key, int offset)
	 *\brief      ���ùؼ���λ�á�
	 *\param[in]  key    ��ָ�롣
	 *\param[in]  offset ƫ�ơ�
	 */
	void KeyOffset(KEY_AREA *key, int offset);

	/**
	 *\fn         InsertImage(const char *fileame, int id)
	 *\brief      ���ͼƬ��
	 *\param[in]  fileame	�ļ�����
	 *\param[in]  id		ID��
	 *\return     �Ƿ�ɹ�,0�ɹ�����ʧ��
	 */
	int InsertImage(const char *fileame, int id);

	/**
	 *\fn         InsertImage(HANDLE image, int id)
	 *\brief      ���ͼƬ��
	 *\param[in]  image		�ļ������
	 *\param[in]  id		ID��
	 *\return     �Ƿ�ɹ�,0�ɹ�����ʧ��
	 */
	int InsertImage(HANDLE image, int id);

	/**
	 *\fn         InsertGif(KEY_AREA *key, const char *filename)
	 *\brief      ���GIFͼƬ��
	 *\param[in]  key		�ؼ��㡣
	 *\param[in]  image		�ļ������
	 *\return     �Ƿ�ɹ�,0�ɹ�����ʧ��
	 */
	int InsertGif(KEY_AREA *key, const char *filename);

	/**
	 *\fn         InsertTransFileCtrl(KEY_AREA *key, 
	 *\fn					  const char *filename,
	 *\fn					  UINT64  fileLen,
	 *\fn					  const char *iconFilename,
	 *\fn					  const char *text)
	 *\brief      ��Ӵ����ļ��ؼ���
	 *\param[in]  key			�ؼ��㡣
	 *\param[in]  filename		�ļ�����
	 *\param[in]  fileLen		�ļ�����
	 *\param[in]  iconFilename	ͼ���ļ�����
	 *\param[in]  text			�ı���Ϣ��
	 *\return     �Ƿ�ɹ���
	 */
	int CXTRichEdit::InsertTransFileCtrl(KEY_AREA *key, 
		const char *filename,
		UINT64  fileLen,
		const char *iconFilename,
		const char *text);

	/**
	 *\fn         LoadOleCtrl(int id)
	 *\brief      ��ӿؼ���
	 *\param[in]  id		�ؼ���ID��
	 *\param[in]  showImage	�ؼ�ָ�롣
	 *\return     �Ƿ�ɹ���
	 */
	bool LoadOleCtrl(int id, IShowImage **showImage);

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEnMsgfilter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnLink(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnDropfiles(NMHDR *pNMHDR, LRESULT *pResult);
};