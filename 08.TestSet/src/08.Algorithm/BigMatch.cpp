
#include "stdafx.h"
#include "BigMatch.h"

enum
{
    BIG_MATCH_SIG,
    BIG_MATCH_POW,
    BIG_MATCH_HEAD,
};

BigMatch::BigMatch()
{

}

BigMatch::~BigMatch()
{

}

/**
 *\fn        VECTOR_INT strToVector(const char *num)
 *\brief     ���ַ���ת��������
 *\param[in] const char * num �ַ���
 *\return    VECTOR_INT ����
 */
VECTOR_INT BigMatch::strToVector(const char *num)
{
    VECTOR_INT ret;

    if (NULL == num) return ret;

    int temp = 0;
    int numLen = strlen(num);

    ret.push_back(num[0] == '-'); // ������
    ret.push_back(0); // λ����

    for (int i = ret[BIG_MATCH_SIG]; i < numLen; i++)
    {
        if ('.' == num[i])
        {
            ret[BIG_MATCH_POW] = i - numLen + 1;
            continue;
        }

        temp = num[i] - '0';

        if (temp < 0 || temp > 9) break;

        ret.push_back(temp);
    }

    return ret;
}

/**
 *\fn        int vectorToStr(const VECTOR_INT &num, char *result, int resultLen)
 *\brief     ������ת�����ַ���
 *\param[in] const VECTOR_INT & num 
 *\param[in] char * result �ַ�������
 *\param[in] int resultLen �ַ�����������
 *\return    int 0�ɹ�,����ʧ��
 */
int BigMatch::vectorToStr(const VECTOR_INT &num, char *result, int resultLen)
{
    if (NULL == result || resultLen < num.size()) return -1;

    char temp[10] = "";
    int len = num.size();

    strcpy(result, (num[BIG_MATCH_SIG] ? "-" : ""));

    for (int i = BIG_MATCH_HEAD; i < len; i++)
    {
        if (num[BIG_MATCH_POW] == (i - len))
        {
            strcat(result, ".");
        }

        itoa(num[i], temp, sizeof(temp));

        strcat(result, temp);
    }

    for (int i = strlen(result) - 1; i >= 0; i--)
    {
        if (result[i] == '0')
        {
            result[i] = '\0';
        }
        else
        {
            break;
        }
    }

    return 0;
}

/**
 *\fn        VECTOR_INT arrayNumber(const VECTOR_INT &num)
 *\brief     ��������ʵ�ֽ�λ��λ����
 *\param[in] const VECTOR_INT & num ����
 *\return    VECTOR_INT ���������
 */
VECTOR_INT BigMatch::arrayNumber(const VECTOR_INT &num)
{
    VECTOR_INT ret = num;

    for (int i = ret.size() - 1; i >= BIG_MATCH_HEAD; i--)
    {
        if (ret[i] >= 10)
        {
            if (i == BIG_MATCH_HEAD) // ���λ,����10��ǰ��λ,�½����ڵ�
            {
                VECTOR_INT::iterator iter = ret.begin();
                iter += BIG_MATCH_HEAD;
                ret.insert(iter, 0);
            }
            
            ret[i-1] += ret[i] / 10; // ��ǰ��λ
            ret[i] %= 10;
        }
    }

    return ret;
}

/**
 *\fn        VECTOR_INT add(const VECTOR_INT &num1, const VECTOR_INT &num2)
 *\brief     �������
 *\param[in] const VECTOR_INT & num1 ����1
 *\param[in] const VECTOR_INT & num2 ����2
 *\return    VECTOR_INT ��Ӻ�Ľ������
 */
VECTOR_INT BigMatch::add(const VECTOR_INT &num1, const VECTOR_INT &num2)
{
    VECTOR_INT n1 = num1;
    VECTOR_INT n2 = num2;

    int move = (n1[BIG_MATCH_POW] != n2[BIG_MATCH_POW]) ? (n1[BIG_MATCH_POW] - n2[BIG_MATCH_POW]) : 0;

    if (move > 0)
    {
        n1[BIG_MATCH_POW] = n2[BIG_MATCH_POW];

        for (int i = 0; i < move; i++)
        {
            n1.push_back(0);
        }
    }
    else if (move < 0)
    {
        n2[BIG_MATCH_POW] = n1[BIG_MATCH_POW];

        for (int i = 0; i > move; i--)
        {
            n2.push_back(0);
        }

        move = -move;
    }

    int n1Len = n1.size();
    int n2Len = n2.size();
    int subLen = n1Len - n2Len;

    if (subLen < 0)
    {
        VECTOR_INT temp;
        temp = n1;
        n1 = n2;
        n2 = temp;

        n1Len = n1.size();
        n2Len = n2.size();
        subLen = n1Len - n2Len;
    }
    
    VECTOR_INT ret;
    ret.push_back(n1[BIG_MATCH_SIG]);
    ret.push_back(n1[BIG_MATCH_POW]);

    for (int i = BIG_MATCH_HEAD; i < n1Len; i++)
    {
        if ((i - subLen) > BIG_MATCH_POW)
        {
            // TRACE("n1[%d]=%d n2[%d]=%d\n", i, n1[i], i - subLen, n2[i - subLen]); 

            ret.push_back(n1[i] + n2[i - subLen]);
        }
        else
        {
            ret.push_back(n1[i]);
        }
    }
    
    return arrayNumber(ret);
}

/**
 *\fn        VECTOR_INT add(const VECTOR_INT &num1, const VECTOR_INT &num2)
 *\brief     �������
 *\param[in] const VECTOR_INT & num1 ����1
 *\param[in] const VECTOR_INT & num2 ����2
 *\return    VECTOR_INT �����Ľ������
 */
VECTOR_INT BigMatch::sub(const VECTOR_INT &num1, const VECTOR_INT &num2)
{
    VECTOR_INT n1 = num1;
    VECTOR_INT n2 = num2;

    int move = (n1[BIG_MATCH_POW] != n2[BIG_MATCH_POW]) ? (n1[BIG_MATCH_POW] - n2[BIG_MATCH_POW]) : 0;

    if (move > 0)
    {
        n1[BIG_MATCH_POW] = n2[BIG_MATCH_POW];

        for (int i = 0; i < move; i++)
        {
            n1.push_back(0);
        }
    }
    else if (move < 0)
    {
        n2[BIG_MATCH_POW] = n1[BIG_MATCH_POW];

        for (int i = 0; i > move; i--)
        {
            n2.push_back(0);
        }

        move = -move;
    }

    int n1Len = n1.size();
    int n2Len = n2.size();
    int subLen = n1Len - n2Len;
    bool sig = false; // ����

    if (subLen < 0)
    {
        VECTOR_INT temp;
        temp = n1;
        n1 = n2;
        n2 = temp;

        n1Len = n1.size();
        n2Len = n2.size();
        subLen = n1Len - n2Len;
        sig = true; // ����
    }

    VECTOR_INT ret;
    ret.push_back(sig);
    ret.push_back(n1[BIG_MATCH_POW]);

    for (int i = n1Len - 1; i >= BIG_MATCH_HEAD; i--)
    {
        if ((i - subLen) >= BIG_MATCH_HEAD)
        {
            // TRACE("n1[%d]=%d n2[%d]=%d -> ", i, n1[i], i - subLen, n2[i - subLen]); 

            if ((n1[i] - n2[i - subLen]) < 0)
            {
                if ((i-1) >= 1) n1[i-1]--;
                n1[i] += 10;
            }

            // TRACE("n1[%d]=%d n2[%d]=%d %d\n ", i, n1[i], i - subLen, n2[i - subLen], n1[i] - n2[i - subLen]);

            ret.push_back(n1[i] - n2[i - subLen]);
        }
        else
        {
            ret.push_back(n1[i]);
        }
    }

    for (int i = 0; i < (n1Len - BIG_MATCH_HEAD)  / 2; i++)
    {
        subLen = ret[i + BIG_MATCH_HEAD];
        ret[i + BIG_MATCH_HEAD] = ret[n1Len - i - 1];
        ret[n1Len - i - 1] = subLen;
    }
    
    return arrayNumber(ret);
}

/**
 *\fn        VECTOR_INT mul(const VECTOR_INT &num1, const VECTOR_INT &num2)
 *\brief     �������
 *\param[in] const VECTOR_INT & num1 ����1
 *\param[in] const VECTOR_INT & num2 ����2
 *\return    VECTOR_INT ��˺�Ľ������
 */
VECTOR_INT BigMatch::mul(const VECTOR_INT &num1, const VECTOR_INT &num2)
{
    VECTOR_INT n1 = num1;
    VECTOR_INT n2 = num2;

    if (n1[BIG_MATCH_SIG] == n2[BIG_MATCH_SIG])
    {
        if (n1[BIG_MATCH_SIG])
        {
            n1[BIG_MATCH_SIG] = FALSE;
            n2[BIG_MATCH_SIG] = FALSE;
        }
    }
    else
    {
        n1[BIG_MATCH_SIG] = TRUE;
        n2[BIG_MATCH_SIG] = FALSE;
    }

    VECTOR_INT temp;
    VECTOR_INT ret;
    ret.push_back(n1[BIG_MATCH_SIG]);
    ret.push_back(0);

    for (int i = num2.size() - 1; i >= BIG_MATCH_HEAD; i--)
    {
        temp = mul(n1, n2[i]);
        temp[BIG_MATCH_POW] += (n2.size() - 1 - i);
        ret = add(ret, temp);
    }
    
    ret[BIG_MATCH_POW] = n1[BIG_MATCH_POW] + n2[BIG_MATCH_POW];

    return arrayNumber(ret);
}

/**
 *\fn        VECTOR_INT div(const VECTOR_INT &num1, const VECTOR_INT &num2)
 *\brief     �������
 *\param[in] const VECTOR_INT & num1 ����1
 *\param[in] const VECTOR_INT & num2 ����2
 *\return    VECTOR_INT �����Ľ������
 */
VECTOR_INT BigMatch::div(const VECTOR_INT &num1, const VECTOR_INT &num2)
{
    VECTOR_INT temp;
    VECTOR_INT ret;
    ret.push_back(num1[BIG_MATCH_SIG]);
    ret.push_back(num1[BIG_MATCH_POW]);

    for (int i = BIG_MATCH_HEAD; i < num2.size(); i++)
    {
        temp = mul(num1, num2[i]);
        temp[BIG_MATCH_POW]++;
        ret = add(ret, temp);
    }

    return arrayNumber(ret);
}

/**
 *\fn        VECTOR_INT mul(const VECTOR_INT &num1, int num2)
 *\brief     �������
 *\param[in] const VECTOR_INT & num1 ����1
 *\param[in] int num2 ����
 *\return    VECTOR_INT ��˺�Ľ������
 */
VECTOR_INT BigMatch::mul(const VECTOR_INT &num1, int num2)
{
    VECTOR_INT ret = num1;
    ret[BIG_MATCH_SIG] = FALSE;

    for (int i = BIG_MATCH_HEAD; i < num1.size(); i++)
    {
        ret[i] *= num2;
    }

    return arrayNumber(ret);
}

/**
 *\fn        bool equ(const char *number1, const char *number2)
 *\brief     �Ƿ����
 *\param[in] const char * number1 ����1
 *\param[in] const char * number2 ����2
 *\return    bool �Ƿ����
 */
bool BigMatch::equ(const char *number1, const char *number2)
{
    return false;
}

/**
 *\fn        int add(const char *number1, const char *number2, char *result, int resultLen)
 *\brief     �ַ������������
 *\param[in] const char * number1 ����1
 *\param[in] const char * number2 ����2
 *\param[in] char * result ��Ӻ������
 *\param[in] int resultLen ��Ӻ�����ֻ�������
 *\return    int 0�ɹ�,����ʧ��
 */
int BigMatch::add(const char *number1, const char *number2, char *result, int resultLen)
{
    if (NULL == number1 || NULL == number2 || NULL == result || resultLen <= strlen(number1)) return -1;

    VECTOR_INT num1 = strToVector(number1);
    VECTOR_INT num2 = strToVector(number2);
    VECTOR_INT num3;

    if (num1[BIG_MATCH_SIG] == num2[BIG_MATCH_SIG])
    {
        num3 = add(num1, num2);
    }
    else if(num1[BIG_MATCH_SIG] < num2[BIG_MATCH_SIG])
    {
        num2[BIG_MATCH_SIG] = FALSE;
        num3 = sub(num1, num2);
    }
    else
    {
        num1[BIG_MATCH_SIG] = FALSE;
        num3 = sub(num2, num1);
    }

    return vectorToStr(num3, result, resultLen);
}

/**
 *\fn        int add(const char *number1, const char *number2, char *result, int resultLen)
 *\brief     �ַ������������
 *\param[in] const char * number1 ����1
 *\param[in] const char * number2 ����2
 *\param[in] char * result ����������
 *\param[in] int resultLen ���������ֻ�������
 *\return    int 0�ɹ�,����ʧ��
 */
int BigMatch::sub(const char *number1, const char *number2, char *result, int resultLen)
{
    if (NULL == number1 || NULL == number2 || NULL == result || resultLen <= strlen(number1)) return -1;

    VECTOR_INT num1 = strToVector(number1);
    VECTOR_INT num2 = strToVector(number2);
    VECTOR_INT num3;

    if (num1[BIG_MATCH_SIG] == num2[BIG_MATCH_SIG])
    {
        if (!num1[BIG_MATCH_SIG])
        {
            num3 = sub(num1, num2);
        }
        else
        {
            num2[BIG_MATCH_SIG] = FALSE;
            num3 = sub(num2, num1);
        }
    }
    else
    {
        num2[BIG_MATCH_SIG] = (num1[BIG_MATCH_SIG] > num2[BIG_MATCH_SIG]);
        num3 = add(num1, num2);
    }

    return vectorToStr(num3, result, resultLen);
}

/**
 *\fn        int mul(const char *number1, const char *number2, char *result, int resultLen)
 *\brief     �ַ������������
 *\param[in] const char * number1 ����1
 *\param[in] const char * number2 ����2
 *\param[in] char * result ��˺������
 *\param[in] int resultLen ��˺�����ֻ�������
 *\return    int 0�ɹ�,����ʧ��
 */
int BigMatch::mul(const char *number1, const char *number2, char *result, int resultLen)
{
    if (NULL == number1 || NULL == number2 || NULL == result || resultLen <= (strlen(number1) + strlen(number2))) return -1;

    VECTOR_INT num1 = strToVector(number1);
    VECTOR_INT num2 = strToVector(number2);
    VECTOR_INT num3 = mul(num1, num2);
    return vectorToStr(num3, result, resultLen);
}

/**
 *\fn        int div(const char *number1, const char *number2, char *result, int resultLen)
 *\brief     �ַ������������
 *\param[in] const char * number1 ����1
 *\param[in] const char * number2 ����2
 *\param[in] char * result ����������
 *\param[in] int resultLen ���������ֻ�������
 *\return    int 0�ɹ�,����ʧ��
 */
int BigMatch::div(const char *number1, const char *number2, char *result, int resultLen)
{
    if (NULL == number1 || NULL == number2 || NULL == result || resultLen <= strlen(number1)) return -1;

    VECTOR_INT num1 = strToVector(number1);
    VECTOR_INT num2 = strToVector(number2);
    VECTOR_INT num3;
    return vectorToStr(num3, result, resultLen);
}

/**
 *\fn        int mod(const char *number1, const char *number2, char *result, int resultLen)
 *\brief     �ַ���������ȡ��
 *\param[in] const char * number1 ����1
 *\param[in] const char * number2 ����2
 *\param[in] char * result ����������
 *\param[in] int resultLen ���������ֻ�������
 *\return    int 0�ɹ�,����ʧ��
 */
int BigMatch::mod(const char *number1, const char *number2, char *result, int resultLen)
{
    VECTOR_INT num1 = strToVector(number1);
    VECTOR_INT num2 = strToVector(number2);
    VECTOR_INT num3;
    return vectorToStr(num3, result, resultLen);
}