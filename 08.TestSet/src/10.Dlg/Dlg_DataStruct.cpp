// Dlg_DataStruct.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dlg_DataStruct.h"
#include "Common/ProcessCode.h"
#include "DataStruct/CDMA.h"
#include "DataStruct/BigMatch.h"
#include "DataStruct/MiraclRSA.h"


IMPLEMENT_DYNAMIC(CDlg_DataStruct, CDialog)
CDlg_DataStruct::CDlg_DataStruct(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_DataStruct::IDD, pParent)
{
}

CDlg_DataStruct::~CDlg_DataStruct()
{
}

void CDlg_DataStruct::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_DataStruct, CDialog)
	ON_BN_CLICKED(IDC_DATASTRUCT_BTN_INSERT, OnBnClickedInsert)
	ON_BN_CLICKED(IDC_DATASTRUCT_BTN_CODE,	 OnBnClickedCode)
    ON_BN_CLICKED(IDC_DATASTRUCT_BTN_CDMA, &CDlg_DataStruct::OnBnClickedDatastructBtnCdma)
    ON_BN_CLICKED(IDC_DATASTRUCT_BTN_BIG_MATCH, &CDlg_DataStruct::OnBnClickedDatastructBtnBigMatch)
END_MESSAGE_MAP()


void CDlg_DataStruct::OnBnClickedInsert()
{
	CString strKey;

	GetDlgItem(IDC_DATASTRUCT_EDIT1)->GetWindowText(strKey);

	int nKey = atoi(strKey);

	if (BTree.Insert(nKey, NULL))
	{
		MessageBox("Insert OK");
	}
}

void CDlg_DataStruct::OnBnClickedCode()
{
	CProcessCode processCode;

	std::string data("void class::CDlg_DataStruct(CWnd* pParent /*=NULL*/, int a ,struct tagn b, AA c );");

	processCode.load(data.c_str(), data.length());
}

#include "wtl/atlrx.h"          // ����

CAtlRegExp<>         regBell_;
CAtlRegExp<>         regFlage_;
CAtlRegExp<>         regBegin_;
CAtlRegExp<>         regSetColor_;
CAtlRegExp<>         regDefaultColor_;
CAtlRegExp<>         regErase1_;
CAtlRegExp<>         regErase2_;
CAtlRegExp<>         regCursorMove1_;
CAtlRegExp<>         regCursorMove2_;
CAtlRegExp<>         regBack_;
CAtlREMatchContext<> matchContext_;

void OutputText(char *text)
{
    //TRACE(text);
}

void show(CAtlREMatchContext<> &matchContext)
{
    ATLTRACE("------------------\n");
    char buff[1024] = "";

    for (int i = 0; i < matchContext.m_uNumGroups; i++)
    {
        const CAtlREMatchContext<>::RECHAR* szStart = 0;
        const CAtlREMatchContext<>::RECHAR* szEnd = 0;
        matchContext.GetMatch(i, &szStart, &szEnd);
        int count = szEnd - szStart;
        strncpy(buff, szStart, count);
        buff[count] = '\0';
        ATLTRACE("%s\n", buff);
    }
}

int putText(const CHAR* text)
{
    REParseError status = regBell_.Parse(_T("^[\x07]+$"));              // ƥ��ȫ��0x07
    status = regBegin_.Parse(_T("{\x1b\\[}|{\x1b\\]0;}"));              // ƥ��\033[
    status = regFlage_.Parse(_T("^{\x1b\\]0;}{.*}{\x07}"));             // ƥ��\033]0;*****0x07
    status = regSetColor_.Parse(_T("^{\x1b\\[}{([0-9][0-9];?)+}{m}"));  // ������ɫ
    status = regDefaultColor_.Parse(_T("^{\x1b\\[}{0?}{m}"));           // �ָ�Ĭ����ɫ
    status = regErase1_.Parse(_T("^{\x1b\\[}{[1-2]?}{K}"));             // Kɾ����굽��β,1Kɾ����굽����,2Kɾ����ǰ��
    status = regErase2_.Parse(_T("^{\x1b\\[}{[1-2]?}{J}"));             // Jɾ����굽��Ļβ,1Jɾ����굽��Ļ��,2J����
    status = regCursorMove1_.Parse(_T("^{\x1b\\[}{[0-9]*}{[ABCD]}"));   // A�������,B�������,C�������,D�������1
    status = regCursorMove2_.Parse(_T("^{\x1b\\[\\?}{[0-9][0-9][0-9][0-9]}{h}")); // ���ù��λ��
    status = regBack_.Parse(_T("^{(.*\x08+)+}{.*}"));                      // �˸�

    int curPos = 0;
    int lastTextPos = 0;
    int textLen = strlen(text);
    char temp[1024*50] = "";
    const char *start = NULL;

    if (regBell_.Match(text, &matchContext_))
    {
        return S_OK;
    }

    while (regBegin_.Match(&text[curPos], &matchContext_))
    {
        start = matchContext_.m_Match.szStart;
        curPos = start - text;

        if (curPos > lastTextPos)
        {
            strncpy(temp, &text[lastTextPos], curPos - lastTextPos);
            temp[curPos - lastTextPos] = '\0';
            OutputText(temp);
        }

        if (regSetColor_.Match(start, &matchContext_))
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            show(matchContext_);
        }
        else if (regDefaultColor_.Match(start, &matchContext_))
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            show(matchContext_);
        }
        else if (regErase1_.Match(start, &matchContext_))
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            show(matchContext_);
        }
        else if (regErase2_.Match(start, &matchContext_))
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            show(matchContext_);
        }
        else if (regCursorMove1_.Match(start, &matchContext_))
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            show(matchContext_);
        }
        else if (regCursorMove2_.Match(start, &matchContext_))
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            show(matchContext_);
        }
        else if (regBack_.Match(start, &matchContext_))
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            show(matchContext_);
        }
        else if (regFlage_.Match(start, &matchContext_)) //\033]0;*****0x07
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            show(matchContext_);
        }
        else
        {
            curPos = matchContext_.m_Match.szEnd - text;
            lastTextPos = curPos;
            ATLTRACE("-----------------------error\n");
        }
    }

    if (textLen > curPos)
    {
        strncpy(temp, &text[lastTextPos], textLen - curPos);
        temp[textLen - curPos] = '\0';
        OutputText(temp);
    }

    return 0;
}

void CDlg_DataStruct::OnBnClickedDatastructBtnCdma()
{
    putText("total 56\n"
        "drwx------   5 zht  zht   4096 Oct 25 10:42 \033[0m\033[01;34m.\033[0m\n"
        "drwxr-xr-x. 15 root root  4096 Oct 24 10:31 \033[01;34m..\033[0m\n"
        "drwxr-xr-x  14 zht  root  4096 Oct 25 10:42 \033[01;34;11;22m01.trunk\033[0m\n"
        "-rw-------   1 zht  root 13196 Oct 30 13:46 .bash_history\n"
        "-rw-r--r--   1 zht  zht     18 Aug 29  2012 .bash_logout\n"
        "-rw-r--r--   1 zht  zht    176 Aug 29  2012 .bash_profile\n"
        "-rw-r--r--   1 zht  zht    124 Aug 29  2012 .bashrc\n"
        "drwxrwxrwx  19 zht  root  4096 Oct 16 15:33 \033[34;42mpegasus\033[0m\n"
        "drwxr-xr-x   3 zht  root  4096 Sep 12 14:48 \033[01;34m.subversion\033[0m\n"
        "lrwxrwxrwx   1 zht  root    63 Aug 27 16:39 \033[01;36mTest\033[0m -> \033[01;34m/usr/zht/01.trunk/thirdparty/sources/glog-0.3.2/vsprojects/Test\033[0m\033[K\n"
        "-rw-------   1 zht  root  3335 Oct 25 10:42 .viminfo\n"
        "\033[?1034h\033[m\033]0;zht@LIMDEV007:~/Test\007[zht@\b\b\bLIMDE\b\bV007 ~]$ ");

    unsigned char output1[1000] = "";
    unsigned char output2[1000] = "";
    unsigned char input1[10] = ""; 
    unsigned char input2[10] = "";
    input1[0] = 0xa0;
    input1[1] = 0xb1;
    input2[0] = 0xc2;
    input2[1] = 0xd3;

    CDMA cdma;

    int ret = cdma.encode(0, input1, 2, output1, sizeof(output1));

    for (int i = 0; i < ret; i++)
    {
        TRACE("%2d ", (char)output1[i]);
    }

    TRACE("\n");

    ret = cdma.encode(1, input2, 2, output2, sizeof(output2));

    for (int i = 0; i < ret; i++)
    {
        TRACE("%2d ", (char)output2[i]);
    }

    TRACE("\n");

    for (int i = 0; i < ret; i++)
    {
        output1[i] += output2[i];
        TRACE("%2d ", (char)output1[i]);
    }

    TRACE("\n");

    int len = ret;

    ret = cdma.decode(0, output1, len, output2, sizeof(output2));

    for (int i = 0; i < ret; i++)
    {
        TRACE("0x%02x ", output2[i]);
    }

    TRACE("\n");

    ret = cdma.decode(1, output1, len, output2, sizeof(output2));

    for (int i = 0; i < ret; i++)
    {
        TRACE("0x%02x ", output2[i]);
    }

    TRACE("\n");
}

void CDlg_DataStruct::OnBnClickedDatastructBtnBigMatch()
{
    char m[1000] = "1234567890";
    char buff[10000] = "";
    char encode[10000] = "";
    int encodeLen = sizeof(encode);
    int buffLen = sizeof(buff);

    {
        MiraclRSA rsa;

        rsa.GetPublicKey(100, buff, buffLen);

        rsa.GetPrivateKey(550, 600, buff, buffLen, true);

        rsa.Encode(m, strlen(m), encode, encodeLen);

        rsa.Decode(encode, encodeLen, buff, buffLen);
    }

/*
    //�������RSA�㷨���ܺͽ�������

    char outStr[50000];
    char mStr[] = "Computer";

    miracl *mip = mirsys(400,10);   //��ʼ��һ��400λ10���ƵĴ���ϵͳ

    big m = mirvar(0); //m����
    big c = mirvar(0); //c����

    big p = mirvar(0); //������p
    big q = mirvar(0); //������q

    big n = mirvar(0); //nģ��
    big pn =mirvar(0); //ŷ������ֵpn = (p - 1)(q - 1)

    big d = mirvar(0); //d˽Կ
    big e = mirvar(0); //e��Կ

    expb2(500, p);      //����2��500�η�, 2^1024 ~= 1.8 * 10^308

    nxprime(p, p);      //��һ����2��500�η��������

    mip->IOBASE = 10;   //��ԭ����16���Ƹ�Ϊ10����ģʽ

    cotstr(p, outStr);  //��һ���������������ת����һ���ַ���

    TRACE("p=%s\n", outStr);

    cotnum(p, stdout);

    if (isprime(p))     //���ǲ���һ���Ƿ�Ϊ����
    {
        TRACE("p is a prime!\n");
    }

    premult(p, 2, q);   //q=p*2

    nxprime(q, q);      //��һ����p*2�������

    cotstr(q, outStr);  //��һ���������������ת����һ���ַ���

    TRACE("q=%s\n", outStr);

    if (isprime(q))     //���ǲ���һ���Ƿ�Ϊ����
    {
        TRACE("q is a prime!\n");
    }

    multiply(p, q, n);          //n = p*q

    //���¼���ŷ������ֵpn
    decr(p, 1, p);              //p = p - 1
    decr(q, 1, q);              //q = q - 1
    multiply(p, q, pn);         //pn = (p - 1)(q - 1)

    convert(65537, e);          //ȡe��ԿΪ2��16�η���1
    //cinstr(e,"65537");        //ȡe��ԿΪ2��16�η���1

    xgcd(e, pn, d, d, d);       //����d = e^-1 mod n,����������������չ���Լ����Ҳ������������ģ��

    cotstr(d, outStr);          //��һ���������������ת����һ���ַ���

    TRACE("d=%s\n", outStr);

    bytes_to_big(8, mStr, m);   //��8���ַ������ģ�ת���ɴ���

    cotstr(m, outStr);          //��һ���������������ת����һ���ַ���

    TRACE("m=%s\n", outStr);

    //����
    powmod(m,e,n,c);            //����c=m^e mod n

    cotstr(c, outStr);          //��һ���������������ת����һ���ַ���

    TRACE("c=%s\n", outStr);

    //����
    powmod(c,d,n,m);            //����m=c^d mod n

    cotstr(m, outStr);          //��һ���������������ת����һ���ַ���

    TRACE("m=%s\n", outStr);

    int ret = big_to_bytes(256, m, outStr, FALSE);  //��mת��������д��temp

    outStr[8] = '\0';

    TRACE("OutStr=%s\n", outStr);

    mirkill(m);          //�ͷŴ�������
    mirkill(c);

    mirkill(p);
    mirkill(q);

    mirkill(d);
    mirkill(e);

    mirkill(n);
    mirkill(pn);

    mirexit();
*/
}
