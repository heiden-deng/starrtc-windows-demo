// CWhitePanelDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "starrtcdemo.h"
#include "CWhitePanelDlg.h"
#include "afxdialogex.h"


// CWhitePanelDlg 对话框

IMPLEMENT_DYNAMIC(CWhitePanelDlg, CDialogEx)

CWhitePanelDlg::CWhitePanelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WHITE_PANEL, pParent)
{
	m_bGetData = false;
	m_bDraw = false;
	m_bLaserOn = false;
	m_lineColor = 0;
	points.clear();
	m_LaserPen.clear();
	m_strTarget = "test";
	m_DrawLineData.clear();
	m_pWhitePanelCallback = NULL;
}

CWhitePanelDlg::~CWhitePanelDlg()
{
	points.clear();
}

void CWhitePanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWhitePanelDlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CWhitePanelDlg 消息处理程序

BOOL CWhitePanelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ModifyStyle(WS_CAPTION, NULL, SWP_DRAWFRAME);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CWhitePanelDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bGetData)
	{
		points.clear();
		WHITE_PANEL_ACTION actionType = WHITE_PANEL_LASER_PEN;
		if (!m_bLaserOn)
		{
			actionType = WHITE_PANEL_START;
			m_bDraw = true;
			m_StartPoint = point;
			points.push_back(point);
		}
		if (m_pWhitePanelCallback != NULL)
		{
			CRect rect;
			GetClientRect(rect);
			CScreenPoint screenPoint;
			screenPoint.x = point.x / (float)rect.Width();
			screenPoint.y = point.y / (float)rect.Height();
			m_pWhitePanelCallback->actionCallback(actionType, screenPoint, m_lineColor);
		}	
	}
	
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CWhitePanelDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bGetData)
	{
		CClientDC dc(this);
		WHITE_PANEL_ACTION actionType = WHITE_PANEL_MOVE;
		if (m_bLaserOn)
		{
			actionType = WHITE_PANEL_LASER_PEN;
		}
		if (!m_bLaserOn && m_bDraw)
		{
			CPen cPen;//生明画笔
			int r = m_lineColor >> 16;
			int g = (m_lineColor & 0x00ff00) >> 8;
			int b = m_lineColor & 0x0000ff;
			cPen.CreatePen(PS_SOLID, 1, RGB(r, g, b));
			CPen* oldPen = dc.SelectObject(&cPen);
			dc.MoveTo(m_StartPoint);
			dc.LineTo(point);
			dc.SelectObject(oldPen);
			m_StartPoint = point;
			points.push_back(point);
			//Invalidate();
			if (m_pWhitePanelCallback != NULL)
			{
				CRect rect;
				GetClientRect(rect);
				CScreenPoint screenPoint;
				screenPoint.x = point.x / (float)rect.Width();
				screenPoint.y = point.y / (float)rect.Height();
				m_pWhitePanelCallback->actionCallback(actionType, screenPoint, m_lineColor);
			}
		}

		if (m_bLaserOn)
		{
			//dc.Ellipse(point.x-5, point.y-5, point.x + 5, point.y + 5);
			map<string, CPoint>::iterator iter = m_LaserPen.find(m_strTarget);
			if (iter != m_LaserPen.end())
			{
				iter->second = point;
			}
			else
			{
				m_LaserPen[m_strTarget] = point;
			}
			Invalidate();
			if (m_pWhitePanelCallback != NULL)
			{
				CRect rect;
				GetClientRect(rect);
				CScreenPoint screenPoint;
				screenPoint.x = point.x / (float)rect.Width();
				screenPoint.y = point.y / (float)rect.Height();
				m_pWhitePanelCallback->actionCallback(actionType, screenPoint, m_lineColor);
			}
		}
			
		
		
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

void CWhitePanelDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bGetData)
	{
		CClientDC dc(this);
		if (!m_bLaserOn && m_bDraw)
		{
			CPen cPen;//生明画笔
			int r = m_lineColor >> 16;
			int g = (m_lineColor & 0x00ff00) >> 8;
			int b = m_lineColor & 0x0000ff;
			cPen.CreatePen(PS_SOLID, 1, RGB(r, g, b));
			CPen* oldPen = dc.SelectObject(&cPen);
			dc.MoveTo(m_StartPoint);
			dc.LineTo(point);
			dc.SelectObject(oldPen);
			
			m_StartPoint = point;
			points.push_back(point);
			//Invalidate();
			CDrawLineData lineData;
			lineData.lineColor = m_lineColor;
			lineData.points.insert(lineData.points.begin(), points.begin(), points.end());

			map<string, CUserDrawInfo>::iterator iter = m_DrawLineData.find(m_strTarget);
			if (iter != m_DrawLineData.end())
			{
				iter->second.drawData.push_back(lineData);
			}
			else
			{
				CUserDrawInfo userDrawInfo;
				userDrawInfo.drawData.push_back(lineData);
				m_DrawLineData.insert(pair<string, CUserDrawInfo>(m_strTarget, userDrawInfo));
			}
			points.clear();
			m_bDraw = false;
		}
			
		WHITE_PANEL_ACTION actionType = WHITE_PANEL_END;
		if (m_bLaserOn)
		{
			actionType = WHITE_PANEL_LASER_PEN;
		}
		if (m_pWhitePanelCallback != NULL)
		{
			CRect rect;
			GetClientRect(rect);
			CScreenPoint screenPoint;
			screenPoint.x = point.x / (float)rect.Width();
			screenPoint.y = point.y / (float)rect.Height();
			m_pWhitePanelCallback->actionCallback(actionType, screenPoint, m_lineColor);
		}
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}
void CWhitePanelDlg::setWhitePanelCallback(IWhitePanelCallback* pWhitePanelCallback, string strTarget)
{
	m_pWhitePanelCallback = pWhitePanelCallback;
	m_strTarget = strTarget;
}

void CWhitePanelDlg::setAction(string strTarget, vector<CWhitePanelInfo>& info)
{
	CRect rect;
	GetClientRect(rect);
	vector<CWhitePanelInfo>::iterator iterInfo = info.begin();
	for (; iterInfo != info.end(); iterInfo++)
	{
		switch (iterInfo->type)
		{
		case WHITE_PANEL_START:
		{
			CDrawLineData data;
			data.lineColor = iterInfo->lineColor;
			CPoint point;
			point.x = (long)(rect.Width()*iterInfo->point.x);
			point.y = (long)(rect.Height()*iterInfo->point.y);
			data.points.push_back(point);

			map<string, CUserDrawInfo>::iterator iter = m_DrawLineData.find(strTarget);
			if (iter != m_DrawLineData.end())
			{
				iter->second.drawData.push_back(data);
			}
			else
			{
				CUserDrawInfo userDrawInfo;
				userDrawInfo.drawData.push_back(data);
				m_DrawLineData[strTarget] = userDrawInfo;
			}
		}
			break;
		case WHITE_PANEL_MOVE: 
		case WHITE_PANEL_END:
		{
			CPoint point;
			point.x = (long)(rect.Width()*iterInfo->point.x);
			point.y = (long)(rect.Height()*iterInfo->point.y);


			map<string, CUserDrawInfo>::iterator iter = m_DrawLineData.find(strTarget);
			if (iter != m_DrawLineData.end())
			{
				if (iter->second.drawData.size() >= 1)
				{
					list<CDrawLineData>::iterator iterList = iter->second.drawData.end();
					iterList--;
					(*iterList).points.push_back(point);
				}
			}

		}
			break;
		case WHITE_PANEL_REVOKE: 
		{
			revoke(strTarget);
		}	
			break;
		case WHITE_PANEL_CLEAR: 
		{
			clear();
		}
			break;
		case WHITE_PANEL_LASER_PEN:
		{
			CPoint point;
			point.x = (long)(rect.Width()*iterInfo->point.x);
			point.y = (long)(rect.Height()*iterInfo->point.y);
			map<string, CPoint>::iterator iter = m_LaserPen.find(strTarget);
			if (iter != m_LaserPen.end())
			{
				iter->second = point;
			}
			else
			{
				m_LaserPen[strTarget] = point;
			}
		}
			break;
		case WHITE_PANEL_LASER_PEN_END:
		{
			map<string, CPoint>::iterator iter = m_LaserPen.find(strTarget);
			if (iter != m_LaserPen.end())
			{
				m_LaserPen.erase(iter);
			}
		}
			break;
		default:
			break;
		}
	}
	Invalidate();
}
//打开激光笔
void CWhitePanelDlg::laserPenOn()
{
	m_bLaserOn = true;
}
//关闭激光笔
void CWhitePanelDlg::laserPenOff()
{
	m_bLaserOn = false;
	map<string, CPoint>::iterator iter = m_LaserPen.find(m_strTarget);
	if (iter != m_LaserPen.end())
	{
		m_LaserPen.erase(iter);
	}
	Invalidate();
}

void CWhitePanelDlg::isGetData(bool bGetData)
{
	m_bGetData = bGetData;
}

void CWhitePanelDlg::setLineColor(int nColor)
{
	m_lineColor = nColor;
}

void CWhitePanelDlg::revoke(string strTarget)
{
	map<string, CUserDrawInfo>::iterator iter = m_DrawLineData.find(strTarget);
	if (iter != m_DrawLineData.end())
	{
		if (iter->second.drawData.size() >= 1)
		{
			list<CDrawLineData>::iterator iterList = iter->second.drawData.end();
			iterList--;
			iter->second.drawData.erase(iterList);
		}
	}
	Invalidate();
}

void CWhitePanelDlg::clear()
{
	m_DrawLineData.clear();
	Invalidate();
}

void CWhitePanelDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	CRect rect;
	GetClientRect(&rect);
	CDC dcMem;                                                  //用于缓冲作图的内存DC
	CBitmap bmp;                                                 //内存中承载临时图象的位图
	dcMem.CreateCompatibleDC(&dc);               //依附窗口DC创建兼容内存DC
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());//创建兼容位图
	dcMem.SelectObject(&bmp);                          //将位图选择进内存DC
	dcMem.FillSolidRect(rect, dc.GetBkColor());//按原来背景填充客户区，不然会是黑色

	map<string, CUserDrawInfo>::iterator iter = m_DrawLineData.begin();
	for (; iter != m_DrawLineData.end(); iter++)
	{
		list<CDrawLineData>::iterator iterDrawData = iter->second.drawData.begin();
		for (; iterDrawData != iter->second.drawData.end(); iterDrawData++)
		{
			CPoint startPoint;
			CPen cPen;//生明画笔
			int r = iterDrawData->lineColor >> 16;
			int g = (iterDrawData->lineColor & 0x00ff00) >> 8;
			int b = iterDrawData->lineColor & 0x0000ff;
			cPen.CreatePen(PS_SOLID, 1, RGB(r,g,b));
			CPen* oldPen = dcMem.SelectObject(&cPen);
			list<CPoint>::iterator iterPoint = iterDrawData->points.begin();
			if (iterPoint != iterDrawData->points.end())
			{
				startPoint = *iterPoint;
				++iterPoint;
			}
			for (; iterPoint != iterDrawData->points.end(); iterPoint++)
			{
				dcMem.MoveTo(startPoint);
				dcMem.LineTo(*iterPoint);
				startPoint = *iterPoint;
			}
			dcMem.SelectObject(oldPen);
		}
	}

	map<string, CPoint>::iterator iterLaserPen = m_LaserPen.begin();
	for (; iterLaserPen != m_LaserPen.end(); iterLaserPen++)
	{
		CBrush brush;//创建画刷
		brush.CreateSolidBrush(RGB(0, 0, 0));
		CBrush* oldBrush = dcMem.SelectObject(&brush);
		dcMem.Ellipse(iterLaserPen->second.x - 3, iterLaserPen->second.y - 3, iterLaserPen->second.x + 3, iterLaserPen->second.y + 3);
		dcMem.SelectObject(&oldBrush);
	}

	if (m_bGetData)
	{
		CPoint startPoint;
		list<CPoint>::iterator iterPoint = points.begin();
		if (iterPoint != points.end())
		{
			startPoint = *iterPoint;
			++iterPoint;
		}
		CPen cPen;//生明画笔
		int r = m_lineColor >> 16;
		int g = (m_lineColor & 0x00ff00) >> 8;
		int b = m_lineColor & 0x0000ff;
		cPen.CreatePen(PS_SOLID, 1, RGB(r, g, b));
		CPen* oldPen = dcMem.SelectObject(&cPen);
		for (; iterPoint != points.end(); iterPoint++)
		{
			dcMem.MoveTo(startPoint);
			dcMem.LineTo(*iterPoint);
			startPoint = *iterPoint;
		}
		dcMem.SelectObject(oldPen);
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);//将内存DC上的图象拷贝到前台
	dcMem.DeleteDC();                                       //删除DC
	bmp.DeleteObject();
}
