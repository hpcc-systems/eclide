#include "StdAfx.h"

#include "GraphContainerImpl.h"

//  ===========================================================================
// {8003B0E5-C521-4db2-81E4-112AE2B25E0F}
//DEFINE_GUID(guid, 0x8003b0e5, 0xc521, 0x4db2, 0x81, 0xe4, 0x11, 0x2a, 0xe2, 0xb2, 0x5e, 0xf);
//
//#if BOOST_VERSION >= 103300
//class progress_cooling : public boost::linear_cooling<double>
//{
//	typedef boost::linear_cooling<double> inherited;
//
//public:
//	explicit progress_cooling(std::size_t iterations) : inherited(iterations) 
//	{
//		display.reset(new boost::progress_display(iterations + 1, std::cerr));
//	}
//
//	double operator()()
//	{
//		++(*display);
//		return inherited::operator()();
//	}
//
//private:
//	boost::shared_ptr<boost::progress_display> display;
//};
//#endif
