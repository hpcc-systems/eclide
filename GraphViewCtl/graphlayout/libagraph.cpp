#include "platform.h"

#include "malloc.h"
#include "types.h"
#include "dotprocs.h"
#include "neato.h"
#include "fdp.h"
#include "circle.h"
#include "circular.h"
#include "gvc.h"

// required for exporting shortest_route
#include "vispath.h"

#include "libagraphsp.h"
#include <gvc.h>

// libagraph.cpp : Defines the entry point for the DLL application.
//

//#ifndef SEISINT_LIBEXPORTS
//BOOL APIENTRY DllMain( HANDLE /* hModule */, DWORD reason, LPVOID /* lpReserved */)
//{
//	switch (reason)	{
//		case DLL_PROCESS_ATTACH:
//		case DLL_THREAD_ATTACH:
//		case DLL_THREAD_DETACH:
//		case DLL_PROCESS_DETACH:
//			break;
//	}
//	return TRUE;
//}
//#endif

void global_cleanup()
{
	free(Gvfilepath);
	free(Files);
	free(Lib);
}

extern "C" gvplugin_library_t gvplugin_dot_layout_LTX_library;
extern "C" gvplugin_library_t gvplugin_neato_layout_LTX_library;
extern "C" gvplugin_library_t gvplugin_core_LTX_library;

lt_symlist_t lt_preloaded_symbols[] = {
	{ "gvplugin_dot_layout_LTX_library", (void*)(&gvplugin_dot_layout_LTX_library) },
	{ "gvplugin_neato_layout_LTX_library", (void*)(&gvplugin_neato_layout_LTX_library) },
	{ "gvplugin_core_LTX_library", (void*)(&gvplugin_core_LTX_library) },
	{ 0, 0 }
};

std::map<GVC_t *, std::string> g_result;
size_t MyWriteFunction(GVJ_t *job, const char *s, size_t len)
{
	g_result[job->gvc].append(s, len);
	return len;
}

static GVC_t *gvc;
LIBAGRAPH_API bool DoLayout(const char * layout, const char* mem, const char* format, const char* scale, std::string & result)
{
	bool retVal = true;
	int  argc = 0;
	char* argv [32];

    gvc = gvContextPlugins(lt_preloaded_symbols, FALSE);
	if (gvc) 
	{
		std::string arg_layout = layout;
		std::string arg_format = "-T";
		std::string arg_scale  = "-s";

		argv[argc++] = const_cast<char*>(arg_layout.c_str());

		if (format) {
			arg_format  += format;
			argv[argc++] = const_cast<char*>(arg_format.c_str());
		}

		if (scale) {
			arg_scale   += scale;
			argv[argc++] = const_cast<char*>(arg_scale.c_str());
		}

		argv[argc] = 0;

		gvParseArgs(gvc, argc, argv);
		graph_t *g = agmemread(const_cast<char *>(mem));

		if (g) 
		{
#ifdef _DEBUG
			std::cerr << std::string("Start - Layout Engine.") << std::endl;
			gvLayout(gvc, g, const_cast<char *>(layout)); 
			gvc->write_fn = &MyWriteFunction;
			gvRenderJobs(gvc, g);
			result = g_result[gvc];
			g_result[gvc].clear();
			gvFreeLayout(gvc, g);
			agclose(g);
			gvFinalize(gvc);
			gvFreeContext(gvc);
			std::cerr << std::string("Stop - Layout Engine.") << std::endl;
#else
			try {
				gvLayout(gvc, g, const_cast<char *>(layout)); 
			} catch (...) {
				std::cerr << std::string("Layout Engine:  Failed at Layout.") << std::endl;
				retVal = false;
			}
			try {
				gvc->write_fn = &MyWriteFunction;
				gvRenderJobs(gvc, g);
				result = g_result[gvc];
				g_result[gvc].clear();
			} catch (...) {
				std::cerr << std::string("Layout Engine:  Failed at RenderJobs.") << std::endl;
				retVal = false;
			}
			try {
				gvFreeLayout(gvc, g);
			} catch (...) {
				std::cerr << std::string("Layout Engine:  Failed at FreeLayout.") << std::endl;
			}
			try {
				agclose(g);
			} catch (...) {
				std::cerr << std::string("Layout Engine:  Failed at Close.") << std::endl;
			}
			try {
				gvFinalize(gvc);
			} catch (...) {
				std::cerr << std::string("Layout Engine:  Failed at Finalize.") << std::endl;
			}
			try {
				gvFreeContext(gvc);
			} catch (...) {
				std::cerr << std::string("Layout Engine:  Failed at FreeContext.") << std::endl;
			}
#endif
		}
		global_cleanup();
	}

	return retVal;
}

LIBAGRAPH_API int shortest_route(Ppoly_t **polys, int num_polys, Ppoint_t * src, Ppoint_t * dst, Ppolyline_t* path)
{
	vconfig_t* vc=Pobsopen(polys, num_polys);
	if(vc!=0)
	{
		int result=Pobspath(vc, *src, POLYID_NONE, *dst, POLYID_NONE, path);
		Pobsclose(vc);
		//free(vc);
		return result;
	}
	return 0;
}

