#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "WizardSprayData.h"
//TJ#include <HtmlHelp.h>
#include "Repository.h"
#include "EclParser.h"
#include "util.h"

CWizardSprayData::CWizardSprayData() : m_showWelcome(true), m_fileSize(0)
{
	m_gretlFlag = true;
}

CWizardSprayData::~CWizardSprayData()
{
}

bool CWizardSprayData::SetShowWelcome(bool showWelcome)
{
	m_showWelcome = showWelcome;
	return (m_showWelcome == showWelcome);
}

bool CWizardSprayData::GetShowWelcome() const
{
	return m_showWelcome;
}

void CWizardSprayData::SetGrETLFlag(bool gretlFlag)
{
	m_gretlFlag = gretlFlag;
}

bool CWizardSprayData::GetGrETLFlag() const
{
	return m_gretlFlag;
}

int CWizardSprayData::FindPreviousPage(int /*pageDialogId*/) const
{
	// 0  = goto previous page
	// -1 = prevent page change
	// >0 = jump to page by dlg ID
	int page = 0;

	//switch(pageDialogId)
	//{
	//case IDD_WIZ97_WELCOME:
	//	break;
	//case IDD_WIZ97_PATHFILTER:
	//	break;
	//case IDD_WIZ97_FILEPREVIEW:
	//	break;
	//case IDD_WIZ97_OUTPUT:
	//	break;
	//case IDD_WIZ97_COMPLETION:
	//	break;
	//}

	return page;
}

int CWizardSprayData::FindNextPage(int /*pageDialogId*/) const
{
	// 0  = goto next page
	// -1 = prevent page change
	// >0 = jump to page by dlg ID
	int page = 0;

	//switch(pageDialogId)
	//{
	//case IDD_WIZ97_WELCOME:
	//	break;
	//case IDD_WIZ97_PATHFILTER:
	//	break;
	//case IDD_WIZ97_FILEPREVIEW:
	//	break;
	//case IDD_WIZ97_OUTPUT:
	//	break;
	//case IDD_WIZ97_COMPLETION:
	//	break;
	//}

	return page;
}

int CWizardSprayData::FindFirstPageAfterWelcome() const
{
	return IDD_WIZ97_COMPLETION;
}

void CWizardSprayData::ShowHelp(int pageDialogId, int /*controlId*/)
{
	LPCTSTR topicPath = NULL;

	switch(pageDialogId)
	{
	case IDD_WIZ97_WELCOME:
		topicPath = _T("/TestWizard_Welcome.html");
		break;
	case IDD_WIZ97_COMPLETION:
		topicPath = _T("/TestWizard_Completion.html");
		break;
	}

	// TJ help commented out

	//TCHAR helpFileLink[2*MAX_PATH+1] = {0};
	//::GetModuleFileName(NULL, helpFileLink, 2*MAX_PATH);
	//::PathRenameExtension(helpFileLink, _T(".chm"));

	//UINT command = HH_DISPLAY_TOC;
	//if(topicPath)
	//{
	//	command = HH_DISPLAY_TOPIC;
	//	::lstrcat(helpFileLink, _T("::"));
	//	::lstrcat(helpFileLink, topicPath);
	//}

	//// Add the window name
	//::lstrcat(helpFileLink, _T(">$global_main"));

	// In real code, you'd probably want the "main frame" to be hWndHelpParent.
	// HWND hWndHelpParent = ::GetActiveWindow();
	//HWND hWndHelp = ::HtmlHelp(hWndHelpParent, helpFileLink, command, NULL);
	//ATLASSERT(hWndHelp != NULL);

	//if(hWndHelp != NULL && topicPath != NULL)
	//{
	//	// NOTE: It'd probably be better to use the notification
	//	//  HHN_NAVCOMPLETE to know when we can synchronize
	//	::Sleep(200);
	//	::PostMessage(hWndHelp, WM_COMMAND, MAKEWPARAM(IDTB_SYNC, 0), 0);
	//}
}

void CWizardSprayData::ShowContextHelp(LPHELPINFO helpInfo)
{
	// TJ commented out help stuff

	//TCHAR helpFileLink[2*MAX_PATH+1] = {0};
	//::GetModuleFileName(NULL, helpFileLink, 2*MAX_PATH);
	//::PathRenameExtension(helpFileLink, _T(".chm"));

	//::lstrcat(helpFileLink, _T("::/Context.txt"));

	//DWORD idList[3] = { helpInfo->iCtrlId, (DWORD)helpInfo->dwContextId, 0};

	//// For more control on the appearance, we could use HH_DISPLAY_TEXT_POPUP
	//HWND hWndHelp = ::HtmlHelp((HWND)helpInfo->hItemHandle, helpFileLink, HH_TP_HELP_WM_HELP, (DWORD_PTR)idList);
	//ATLASSERT(hWndHelp != NULL);
	//if(hWndHelp == NULL)
	//{
	//	::MessageBox(::GetActiveWindow(), _T("Unable to show context help"), _T("Error"), MB_OK | MB_ICONERROR);
	//}
}

bool CWizardSprayData::FinishWizard(HWND hWndParent)
{
	std::_tstring module, label, ecl;
	GetModule(module);

	GetEclAttributeLabel(ATTRIBUTELABEL_RECORD, false, label);
	GetEclRecord(ecl);
	SaveEcl(module.c_str(), label.c_str(), ecl.c_str(), _T("Auto Checkin by GrETL"));

	GetEclAttributeLabel(ATTRIBUTELABEL_DATASET, false, label);
	GetEclDataset(ecl);
	SaveEcl(module.c_str(), label.c_str(), ecl.c_str(), _T("Auto Checkin by GrETL"));

//	GetEclAttributeLabel(ATTRIBUTELABEL_SPRAY, false, label);
//	GetEclSpray(ecl);
//	SaveEcl(module.c_str(), label.c_str(), ecl.c_str(), _T("Auto Checkin by GrETL"));

//	GetEclAttributeLabel(ATTRIBUTELABEL_TARGET, false, label);
//	GetEclTarget(ecl);
//	SaveEcl(module.c_str(), label.c_str(), ecl.c_str(), _T("Auto Checkin by GrETL"));

	GetEclAttributeLabel(ATTRIBUTELABEL_TRANSFORM, false, label);
	GetEclTransform(ecl);
	SaveEcl(module.c_str(), label.c_str(), ecl.c_str(), _T("Auto Checkin by GrETL"));
	return true;
}

void CWizardSprayData::SetSource(const std::_tstring & ecl)
{
	m_sourceEcl = ecl;
	//std::_tstring tmp_path;
	//ecl = (boost::_tformat(_T("file_path := '~file::%1%';")) % m_lz_path.GetEclPath(tmp_path)).str();
	//return ecl.c_str();
}

const TCHAR * CWizardSprayData::GetSource()
{
	return m_sourceEcl.c_str();
	//CComPtr<IEclProgram> program = ParseEcl(ecl.c_str());
	//if (program)
	//{
	//	for(unsigned i = 0; i < program->GetElementCount(); ++i)
	//	{
	//		CComQIPtr<IEclAssign>assign = program->GetElement(i);
	//		if (assign)
	//		{
	//			assign->GetIdentifier();
	//			CComQIPtr<IEclValue> rhs = assign->GetRHS();
	//			if (rhs && rhs->IsStringLiteral())
	//			{
	//				std::_tstring str;
	//				rhs->GetStringValue(str);
	//				m_lz_path.SetEclPath(str);
	//			}
	//		}
	//	}
	//}
}

const TCHAR * CWizardSprayData::GetSourceFilename(std::_tstring & filename)
{
	typedef std::vector<std::_tstring> split_vector_type;
	split_vector_type SplitVec; 
	boost::algorithm::split(SplitVec, m_sourceEcl, boost::algorithm::is_any_of("::"), boost::algorithm::token_compress_on);
	split_vector_type::reverse_iterator itr = SplitVec.rbegin();
	if (itr != SplitVec.rend())
	{
		std::_tstring filenameAndExt = itr->c_str();
		SplitVec.clear();
		boost::algorithm::split(SplitVec, filenameAndExt, boost::algorithm::is_any_of("."), boost::algorithm::token_compress_on);
		split_vector_type::iterator itr = SplitVec.begin();
		if (itr != SplitVec.end())
		{
			filename = itr->c_str();
		}
	}
	return filename.c_str();
}

void CWizardSprayData::SetModule(const std::_tstring & module)
{
	m_module = module;
}

const TCHAR * CWizardSprayData::GetModule(std::_tstring & module)
{
	module = m_module;
	return module.c_str();
}

void CWizardSprayData::SetPrefix(const std::_tstring & prefix)
{
	m_prefix = prefix;
}

const TCHAR * CWizardSprayData::GetPrefix(std::_tstring & prefix)
{
	prefix = m_prefix;
	return prefix.c_str();
}

void CWizardSprayData::SetDestination(const std::_tstring & destination)
{
	m_destination = destination;
}

const TCHAR * CWizardSprayData::GetDestination(std::_tstring & destination)
{
	destination = m_destination;
	return destination.c_str();
}

void CWizardSprayData::SetTarget(const std::_tstring & target)
{
	m_target = target;
}

const TCHAR * CWizardSprayData::GetTarget(std::_tstring & target)
{
	target = m_target;
	return target.c_str();
}

void CWizardSprayData::SetQueue(const std::_tstring & queue)
{
	m_queue = queue;
}

const TCHAR * CWizardSprayData::GetQueue(std::_tstring & queue)
{
	queue = m_queue;
	return queue.c_str();
}

void CWizardSprayData::SetCluster(const std::_tstring & cluster)
{
	m_cluster = cluster;
}

const TCHAR * CWizardSprayData::GetCluster(std::_tstring & cluster)
{
	cluster = m_cluster;
	return cluster.c_str();
}

void CWizardSprayData::SetFileSize(__int64 fileSize)
{
	m_fileSize = fileSize;
}
__int64 CWizardSprayData::GetFileSize()
{
	return m_fileSize;
}

const TCHAR * CWizardSprayData::GetEclAttributeLabel(ATTRIBUTELABEL type, bool qualified, std::_tstring & attribute)
{
	std::_tstring module, prefix;
	GetModule(module);
	GetPrefix(prefix);
	if (qualified)
		attribute = (boost::_tformat(_T("%1%.%2%")) % module % prefix).str();
	else
		attribute = prefix;

	switch (type)
	{
	case ATTRIBUTELABEL_RECORD:
		attribute += _T("_Record");
		break;
	case ATTRIBUTELABEL_DATASET:
		attribute += _T("_Dataset");
		break;
	case ATTRIBUTELABEL_SPRAY:
		attribute += _T("_Spray");
		break;
	case ATTRIBUTELABEL_TARGET:
		attribute += _T("_Target");
		break;
	case ATTRIBUTELABEL_TRANSFORM:
		attribute += _T("_Transform");
		break;
	default:
		ATLASSERT(!"Unknown attribute label type");
	}

	return attribute.c_str();
}

const TCHAR * CWizardSprayData::GetEclRecord(std::_tstring & record)
{
	record.clear();

	std::_tstring label;
	GetEclAttributeLabel(ATTRIBUTELABEL_RECORD, false, label);
	CComPtr<IEclRecord>ecl_record = CreateIEclRecord();
	m_analyzerInfo.GetEclRecord(ecl_record);

	//record.Format(_T("export %s := "),label);
	//ecl_record->Serialize(record);

	std::_tstring temp;
	CComPtr<IEclIdentifier> lhs = CreateIEclIdentifier(label);
	CComPtr<IEclIdentifier> rhs = CreateIEclIdentifier(ecl_record->Serialize(temp));
	CComPtr<IEclAssign> assignment = CreateIEclAssign(lhs,rhs);
	assignment->SetExported(true);
	assignment->Serialize(record);
	record += _T(";\r\n");
	return record.c_str();
}

const TCHAR * CWizardSprayData::GetEclDataset(std::_tstring & dataset)
{
	dataset.clear();
	std::_tstring module, prefix, label, labelRecord, source, source2, datasetStructure, format;
	GetModule(module);
	GetPrefix(prefix);
	GetEclAttributeLabel(ATTRIBUTELABEL_DATASET, false, label);
	GetEclAttributeLabel(ATTRIBUTELABEL_RECORD, false, labelRecord);
	source = (boost::_tformat(_T("%1%::%2%")) % module % prefix).str();
	std::_tstring lz_path;
	source2 = (boost::_tformat(_T("~file::%1%")) % m_lz_path.GetEclPath(lz_path)).str();
	m_analyzerInfo.GetEclFormat(format, false);
	CComPtr<IEclIdentifier> ecl_identifier = CreateIEclIdentifier(label);
	CComPtr<IEclValue> ecl_file = CreateIEclValue(_T("file_path"));
	CComPtr<IEclValue> ecl_record = CreateIEclValue(labelRecord);
	CComPtr<IEclDataset> ecl_dataset;
	if (boost::algorithm::iequals(format, _T("XML")))
	{
		ecl_dataset = CreateIEclDataset(ecl_file, ecl_record, ECLDATASETFORMAT_XML);
		std::_tstring xmlrowtag;
		m_analyzerInfo.GetXMLRowTag(xmlrowtag);
		xmlrowtag.insert(xmlrowtag.begin(),_T('\''));
		xmlrowtag.append(_T("'"));
		ecl_dataset->SetXmlXPath(xmlrowtag.c_str());
	}
	else if (boost::algorithm::iequals(format, _T("CSV")))
	{
		ecl_dataset = CreateIEclDataset(ecl_file, ecl_record, ECLDATASETFORMAT_CSV);
		ecl_dataset->SetCSVHeading(m_analyzerInfo.GetHeaderRows());
		ecl_dataset->SetCSVMaxLength(16*1204);
		StdString delim_raw, term_raw;
		m_analyzerInfo.GetDelimiter(delim_raw);
		m_analyzerInfo.GetTerminator(term_raw);
		CString delim, term;
		EncodeForEcl(delim_raw.c_str(), delim);
		EncodeForEcl(term_raw.c_str(), term);
		ecl_dataset->SetCSVSeparator(delim.GetString());
		ecl_dataset->SetCSVTerminator(term.GetString());
	}
	else
	{
		ecl_dataset = CreateIEclDataset(ecl_file, ecl_record, ECLDATASETFORMAT_FLAT);
	}
	ecl_dataset->Serialize(datasetStructure);

	static const TCHAR * const tpl =
	_T("import lib_fileservices;\n")
	_T("file_path := if(FileServices.FileExists('%1%'), '%1%', '%2%');\n")
	_T("export %3% := %4%;\n");
	dataset = (boost::_tformat(tpl) % source % source2 % label % datasetStructure).str();
	return dataset.c_str();
}

//const TCHAR * CWizardSprayData::GetEclDatasetRaw(CString & dataset)
//{
//	CString module, prefix, label, labelRecord, source, datasetStructure;
//	GetModule(module);
//	GetPrefix(prefix);
//	GetEclAttributeLabel(ATTRIBUTELABEL_DATASETRAW, false, label);
//	GetEclAttributeLabel(ATTRIBUTELABEL_RECORD, false, labelRecord);
//	std::_tstring lz_path;
//	source.Format(_T("'~file::%s'"), m_lz_path.GetEclPath(lz_path));
//	m_analyzerInfo.GetEclDataset(source, labelRecord, datasetStructure);
//	dataset.Format(_T("export %s := %s;\r\n"), label, datasetStructure); 
//	return dataset;
//}

const TCHAR * CWizardSprayData::GetEclSpray(std::_tstring & spray)
{
	spray.clear();

	std::_tstring module, prefix, cluster, label, labelRecord;
	GetModule(module);
	GetPrefix(prefix);
	GetCluster(cluster);
	GetEclAttributeLabel(ATTRIBUTELABEL_SPRAY, false, label);
	GetEclAttributeLabel(ATTRIBUTELABEL_RECORD, false, labelRecord);
	std::_tstring ip = m_lz_path.GetIP();
	CString path;
	EncodeForEcl(m_lz_path.GetPath(), path);
	if (m_analyzerInfo.IsFixedFileFormat())
	{
		spray = (boost::_tformat(_T("import lib_fileservices;\r\nimport lib_system;\r\nexport %1% := lib_fileservices.FileServices.SprayFixed('%2%', %3%, sizeof(%4%), '%5%', '%6%::%7%', -1, lib_system.ws_fs_server, -1, true);\r\n")) % 
			label % 
			ip % 
			path.GetString() %
			labelRecord % 
			cluster %
			module %
			prefix).str();
	}
	else if(m_analyzerInfo.IsVariableFileFormat())
	{
		StdString seperator, terminator, quote;
		CString enc_seperator, enc_terminator, enc_quote;
		m_analyzerInfo.GetDelimiter(seperator);
		m_analyzerInfo.GetTerminator(terminator);
		m_analyzerInfo.GetQuote(quote);
		EncodeForEcl(seperator.c_str(), enc_seperator);
		EncodeForEcl(terminator.c_str(), enc_terminator);
		EncodeForEcl(quote.c_str(), enc_quote);
		spray =  (boost::_tformat(_T("import lib_fileservices;\r\nexport %1% := lib_fileservices.FileServices.SprayVariable('%2%', %3%, 16384, %4%, %5%, %6%, '%7%', '%8%::%9%');\r\n")) % 
			label % 
			ip % 
			path %
			enc_seperator.GetString() %
			enc_terminator.GetString() %
			enc_quote.GetString() %
			cluster %
			module %
			prefix).str();
	}
	return spray.c_str();
}

const TCHAR * CWizardSprayData::GetEclTarget(std::_tstring & target)
{
	target.clear();

	return target.c_str();
}

const TCHAR * CWizardSprayData::GetEclTransform(std::_tstring & transform)
{
	transform.clear();
	//  Dataset  ---
	std::_tstring dataset_label;
	GetEclAttributeLabel(ATTRIBUTELABEL_DATASET, true, dataset_label);

	CComPtr<IEclAssign> datasetAssign = CreateIEclAssign(CreateIEclIdentifier(_T("d")), CreateIEclIdentifierReference(dataset_label));
	datasetAssign->Serialize(transform);
	transform += _T(";\r\n");

	if (m_target.length())
	{
		//  Transform ---
		static const TCHAR * const DEFUALT_TRANSFORM_LABEL = _T("t_transform");
		CComPtr<IEclTransform> ecl_transform = CreateIEclTransform(CreateIEclIdentifier(DEFUALT_TRANSFORM_LABEL), CreateIEclIdentifierReference(m_target));
		ecl_transform->SetNameMatchValues(false);
		ecl_transform->SetDefaultValues(false);
		ecl_transform->Serialize(transform);
		transform += _T(";\r\n");

		//  Project  ---
		CComPtr<IEclAssign> projectAssign = CreateIEclAssign(CreateIEclIdentifier(_T("t")), 
															 CreateIEclProject(CreateIEclIdentifier(_T("")), 
																			   CreateIEclIdentifier(DEFUALT_TRANSFORM_LABEL)));
		projectAssign->Serialize(transform);
		transform += _T(";\r\n");
	}
	//  ---  ---  ---

	return transform.c_str();
}

void CWizardSprayData::SaveEcl(const CString & module, const CString & label, const CString & _ecl, const CString & comments)
{
	CComPtr<IRepository> rep = AttachRepository();
	CComPtr<IAttribute> attr;
	if (rep->AttributeExists(module, label))
	{
		attr = rep->GetAttribute(module, label);
		if (attr->IsCheckedOut())
		{
			attr->Checkin(comments);
		}
	}
	else
	{
		attr = rep->InsertAttribute(module, label);
	}
	CString ecl = _ecl;
	//TidyCRLF(ecl);
	ecl.Trim();
	CString currentEcl = attr->GetText();
	//TidyCRLF(currentEcl);
	currentEcl.Trim();
	if (ecl.Compare(currentEcl) != 0)
	{
		attr->Checkout();
		attr->SetText(ecl);
		attr->Checkin(comments);
	}
}

const TCHAR * CWizardSprayData::GetSprayBuilderEcl(std::_tstring & ecl)
{
	{
		//  Create record structure  ---
		std::_tstring record;
		CComPtr<IEclRecord>ecl_record = CreateIEclRecord();
		m_analyzerInfo.GetEclRecord(ecl_record);

		std::_tstring temp;
		CComPtr<IEclIdentifier> lhs = CreateIEclIdentifier(_T("spray_record"));
		CComPtr<IEclIdentifier> rhs = CreateIEclIdentifier(ecl_record->Serialize(temp));
		CComPtr<IEclAssign> assignment = CreateIEclAssign(lhs,rhs);
		assignment->Serialize(record);
		record += _T(";\r\n");
		ecl += record;
	}

	{
		//  Create dataset structre  ---
		std::_tstring dataset;
		std::_tstring label, source, source2, format, destination;
		GetEclAttributeLabel(ATTRIBUTELABEL_DATASET, false, label);
		GetDestination(destination);
		source = (boost::_tformat(_T("'%1%'")) % destination).str();
		std::_tstring lz_path;
		source2 = (boost::_tformat(_T("'~file::%1%'")) % m_lz_path.GetEclPath(lz_path)).str();
		m_analyzerInfo.GetEclFormat(format, false);
		CComPtr<IEclValue> ecl_file = CreateIEclValue(_T("file_path"));
		CComPtr<IEclIdentifier> ecl_identifier = CreateIEclIdentifier(label);
		CComPtr<IEclValue> ecl_record = CreateIEclValue(_T("spray_record"));
		CComPtr<IEclDataset> ecl_dataset;
		if (boost::algorithm::iequals(format, _T("XML")))
		{
			ecl_dataset = CreateIEclDataset(ecl_file, ecl_record, ECLDATASETFORMAT_XML);
			std::_tstring xmlrowtag;
			m_analyzerInfo.GetXMLRowTag(xmlrowtag);
			xmlrowtag.insert(xmlrowtag.begin(),_T('\''));
			xmlrowtag.append(_T("'"));
			ecl_dataset->SetXmlXPath(xmlrowtag.c_str());
		}
		else if (boost::algorithm::iequals(format, _T("CSV")))
		{
			ecl_dataset = CreateIEclDataset(ecl_file, ecl_record, ECLDATASETFORMAT_CSV);
			ecl_dataset->SetCSVHeading(m_analyzerInfo.GetHeaderRows());
			ecl_dataset->SetCSVMaxLength(16*1204);
			StdString delim_raw, term_raw;
			m_analyzerInfo.GetDelimiter(delim_raw);
			m_analyzerInfo.GetTerminator(term_raw);
			CString delim, term;
			EncodeForEcl(delim_raw.c_str(), delim);
			EncodeForEcl(term_raw.c_str(), term);
			ecl_dataset->SetCSVSeparator(delim.GetString());
			ecl_dataset->SetCSVTerminator(term.GetString());
		}
		else
		{
			ecl_dataset = CreateIEclDataset(ecl_file, ecl_record, ECLDATASETFORMAT_FLAT);
		}
		CComPtr<IEclValue> ecl_source_file = CreateIEclValue(source2);
		ecl_dataset->SetFile(ecl_source_file);
		std::_tstring sourceDatasetStructure;
		ecl_dataset->Serialize(sourceDatasetStructure);

		CComPtr<IEclValue> ecl_target_file = CreateIEclValue(source);
		ecl_dataset->SetFile(ecl_target_file);
		std::_tstring targetDatasetStructure;
		ecl_dataset->Serialize(targetDatasetStructure);

		static const TCHAR * const tpl =
		_T("spray_source_dataset := %1%;\n")
		_T("spray_target_dataset := %2%;\n");
		dataset = (boost::_tformat(tpl) % sourceDatasetStructure % targetDatasetStructure).str();
		ecl += dataset;
	}

	{
		//  Create spray ecl  ---
		std::_tstring spray;
		std::_tstring cluster, destination;
		GetCluster(cluster);
		GetDestination(destination);
		std::_tstring ip = m_lz_path.GetIP();
		CString path;
		EncodeForEcl(m_lz_path.GetPath(), path);
		if (m_analyzerInfo.IsFixedFileFormat())
		{
			spray = (boost::_tformat(_T("%1% := lib_fileservices.FileServices.SprayFixed('%2%', %3%, sizeof(%4%), '%5%', '%6%', -1, lib_system.ws_fs_server, -1, true);\r\n")) % 
				_T("execute_spray") % 
				ip % 
				path.GetString() %
				_T("spray_record") % 
				cluster %
				destination).str();
		}
		else if(m_analyzerInfo.IsVariableFileFormat())
		{
			StdString seperator, terminator, quote;
			CString enc_seperator, enc_terminator, enc_quote;
			m_analyzerInfo.GetDelimiter(seperator);
			m_analyzerInfo.GetTerminator(terminator);
			m_analyzerInfo.GetQuote(quote);
			EncodeForEcl(seperator.c_str(), enc_seperator);
			EncodeForEcl(terminator.c_str(), enc_terminator);
			EncodeForEcl(quote.c_str(), enc_quote);
			spray =  (boost::_tformat(_T("%1% := lib_fileservices.FileServices.SprayVariable('%2%', %3%, 16384, %4%, %5%, %6%, '%7%', '%8%');\r\n")) % 
				_T("execute_spray") % 
				ip % 
				static_cast<const TCHAR *>(path) %
				enc_seperator.GetString() %
				enc_terminator.GetString() %
				enc_quote.GetString() %
				cluster %
				destination).str();
		}
		ecl += spray;
		ecl += 
			_T("//output(spray_source_dataset);\r\n")
			_T("//execute_spray;\r\n")
			_T("//output(spray_target_dataset);\r\n");
	}

	return ecl.c_str();
}
