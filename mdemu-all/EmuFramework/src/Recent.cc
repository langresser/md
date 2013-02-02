#include <Recent.hh>

static DLList<RecentGameInfo>::Node recentGameListNode[10];
DLList<RecentGameInfo> recentGameList(recentGameListNode);

void recent_addGame(const char *fullPath, const char *name)
{
	RecentGameInfo recent;
	string_copy(recent.path, fullPath, sizeof(recent.path));
	string_copy(recent.name, name, sizeof(recent.name));
	if(recentGameList.contains(recent)) // remove existing entry so it's added to the front
		recentGameList.remove(recent);
	else if(recentGameList.size == 10) // list full
		recentGameList.removeLast();

	if(!recentGameList.add(recent))
	{
		bug_exit("error adding item to recent games list");
	}

	/*logMsg("list contents:");
	forEachInDLList(&recentGameList, e)
	{
		logMsg("path: %s name: %s", e.path, e.name);
	}*/
}
