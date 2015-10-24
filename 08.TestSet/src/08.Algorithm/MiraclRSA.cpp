#include "StdAfx.h"
#include "MiraclRSA.h"

#ifdef __cplusplus
extern "C"
{
#endif

    #pragma comment(lib, "../lib/miracl/ms32.lib")

    FILE _iob[3] = {__iob_func()[0], __iob_func()[1], __iob_func()[2]};

#ifdef __cplusplus
}
#endif

MiraclRSA::MiraclRSA(void)
{
    _mip = mirsys(10000,10);    // ��ʼ��һ��1000λ10���ƵĴ���ϵͳ
    _mip->IOBASE = 10;          // ��ԭ����16���Ƹ�Ϊ10����ģʽ
    _p  = mirvar(0);
    _q  = mirvar(0);
    _n  = mirvar(0);
    _pn = mirvar(0);
    _d  = mirvar(0);
    _e  = mirvar(0);

    /*irand(time(NULL)%1000);*/
}

MiraclRSA::~MiraclRSA(void)
{
    mirkill(_p);
    mirkill(_q);
    mirkill(_n);
    mirkill(_pn);
    mirkill(_d);
    mirkill(_e);
    mirexit();
}

int MiraclRSA::GetPublicKey(int num, char *key, int &keyBufLen)
{
    if (num <= 0 || NULL == key || keyBufLen <= num) return -1;

    //irand(time(NULL));    // �������
    //bigdig(num, 10, t);   // ����numλ10���Ƶ������p
    //bigbits(num, t);

    char buff[10000] = "";

    for (int i = 0; i < num; i++)
    {
        buff[i] = '0'+rand()%10;
    }
    
    big t = mirvar(0);

    cinstr(t, buff);

    keyBufLen = big_to_bytes(10000, t, key, FALSE);//��tת��������д��key

#ifdef _DEBUG
    cotstr(t, buff);
    TRACE("get publick key------------\ne=%s strlen=%d\n", buff, strlen(buff));
#endif // _DEBUG

    mirkill(t);

    return 0;
}

int MiraclRSA::GetPrivateKey(int numP, int numQ, char *key, int &keyBufLen, bool save)
{
    if (numP <= 0 || numQ <= 0 || NULL == key) return -1;

    big p  = mirvar(0); // ������p
    big q  = mirvar(0); // ������q

    big n  = mirvar(0); // nģ��
    big pn = mirvar(0); // ŷ������ֵpn = (p - 1)(q - 1)

    big d  = mirvar(0); // d˽Կ
    big e  = mirvar(0); // e��Կcinstr(e, key);

    expb2(numP, p);     // ����2��numP�η�, 2^1024 ~= 1.8 * 10^308

    nxprime(p, p);      // ��һ����2��numP�η��������

    expb2(numQ, q);

    nxprime(q, q);

    multiply(p, q, n);  //n = p * q

    decr(p, 1, p);      //p = p - 1

    decr(q, 1, q);      //q = q - 1

    multiply(p, q, pn); //ŷ������ֵpn = (p - 1)(q - 1)
    
    bytes_to_big(keyBufLen, key, e); // ����,ת���ɴ���

#ifdef _DEBUG
    char buff[10000] = "";
    cotstr(p, buff);
    TRACE("get private key-----------------------\np=%s strlen=%d\n", buff, strlen(buff));
    cotstr(q, buff);
    TRACE("q=%s strlen=%d\n", buff, strlen(buff));
    cotstr(n, buff);
    TRACE("n=%s strlen=%d\n", buff, strlen(buff));
    cotstr(pn, buff);
    TRACE("pn=%s strlen=%d\n", buff, strlen(buff));
    cotstr(e, buff);
    TRACE("e=%s strlen=%d\n", buff, strlen(buff));
#endif // _DEBUG

    xgcd(e, pn, d, d, d); // ����d = e^-1 mod n,����������������չ���Լ����Ҳ������������ģ��

    keyBufLen = big_to_bytes(10000, d, key, FALSE); //��dת��������д��key

    if (save)
    {
        incr(p,  0, _p);
        incr(q,  0, _q);
        incr(n,  0, _n);
        incr(pn, 0, _pn);
        incr(d,  0, _d);
        incr(e,  0, _e);
    }

#ifdef _DEBUG
    cotstr(d, buff);
    TRACE("d=%s strlen=%d\n", buff, strlen(buff));
#endif // _DEBUG

    mirkill(p);
    mirkill(q);

    mirkill(d);
    mirkill(e);

    mirkill(n);
    mirkill(pn);

    return 0;
}

int MiraclRSA::Encode(char *in, int inLen, char *out, int &outLen)
{
    if (NULL == in || inLen <= 0 || NULL == out || outLen <= 0) return -1;

    miracl *mip = mirsys(10000,10);

    big m = mirvar(0); //m����

    big c = mirvar(0); //c����

    bytes_to_big(inLen, in, m); //��8���ַ������ģ�ת���ɴ���

#ifdef _DEBUG
    char buff[10000] = "";
    cotstr(_p, buff);
    TRACE("encode-----------------------\np=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_q, buff);
    TRACE("q=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_n, buff);
    TRACE("n=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_pn, buff);
    TRACE("pn=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_e, buff);
    TRACE("e=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_d, buff);
    TRACE("d=%s strlen=%d\n", buff, strlen(buff));
    cotstr(m, buff);
    TRACE("m=%s\nm=%s strlen=%d\n", in, buff, strlen(buff));
#endif // _DEBUG

    powmod(m, _e, _n, c); //����c=m^e mod n

    outLen = big_to_bytes(10000, c, out, FALSE);//��dת��������д��key

#ifdef _DEBUG
    cotstr(c, buff);
    TRACE("c=%s strlen=%d\n", buff, strlen(buff));
#endif // _DEBUG

    mirkill(m);
    mirkill(c);

    return 0;
}

int MiraclRSA::Decode(char *in, int inLen, char *out, int &outLen)
{
    if (NULL == in || inLen <= 0 || NULL == out || outLen <= 0) return -1;

    miracl *mip = mirsys(10000,10);

    big m = mirvar(0); //m����

    big c = mirvar(0); //c����

    bytes_to_big(inLen, in, c); //��8���ַ������ģ�ת���ɴ���

#ifdef _DEBUG
    char buff[10000] = "";
    cotstr(_p, buff);
    TRACE("decode-----------------------\np=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_q, buff);
    TRACE("q=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_n, buff);
    TRACE("n=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_pn, buff);
    TRACE("pn=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_e, buff);
    TRACE("e=%s strlen=%d\n", buff, strlen(buff));
    cotstr(_d, buff);
    TRACE("d=%s strlen=%d\n", buff, strlen(buff));
    cotstr(c, buff);
    TRACE("c=%s strlen=%d\n", buff, strlen(buff));
#endif // _DEBUG

    powmod(c, _d, _n, m); //����m=c^d mod n

    outLen = big_to_bytes(10000, m, out, FALSE);//��dת��������д��key

    out[outLen] = '\0';

#ifdef _DEBUG
    cotstr(m, buff);
    TRACE("m=%s\nm=%s strlen=%d\n", out, buff, strlen(buff));
#endif // _DEBUG

    mirkill(m);
    mirkill(c);

    return 0;
}
