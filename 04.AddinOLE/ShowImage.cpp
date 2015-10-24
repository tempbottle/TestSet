
// ShowImage.cpp : CShowImage ��ʵ��
#include "stdafx.h"
#include "ShowImage.h"


#define KB (1024)
#define MB (1024*KB)
#define GB (1024*MB)

/**
 *\fn         LoadFileTrans(BSTR filename, UINT64 fileLen, BSTR iconFilename)
 *\brief      ���ش��������
 *\param[in]  filename     �ļ�ȫ·������
 *\param[in]  fileLen      �ļ���С��
 *\param[in]  iconFilename ͼ���ļ�ȫ·������
 */
STDMETHODIMP CShowImage::LoadFileTrans(BSTR filename, UINT64 fileLen, BSTR iconFilename)
{
	if (0 != type_) return S_OK;

	if (NULL != filename)
	{
        startTime_ = ::GetTickCount();
        lastTime_  = startTime_;
		fileLen_   = fileLen;
		drawIcon_  = false;		
		type_      = TRANSFILE;

		USES_CONVERSION;
		std::string pathFile = W2A(filename);
		message_ = pathFile.substr(pathFile.rfind('\\') + 1, pathFile.length());

        FLOAT fileLenG = (float)fileLen_ / GB;
        FLOAT fileLenM = (float)fileLen_ / MB;
        FLOAT fileLenK = (float)fileLen_ / KB;

		char message[MAX_PATH] = "";

        if (fileLenG > 1.0) 
            sprintf(message, "%s (%0.2fGB)", message_.c_str(), fileLenG);
        else if (fileLenM > 1.0) 
            sprintf(message, "%s (%0.2fMB)", message_.c_str(), fileLenM);
        else if (fileLenK > 1.0)
            sprintf(message, "%s (%0.2fKB)", message_.c_str(), fileLenK);
        else
            sprintf(message, "%s (%dB)", message_.c_str(), fileLen_);

		message_ = message;

		RECT rcText = {0};
        DrawText(dc_, message_.c_str(), message_.length(), &rcText, DT_CALCRECT);

		wndSize_.cx = rcText.right - rcText.left;
		wndSize_.cx = (wndSize_.cx < 200) ? 200 : wndSize_.cx;
		wndSize_.cy = 70;

		SIZEL szl = {0};
		AtlPixelToHiMetric(&wndSize_, &szl);
        SetExtent(DVASPECT_CONTENT, &szl);	// ���ô��ڴ�С
        
        HWND hWnd = GetContainerWindow();
		RECT wndRect = {0};
        ::GetClientRect(hWnd, &wndRect);

		if (NULL != iconFilename)
		{
			icon_ = (HICON)::LoadImage(NULL, W2A(iconFilename), IMAGE_ICON, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
		}

		if (NULL == icon_)
		{
			SHFILEINFO fileInfo = {0};
			SHGetFileInfo(pathFile.c_str(), 0, &fileInfo, sizeof(fileInfo), SHGFI_ICON|SHGFI_LARGEICON|SHGFI_USEFILEATTRIBUTES);
			icon_ = fileInfo.hIcon;
		}

		DrawTransFileIcon();
		FireViewChange();
	}
	return S_OK;
}

/**
 *\fn         SetFileTrans(BSTR msg, DWORD state, UINT64 param)
 *\brief      ���ô��������
 *\param[in]  msg   ��ʾ����Ϣ��
 *\param[in]  state ״̬��(0-��ʾ�ļ������ļ���С,1-��ʾ����,2-��ʾ�ļ�������ɺ���ʱ)
 *\param[in]  param ������
 */
STDMETHODIMP CShowImage::SetFileTrans(BSTR msg, DWORD state, UINT64 param)
{
    if (NULL != msg)
    {
        switch(state)
        {
        case TRANS_STATE_BEGIN:
            {
				USES_CONVERSION;
                message_ = W2A(msg);
                progress_ = 0;
                break;
            }
        case TRANS_STATE_TANSING:
            {
                DWORD nowTime = ::GetTickCount();
                DWORD time    = (nowTime - lastTime_) / 1000; // ʱ����
                UINT64 size   = param - progress_;
                progress_     = param;
                lastTime_     = nowTime;

                float fProgress = (float)progress_ / fileLen_ * 100;
                float speed    = (float)size / time;

                FLOAT speedG  = speed / GB;
                FLOAT speedM  = speed / MB;
                FLOAT speedK  = speed / KB;

				char message[MAX_PATH] = "";

                if (speedG > 0.5) 
                    sprintf(message, "%.2f%% (%.2fGB/S)", fProgress, speedG);
                else if (speedM > 0.5) 
                    sprintf(message, "%.2f%% (%.2fMB/S)", fProgress, speedM);
                else if (speedK > 0.5)
                    sprintf(message, "%.2f%% (%.2fKB/S)", fProgress, speedK);
                else
                    sprintf(message, "%.2f%% (%.2fB/S)",  fProgress, speed);

				message_ = message;

                break;
            }
        case TRANS_STATE_END:
            {
                progress_ = fileLen_;
                DWORD time = (::GetTickCount() - startTime_) / 1000;

                int hour   = time / (60*60);
                int minute = time % (60*60) / 60;
                int second = time % 60;

				char message[MAX_PATH] = "";

                if (hour > 0)
                    sprintf(message, "�������,��ʱ%dʱ%d��%d��", hour, minute, second);
                else if (minute > 0)
                    sprintf(message, "�������,��ʱ%d��%d��", minute, second);
                else
                    sprintf(message, "�������,��ʱ%d��", second);

				message_ = message;

                break;
            }
        default:
            break;
        }

		DrawProgress();   // �ػ������
		FireViewChange();
	}
	return S_OK;
}

// ���ļ�ͼ��
void CShowImage::DrawTransFileIcon()
{
	RECT rc = {0, 0, wndSize_.cx, wndSize_.cy};
	::FillRect(compDC_, &rc, brush_);

	if (NULL != icon_)
	{
		::DrawIcon(compDC_, wndSize_.cx / 2 - 16, 1, icon_);
	}

	if (message_.length() > 0)
	{
		RECT rcText = rc;
		rcText.top += 32;
		rcText.bottom -= 20;

		HFONT oldFont = (HFONT)::SelectObject(compDC_, (HGDIOBJ)font_);

		::SetTextColor(compDC_, RGB(0, 0, 255));

		::DrawText(compDC_, message_.c_str(), message_.length(), &rcText, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		::SelectObject(compDC_, (HGDIOBJ)oldFont);
	}
}

// nProgress [in] ��ǰ����
// nLength   [in] �ܳ���
void CShowImage::DrawProgress()
{
    if (progress_ > fileLen_) progress_ = fileLen_;

	int progLen = (int)((wndSize_.cx-8)/(float)fileLen_ * progress_);
	int y = wndSize_.cy - 15;

    // ������
	::BitBlt(compDC_, 1, y, 3, 14, imageDC_, 0, 0, SRCCOPY);										// ���
	::BitBlt(compDC_, wndSize_.cx-4, y, 3, 14, imageDC_, 7, 0, SRCCOPY);								// �Ҷ�
	::StretchBlt(compDC_, 4, y, wndSize_.cx-8, 14, imageDC_, 2, 0, 1, 14, SRCCOPY);					// ��
	if (progress_ > 0) ::StretchBlt(compDC_, 4, y, progLen, 14, imageDC_, 3, 0, 1, 14, SRCCOPY);	// ����

	HFONT oldFont2 = (HFONT)::SelectObject(compDC_, (HGDIOBJ)font1_);
	RECT rc = {2, y, wndSize_.cx-2, wndSize_.cy};

	::DrawText(compDC_, message_.c_str(), message_.length(), &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	::SelectObject(compDC_, oldFont2);

	update_ = false;
}


STDMETHODIMP CShowImage::LoadGif(BSTR filename)
{
	if (0 != type_) return S_OK;

	type_ = SHOWIMAGE;

	//HWND hWnd = GetContainerWindow();
	//::GetClientRect(hWnd, &m_rcParWnd);

	USES_CONVERSION;
	std::string fileName = W2A(filename);

	if (gif_.Load(fileName.c_str()))
	{
		frameCount_ = gif_.getFrameCount();

		delayTime_ = new WORD[frameCount_];
		ATLASSERT(NULL != delayTime_);

		wndSize_.cx = gif_.getWidth();
		wndSize_.cy = gif_.getHeight();

		SIZEL szl = {0};
		AtlPixelToHiMetric(&wndSize_, &szl);
		SetExtent(DVASPECT_CONTENT, &szl);

		imageDC_ = xtDC_.AddNullDC(1, wndSize_.cx, wndSize_.cy * frameCount_);

		for (int i = 0; i < frameCount_; i++)
		{
			gif_.Draw(imageDC_, 0, i * wndSize_.cy, i);

			delayTime_[i] = gif_.getDelayTime(i) * 10;

			if (0 == delayTime_[i]) delayTime_[i] = 150;
		}

		curFrame_ = 0;
		gif_.Unload();

		AddTimer(this, delayTime_[0]);

		return S_OK;
	}	

	return S_OK;
}

// ��ʾ��һ֡��������һ֡��ʾʱ��
LRESULT CShowImage::OnTimer(void)
{
	BitBlt(compDC_, 0, 0, wndSize_.cx, wndSize_.cy, imageDC_, 0, curFrame_ * wndSize_.cy, SRCCOPY);
	
	FireViewChange();

	curFrame_ = (++curFrame_) % frameCount_;

	DelTimer(m_hWnd, timerId_);

	AddTimer(this, delayTime_[curFrame_]);

	return 0;
}

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	CShowImage *pThis = CShowImage::GetThis(nIDEvent);

	if (NULL != pThis) pThis->OnTimer();
}


void CShowImage::DrawCtrl(HDC hDC, RECT& rc)
{
	int x = rc.left;
	int y = rc.top;
	int cx = wndSize_.cx;
	int cy = wndSize_.cy;
	int sx = 0;
	int sy = 0;

	if (x <= 0)
	{
		x = 1;
		sx = 0 - rc.left;
		cx -= sx;
	}

	if (y <= 0)
	{
		y = 1;
		sy = 0 - rc.top;
		cy -= sy;
	}

// 	if ((x < m_rcParWnd.right) && (x + m_szWnd.cx > m_rcParWnd.right))
// 	{
// 		cx = m_rcParWnd.right - x - 1;
// 	}
// 
// 	if ((y < m_rcParWnd.bottom) && (y + m_szWnd.cy > m_rcParWnd.bottom))
// 	{
// 		cy = m_rcParWnd.bottom - y;
// 	}

	BitBlt(hDC, x, y, cx, cy, compDC_, sx, sy, SRCCOPY);
}

HWND CShowImage::GetContainerWindow()
{
	HWND hwnd = NULL;
	HRESULT hr;

	// ��δ�����VC++ ����
	if (m_spInPlaceSite != NULL)
	{
		m_spInPlaceSite->GetWindow(&hwnd);
		return hwnd;
	}

	// ��δ�����Visual Basic����
	LPOLECLIENTSITE pOleClientSite = NULL;
	GetClientSite(&pOleClientSite);

	if (pOleClientSite)
	{
		IOleWindow* pOleWindow;
		hr = pOleClientSite->QueryInterface(IID_IOleWindow, (LPVOID*)&pOleWindow);

		if (pOleWindow)
		{
			pOleWindow->GetWindow(&hwnd);
			pOleWindow->Release();
			return hwnd;
		}
	}

	return NULL;
}


