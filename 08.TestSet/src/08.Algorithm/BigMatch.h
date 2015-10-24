
#pragma once
#include <vector>


typedef std::vector<int> VECTOR_INT; // ��һ��Ԫ��Ϊ������, ��һ��Ԫ��Ϊλ����(С�����λ��)

class BigMatch
{
public:
    BigMatch();
    ~BigMatch();

protected:    
    /**
     *\fn        VECTOR_INT strToVector(const char *num)
     *\brief     ���ַ���ת��������
     *\param[in] const char * num �ַ���
     *\return    VECTOR_INT ����
     */
    VECTOR_INT strToVector(const char *num);

    /**
     *\fn        int vectorToStr(const VECTOR_INT &num, char *result, int resultLen)
     *\brief     ������ת�����ַ���
     *\param[in] const VECTOR_INT & num 
     *\param[in] char * result �ַ�������
     *\param[in] int resultLen �ַ�����������
     *\return    int 0�ɹ�,����ʧ��
     */
    int vectorToStr(const VECTOR_INT &num, char *result, int resultLen);

    /**
     *\fn        VECTOR_INT arrayNumber(const VECTOR_INT &num)
     *\brief     ��������ʵ�ֽ�λ��λ����
     *\param[in] const VECTOR_INT & num ����
     *\return    VECTOR_INT ���������
     */
    VECTOR_INT arrayNumber(const VECTOR_INT &num);
    
    /**
     *\fn        VECTOR_INT add(const VECTOR_INT &num1, const VECTOR_INT &num2)
     *\brief     �������
     *\param[in] const VECTOR_INT & num1 ����1
     *\param[in] const VECTOR_INT & num2 ����2
     *\return    VECTOR_INT ��Ӻ�Ľ������
     */
    VECTOR_INT add(const VECTOR_INT &num1, const VECTOR_INT &num2);

    /**
     *\fn        VECTOR_INT add(const VECTOR_INT &num1, const VECTOR_INT &num2)
     *\brief     �������
     *\param[in] const VECTOR_INT & num1 ����1
     *\param[in] const VECTOR_INT & num2 ����2
     *\return    VECTOR_INT �����Ľ������
     */
    VECTOR_INT sub(const VECTOR_INT &num1, const VECTOR_INT &num2);

    /**
     *\fn        VECTOR_INT mul(const VECTOR_INT &num1, const VECTOR_INT &num2)
     *\brief     �������
     *\param[in] const VECTOR_INT & num1 ����1
     *\param[in] const VECTOR_INT & num2 ����2
     *\return    VECTOR_INT ��˺�Ľ������
     */
    VECTOR_INT mul(const VECTOR_INT &num1, const VECTOR_INT &num2);

    /**
     *\fn        VECTOR_INT div(const VECTOR_INT &num1, const VECTOR_INT &num2)
     *\brief     �������
     *\param[in] const VECTOR_INT & num1 ����1
     *\param[in] const VECTOR_INT & num2 ����2
     *\return    VECTOR_INT �����Ľ������
     */
    VECTOR_INT div(const VECTOR_INT &num1, const VECTOR_INT &num2);

    /**
     *\fn        VECTOR_INT mul(const VECTOR_INT &num1, int num2)
     *\brief     �������
     *\param[in] const VECTOR_INT & num1 ����1
     *\param[in] int num2 ����
     *\return    VECTOR_INT ��˺�Ľ������
     */
    VECTOR_INT mul(const VECTOR_INT &num1, int num2);

public:
    /**
     *\fn        int add(const char *number1, const char *number2, char *result, int resultLen)
     *\brief     �ַ������������
     *\param[in] const char * number1 ����1
     *\param[in] const char * number2 ����2
     *\param[in] char * result ��Ӻ������
     *\param[in] int resultLen ��Ӻ�����ֻ�������
     *\return    int 0�ɹ�,����ʧ��
     */
    int add(const char *number1, const char *number2, char *result, int resultLen);

    /**
     *\fn        int add(const char *number1, const char *number2, char *result, int resultLen)
     *\brief     �ַ������������
     *\param[in] const char * number1 ����1
     *\param[in] const char * number2 ����2
     *\param[in] char * result ����������
     *\param[in] int resultLen ���������ֻ�������
     *\return    int 0�ɹ�,����ʧ��
     */
    int sub(const char *number1, const char *number2, char *result, int resultLen);

    /**
     *\fn        int mul(const char *number1, const char *number2, char *result, int resultLen)
     *\brief     �ַ������������
     *\param[in] const char * number1 ����1
     *\param[in] const char * number2 ����2
     *\param[in] char * result ��˺������
     *\param[in] int resultLen ��˺�����ֻ�������
     *\return    int 0�ɹ�,����ʧ��
     */
    int mul(const char *number1, const char *number2, char *result, int resultLen);

    /**
     *\fn        int div(const char *number1, const char *number2, char *result, int resultLen)
     *\brief     �ַ������������
     *\param[in] const char * number1 ����1
     *\param[in] const char * number2 ����2
     *\param[in] char * result ����������
     *\param[in] int resultLen ���������ֻ�������
     *\return    int 0�ɹ�,����ʧ��
     */
    int div(const char *number1, const char *number2, char *result, int resultLen);

    /**
     *\fn        int mod(const char *number1, const char *number2, char *result, int resultLen)
     *\brief     �ַ���������ȡ��
     *\param[in] const char * number1 ����1
     *\param[in] const char * number2 ����2
     *\param[in] char * result ����������
     *\param[in] int resultLen ���������ֻ�������
     *\return    int 0�ɹ�,����ʧ��
     */
    int mod(const char *number1, const char *number2, char *result, int resultLen);

    /**
     *\fn        bool equ(const char *number1, const char *number2)
     *\brief     �Ƿ����
     *\param[in] const char * number1 ����1
     *\param[in] const char * number2 ����2
     *\return    bool �Ƿ����
     */
    bool equ(const char *number1, const char *number2);

};