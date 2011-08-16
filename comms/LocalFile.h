#pragma once

#include "clib.h"
#include "table.h"

namespace Dali
{
__interface ILocalFile;

typedef boost::signal<void(ILocalFile *)> localfile_signal_type;
typedef localfile_signal_type::slot_type localfile_slot_type;

__interface ILocalFile : public IUnknown
{
	const TCHAR *GetID() const;
	int GetType() const;
	const TCHAR *GetSource() const;
	const TCHAR *GetDestination() const;

	boost::signals::connection on_refresh_connect(const localfile_slot_type& s);
};

typedef StlLinked<ILocalFile> ILocalFileAdapt;
typedef std::vector<ILocalFileAdapt> ILocalFileVector;
typedef std::map<std::_tstring, ILocalFileAdapt> ILocalFileMap;
}
