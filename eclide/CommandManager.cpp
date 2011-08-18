#include "StdAfx.h"

#include "CommandManager.h"
#include "ImageHelper.h"

//  ===========================================================================
void GetImageFromList(HDC dc, CImageList *lstImages, CBitmap* destBitmap)
{    
	CImageList tmpList = lstImages->Duplicate();

	IMAGEINFO lastImage;
	tmpList.GetImageInfo(0,&lastImage);

	CDC dcMem; 
	
	dcMem.CreateCompatibleDC(dc); 

	CRect rect (lastImage.rcImage);

	destBitmap->CreateCompatibleBitmap(dc, rect.Width () * tmpList.GetImageCount(), rect.Height ());

	CBitmap pBmpOld = dcMem.SelectBitmap(*destBitmap);

	for (int i=0;i < tmpList.GetImageCount();i++)
	{
		tmpList.Draw(dcMem, i, i * rect.Width (), 0, ILD_NORMAL);
	}

	dcMem.SelectBitmap(pBmpOld);
}
//  ===========================================================================

CCommandManager::CCommandManager() 
{
}

CCommandManager::~CCommandManager()
{
}

HICON CCommandManager::SetCommandIcon(int command, ATL::_U_STRINGorID icon)
{
	return m_commandList[command].LoadIcon(icon);
}

void  CCommandManager::UpdateIcons(CMDICommandBarCtrl & cmdBar) const
{
	for(CommandIconMap::const_iterator itr = m_commandList.begin(); itr != m_commandList.end(); ++itr)
	{
		if (cmdBar.ReplaceIcon(itr->second, itr->first) == FALSE)
		{
			cmdBar.AddIcon(itr->second, itr->first);
		}
	}
}

void CCommandManager::UpdateIcons(CToolBarCtrl & toolBar) const
{
	int size = 24;
	CImageList imgList, imgList_ht, imgList_gr;
	imgList.Create(size, size, ILC_COLOR32|ILC_MASK, 4, 12);
	imgList_gr.Create(size, size, ILC_COLOR32|ILC_MASK, 4, 12);
	imgList_ht.Create(size, size, ILC_COLOR32|ILC_MASK, 4, 12);
	for(int i = 0; i < toolBar.GetButtonCount(); ++i)
	{
		TBBUTTON button = { 0 };
		toolBar.GetButton(i, &button);
		CommandIconMap::const_iterator item = m_commandList.find(button.idCommand);
		if (item == m_commandList.end())
		{
			ATLASSERT(!"Missing Icon");
			return;
		}
		imgList.AddIcon(item->second);

		{
			imgList_gr.Add(ToGrayScale(imgList.GetIcon(i)), CLR_NONE);
		}
		{
			imgList_ht.AddIcon(imgList.GetIcon(i, ILD_FOCUS));
		}
	}
	toolBar.SetImageList(imgList);
	toolBar.SetDisabledImageList(imgList_gr);
	toolBar.SetHotImageList(imgList_ht);
}

