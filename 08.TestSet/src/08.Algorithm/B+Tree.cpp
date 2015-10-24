
#include "stdafx.h"
#include "B+Tree.h"



CBTree::CBTree()
{
	m_pRoot = new BTreeNode;
	memset(m_pRoot, 0, sizeof(BTreeNode));
}

CBTree::~CBTree()
{
	delete m_pRoot;
}

bool CBTree::InsertKey(BTreeNode *pNode, int nKey, void *pData, BTreeNode *pRightSonNode)
{
	if (NULL == pNode) return false;

	// Ҷ�ӽڵ�����ӽڵ���������Ľڵ�
	if ((NULL == pNode->pSon[0]) || (NULL != pRightSonNode))
	{
		// �ƶ�Key
		int i = pNode->nKeyNum - 1;
		for (; i >= 0; i--)
		{
			// ��������ͬ��Key
			if (nKey < pNode->Key[i].nKey)
			{
				pNode->Key[i+1] = pNode->Key[i];
				pNode->pSon[RIGHT_SON(i+1)] = pNode->pSon[i+1];
			}
			else
			{
				pNode->Key[i+1].nKey = nKey;
				pNode->Key[i+1].pData = pData;
				pNode->pSon[RIGHT_SON(i+1)] = pRightSonNode;
				pNode->nKeyNum++;
				break;
			}
		}

		if (-1 == i)
		{
			pNode->Key[0].nKey = nKey;
			pNode->Key[0].pData = pData;
			pNode->pSon[RIGHT_SON(0)] = pRightSonNode;
			pNode->nKeyNum++;
		}
	}
	else
	{
		int i = pNode->nKeyNum - 1;
		for (; i >= 0; i--)
		{
			// ��������ͬ��Key
			if (nKey > pNode->Key[i].nKey)
			{
				// �ݹ���룬�ҽڵ�
				this->InsertKey(pNode->pSon[RIGHT_SON(i)], nKey, pData, NULL);
				break;
			}
		}

		if (-1 == i)
		{
			// �ݹ���룬��ڵ�
			this->InsertKey(pNode->pSon[LEFT_SON(0)], nKey, pData, NULL);
		}
	}

	// ��ʼ����
	if (pNode->nKeyNum >= MLEVEL)
	{
		int nCount = pNode->nKeyNum;			// ���ѽڵ�Ĺؼ�����
		int nIndex = nCount / 2;				// ���ѽڵ��µĹؼ�����
		int i = nIndex + 1;						// ���ѽڵ����
		int j = 0;								// �½ڵ����
		
		BTreeNode *pNewNode = new BTreeNode;
		memset(pNewNode, 0, sizeof(BTreeNode));

		pNewNode->nKeyNum = nCount - nIndex - 1;// �½ڵ�Ĺؼ�����
		pNode->nKeyNum = nIndex;				// ���ѽڵ��µĹؼ�����

		// �ӷ��ѽڵ��ƶ��ؼ��ֺ��ӽڵ�ָ�뵽�½ڵ�
		for (int i = nIndex + 1;  i < nCount; i++,j++)
		{
			pNewNode->Key[j] = pNode->Key[i];

			// �����ӽڵ�ĸ�ָ��
			if (NULL != pNode->pSon[LEFT_SON(i)])
			{
				pNewNode->pSon[LEFT_SON(j)] = pNode->pSon[LEFT_SON(i)];
				pNewNode->pSon[LEFT_SON(j)]->pParent = pNewNode;
			}

			if ((i == nCount-1) && (NULL != pNode->pSon[RIGHT_SON(i)]))	// ���һ���ڵ�
			{
				// �����ӽڵ�ĸ�ָ��
				pNewNode->pSon[RIGHT_SON(j)] = pNode->pSon[RIGHT_SON(i)];
				pNewNode->pSon[RIGHT_SON(j)]->pParent = pNewNode;
			}
		}
		
		if (NULL == pNode->pParent)				// ��һ���ӽڵ�
		{
			m_pRoot = new BTreeNode;
			memset(m_pRoot, 0, sizeof(BTreeNode));

			m_pRoot->Key[0].nKey = pNode->Key[nIndex].nKey;
			m_pRoot->Key[0].pData = pNode->Key[nIndex].pData;
			m_pRoot->pSon[LEFT_SON(0)] = pNode;
			m_pRoot->pSon[RIGHT_SON(0)] = pNewNode;
			m_pRoot->nKeyNum = 1;

			pNode->pParent = m_pRoot;
			pNewNode->pParent = m_pRoot;
		}
		else
		{
			pNewNode->pParent = pNode->pParent;

			// �ݹ����
			this->InsertKey(pNode->pParent, pNode->Key[nIndex].nKey, pNode->Key[nIndex].pData, pNewNode);
		}
	}

	return true;
}

bool CBTree::DeleteKey(BTreeNode *pNode, int nKey)
{
	return false;
}

BTreeNode* CBTree::SelectKey(BTreeNode *pNode, int nKey)
{
	return false;
}

bool CBTree::Insert(int nKey, void *pData)
{
	return InsertKey(m_pRoot, nKey, pData, NULL);
}