#pragma once

#include "TreeNode.h"
#include "Repository.h"
#include "Workspace.h"

class __declspec(uuid("4DD48532-FDEE-44ae-A3B1-AACFD1F68457")) CTreePairNode : public CTreeNode
{
public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(CTreePairNode)
	END_CUNKNOWN(CTreeNode)

	virtual bool hasLHS() = 0;
	virtual bool hasRHS() = 0;
	virtual int GetDispImageRHS() = 0;
	virtual int GetDispSelectedImageRHS() = 0;
	virtual ATTRSTATE GetState() = 0;
	virtual ATTRSTATE GetStateRHS() = 0;
};
//  ===========================================================================
class __declspec(uuid("1F4352F8-8BA7-4263-8263-BCD2664B42BB")) CWorkspacePairNode : public CTreePairNode
{
protected:
	std::_tstring m_id;
	std::_tstring m_label;

public:
	CComPtr<IWorkspace> m_lhs;
	CComPtr<IWorkspace> m_rhs;

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(CWorkspacePairNode)
		IMPLEMENT_INTERFACE(CTreePairNode)
	END_CUNKNOWN(CTreePairNode)

	CWorkspacePairNode(IWorkspace * lhs, IWorkspace * rhs);
	bool hasLHS();
	bool hasRHS();
	const TCHAR * GetID();
	const TCHAR * GetDispText();
	int GetDispChildren();
	int GetDispImage(const IWorkspace * ws, const IWorkspace * otherWs);
	int GetDispImage();
	int GetDispImageRHS();
	int GetDispSelectedImage(const IWorkspace * ws, const IWorkspace * otherWs);
	int GetDispSelectedImage();
	int GetDispSelectedImageRHS();

	ATTRSTATE GetState();
	ATTRSTATE GetStateRHS();
};
//  ===========================================================================
class __declspec(uuid("F5A60843-CBBC-44c7-A923-CF4FE87C71F7")) CModulePairNode : public CTreePairNode
{
protected:
	std::_tstring m_id;
	std::_tstring m_label;

public:
	CComPtr<IModule> m_lhs;
	CComPtr<IModule> m_rhs;

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(CModulePairNode)
		IMPLEMENT_INTERFACE(CTreePairNode)
	END_CUNKNOWN(CTreePairNode)

	CModulePairNode(IModule * lhs, IModule * rhs);
	bool hasLHS();
	bool hasRHS();
	const TCHAR * GetID();
	const TCHAR * GetDispText();
	int GetDispChildren();
	int GetDispImage(const IModule * mod, const IModule * otherMod);
	int GetDispImage();
	int GetDispImageRHS();
	int GetDispSelectedImage(const IModule * mod, const IModule * otherMod);
	int GetDispSelectedImage();
	int GetDispSelectedImageRHS();

	ATTRSTATE GetState();
	ATTRSTATE GetStateRHS();
};
//  ===========================================================================
class __declspec(uuid("A64040C4-1356-44b1-9407-E70BF89D545B")) CAttributePairNode : public CTreePairNode
{
protected:
	std::_tstring m_id;
	std::_tstring m_label;

public:
	CComPtr<IAttribute> m_lhs;
	CComPtr<IAttribute> m_rhs;

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(CAttributePairNode)
		IMPLEMENT_INTERFACE(CTreePairNode)
	END_CUNKNOWN(CTreePairNode)

	CAttributePairNode(IAttribute * lhs, IAttribute * rhs);
	bool hasLHS();
	bool hasRHS();
	const TCHAR * GetID();
	const TCHAR * GetDispText();
	int GetDispChildren();
	int GetDispImage(const IAttribute * attr, const IAttribute * otherAttr);
	int GetDispImage();
	int GetDispImageRHS();
	int GetDispSelectedImage(const IAttribute * attr, const IAttribute * otherAttr);
	int GetDispSelectedImage();
	int GetDispSelectedImageRHS();

	ATTRSTATE GetState();
	ATTRSTATE GetStateRHS();
};
//  ===========================================================================
class __declspec(uuid("82C81439-CE57-4166-865E-9E89E8C012C3")) CAttributeHistoryPairNode : public CTreePairNode
{
protected:
	std::_tstring m_id;
	std::_tstring m_label;

public:
	CComPtr<IAttributeHistory> m_lhs;
	CComPtr<IAttribute> m_rhs;

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(CAttributeHistoryPairNode)
		IMPLEMENT_INTERFACE(CTreePairNode)
	END_CUNKNOWN(CTreePairNode)

	CAttributeHistoryPairNode(IAttributeHistory * lhs, IAttribute * rhs);
	bool hasLHS();
	bool hasRHS();
	const TCHAR * GetID();
	const TCHAR * GetDispText();
	int GetDispChildren();
	int GetDispImage();
	int GetDispImageRHS();
	int GetDispSelectedImage();
	int GetDispSelectedImageRHS();

	ATTRSTATE GetState();
	ATTRSTATE GetStateRHS();
};
//  ===========================================================================
