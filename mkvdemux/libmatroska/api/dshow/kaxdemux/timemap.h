/*
** CTimeMap class
**
** this class buffers timecodes (of clusters) and returns the appropriate
** timecodes upon request. (for searching within streams and files containing
** no seek information).
**
*/


#ifndef __TIMEMAP_H__
#define __TIMEMAP_H__

struct TMEntry {
	TMEntry	*pnext;
	TMEntry	*pprev;

	uint64	pos;
	uint32	cluster_time;
};

class CTimeMap {
private:
	TMEntry	*pBase, *pCurr;

public:
	CTimeMap();
	~CTimeMap();

	int Add(uint64 pos, uint32 cluster_time);
	uint64 GetPos(uint32 timecode);
};

#endif __TIMEMAP_H__
