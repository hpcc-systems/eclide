#pragma once

#include "clib.h"
#include "table.h"
#include "GraphContainer.h"

namespace Dali
{
__interface IGraph;

typedef boost::signal<void(IGraph *)> graph_signal_type;
typedef graph_signal_type::slot_type graph_slot_type;

__interface IGraph : public IUnknown
{
	const TCHAR *GetID() const;
	int GetSequence() const;
	const TCHAR *GetName() const;
	const TCHAR *GetLabel() const;
	const TCHAR *GetType() const;
	const TCHAR *GetGraph();
	WUVisualState GetState() const;
	const TCHAR *GetStateString() const;
	bool IsRunning() const;
	bool IsLoaded() const;
	bool IsComplete() const;
	bool Refresh();

	boost::signals::connection on_refresh_connect(const graph_slot_type& s);
};

typedef StlLinked<IGraph> IGraphAdapt;
typedef std::vector<IGraphAdapt> IGraphVector;
typedef std::map<std::_tstring, IGraphAdapt> IGraphMap;
}
