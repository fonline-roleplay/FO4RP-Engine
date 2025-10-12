#ifndef __JOBS__
#define __JOBS__

// Server job types
enum JOB_TYPES
{
	JOB_NOP = 0,
	JOB_CLIENT,
	JOB_CRITTER,
	JOB_MAP,
	JOB_TIME_EVENTS,
	JOB_GARBAGE_ITEMS,
	JOB_GARBAGE_CRITTERS,
	JOB_GARBAGE_LOCATIONS,
	JOB_GARBAGE_SCRIPT,
	JOB_GARBAGE_VARS,
	JOB_DEFERRED_RELEASE,
	JOB_GAME_TIME,
	JOB_BANS,
	JOB_LOOP_SCRIPT,
	JOB_THREAD_LOOP,
	JOB_THREAD_SYNCHRONIZE,
	JOB_THREAD_FINISH,
	
	JOB_COUNT
};

class Critter;
class Map;
class Location;
class Item;
class GameVar;

class Job
{
public:
    int   Type;
    void* Data;
    uint  ThreadId;

    Job();
    Job( int type, void* data, bool cur_thread );

    static void PushBack( int type );
    static void PushBack( int type, void* data );
    static uint PushBack( const Job& job );
    static void PushFront( const Job& job );
    static Job  PopFront();
    static void Erase( int type );
    static uint Count();

    // Deferred releasing
    static void DeferredRelease( Critter* cr );
    static void DeferredRelease( Map* cr );
    static void DeferredRelease( Location* cr );
    static void DeferredRelease( Item* cr );
    static void DeferredRelease( GameVar* cr );
    static void SetDeferredReleaseCycle( uint cycle );
    static void ProcessDeferredReleasing();
};

#endif // __JOBS__
