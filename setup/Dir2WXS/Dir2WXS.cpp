// Dir2CMake.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

namespace fs = boost::filesystem;
typedef std::vector<fs::path> PathVector;

const char * const CMakelistFile = 
"project( %1% )\n"
"\n"
"set ( SRCS\n"
"%2%"
"\t)\n"
"\n"
"include_directories (\n"
"\t)\n"
"\n"
"if (Boost_FOUND)\n"
"\tinclude_directories ( ${Boost_INCLUDE_DIRS} )\n"
"\tlink_directories ( ${Boost_LIBRARY_DIRS} )\n"
"endif (Boost_FOUND)\n"
"\n"
"add_library ( %1% \n"
"\t\t${SRCS}\n"
"\t)\n"
"%3%\n";

const char * const SrcLine = "\t\t%1%\n";
const char * const SubDirLine = "add_subdirectory ( %1% )\n";

const char * const FragmentXML = "<?xml version='1.0' encoding='UTF-8'?><Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'><Fragment>%1%</Fragment><Fragment>%2%</Fragment><Fragment>%3%</Fragment></Wix>";
const char * const FolderRefXML = "<DirectoryRef Id='%1%%2%_FOLDER'>%3%</DirectoryRef>";
const char * const FolderXML = "<Directory Id='%1%%2%_FOLDER' Name='%3%'>%4%</Directory>";
const char * const ComponentXML = "<Component Id='%1%%2%_Component' Guid='{%3%}'>%4%</Component>";
const char * const ComponentRefXML = "<ComponentRef Id='%1%%2%_Component' />";
const char * const FileXML = "<File Id='%1%_File' Name='%2%' DiskId='1' Source='%3%' />";
const char * const FeatureXML = "<Feature Id='%1%%2%_Feature' Title='%3%' Level='1'>%4%</Feature>";

class FileFolder
{
protected:

public:
	FileFolder(const fs::path & path) : m_path(path)
	{
		m_isFolder = fs::is_directory(m_path);
		m_exists = fs::exists(m_path);
		m_name = m_path.filename();
		m_ext = fs::extension(m_path);
		m_hasChildFolder = false;

		m_id = m_name;//m_path.stem();
		boost::algorithm::replace_all(m_id, "-", "_");
		boost::algorithm::replace_all(m_id, " ", "_");

		if (m_isFolder)
		{
			GUID *pguid = 0x00;
			pguid = new GUID; 
			CoCreateGuid(pguid); 

			_TUCHAR *guidStr = 0x00;
			UuidToString(pguid, &guidStr);
			m_guid = (char *)guidStr;
			RpcStringFree(&guidStr);

			delete pguid; 	
		}
	}

	const fs::path m_path;

	std::string m_id;
	std::string m_name;
	std::string m_ext;
	std::string m_guid;
	bool m_isFolder;
	bool m_exists;
	bool m_hasChildFolder;
};
typedef tree<FileFolder> FileFolderTree;

void walkFolders(const FileFolderTree::iterator & node, FileFolderTree & files)
{
	bool retVal = false;
	try
	{
		std::string fileList, childList;
		for(fs::directory_iterator ditr = fs::directory_iterator(node->m_path); ditr != fs::directory_iterator(); ++ditr)
		{
			FileFolderTree::iterator child = files.append_child(node, ditr->path());
			if (child->m_isFolder)
			{
				node->m_hasChildFolder = true;
				walkFolders(child, files);
			}
		}
	}
	catch (const fs::filesystem_error& ex)
	{
		std::cout << ex.what() << '\n';
	}
}

static std::map<std::string, int> dups;
void getXML(const FileFolderTree & files, const FileFolderTree::iterator & node, std::string parentID, std::string & folderXML, std::string & featureXML, std::string & componentXML, std::string & componentRefXML, std::string & fileXML)
{
	if (node->m_isFolder)
	{
		if (boost::iequals(node->m_id, "PDB"))
			return;

		std::string childFolderXML, childFileXML;
		for (unsigned int i = 0; i < node.number_of_children(); ++i)
		{
			FileFolderTree::iterator child = files.child(node, i);
			std::string xml;
			getXML(files, child, files.depth(node) > 0 ? (parentID + node->m_id) : "", childFolderXML, featureXML, componentXML, componentRefXML, childFileXML);

			switch (files.depth(node))
			{
			case 1:
				if (!componentRefXML.empty())
				{
					featureXML += (boost::format(FeatureXML) % 
						node->m_id %
						child->m_id % 
						child->m_name % 
						componentRefXML).str();
					componentRefXML = "";
				}
				break;
			}
		}

		switch (files.depth(node))
		{
		case 0:
			folderXML = childFolderXML;
			break;
		case 1:
			folderXML += (boost::format(FolderRefXML) % 
				parentID %
				node->m_id % 
				childFolderXML).str();
			break;
		default:
			folderXML += (boost::format(FolderXML) % 
				parentID %
				node->m_id % 
				node->m_name % 
				childFolderXML
				).str();
			break;
		}

		if (!childFileXML.empty())
		{
			componentXML += (boost::format(FolderRefXML) % 
				parentID %
				node->m_id % 
					(	
					boost::format(ComponentXML) % 
					parentID %
					node->m_id % 
					node->m_guid % 
					childFileXML
					)
				).str();

			componentRefXML += (boost::format(ComponentRefXML) % 
				parentID %
				node->m_id
				).str(); 
		}
	}
	else if (!boost::algorithm::iequals(node->m_name, "npHPCCSystemsGraphViewControl.dll"))
	{
		std::string id = node->m_id + (++dups[node->m_id] > 1 ? boost::lexical_cast<std::string>(dups[node->m_id]) : "");
		fileXML += (boost::format(FileXML) % 
			id %
			node->m_name % 
			node->m_path.native_file_string()).str();
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc <= 1)
		return 0;

	fs::path rootPath(argv[1]);
	if (!exists(rootPath) || !is_directory(rootPath))
		return 0;

	FileFolderTree files;
	FileFolderTree::iterator top, root;
	top = files.begin();
	root = files.insert(top, rootPath);

	walkFolders(root, files);
	std::string folderXML, featureXML, componentXML, componentRefXML, fileXML;
	getXML(files, root, "", folderXML, featureXML, componentXML, componentRefXML, fileXML);
	std::cout << (boost::format(FragmentXML) % folderXML % componentXML % featureXML).str();

	return 0;
}

