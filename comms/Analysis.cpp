#include "StdAfx.h"
#include "Analysis.h"

#include "clib.h"
#include "SoapUtil.h"
#include "cache.h"
#include "logger.h"
#include "global.h"
#include "util.h"
#include "utilDateTime.h"
#include "thread.h"
#include "SAXHandlers.h"
#include "SeisintDatasetSAXHandler.h"
#include "XMLWriter.h"
#include "htmlhelper.h"

namespace Analysis
{
//  ===========================================================================
typedef boost::tuple<std::_tstring, int, int> CacheKey;
typedef std::map<CacheKey, IEntityInstanceVector> RequestResponseCacheT;
//  ===========================================================================
class CAnalysis : public IServer, public clib::CLockableUnknown
{
protected:
	CString m_label;
	logging_signal_type on_logging;

public:
	IMPLEMENT_CUNKNOWN;

	CAnalysis(const CString & label) : m_label(label)
	{
	}

	~CAnalysis()
	{
	}

	const TCHAR * GetID() const
	{
		return m_label;
	}

	const TCHAR * GetCacheID() const
	{
		return m_label;
	}

	const TCHAR * GetLabel() const
	{
		return m_label;
	}

	void ParseEntityList(IWebPage * webPage, CComPtr<ITable> entities, IEntityVector * entityVector, ClassifyStructVector * textNodes)
	{
		if (!entities)
			return;

		int _orig = entities->FindColumn(_T("orig"), false);
		int _docid = entities->FindColumn(_T("docid"), false);
		int _pos = entities->FindColumn(_T("pos"), false);
		int _classify_score = entities->FindColumn(_T("classify_score"), false);
		int _cleanedinput = entities->FindColumn(_T("cleanedinput"), false);
		int _entity_type = entities->FindColumn(_T("entity_type"), false);
		int _local_entity_number = entities->FindColumn(_T("local_entity_number"), false);
		int _verified = entities->FindColumn(_T("verified"), false);
		int _resolved = entities->FindColumn(_T("resolved"), false);
		int _identifications = entities->FindColumn(_T("identifications"), false);
		if (_docid != -1 && _pos != -1 && _orig != -1)
		{
			for (int i = 0; i < entities->GetRowCount(); ++i)
			{
				std::_tstring _xml;
				entities->ToXML(i, _T("Dataset"), _T("Row"), _xml, false);
				CString xml = _xml.c_str();
				CString itemXML;
				GetEntityListItemXML(xml, itemXML);
				//std::_tstring itemXML;
				//UnescapeXML((const TCHAR *)_itemXML, itemXML);
				//CComPtr<ITable> ids = entities->GetChildDataset(0, _T("identifications"));
				std::_tstring cleanedInput;
				int docid = -1, pos = -1, entity_type = ENTITY_TYPE_UNKNOWN, localEntityNumber = -1, classifyScore = -1;
				bool verified = false, resolved = false;
				try {
					std::_tstring docidStr = entities->GetCell(i, _docid);
					docid = boost::lexical_cast<int>(docidStr) - 1;
				} catch(boost::bad_lexical_cast &) {}
				try {
					std::_tstring posStr = entities->GetCell(i, _pos);
					pos = boost::lexical_cast<int>(posStr);
				} catch(boost::bad_lexical_cast &) {}
				try {
					std::_tstring entity_typeStr = entities->GetCell(i, _entity_type);
					entity_type = boost::lexical_cast<int>(entity_typeStr);
				} catch(boost::bad_lexical_cast &) {}
				cleanedInput = entities->GetCell(i, _cleanedinput);
				try {
					std::_tstring localEntityNumberStr = entities->GetCell(i, _local_entity_number);
					localEntityNumber = boost::lexical_cast<int>(localEntityNumberStr);
				} catch(boost::bad_lexical_cast &) {}
				try {
					std::_tstring classifyScoreStr = entities->GetCell(i, _classify_score);
					classifyScore = boost::lexical_cast<int>(classifyScoreStr);
				} catch(boost::bad_lexical_cast &) {}
				verified = boost::algorithm::iequals(entities->GetCell(i, _verified), _T("true")) ? true : false;
				resolved = boost::algorithm::iequals(entities->GetCell(i, _resolved), _T("true")) ? true : false;;

				std::_tstring text = entities->GetCell(i, _orig);
				ATLASSERT(text.size());
				IEntityAdapt entity = CreateIEntity(webPage, entity_type, text, cleanedInput, localEntityNumber, classifyScore, verified, resolved);
				entity->SetServerResults((const TCHAR *)itemXML);
				if (entityVector)
					entityVector->push_back(entity);
				if (webPage)
					webPage->AppendEntity(entity);
				if (textNodes)
				{
					CComPtr<IEntityInstance> entityInstance = CreateIEntityInstance(webPage, entity.get(), textNodes->operator[](docid).textNode, textNodes->operator[](docid).text, docid, pos);
					webPage->AppendEntityInstance(entityInstance);
				}
				if(_identifications >= 0)
					entity->SetIdentities(entities->GetChildDataset(i, _identifications));
			}
		}
	}

	void ParseEntityList(IWebPage * webPage, CComPtr<ITable> entities)
	{
		ParseEntityList(webPage, entities, NULL, NULL);
	}

	void ParseGraph(IWebPage * webPage, CComPtr<ITable> edges)
	{
		if (!edges)
			return;

		//  Load Graph
		int _category = edges->FindColumn(_T("category"), false);
		int _from_category = edges->FindColumn(_T("from_category"), false);
		int _from_id = edges->FindColumn(_T("from_id"), false);
		int _to_category = edges->FindColumn(_T("to_category"), false);
		int _to_id = edges->FindColumn(_T("to_id"), false);
		//int _score = edges->FindColumn(_T("score"), false);
		//int _verified = edges->FindColumn(_T("verified"), false);
		if (_from_category && _from_id && _to_category &&_to_id)
		{
			for (int i = 0; i < edges->GetRowCount(); ++i)
			{
				int category = 0;
				int from_category = 0;
				std::_tstring from_id;
				int to_category = 0;
				std::_tstring to_id;
				try
				{
					category = boost::lexical_cast<int>(edges->GetCell(i, _category));
					from_category = boost::lexical_cast<int>(edges->GetCell(i, _from_category));
					from_id = edges->GetCell(i, _from_id);
					to_category = boost::lexical_cast<int>(edges->GetCell(i, _to_category));
					to_id = edges->GetCell(i, _to_id);
				}
				catch (boost::bad_lexical_cast &)
				{
					ATLASSERT(false);
					continue;
				}
				CComPtr<IEntity> from = FindIEntity(webPage, from_category, from_id);
				CComPtr<IEntity> to = FindIEntity(webPage, to_category, to_id);
				if (from && to)
					from->AppendOutEdge(to);
			}
		}
	}

	void ParseStats(IWebPage * webPage, CComPtr<ITable> stats)
	{
		if (!stats)
			return;

		CComPtr<IHtmlHelper> htmlHelper = CreateIHtmlHelper(_T("Stats"), true);
#ifndef _DEBUG
		try
		{
#endif
			htmlHelper->AppendTableDebug(stats);
#ifndef _DEBUG
		}
		catch(...)	//Big hack for demo
		{
			ATLASSERT(false);
		}
#endif
		CString html = htmlHelper->Close();
		webPage->SetStatsHTML((const TCHAR *)html);
	}

	void CleanClassifyText(std::_tstring & cleanText)
	{
		for(unsigned int i = 0; i < cleanText.length(); ++i)
		{
			TCHAR c = cleanText[i];
			if (c != 32)
				break;
			cleanText[i] = _T('?');
		}
		for(unsigned int i = 0; i < cleanText.length(); ++i)
		{
			TCHAR c = cleanText[i];
			if (c < 32 || c > 128)
				cleanText[i] = _T(' ');
		}
	}

	bool Classify(const std::_tstring & text, IEntityVector * entityVector)
	{
		using namespace SEEP3;
		std::_tstring log_req, log_resp;
		StdStringVector strArr;

		std::_tstring cleanText = text;
		CleanClassifyText(cleanText);
		strArr.push_back(cleanText);
		log_req = text;

		if (strArr.size())
		{
			CComInitialize com;
			CSEEP3T<CCustomActionSecureSoapSocketClient> server;
			server.SetUrl(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ANALYSIS)));
			server.SetAction(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ACTION_CLASSIFY)));
			server.SetUser(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
			server.SetPassword(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORDLNBAND)));

			CComBSTR cluster_ = L"";
			CComBSTR queue_ = L""; 
			bool include_schemas_ = false;
			bool limitResults_ = false;
			CComBSTR SParam = _T("");
			BSTR * Contents = CreateBstrArray(strArr);
			StdStringVector catArr;
			catArr.push_back(_T(""));
			BSTR * Categories = CreateBstrArray(catArr);
			CComBSTR Format = L"";
			CComBSTR ResultXML = L"";
			CComBSTR Workunit = L"";
			bool Async = false;
#if _COMMS_VER < 67205
			ESP_EXCEPTION_LOG(EspException);
			if (server.SEEP3_EntityClassify(cluster_, queue_, include_schemas_, limitResults_, SParam, Contents, strArr.size(), NULL, 0, Format, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &ResultXML, &Workunit, &Async) == S_OK)
#else
			ESP_EXCEPTION_LOG2(EspException);
			if (server.SEEP3_EntityClassify(cluster_, queue_, include_schemas_, limitResults_, SParam, Contents, strArr.size(), NULL, 0, Format, exceptions, &ResultXML, &Workunit, &Async) == S_OK)
#endif
			{
				log_resp = CW2T(ResultXML);
				CComPtr<ISAXXMLReader> pRdr; 

				pRdr.CoCreateInstance(CLSID_SAXXMLReader30); 

				ITableMap resultTableMap;
				CComPtr<SeisintDatasetHandlerImpl> pMc = new SeisintDatasetHandlerImpl(resultTableMap); 
				HRESULT hr = pRdr->putContentHandler(pMc); 

				CComVariant xml(ResultXML);
#ifdef _DEBUG
				if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
				{
					std::ofstream file("c:\\temp\\resultlog.xml");
					file << CT2A(CString(xml));
				}
#endif
				hr = pRdr->parse(xml);
				ParseEntityList(NULL, resultTableMap[_T("EntityList")].m_table.get(), entityVector, NULL);
			}
			else
			{
 				_DBGLOG(LEVEL_WARNING, server.GetClientError());
			}
			DestroyBstrArray(Categories, catArr.size());	
			DestroyBstrArray(Contents, strArr.size());	
		}
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		on_logging(_T("Classify"), log_req, log_resp);
		return true;
	}

	bool Classify(IWebPage * webPage, ClassifyStructVector & request)
	{
		using namespace SEEP3;
		std::_tstring log_req, log_resp;
		StdStringVector strArr;
		for(ClassifyStructVector::iterator itr = request.begin(); itr != request.end(); ++itr)
		{
			std::_tstring cleanText = CW2T(itr->text.c_str(), CP_UTF8);
			CleanClassifyText(cleanText);
			strArr.push_back(cleanText);
			log_req += cleanText + _T("\r\n");
		}

		if (strArr.size())
		{
			CComInitialize com;
			CSEEP3T<CCustomActionSecureSoapSocketClient> server;
			server.SetUrl(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ANALYSIS)));
			server.SetAction(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ACTION_CLASSIFY)));
			server.SetUser(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
			server.SetPassword(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORDLNBAND)));

			CComBSTR cluster_ = L"";
			CComBSTR queue_ = L""; 
			bool include_schemas_ = false;
			bool limitResults_ = false;
			CComBSTR SParam = CT2W(webPage->GetID(), CP_UTF8);
			BSTR * Contents = CreateBstrArray(strArr);
			StdStringVector catArr;
			catArr.push_back(_T(""));
			BSTR * Categories = CreateBstrArray(catArr);
			CComBSTR Format = L"";
			CComBSTR ResultXML = L"";
			CComBSTR Workunit = L"";
			bool Async = false;
#if _COMMS_VER < 67205
			ESP_EXCEPTION_LOG(EspException);
			if (server.SEEP3_EntityClassify(cluster_, queue_, include_schemas_, limitResults_, SParam, Contents, strArr.size(), NULL, 0, Format, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &ResultXML, &Workunit, &Async) == S_OK)
#else
			ESP_EXCEPTION_LOG2(EspException);
			if (server.SEEP3_EntityClassify(cluster_, queue_, include_schemas_, limitResults_, SParam, Contents, strArr.size(), NULL, 0, Format, exceptions, &ResultXML, &Workunit, &Async) == S_OK)
#endif
			{
				log_resp = CW2T(ResultXML);
				CComPtr<ISAXXMLReader> pRdr; 

				pRdr.CoCreateInstance(CLSID_SAXXMLReader30); 

				ITableMap resultTableMap;
				CComPtr<SeisintDatasetHandlerImpl> pMc = new SeisintDatasetHandlerImpl(resultTableMap); 
				HRESULT hr = pRdr->putContentHandler(pMc); 

				CComVariant xml(ResultXML);
#ifdef _DEBUG
				if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
				{
					std::string fileName = (boost::format("c:\\temp\\resultlog_%1%.xml") % CW2A(SParam)).str();
					std::ofstream file(fileName.c_str());
					file << CT2A(CString(xml));
				}
#endif
				hr = pRdr->parse(xml);
				ParseEntityList(webPage, resultTableMap[_T("EntityList")].m_table.get(), NULL, &request);
			}
			else
			{
 				_DBGLOG(LEVEL_WARNING, server.GetClientError());
			}
			DestroyBstrArray(Categories, catArr.size());	
			DestroyBstrArray(Contents, strArr.size());	
		}
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		on_logging(_T("Classify"), log_req, log_resp);
		return true;
	}

	bool GetEntityListItemXML(const CString & entityListXml, CString & result)
	{
		int startPos = entityListXml.Find(_T("<Row>"));
		if (startPos > 0)
		{
			int endPos = entityListXml.Find(_T("</Dataset>"), startPos);
			if (endPos > 0)
			{
				result = entityListXml.Mid(startPos, endPos - startPos);
				return true;
			}
		}
		return false;
	}

	bool GetEntityListXML(const CComBSTR & xml, CString & result)
	{
		CString entityListXml = xml;
		int startPos = entityListXml.Find(_T("<Dataset name='EntityList'"));
		if (startPos > 0)
		{
			startPos = entityListXml.Find(_T("<Row>"), startPos);
			if (startPos > 0)
			{
				int endPos = entityListXml.Find(_T("</Dataset>"), startPos);
				if (endPos > 0)
				{
					result = entityListXml.Mid(startPos, endPos - startPos);
					return true;
				}
			}
		}
		return false;
	}

	bool Resolve(IWebPage * webPage, const IEntityVector & entityVector)
	{
		std::_tstring result;
		using namespace SEEP3;
		std::_tstring log_req, log_resp;

		CComInitialize com;
		CSEEP3T<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ANALYSIS)));
		server.SetAction(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ACTION_RESOLVE)));
		server.SetUser(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
		server.SetPassword(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORDLNBAND)));

		CComBSTR cluster_ = L"";
		CComBSTR queue_ = L""; 
		bool include_schemas_ = false;
		bool limitResults_ = false;
		CComBSTR ResultXML = L"";
		CComBSTR Workunit = L"";
		bool Async = false;
#if _COMMS_VER < 67205
		CComBSTR EntityList = log_req.c_str();
		CComPtr<IXMLWriter> xmlwriter = CreateIXMLWriter(false);
		int i = 0;
		for(IEntityVector::const_iterator itr = entityVector.begin(); itr != entityVector.end(); ++itr, ++i)
		{
			xmlwriter->PushElement(_T("Row"));{
				xmlwriter->PushElement(_T("SParam"), entityVector[i]->GetID());
				xmlwriter->PopElement();
				xmlwriter->PushElement(_T("orig"), entityVector[i]->GetOrig());
				xmlwriter->PopElement();
				xmlwriter->PushElement(_T("CleanedInput"), entityVector[i]->GetCleanedInput());
				xmlwriter->PopElement();
				xmlwriter->PushElement(_T("classify_score"), entityVector[i]->GetClassifyScore());
				xmlwriter->PopElement();
				xmlwriter->PushElement(_T("entity_type"), entityVector[i]->GetEntityType());
				xmlwriter->PopElement();
				xmlwriter->PushElement(_T("local_entity_number"), entityVector[i]->GetLocalEntityNumber());
				xmlwriter->PopElement();
				xmlwriter->PushElement(_T("verified"), entityVector[i]->GetVerified());
				xmlwriter->PopElement();
				xmlwriter->PushElement(_T("resolved"), entityVector[i]->GetResolved());
				xmlwriter->PopElement();
			}xmlwriter->PopElement();
		}
		xmlwriter->EndDocument(log_req);
		ESP_EXCEPTION_LOG(EspException);
		if (server.SEEP3_EntityResolve(cluster_, queue_, include_schemas_, limitResults_, EntityList, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &ResultXML, &Workunit, &Async) == S_OK)
#else
		CStructArrayIn<RowType> EntityList;
		EntityList.Resize(entityVector.size());
		int i = 0;
		for(IEntityVector::const_iterator itr = entityVector.begin(); itr != entityVector.end(); ++itr, ++i)
		{
			EntityList.SetBSTR(EntityList.GetItem(i)->sparam, entityVector[i]->GetID());
			EntityList.SetBSTR(EntityList.GetItem(i)->orig, entityVector[i]->GetOrig());
			EntityList.SetBSTR(EntityList.GetItem(i)->cleanedinput, entityVector[i]->GetCleanedInput());
			EntityList.GetItem(i)->classify_score = entityVector[i]->GetClassifyScore();
			EntityList.GetItem(i)->entity_type = entityVector[i]->GetEntityType();
			EntityList.GetItem(i)->local_entity_number = entityVector[i]->GetLocalEntityNumber();
			EntityList.GetItem(i)->verified = entityVector[i]->GetVerified();
			EntityList.GetItem(i)->resolved = entityVector[i]->GetResolved();
		}
		ESP_EXCEPTION_LOG2(EspException);
		if (server.SEEP3_EntityResolve(cluster_, queue_, include_schemas_, limitResults_, EntityList.GetArray(), EntityList.GetCount(), exceptions, &ResultXML, &Workunit, &Async) == S_OK)
#endif
		{
			CComVariant xml(ResultXML);
#ifdef _DEBUG
			if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
			{
				std::ofstream file("c:\\temp\\resultlog.xml");
				file << CT2A(CString(xml));
			}
#endif

			log_resp = CW2T(ResultXML);
			CComPtr<ISAXXMLReader> pRdr; 
			pRdr.CoCreateInstance(CLSID_SAXXMLReader30); 

			ITableMap resultTableMap;
			CComPtr<SeisintDatasetHandlerImpl> pMc = new SeisintDatasetHandlerImpl(resultTableMap); 
			HRESULT hr = pRdr->putContentHandler(pMc); 

			hr = pRdr->parse(xml);
			ParseEntityList(webPage, resultTableMap[_T("EntityList")].m_table.get());
			ParseGraph(webPage, resultTableMap[_T("edges")].m_table.get());
			ParseStats(webPage, resultTableMap[_T("stats")].m_table.get());

			CString entityListXml;
			if (GetEntityListXML(ResultXML, entityListXml))
			{
				webPage->SetResolveResult((const TCHAR *)entityListXml);
			}
		}
		else
		{
			_DBGLOG(LEVEL_WARNING, server.GetClientError());
		}
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		on_logging(_T("Resolve"), log_req, log_resp);
		return true;
	}

	bool Enhance(IWebPage * webPage, IEntity * entity, __int64 MaxDids, __int64 MaxFull)
	{
		using namespace SEEP3;
		std::_tstring log_req, log_resp;

		CComInitialize com;
		CSEEP3T<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ANALYSIS)));
		server.SetAction(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ACTION_ENHANCE)));
		server.SetUser(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
		server.SetPassword(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORDLNBAND)));

		CComBSTR cluster_ = L"";
		CComBSTR queue_ = L""; 
		bool include_schemas_ = false;
		bool limitResults_ = false;
		if (webPage)
			log_req = webPage->GetResolveResults();
		CComBSTR ResultXML = L"";
		CComBSTR Workunit = L"";
		bool Async = false;

#if _COMMS_VER < 67205
		CComBSTR EntityList = entity->GetServerResults();  
		ESP_EXCEPTION_LOG(EspException);
		if (server.SEEP3_EntityEnhance(cluster_, queue_, include_schemas_, limitResults_, NULL, 0, MaxDids, MaxFull, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &ResultXML, &Workunit, &Async) == S_OK)
#else
		CStructArrayIn<RowType> EntityList;
		EntityList.Resize(1);
		EntityList.SetBSTR(EntityList.GetItem(0)->sparam, entity->GetID());
		EntityList.SetBSTR(EntityList.GetItem(0)->orig, entity->GetOrig());
		EntityList.SetBSTR(EntityList.GetItem(0)->cleanedinput, entity->GetCleanedInput());
		EntityList.GetItem(0)->classify_score = entity->GetClassifyScore();
		EntityList.GetItem(0)->entity_type = entity->GetEntityType();
		EntityList.GetItem(0)->local_entity_number = entity->GetLocalEntityNumber();
		EntityList.GetItem(0)->verified = entity->GetVerified();
		EntityList.GetItem(0)->resolved = entity->GetResolved();

		CStructArrayIn<RowType2> IdList;
		ITable * ids = entity->GetIdentities();
		if (ids)
		{
			IdList.Resize(ids->GetRowCount());

			int _id_type = ids->FindColumn(_T("id_type"), false);
			int _id = ids->FindColumn(_T("id"), false);
			int _score = ids->FindColumn(_T("score"), false);
			ATLASSERT(_id_type >= 0 && _id >= 0 && _score >= 0);
			for(int i = 0; i < ids->GetRowCount(); ++i)
			{
				try
				{
					IdList.GetItem(i)->id_type = boost::lexical_cast<int>(ids->GetCell(i, _id_type));
					IdList.GetItem(i)->id = boost::lexical_cast<int>(ids->GetCell(i, _id));
					IdList.GetItem(i)->score = boost::lexical_cast<int>(ids->GetCell(i, _score));
				}
				catch(boost::bad_lexical_cast &)
				{
				}
			}
			EntityList.GetItem(0)->identifications = IdList.GetArray();
			EntityList.GetItem(0)->__identifications_nSizeIs = IdList.GetCount();
		}

		ESP_EXCEPTION_LOG2(EspException);
		if (server.SEEP3_EntityEnhance(cluster_, queue_, include_schemas_, limitResults_, EntityList.GetArray(), EntityList.GetCount(), MaxDids, MaxFull, exceptions, &ResultXML, &Workunit, &Async) == S_OK)
#endif
		{
			CComVariant xml(ResultXML);
#ifdef _DEBUG
			if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
			{
				std::ofstream file("c:\\temp\\resultlog.xml");
				file << CT2A(CString(xml));
			}
#endif

			log_resp = CW2T(ResultXML);

			CComPtr<ISAXXMLReader> pRdr; 
			pRdr.CoCreateInstance(CLSID_SAXXMLReader30); 

			ITableMap resultTableMap;
			CComPtr<SeisintDatasetHandlerImpl> pMc = new SeisintDatasetHandlerImpl(resultTableMap); 
			HRESULT hr = pRdr->putContentHandler(pMc); 

			hr = pRdr->parse(xml);
			for(ITableMap::iterator itr = resultTableMap.begin(); itr != resultTableMap.end(); ++itr)
			{
				if (boost::algorithm::iequals(itr->first, _T("EntityList")) ||
					boost::algorithm::iequals(itr->first, _T("vertices")) ||
					boost::algorithm::iequals(itr->first, _T("edges")) ||
					boost::algorithm::istarts_with(itr->first, _T("Result ")))
				{
					continue;
				}
				if (entity && itr->second.m_table->GetRowCount())
				{
					entity->SetDetails(itr->second.m_table);
					break;
				}
			}
		}
		else
		{
			_DBGLOG(LEVEL_WARNING, server.GetClientError());
		}
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		on_logging(_T("GetData"), log_req, log_resp);
		return true;
	}

	boost::signals::connection on_logging_connect(const logging_slot_type& s)
    { 
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_logging.connect(s); 
	}

	void on_logging_disconnect(boost::signals::connection& sc)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		sc.disconnect();
	}
};

static CacheT<std::_tstring, CAnalysis> AnalysisCache;

COMMS_API IServer * AttachAnalysis()
{
	return AnalysisCache.Get(new CAnalysis(_T("Analysis")));
}
}
