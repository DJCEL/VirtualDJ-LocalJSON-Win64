#include "LocalJSON.h"

#define SAFE_DELETE(p) { if (p) {delete (p); (p) = NULL;} }
#define SAFE_DELETE_ARRAY(p) { if (p) {delete[] (p); (p) = NULL;} }


//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::OnLoad()
{
	ListVdjTracks = NULL;
	ListLocalJSONTracks = NULL;
	_ListVdjTracksSize = 0;
	_ListLocalJSONTracksSize = 0;
	UserIsLogged = true;


	char m_szpDLL[MAX_PATH + 1];	// The DLL filename
	char m_szpPath[MAX_PATH + 1];	// The DLL path

	// Get the DLL path and name (without extension)
	DWORD dwd = GetModuleFileName(hInstance, (char*)&m_szpPath, MAX_PATH);
	strcpy_s(m_szpDLL, m_szpPath);
	StripPath(m_szpPath);
	StripFileName(m_szpDLL);
	
	string DatabasePath = m_szpPath;
	string DatabaseFilename = m_szpDLL;

	DatabasePathFull = DatabasePath + DatabaseFilename + ".json";


	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::OnGetPluginInfo(TVdjPluginInfo8 *info)
{
	versionContextMenu = "[LocalJSON] v1.0 by djcel";

	info->PluginName = "LocalJSON";
	info->Author = "djcel";
	info->Version = "1.0";
	info->Description = "Use your own JSON file";
	info->Flags = 0x00;

	return S_OK;
}
//------------------------------------------------------------------------------------------
ULONG VDJ_API CLocalJSON::Release()
{	
	ClearListVdjTracks();
	ClearListLocalJSONTracks();
	UserIsLogged = false;

	delete this;
	return 0;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::GetFolderList(IVdjSubfoldersList* subfoldersList)
{
	if (UserIsLogged == false) return S_OK;
	
	subfoldersList->add("AllCatalog", "Local database");

	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::GetFolderContextMenu(const char* folderUniqueId, IVdjContextMenu* contextMenu)
{
	if (folderUniqueId == NULL) // Main Folder
	{
		contextMenu->add(versionContextMenu.c_str());
		contextMenu->add("[LocalJSON] Open ask.the.dj/djcel");
	}
	else if (strcmp(folderUniqueId, "AllCatalog") == 0)
	{
		contextMenu->add("[LocalJSON] Open the JSON file");
	}
	
	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::OnFolderContextMenu(const char* folderUniqueId, size_t menuIndex)
{
	bool bRes = false;
	string strFolderUniqueId;

	if (folderUniqueId == NULL) // Main Folder
	{
		if (menuIndex == 1)
		{
			string url = "https://virtualdj.com/ask/djcel";
			openBrowser(url);
		}
	}
	else if (strcmp(folderUniqueId, "AllCatalog") == 0)
	{
		if (menuIndex == 0)
		{
			OpenFileLocalJSON();
		}
	}

	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::GetFolder(const char* folderUniqueId, IVdjTracksList* tracksList)
{
	if (folderUniqueId == NULL) return S_OK;

	size_t i = 0;
	size_t Nb_Files = 0;
	VdjTrack item;
	bool bRes = false;

	bRes = ComputeListFilesFolder(folderUniqueId);
	Nb_Files = SizeOfListVdjTracks();

	if (bRes == false) return S_OK;
	if (Nb_Files==0 || ListVdjTracks == NULL) return S_OK;

	for (i = 0; i < Nb_Files; i++)
	{
		ZeroMemory(&item, sizeof(VdjTrack));
		item = (VdjTrack) (ListVdjTracks[i]);

		//tracksList->add(item.uniqueId.c_str(), item.title.c_str(), item.artist.c_str(), item.remix.c_str(), item.genre.c_str(), item.label.c_str(), item.comment.c_str(), item.coverUrl.c_str(), item.streamUrl.c_str(), item.length, item.bpm, item.key, item.year, item.isVideo, item.isKaraoke);
		tracksList->add(item.uniqueId.c_str(), item.title.c_str(), item.artist.c_str(), item.remix.c_str(), item.genre.c_str(), item.label.c_str(), item.comment.c_str(), item.coverUrl.c_str(), nullptr, item.length, item.bpm, item.key, item.year, item.isVideo, item.isKaraoke);
	}
	
	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::GetContextMenu(const char* uniqueId, IVdjContextMenu* contextMenu)
{
	
	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::OnContextMenu(const char* uniqueId, size_t menuIndex)
{
	
	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::GetStreamUrl(const char* uniqueId, IVdjString& Vdjurl, IVdjString& errorMessage)
{
	if (UserIsLogged == false)
	{
		errorMessage = "Please Sign in first";
		return S_OK;
	}

	VdjTrack track;
	size_t idx;

	idx = GetVdjTrackIndexFromUniqueId(uniqueId);
	if (idx == 0)
	{
		errorMessage = "StreamUrl not found";
		return S_OK;
	}

	idx = idx - 1;


	string streamUrl = ListVdjTracks[idx].streamUrl;
	if (streamUrl == "")
	{
		errorMessage = "StreamUrl not found";
		return S_OK;
	}

	if (streamUrl == "https://url1b.mp3" || streamUrl == "https://url2b.mp3")
	{
		errorMessage = "Open and edit the JSON file first";
		return S_OK;
	}

	Vdjurl = streamUrl.c_str();
	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::OnSearch(const char* search, IVdjTracksList* tracksList)
{
	if (search == NULL) return S_OK;

	VdjTrack item;
	size_t i = 0;
	size_t Nb_Files = 0;
	bool bRes = false;

	string search_fixed = search;
	bRes = ComputeListFilesSearch(search_fixed);
	if (bRes == false || ListVdjTracks == NULL) return S_OK;

	Nb_Files = SizeOfListVdjTracks();
	if (Nb_Files == 0) return S_OK;

	for (i = 0; i < Nb_Files; i++)
	{
		item = (VdjTrack)(ListVdjTracks[i]);

		//tracksList->add(item.uniqueId.c_str(), item.title.c_str(), item.artist.c_str(), item.remix.c_str(), item.genre.c_str(), item.label.c_str(), item.comment.c_str(), item.coverUrl.c_str(), item.streamUrl.c_str(), item.length, item.bpm, item.key, item.year, item.isVideo, item.isKaraoke);
		tracksList->add(item.uniqueId.c_str(), item.title.c_str(), item.artist.c_str(), item.remix.c_str(), item.genre.c_str(), item.label.c_str(), item.comment.c_str(), item.coverUrl.c_str(), nullptr, item.length, item.bpm, item.key, item.year, item.isVideo, item.isKaraoke);
	}

	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CLocalJSON::OnSearchCancel()
{

	return S_OK;
}
//------------------------------------------------------------------------------------------
// Additional Functions
//------------------------------------------------------------------------------------------
void CLocalJSON::openBrowser(string url)
{
	ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::CreateListVdjTracks(size_t NumberOfTracks)
{
	bool bRes = false;

	bRes = ClearListVdjTracks();

	if (NumberOfTracks == 0) return false;

	ListVdjTracks = new VdjTrack[NumberOfTracks];

	_ListVdjTracksSize = NumberOfTracks;

	return true;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::ClearListVdjTracks()
{
	size_t Nb_Files = 0;
	size_t i = 0;

	if (ListVdjTracks == NULL) return false;

	Nb_Files = SizeOfListVdjTracks();
	VdjTrack* pTrack;

	// we use that first (before safe_delete_array) to manage an exception (maybe an end of string missing).
	for (i = 0; i < Nb_Files; i++)
	{
		pTrack = &(ListVdjTracks[i]);
		ZeroMemory(pTrack, sizeof(VdjTrack));
		//SAFE_DELETE(pTrack);
	}

	SAFE_DELETE_ARRAY(ListVdjTracks);
	_ListVdjTracksSize = 0;

	return true;
}
//------------------------------------------------------------------------------------------
size_t CLocalJSON::SizeOfListVdjTracks()
{
	return _ListVdjTracksSize;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::CreateListLocalJSONTracks(size_t NumberOfTracks)
{
	bool bRes = false;

	bRes = ClearListLocalJSONTracks();

	if (NumberOfTracks == 0) return false;

	ListLocalJSONTracks = new VdjTrack[NumberOfTracks];

	_ListLocalJSONTracksSize = NumberOfTracks;

	return true;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::ClearListLocalJSONTracks()
{
	size_t Nb_Files = 0;
	size_t i = 0;

	if (ListLocalJSONTracks == NULL) return false;

	Nb_Files = SizeOfListLocalJSONTracks();
	VdjTrack* pTrack;

	// we use that first (before safe_delete_array) to manage an exception (maybe an end of string missing).
	for (i = 0; i < Nb_Files; i++)
	{
		pTrack = &(ListLocalJSONTracks[i]);
		ZeroMemory(pTrack, sizeof(VdjTrack));
		//SAFE_DELETE(pTrack);
	}

	SAFE_DELETE_ARRAY(ListLocalJSONTracks);
	_ListLocalJSONTracksSize = 0;

	return true;
}
//------------------------------------------------------------------------------------------
size_t CLocalJSON::SizeOfListLocalJSONTracks()
{
	return _ListLocalJSONTracksSize;
}

//------------------------------------------------------------------------------------------
bool CLocalJSON::ComputeListFilesFolder(const char* folderUniqueId)
{
	if (folderUniqueId == NULL) return false;

	bool bRes = false;
	
	if (strcmp(folderUniqueId, "AllCatalog") == 0)
	{
		bRes = LoadLocalDatabase();
		if (bRes == false) return false;

		bRes = DisplayLocalJSON();
		if (bRes == false) return false;
	}

	return bRes;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::ComputeListFilesSearch(string search)
{
	bool bRes = false;
	
	bRes = LoadLocalDatabase();
	if (bRes == false) return false;

	bRes = SearchLocalJSON(search);
	if (bRes == false) return false;
	
	return bRes;
}
//------------------------------------------------------------------------------------------
size_t CLocalJSON::GetVdjTrackIndexFromUniqueId(const char* uniqueId)
{
	size_t idx = 0;

	if (uniqueId == NULL) return idx;
	if (uniqueId == "") return idx;

	size_t Nb_Files = 0;
	size_t i;
	const char* uniqueIdfromlist = "";
	VdjTrack searchtrack;
	string uid;

	Nb_Files = SizeOfListVdjTracks();
	if (Nb_Files == 0 || ListVdjTracks == NULL) return idx;

	for (i = 0; i < Nb_Files; i++)
	{
		ZeroMemory(&searchtrack, sizeof(VdjTrack));
		searchtrack = (VdjTrack)(ListVdjTracks[i]);

		uid = searchtrack.uniqueId;
		uniqueIdfromlist = uid.c_str();

		if (strcmp(uniqueIdfromlist, uniqueId) == 0)
		{
			idx = i + 1;
			break;
		}
	}

	return idx;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::IsFileExists(const char* filepath)
{
	bool bRes = false;

	DWORD dwAttrib = GetFileAttributes(filepath);
	DWORD dwError = GetLastError();

	if ((dwAttrib == INVALID_FILE_ATTRIBUTES) && (dwError == ERROR_FILE_NOT_FOUND)) return false;
	else return true;
}
//---------------------------------------------------------------------------------------
void CLocalJSON::StripFileName(char* lpszPath, BOOL bStripExtension, BOOL bStripFolders)
{
	if (lpszPath && *lpszPath && (bStripExtension || bStripFolders))
	{
		LPSTR	lpszCurrent, lpszEnd,
			lpszSlash = NULL,
			lpszDot = NULL;

		for (lpszCurrent = lpszPath; *lpszCurrent; ++lpszCurrent)
		{
			if (*lpszCurrent == '\\' || *lpszCurrent == '/')
				lpszSlash = ++lpszCurrent;
			if (*lpszCurrent == '.')
				lpszDot = lpszCurrent;
		}
		lpszEnd = lpszCurrent;

		if (bStripExtension && lpszDot && lpszDot > lpszSlash)
		{
			lpszEnd = lpszDot;
			*lpszDot = '\0';
		}

		lpszCurrent = ((bStripFolders && lpszSlash) ? lpszSlash : lpszPath);
		for (; lpszCurrent <= lpszEnd; ++lpszPath, ++lpszCurrent)
			*lpszPath = *lpszCurrent;
	}
}
//---------------------------------------------------------------------------------------
void CLocalJSON::StripPath(char* lpszPath)
{
	if (lpszPath && *lpszPath)
	{
		LPSTR	lpszCurrent,
			lpszSlash = NULL,
			lpszDot = NULL;

		for (lpszCurrent = lpszPath; *lpszCurrent; ++lpszCurrent)
			if (*lpszCurrent == '\\' || *lpszCurrent == '/')
				lpszSlash = ++lpszCurrent;

		if (lpszSlash)
			*lpszSlash = '\0';
	}
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::LoadLocalDatabase()
{
	bool bRes = false;

	bRes = IsFileExists(DatabasePathFull.c_str());
	if (bRes == false)
	{
		bRes = SaveFileTemplateLocalJSON();
		if (bRes == false) return false;
	}

	bRes = LoadFileLocalJSON();
	if (bRes == false) return false;

	return true;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::LoadFileLocalJSON()
{
	Json::Reader json_reader;
	Json::Value jsonData;
	Json::Value jsonSong;
	bool bRes = false;
	size_t Nb_Files = 0;
	unsigned int i;

	if (DatabasePathFull == "") return false;

	std::ifstream file_input(DatabasePathFull);

	bRes = json_reader.parse(file_input, jsonData);
	if (bRes == false) return false;

	Nb_Files = jsonData.size();
	if (Nb_Files == 0) return false;

	bRes = CreateListLocalJSONTracks(Nb_Files);
	if (bRes == false || ListLocalJSONTracks == NULL) return false;

	Nb_Files = SizeOfListLocalJSONTracks();
	if (Nb_Files == 0) return false;

	for (i = 0; i < Nb_Files; i++)
	{
		jsonSong = jsonData[i];

		ListLocalJSONTracks[i].uniqueId = jsonSong["uniqueId"].asString();
		ListLocalJSONTracks[i].title = jsonSong["title"].asString();
		ListLocalJSONTracks[i].artist = jsonSong["artist"].asString();
		ListLocalJSONTracks[i].remix = jsonSong["remix"].asString();
		ListLocalJSONTracks[i].genre = jsonSong["genre"].asString();
		ListLocalJSONTracks[i].label = jsonSong["label"].asString();
		ListLocalJSONTracks[i].comment = jsonSong["comment"].asString();
		ListLocalJSONTracks[i].coverUrl = jsonSong["coverUrl"].asString();
		ListLocalJSONTracks[i].streamUrl = jsonSong["streamUrl"].asString();
		ListLocalJSONTracks[i].length = jsonSong["length"].asFloat();
		ListLocalJSONTracks[i].bpm = jsonSong["bpm"].asFloat();
		ListLocalJSONTracks[i].key = jsonSong["key"].asInt();
		ListLocalJSONTracks[i].year = jsonSong["year"].asInt();
		ListLocalJSONTracks[i].isVideo = jsonSong["isVideo"].asBool();
		ListLocalJSONTracks[i].isKaraoke = jsonSong["isKaraoke"].asBool();
	}

	return true;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::DisplayLocalJSON()
{
	bool bRes = false;
	size_t Nb_Files_LocalJSON = 0;
	size_t Nb_Files = 0;
	unsigned int i;

	Nb_Files_LocalJSON = SizeOfListLocalJSONTracks();
	if (Nb_Files_LocalJSON == 0) return false;

	bRes = CreateListVdjTracks(Nb_Files_LocalJSON);
	if (bRes == false || ListVdjTracks == NULL) return false;

	Nb_Files = SizeOfListVdjTracks();
	if (Nb_Files == 0) return false;

	for (i = 0; i < Nb_Files; i++)
	{
		ListVdjTracks[i].uniqueId = ListLocalJSONTracks[i].uniqueId;
		ListVdjTracks[i].title = ListLocalJSONTracks[i].title;
		ListVdjTracks[i].artist = ListLocalJSONTracks[i].artist;
		ListVdjTracks[i].remix = ListLocalJSONTracks[i].remix;
		ListVdjTracks[i].genre = ListLocalJSONTracks[i].genre;
		ListVdjTracks[i].label = ListLocalJSONTracks[i].label;
		ListVdjTracks[i].comment = ListLocalJSONTracks[i].comment;
		ListVdjTracks[i].coverUrl = ListLocalJSONTracks[i].coverUrl;
		ListVdjTracks[i].streamUrl = ListLocalJSONTracks[i].streamUrl;
		ListVdjTracks[i].length = ListLocalJSONTracks[i].length;
		ListVdjTracks[i].bpm = ListLocalJSONTracks[i].bpm;
		ListVdjTracks[i].key = ListLocalJSONTracks[i].key;
		ListVdjTracks[i].year = ListLocalJSONTracks[i].year;
		ListVdjTracks[i].isVideo = ListLocalJSONTracks[i].isVideo;
		ListVdjTracks[i].isKaraoke = ListLocalJSONTracks[i].isKaraoke;
	}

	bRes = true;

	return bRes;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::SearchLocalJSON(string search)
{
	size_t Nb_Files = 0;

	Nb_Files = SizeOfListLocalJSONTracks();
	if (Nb_Files == 0 || ListLocalJSONTracks == NULL) return false;

	size_t search_length = search.length();
	if (search_length < 2) return false;

	std::vector<size_t> ListResultSearch;
	std::transform(search.begin(), search.end(), search.begin(), ::toupper);

	size_t fd1 = 0;
	size_t fd2 = 0;
	string title;
	string artist;

	for (int p = 0; p < Nb_Files; p++)
	{
		title = ListLocalJSONTracks[p].title;
		artist = ListLocalJSONTracks[p].artist;
		std::transform(title.begin(), title.end(), title.begin(), ::toupper);
		std::transform(artist.begin(), artist.end(), artist.begin(), ::toupper);

		fd1 = title.find(search);
		fd2 = artist.find(search);
		if (fd1 != string::npos || fd2 != string::npos)
		{
			ListResultSearch.push_back(p);
		}
	}

	size_t Nb_Files_Search = ListResultSearch.size();
	if (Nb_Files_Search == 0) return false;

	bool bRes = CreateListVdjTracks(Nb_Files_Search);
	if (bRes == false || ListVdjTracks == NULL) return false;

	Nb_Files_Search = SizeOfListVdjTracks();
	if (Nb_Files_Search == 0) return false;

	size_t i = 0;
	size_t idx = 0;

	for (int p = 0; p < ListResultSearch.size(); p++)
	{
		idx = ListResultSearch[p];

		ListVdjTracks[i].uniqueId = ListLocalJSONTracks[idx].uniqueId;
		ListVdjTracks[i].title = ListLocalJSONTracks[idx].title;
		ListVdjTracks[i].artist = ListLocalJSONTracks[idx].artist;
		ListVdjTracks[i].remix = ListLocalJSONTracks[idx].remix;
		ListVdjTracks[i].genre = ListLocalJSONTracks[idx].genre;
		ListVdjTracks[i].label = ListLocalJSONTracks[idx].label;
		ListVdjTracks[i].comment = ListLocalJSONTracks[idx].comment;
		ListVdjTracks[i].coverUrl = ListLocalJSONTracks[idx].coverUrl;
		ListVdjTracks[i].streamUrl = ListLocalJSONTracks[idx].streamUrl;
		ListVdjTracks[i].length = ListLocalJSONTracks[idx].length;
		ListVdjTracks[i].bpm = ListLocalJSONTracks[idx].bpm;
		ListVdjTracks[i].key = ListLocalJSONTracks[idx].key;
		ListVdjTracks[i].year = ListLocalJSONTracks[idx].year;
		ListVdjTracks[i].isVideo = ListLocalJSONTracks[idx].isVideo;
		ListVdjTracks[i].isKaraoke = ListLocalJSONTracks[idx].isKaraoke;

		i++;
	}


	return true;
}
//------------------------------------------------------------------------------------------
bool CLocalJSON::SaveFileTemplateLocalJSON()
{
	bool bRes = false;

	if (DatabasePathFull == "") return false;

	std::ofstream file_id;
	Json::StreamWriterBuilder builder;
	builder.settings_["emitUTF8"] = true;
	std::unique_ptr<Json::StreamWriter> json_writer(builder.newStreamWriter());
	Json::Value json_root(Json::arrayValue);
	
	Json::Value json_song1;
	json_song1["uniqueId"] = "1";
	json_song1["artist"] = "artist1";
	json_song1["title"] = "title1";
	json_song1["remix "] = "";
	json_song1["genre"] = "";
	json_song1["label"] = "";
	json_song1["comment"] = "";
	json_song1["coverUrl"] = "https://url1a.jpg";
	json_song1["streamUrl"] = "https://url1b.mp3";
	json_song1["length"] = 0.0;
	json_song1["bpm"] = 0.0;
	json_song1["key"] = 0;
	json_song1["year"] = 0;
	json_song1["isVideo"] = false;
	json_song1["isKaraoke"] = false;
	
	json_root.append(json_song1);

	Json::Value json_song2;
	json_song2["uniqueId"] = "2";
	json_song2["artist"] = "artist2";
	json_song2["title"] = "title2";
	json_song2["remix "] = "";
	json_song2["genre"] = "";
	json_song2["label"] = "";
	json_song2["comment"] = "";
	json_song2["coverUrl"] = "https://url2a.jpg";
	json_song2["streamUrl"] = "https://url2b.mp3";
	json_song2["length"] = 0.0;
	json_song2["bpm"] = 0.0;
	json_song2["key"] = 0;
	json_song2["year"] = 0;
	json_song2["isVideo"] = false;
	json_song2["isKaraoke"] = false;

	json_root.append(json_song2);

	if (json_root.size() != 0)
	{
		file_id.open(DatabasePathFull);
		json_writer->write(json_root, &file_id);
		file_id.close();
		bRes = true;
	}

	return bRes;
}
//------------------------------------------------------------------------------------------
void CLocalJSON::OpenFileLocalJSON()
{
	if (DatabasePathFull == "") return;
	ShellExecute(NULL, "open", DatabasePathFull.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
