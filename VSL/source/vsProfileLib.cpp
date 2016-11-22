/** ----------------------------------------------------------
 * \class VSProfileLib
 *
 * Lighthouse3D
 *
 * VSProfileLib - Very Simple Profile Library
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs 
 *
 * Simple profiler class for both CPU and GPU
 *
 * \version 0.2.0
 * uses OpenGL timers
 *
 * version 0.1.0
 * Initial Release
 *
 ---------------------------------------------------------------*/

#include "vsProfileLib.h"

#include <GL/glew.h>
#include <ctime>
#include <stdio.h>


// Declare static variables
std::string VSProfileLib::sDump;
int VSProfileLib::sDisp = 0;
int VSProfileLib::sCurrLevel = -1;
int VSProfileLib::sTotalLevels = 0;
VSProfileLib::level VSProfileLib::sLevels[PROFILE_MAX_LEVELS];
unsigned int VSProfileLib::sBackBuffer = 0;
unsigned int VSProfileLib::sFrontBuffer = 1;

#if VSPL_CLOCK == VSPL_WIN_HIGH_PERFORMANCE_COUNTER
LARGE_INTEGER VSProfileLib::sFreq;
#endif

// Timer function defined apart for easier replacement
void 
VSProfileLib::GetTicks(pTime *ticks) {

#if VSPL_CLOCK == VSPL_WIN_HIGH_PERFORMANCE_COUNTER
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t) ;
	*ticks = 1000.0*(pTime)((double)t.QuadPart/
								(double)sFreq.QuadPart);

#elif VSPL_CLOCK == VSPL_C_CLOCK
	*ticks =  (pTime)clock()/VSPL_CLOCK_RATE;

#elif VSPL_CLOCK == VSPL_WIN_SYSTEMTIME
	SYSTEMTIME systemTime;
	GetSystemTime( &systemTime );

	FILETIME fileTime;
	SystemTimeToFileTime( &systemTime, &fileTime );

	ULARGE_INTEGER uli;
	uli.LowPart = fileTime.dwLowDateTime; 
	uli.HighPart = fileTime.dwHighDateTime;

	ULONGLONG systemTimeIn_ms( uli.QuadPart/10000 );
	*ticks = (double)systemTimeIn_ms;

#elif VSPL_CLOCK == VSPL_GETTIMEOFDAY
	timeval t2;
	gettimeofday(&t2,NULL);
	*ticks = t2.tv_sec * 1000.0 + t2.tv_usec / 1000.0;     
#endif
}


// Constructor
// begin of a profile section
VSProfileLib::VSProfileLib(std::string name, bool profileGL) {

	int found;
	pTime w;
	sCurrLevel++;

#if VSPL_CLOCK == VSPL_WIN_HIGH_PERFORMANCE_COUNTER
	QueryPerformanceFrequency(&sFreq);
#endif

	GetTicks(&w);

	// create new level
	if (sCurrLevel == sTotalLevels) {

		sLevels[sCurrLevel].cursor = -1;
		createNewSection(name, w, profileGL);
		// store the size of the largest section name
		int aux = (int)name.size() ;
		if (aux > sDisp)
			sDisp = aux;
		sTotalLevels++;
	}
	else {  	
		// search for name and parent
		found = searchSection(name);
		if (found != -1)
			updateSection(found, w);
		else {
			// create new section inside current level
			createNewSection(name, w, profileGL);
			// store the size of the largest section name
			// for report formatting purposes
			int aux = (int)name.size() ;
			if (aux > sDisp)
				sDisp = aux;
		}
	}

}

// End Section
VSProfileLib::~VSProfileLib() {
	// add the time spent in the current section
	accumulate();
	// decrease current level
	sCurrLevel--;
}


//////////////////////////////////////////////////////////////////////
// Instance Methods
//////////////////////////////////////////////////////////////////////

// Create a new profile section
void VSProfileLib::createNewSection(std::string &name, pTime w, bool profileGL) {

	section s;

#if VSPL_PROFILE == VSPL_PROFILE_CPU_AND_GPU
	s.profileGL = profileGL;
#else
	s.profileGL = false;
#endif
	s.parent = (sCurrLevel > 0 ? 
		sLevels[sCurrLevel-1].cursor : -1);
	s.name = name;
	s.calls = 1;
	s.totalTime = 0;
	s.totalQueryTime = 0;
	
	sLevels[sCurrLevel].cursor++;

	if (profileGL) {
		pair p;
		glGenQueries(2, p.queries);
		glQueryCounter(p.queries[0], GL_TIMESTAMP);
		s.queriesGL[sBackBuffer].push_back(p);
	}

	GetTicks(&(s.startTime));
	s.wastedTime = s.startTime - w;
	sLevels[sCurrLevel].sec.push_back(s);
}

// Serach for a profile section
int VSProfileLib::searchSection(std::string &name) {

	int i,max,par;

	max = (int)sLevels[sCurrLevel].sec.size();
	par = (sCurrLevel==0 ? -1 : sLevels[sCurrLevel-1].cursor);

	for(i=0;i<max;i++) {
		if (( name == sLevels[sCurrLevel].sec[i].name)  && 
			(par == sLevels[sCurrLevel].sec[i].parent))
			return(i);
	}
	return(-1);
}

// updates a profile section
void VSProfileLib::updateSection(int cur, pTime w) {

	section *s;

	s = &(sLevels[sCurrLevel].sec[cur]);
	s->calls++;
	sLevels[sCurrLevel].cursor = cur;

	if (s->profileGL) {
		pair p;
		glGenQueries(2, p.queries);
		glQueryCounter(p.queries[0], GL_TIMESTAMP);
		s->queriesGL[sBackBuffer].push_back(p);
	}
	GetTicks(&s->startTime);
	s->wastedTime += s->startTime - w;
}


// accumulates the time spent in the section
void VSProfileLib::accumulate() {

	section *s;
	pTime t,t2;
	GetTicks(&t);

	s = &(sLevels[sCurrLevel].sec[sLevels[sCurrLevel].cursor]);

	if (s->profileGL) {
		glQueryCounter(s->queriesGL[sBackBuffer][s->queriesGL[sBackBuffer].size()-1].queries[1], GL_TIMESTAMP);
	}
	// to measure wasted time when accumulating
	GetTicks(&t2);
	s->wastedTime += (t2-t);
	s->totalTime += (t - s->startTime);

}


//////////////////////////////////////////////////////////////////////
// Class Methods
//////////////////////////////////////////////////////////////////////

// Resets profile stats
void VSProfileLib::Reset() {

	for(int i=0; i < sTotalLevels; ++i) {

		for (unsigned int s = 0; s < sLevels[i].sec.size(); ++s) {
			for (unsigned int k = 0; k < sLevels[i].sec[s].queriesGL[0].size(); ++k) {
				glDeleteQueries(2, sLevels[i].sec[s].queriesGL[0][k].queries);
			}
			for (unsigned int k = 0; k < sLevels[i].sec[s].queriesGL[1].size(); ++k) {
				glDeleteQueries(2, sLevels[i].sec[s].queriesGL[1][k].queries);
			}
		}
		sLevels[i].sec.clear();
	}
	sTotalLevels = 0;
}



// Builds a string, sDump, with the profile report
const std::string &
VSProfileLib::DumpLevels() {

#if VSPL_PROFILE != VSPL_PROFILE_NONE
	int indent = sTotalLevels * LEVEL_INDENT + sDisp;
	char saux[100];
		
	char t1[5]="Name";
	char t2[7]="#c";
	char t3[9]="#tc";
	char t4[8]="CPU(ms)";
	char t41[8] ="GPU(ms)";
	char t5[3]="wt";

	sDump = "";
	sprintf(saux,"%-*s  %s  %s  %s       %s\n",indent+4,t1,t2,t4,t41,t5);
	sDump += saux; 
	sprintf(saux,"---- %*s\n",indent+31,"------------------------------------");
	sDump += saux;

	DumpLevels(0,-1,sLevels[0].sec[0].calls);
#else
	sDump = "";
#endif

	return sDump;
}



// private method to recursively 
// build the profile report
void 
VSProfileLib::DumpLevels(int l, int p, pTime calls) {


	int siz;
	char a[2] = "";
	char s[200];
	char s2[2000];
	section *sec;

	siz = (int)sLevels[l].sec.size();

	for(int cur = 0; cur < siz; ++cur) {
		sec = &(sLevels[l].sec[cur]);

		if (l==0)
			calls = sec->calls;

		if ((p == -1) || (sec->parent == p)) {

			sprintf(s,"%#*s%s", l * LEVEL_INDENT ," ",sec->name.c_str());

			if (sec->profileGL)
				sprintf(s2,"%-*s %5.0f %8.2f %8.2f %8.2f\n",
					sDisp + sTotalLevels * LEVEL_INDENT + 2,
					s,
					(float)(sec->calls/calls),
					(float)(sec->totalTime)/(calls), 
					(sec->totalQueryTime/(1000000.0 /** calls*/)),
					(float)(sec->wastedTime/(calls)));
			else
				sprintf(s2,"%-*s %5.0f %8.2f          %8.2f\n",
					sDisp + sTotalLevels * LEVEL_INDENT + 2,
					s,
					(float)(sec->calls/calls),
					(float)(sec->totalTime)/(calls),
					(float)(sec->wastedTime)/(calls));

			sDump += s2;

			if (l+1 < sTotalLevels)
				DumpLevels(l+1,cur,calls);
		}
		
	}
}


// Collect Queries results
void 
VSProfileLib::CollectQueryResults() {

#if VSPL_PROFILE == VSPL_PROFILE_CPU_AND_GPU
	int siz;
	section *sec;
	int availableEnd = 0;
	GLuint64 timeStart=0, timeEnd = 0;
	unsigned long long int aux = 0;

	for (int l = 0; l < sTotalLevels; ++l) {
		siz = sLevels[l].sec.size();

		for(int cur = 0; cur < siz; ++cur) {
			sec = &(sLevels[l].sec[cur]);

			if (sec->profileGL) {
				sec->totalQueryTime = 0;
				aux = 0;

				for (unsigned int j = 0; j < sec->queriesGL[sFrontBuffer].size(); ++j) {

					glGetQueryObjectui64v(sec->queriesGL[sFrontBuffer][j].queries[0], GL_QUERY_RESULT, &timeStart);
					glGetQueryObjectui64v(sec->queriesGL[sFrontBuffer][j].queries[1], GL_QUERY_RESULT, &timeEnd);
					aux +=  (timeEnd - timeStart);
					glDeleteQueries(2, sec->queriesGL[sFrontBuffer][j].queries);
				}
				sec->totalQueryTime += aux;
				sec->queriesGL[sFrontBuffer].clear();
			}
		}
	}
	// SWAP QUERY BUFFERS
	if (sBackBuffer) {
		sBackBuffer = 0;
		sFrontBuffer = 1;
	}
	else {
		sBackBuffer = 1;
		sFrontBuffer = 0;
	}
#endif
}
