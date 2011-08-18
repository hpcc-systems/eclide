#pragma once

namespace Dfu
{
__interface IFile;
typedef boost::signal<void(IFile *)> refresh_signal_type;
typedef refresh_signal_type::slot_type refresh_slot_type;

__interface IFile : public IUnknown
{
	const TCHAR * GetLabel() const;
	const TCHAR * GetLeaf() const;
	bool IsDir() const;
	IFile * GetParent();
	boost::signals::connection on_refresh_connect(const refresh_slot_type& s);
	bool operator < (const IFile & r) const;
};
typedef StlLinked<IFile> IFileAdapt;
typedef std::vector<IFileAdapt> IFileVector;
typedef CUnknownT<IFileVector> IFileVectorPtr;
typedef CComPtr<IFileVectorPtr> IFileVectorAdapt;

}