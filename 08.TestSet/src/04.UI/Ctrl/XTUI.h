#pragma once

class CXTDC;

class CXTUI
{
public:
	CXTUI();
	~CXTUI();

public:
	CXTDC *xtDC_;
	
	HDC	dc_;		// ����DC
	HDC	compDC_;	// ����ͼ��DC
	HDC	imageDC_;	// ����ͼ��DC

	int x_;			// X����
	int y_;			// Y����
	int cx_;		// ��
	int cy_;		// ��
	int sx_;		// ԭͼX����
	int sy_;		// ԭͼY����
};
