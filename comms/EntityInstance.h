#pragma once

#include "comms.h"
#include "recursivemutex.h"

namespace Analysis
{
__interface IWebPage;
__interface IEntity;
__interface __declspec(uuid("dbc2b210-70ab-11db-9690-00e08161165f")) IEntityInstance : public clib::ILockableUnknown
{
	const TCHAR * GetID() const;
	IEntity * GetEntity() const;
//	IHTMLDOMTextNode * GetDOMTextNode() const;
	int GetPos() const;
	int GetX() const;
	void SetX(int x);
	int GetY() const;
	void SetY(int y);

	void Init();
	bool Inited() const;

	void RestoreHTML();
	void Highlight();
	void SetFocus();
	const TCHAR * GetContext();
};

typedef StlLinked<IEntityInstance> IEntityInstanceAdapt;
typedef std::vector<IEntityInstanceAdapt> IEntityInstanceVector;

IEntityInstance * CreateIEntityInstance(const IWebPage * webPage, const IEntity * entity, const IMarkupPointer * textNode, const std::wstring & context, int docID, int pos);
}
