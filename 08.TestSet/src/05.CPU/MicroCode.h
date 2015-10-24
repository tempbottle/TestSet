
#pragma once
#include <map>
#include <string>


// �Ĵ���
enum {AX,BX,CX,DX,CS,DS,SS,ES,BP,SP,IP,T1,AT,DRH,DRL,MSW,IR,MEM,    // �����ļĴ���
DRH_DRL,CS_IP,SS_SP,DS_T1,CS_T1,REG_COUNT}; // ��Ҫ��Ϊ�˼�����

// ALU����
enum {ALU_NULL,SEL_IR,ALU_OUTPUT_0,INC,DEC,ADD,SUB,NOT,AND,OR,XOR,SAR,SAL,JA,JAE,JB,JBE,JG,JGE,JL,JLE,JE,JNE,JC,JNC,CMP,JMP};

// ����
enum {IBUS=0, OBUS, ABUS, ATBUS};

// �Ĵ�������
enum {REG_READ=0/*��*/,  REG_NONE_READ=1/*����*/};
enum {REG_WRITE=1/*д*/, REG_NONE_WRITE=0/*��д*/};

// ���߲���
enum {BUS_CE=0/*Ƭѡ*/, BUS_NONE_CE=1/*��ѡȡ*/};
enum {BUS_TO=0/*���������*/, BUS_FROM=1/*�����߶�ȡ*/};

// �ڴ����
enum {MEM_READ=0/*��*/,  MEM_NONE_READ=0/*����*/};
enum {MEM_WRITE=0/*д*/, MEM_NONE_WRITE=0/*��д*/};


typedef struct _tagMICROCODE_ITEM MICROCODE_ITEM;

typedef std::map<std::string, unsigned int>     MAP_STR_UINT;
typedef std::map<unsigned int, std::string>     MAP_UINT_STR;
typedef std::map<unsigned int, MICROCODE_ITEM*> MAP_UINT_MICROCOE;

typedef union _tagREG_OPER_ITEM
{
    struct
    {
        unsigned char byValue;
    };

    struct
    {
        unsigned char bRegRead:1;   // �Ĵ���������(0)
        unsigned char bRegWrite:1;  // �Ĵ���д����(1)
        unsigned char bIBusCE:1;    // �ڲ�����Ƭѡ
        unsigned char bIBusFrom:1;  // �ڲ�����,�����߶�����
        unsigned char bOBusCE:1;    // �ⲿ����Ƭѡ
        unsigned char bOBusFrom:1;  // �ⲿ����,�����߶�����
        unsigned char bABusCE:1;    // ��ַ����Ƭѡ
        unsigned char bABusFrom:1;  // ��ַ����,�����߶�����
    };

    _tagREG_OPER_ITEM()
    {
        bRegRead  = REG_NONE_READ;
        bRegWrite = REG_NONE_WRITE;
        bIBusCE   = BUS_NONE_CE;
        bIBusFrom = BUS_TO;
        bOBusCE   = BUS_NONE_CE;
        bOBusFrom = BUS_TO;
        bABusCE   = BUS_NONE_CE;
        bABusFrom = BUS_TO;
    }

}REG_OPER_ITEM;

typedef union _tagALU_OPER_ITEM
{
    struct
    {
        unsigned char byValue;
    };

    struct
    {
        unsigned char bSelIR:1;     // ѡ��IR�Ĵ���
        unsigned char bCn:1;        // 74181Cn���
        unsigned char bM:1;         // 74181M���
        unsigned char bS0:1;        // 74181S0���
        unsigned char bS1:1;        // 74181S1���
        unsigned char bS2:1;        // 74181S2���
        unsigned char bS3:1;        // 74181S3���
    };

    _tagALU_OPER_ITEM()
    {
        byValue = 0;                // ֻҪ��д��AT�Ĵ����Ϳ�����
    }

}ALU_OPER_ITEM;

typedef union _tagMEM_OPER_ITEM
{
    struct
    {
        unsigned char byValue;
    };

    struct
    {
        unsigned char bRead:1;      // �ڴ������
        unsigned char bWrite:1;     // �ڴ�д����
    };

    _tagMEM_OPER_ITEM()
    {
        bRead = MEM_NONE_READ;
        bWrite = MEM_NONE_WRITE;
    }

}MEM_OPER_ITEM;

typedef union _tagADDR_ITEM
{
    struct
    {
        unsigned char byLow;    // ΢ָ���ַ(��8λ)
        unsigned char byHigh;   // ΢ָ���ַ(��8λ)
    };

    struct
    {
        unsigned short wAddr:12;    // ΢ָ���ַ
    };

    _tagADDR_ITEM()
    {
        byLow = 0;
        byHigh = 0;
    }

}ADDR_ITEM;

typedef struct _tagMICROCODE_ITEM
{
    ADDR_ITEM       Addr;
    ADDR_ITEM       NextAddr;
    REG_OPER_ITEM   Reg[REG_COUNT];
    MEM_OPER_ITEM   Mem;
    ALU_OPER_ITEM   ALU;

}MICROCODE_ITEM;



class CMicroCode
{
public:
    CMicroCode(void);
    ~CMicroCode(void);

protected:
    MAP_UINT_STR m_mapAsm;  // ���

    MAP_STR_UINT m_mapReg;  // �Ĵ���(����->���)
    MAP_STR_UINT m_mapBus;  // ����
    MAP_STR_UINT m_mapALU;  // ALU����

    MAP_UINT_MICROCOE m_mapMicroCode;

    bool ParseMicroCode(const char *pszMicroCode,
        const char *pszAddr,
        const char *pszNextAddr,
        MICROCODE_ITEM &Item);

    bool ParseItem(const char *pszItem, MICROCODE_ITEM &Item);

    bool ParseRegMemOper(const std::string &strLeft, const std::string &strRight, MICROCODE_ITEM &Item);

    bool ParseALUOper(const char *pszOper, MICROCODE_ITEM &Item);

    bool SetBusOper(REG_OPER_ITEM &Reg, int nBusType, bool bLeftIsBus);

    void SetRegOper(REG_OPER_ITEM &Reg, bool bRead);

    void SetMemOper(MEM_OPER_ITEM &Mem, bool bRead);

public:
	bool Init(const char *pszFileName);
    bool LoadMicroCode(const char *pszFileName);
    bool SaveMicroCode(const char *pszPath, const char *pszName);
    MICROCODE_ITEM* GetMicroCode(WORD wAddr);
    MAP_UINT_STR* GetMapAsm(void) { return &m_mapAsm; }
};