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
 * uses ARB_timer_query extension
 *
 * \version 0.2.1
 * renamed to VSProfileLib
 *
 * \version 0.2.0
 * uses OpenGL timers
 *
 * version 0.1.0
 * Initial Release
 *
 ---------------------------------------------------------------*/


#ifndef __VSProfileLib__
#define __VSProfileLib__

#define VSPL_PROFILE_NONE 0
#define VSPL_PROFILE_CPU 1
#define VSPL_PROFILE_CPU_AND_GPU 2


#define VSPL_PROFILE 0

// Available clocks
// note that gettimeofday is not available on windows!
#define VSPL_C_CLOCK 0
#define VSPL_GETTIMEOFDAY 1
#define VSPL_WIN_HIGH_PERFORMANCE_COUNTER 2
#define VSPL_WIN_SYSTEMTIME 3


// Use this define to select your favourite clock!
#define VSPL_CLOCK VSPL_C_CLOCK


// do the required includes for each clock
#if VSPL_CLOCK == VSPL_WIN_HIGH_PERFORMANCE_COUNTER
	#include <windows.h>
#elif (VSPL_CLOCK == VSPL_GETTIMEOFDAY)
	#include <sys/time.h>
#elif VSPL_CLOCK == VSPL_WIN_SYSTEMTIME
	#include <windows.h>
#elif VSPL_CLOCK == VSPL_C_CLOCK
	#define VSPL_CLOCK_RATE CLOCKS_PER_SEC * 1000.0
#endif

#include <vector>
#include <string>

#define PROFILE_MAX_LEVELS 50
#define LEVEL_INDENT 2

#define pTime double 


class VSProfileLib
{
public:
	/// String that contains the profile report
	static std::string sDump;
	/// create the profile report and store in sDump
	static const std::string &DumpLevels();
	/// resets profile data
	static void Reset();

	///
	static void CollectQueryResults();

	/// begin profile section
	VSProfileLib (std::string name, bool profileGL = false);
	/// end profile section
	~VSProfileLib();

protected:
	/// Contains information about a profiler section
	typedef struct {
		unsigned int queries[2];
	} pair;
	typedef struct s {
		/** Index of the parent section 
		  * in the previous level
		*/
		int parent;
		/// name of the section
		std::string name;
		/// stores the time when the section starts
		pTime startTime;
		/// query indexes for the beginning 
		/// and end of the section
		std::vector<pair> queriesGL[2];
		/** wasted time running the 
		  * profiler code for the section
		*/
		unsigned long long int totalQueryTime;
		pTime wastedTime;
		/// Total number of calls
		pTime calls;
		/// total time spend in the profiler section
		pTime totalTime;
		/// 
		bool profileGL;
	}section;

	/// Information about a level of profiling
	typedef struct l {
		/// set of sections in this level
		std::vector <section> sec;
		/** stores the current profile section for
		  * each level */
		int cursor;
	}level;

	/// space displacement for dump string formating
	static int sDisp;

	static unsigned int sBackBuffer, sFrontBuffer;

	/// list of all levels
	static level sLevels[PROFILE_MAX_LEVELS];
	/// current level
	static int sCurrLevel;
	/// total number of levels
	static int sTotalLevels;

	// AUX FUNCTIONS

	/// Puts the profile result in sDump
	static void DumpLevels(int l, int p, pTime calls);
	/// Creates a new section
	void createNewSection(std::string &name, pTime w, bool profileGL);
	/// returns the index of a section
	int searchSection(std::string &name);
	/// updates the times in a section
	void updateSection(int cur, pTime w);
	/// add the time spent in the current section
	void accumulate();
	/// Gets the time
	static void GetTicks(pTime *ticks);

#if VSPL_CLOCK == VSPL_WIN_HIGH_PERFORMANCE_COUNTER
	static LARGE_INTEGER sFreq;
#endif

public:

};

/// 
#if VSPL_PROFILE == VSPL_PROFILE_NONE
#define PROFILE(name)
#define PROFILE_GL(name)
#elif VSPL_PROFILE == VSPL_PROFILE_CPU
#define PROFILE(name) VSProfileLib __profile(name)
#define PROFILE_GL(name) VSProfileLib __profile(name, false)
#elif VSPL_PROFILE == VSPL_PROFILE_CPU_AND_GPU
#define PROFILE(name) VSProfileLib __profile(name)
#define PROFILE_GL(name) VSProfileLib __profile(name, true)
#endif

// include file end if
#endif

