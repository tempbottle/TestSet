
#pragma once

#define MLEVEL			4
#define MAX				MLEVEL+1		// ����йؼ��ֵ������Ŀ��Max = m-1��m��B-���Ľ�
#define MIN				MLEVEL/2-1		// �Ǹ�����йؼ��ֵ���С��Ŀ��Min=��m/2��-1
#define RIGHT_SON(x)	(x+1)			// ���ӽڵ��
#define LEFT_SON(x)		(x)				// ���ӽڵ��

typedef struct _tagData
{
	int nKey;
	void *pData;

}DATA, *PDATA;


typedef struct node				// ��㶨����ʡ����ָ��ؼ��ִ���ļ�¼��ָ��
{
	int nKeyNum;				// ����е�ǰӵ�еĹؼ��ֵĸ�����nKeyNum��Max
	DATA Key[MLEVEL];			// �ؼ�������Ϊkey[1..nKeyNum]��key[0]���á�
	struct node *pParent;		// ָ��˫�׽��
	struct node *pSon[MAX];		// ����ָ������Ϊson[0..nKeyNum]

}BTreeNode;



class CBTree
{
public:
	CBTree();
	~CBTree();

protected:
	BTreeNode *m_pRoot;

	bool InsertKey(BTreeNode *pNode, int nKey, void *pData, BTreeNode *pRightSonNode);
	bool DeleteKey(BTreeNode *pNode, int nKey);
	BTreeNode* SelectKey(BTreeNode *pNode, int nKey);

public:
	bool Insert(int nKey, void *pData);
};