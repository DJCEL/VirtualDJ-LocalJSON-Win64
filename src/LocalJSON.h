#ifndef LOCALJSON_H
#define LOCALJSON_H


#include "vdjOnlineSource.h"


// we load jsoncpp
#include <json/json.h>


#include <string>
//#include <list>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>	
#include <shellapi.h> // for ShellExecute()

using namespace std;

struct VdjTrack
{
	string uniqueId;
	string title;
	string artist;
	string remix;
	string genre;
	string label;
	string comment;
	string coverUrl;
	string streamUrl;
	float length = 0;
	float bpm = 0;
	int key = 0;
	int year = 0;
	bool isVideo = false;
	bool isKaraoke = false;
};

//////////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////////

class CLocalJSON : public IVdjPluginOnlineSource
{
public:
	HRESULT VDJ_API OnLoad() override;
	HRESULT VDJ_API OnGetPluginInfo(TVdjPluginInfo8* info) override;
	ULONG   VDJ_API Release() override;
	HRESULT VDJ_API GetContextMenu(const char* uniqueId, IVdjContextMenu* contextMenu) override;
	HRESULT VDJ_API OnContextMenu(const char* uniqueId, size_t menuIndex) override;
	HRESULT VDJ_API GetFolderList(IVdjSubfoldersList* subfoldersList) override;
	HRESULT VDJ_API GetFolder(const char* folderUniqueId, IVdjTracksList* tracksList) override;
	HRESULT VDJ_API GetFolderContextMenu(const char* folderUniqueId, IVdjContextMenu* contextMenu) override;
	HRESULT VDJ_API OnFolderContextMenu(const char* folderUniqueId, size_t menuIndex) override;
	HRESULT VDJ_API OnSearch(const char* search, IVdjTracksList* tracksList) override;
	HRESULT VDJ_API OnSearchCancel() override;
	HRESULT VDJ_API GetStreamUrl(const char* uniqueId, IVdjString& Vdjurl, IVdjString& errorMessage) override;

private:

	void openBrowser(string url);
	bool CreateListVdjTracks(size_t NumberOfTracks);
	bool CreateListLocalJSONTracks(size_t NumberOfTracks);
	bool ClearListVdjTracks();
	bool ClearListLocalJSONTracks();
	size_t SizeOfListVdjTracks();
	size_t SizeOfListLocalJSONTracks();
	size_t GetVdjTrackIndexFromUniqueId(const char* uniqueId);
	bool IsFileExists(const char* filepath);
	void StripFileName(char* lpszPath, BOOL bStripExtension = TRUE, BOOL bStripFolders = TRUE);
	void StripPath(char* lpszPath);
	bool ComputeListFilesSearch(string search);
	bool ComputeListFilesFolder(const char* folderUniqueId);
	bool LoadFileLocalJSON();
	bool DisplayLocalJSON();
	bool SearchLocalJSON(string search);
	bool SaveFileTemplateLocalJSON();
	void OpenFileLocalJSON();
	bool LoadLocalDatabase();

	string versionContextMenu;
	bool UserIsLogged;
	string DatabasePathFull;
	VdjTrack* ListVdjTracks = nullptr;
	VdjTrack* ListLocalJSONTracks = nullptr;
	size_t _ListVdjTracksSize;
	size_t _ListLocalJSONTracksSize;
};

#endif