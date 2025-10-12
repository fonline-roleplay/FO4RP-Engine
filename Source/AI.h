#ifndef __AI__
#define __AI__

#include "Common.h"

#define BAGS_FILE_NAME               "Bags.cfg"
#define MAX_NPC_BAGS                 ( 50 )
#define MAX_NPC_BAGS_PACKS           ( 20 )
#define NPC_GO_HOME_WAIT_TICK        ( Random( 4000, 6000 ) )

enum AI_PLANE_TYPES
{
	AI_PLANE_MISC = 0,
	AI_PLANE_ATTACK,
	AI_PLANE_WALK,
	AI_PLANE_PICK,
	AI_PLANE_PATROL,
	AI_PLANE_COURIER,

	AI_PLANE_TYPE_MAX
};

#define AI_PLANE_MISC_PRIORITY       ( 10 )
#define AI_PLANE_ATTACK_PRIORITY     ( 50 )
#define AI_PLANE_WALK_PRIORITY       ( 20 )
#define AI_PLANE_PICK_PRIORITY       ( 35 )
#define AI_PLANE_PATROL_PRIORITY     ( 25 )
#define AI_PLANE_COURIER_PRIORITY    ( 30 )

struct AIDataPlane
{
    int          Type;
    uint         Priority;
    int          Identifier;
    uint         IdentifierExt;
    AIDataPlane* ChildPlane;
    bool         IsMove;
	uint		 Delay;
	bool		 IsNotProccess;

    union
    {
        struct
        {
            bool IsRun;
            uint WaitSecond;
            int  ScriptBindId;
        } Misc;

        struct
        {
            bool   IsRun;
            uint   TargId;
            int    MinHp;
            bool   IsGag;
            ushort GagHexX, GagHexY;
            ushort LastHexX, LastHexY;
        } Attack;

        struct
        {
            bool   IsRun;
            ushort HexX;
            ushort HexY;
            uchar  Dir;
            uint   Cut;
        } Walk;

        struct
        {
            bool   IsRun;
            ushort HexX;
            ushort HexY;
            ushort Pid;
            uint   UseItemId;
            bool   ToOpen;
        } Pick;

        struct
        {
            uint Buffer[ 8 ];
        } Buffer;
    };

    struct
    {
        uint   PathNum;
        uint   Iter;
        bool   IsRun;
        uint   TargId;
        ushort HexX;
        ushort HexY;
        uint   Cut;
        uint   Trace;
    } Move;

    AIDataPlane* GetCurPlane()           { return ChildPlane ? ChildPlane->GetCurPlane() : this; }
    bool         IsSelfOrHas( int type ) { return Type == type || ( ChildPlane ? ChildPlane->IsSelfOrHas( type ) : false ); }
    void         DeleteLast()
    {
        if( ChildPlane )
        {
            if( ChildPlane->ChildPlane ) ChildPlane->DeleteLast();
            else SAFEREL( ChildPlane );
        }
    }

    AIDataPlane* GetCopy()
    {
        AIDataPlane* copy = new AIDataPlane( Type, Priority );
        if( !copy ) return NULL;
        memcpy( copy->Buffer.Buffer, Buffer.Buffer, sizeof( Buffer.Buffer ) );
        AIDataPlane* result = copy;
        AIDataPlane* plane_child = ChildPlane;
        while( plane_child )
        {
            copy->ChildPlane = new AIDataPlane( plane_child->Type, plane_child->Priority );
            if( !copy->ChildPlane ) return NULL;
            copy->ChildPlane->Assigned = true;
            memcpy( copy->ChildPlane->Buffer.Buffer, plane_child->Buffer.Buffer, sizeof( plane_child->Buffer.Buffer ) );
            plane_child = plane_child->ChildPlane;
            copy = copy->ChildPlane;
        }
        return result;
    }

    bool Assigned;
    int  RefCounter;
    void AddRef() { RefCounter++; }
    void Release()
    {
        RefCounter--;
        if( !RefCounter ) delete this;
    }
    AIDataPlane( uint type, uint priority ): Type( type ), Priority( priority ), Identifier( 0 ), IdentifierExt( 0 ), ChildPlane( NULL ), IsMove( false ), Delay(0), IsNotProccess(false), Assigned( false ), RefCounter( 1 )
    {
        memzero( &Buffer, sizeof( Buffer ) );
        memzero( &Move, sizeof( Move ) );
        MEMORY_PROCESS( MEMORY_NPC_PLANE, sizeof( AIDataPlane ) );
    }
    ~AIDataPlane()
    {
        SAFEREL( ChildPlane );
        MEMORY_PROCESS( MEMORY_NPC_PLANE, -(int) sizeof( AIDataPlane ) );
    }
private: AIDataPlane() {}        // Disable default constructor
};
typedef vector< AIDataPlane* > AIDataPlaneVec;

class NpcBagItem
{
public:
    uint ItemPid;
    uint MinCnt;
    uint MaxCnt;
    uint ItemSlot;

    NpcBagItem(): ItemPid( 0 ), MinCnt( 0 ), MaxCnt( 0 ), ItemSlot( SLOT_INV ) {}
    NpcBagItem( const NpcBagItem& r ): ItemPid( r.ItemPid ), MinCnt( r.MinCnt ), MaxCnt( r.MaxCnt ), ItemSlot( r.ItemSlot ) {}
};
typedef vector< NpcBagItem >             NpcBagItems;
typedef vector< NpcBagItems >            NpcBagCombination;
typedef vector< NpcBagCombination >      NpcBag;
typedef vector< NpcBag >                 NpcBagVec;
typedef map< string, NpcBagCombination > StringNpcBagCombMap;

/************************************************************************/
/*                                                                      */
/************************************************************************/

class NpcAIMngr
{
public:
    bool Init();
    void Finish();

    NpcBag& GetBag( uint num );

private:
    NpcBagVec npcBags;
    bool LoadNpcBags();
};
extern NpcAIMngr AIMngr;


// Plane begin/end/run reasons
enum PLANE_REASONS
{
	// Begin
	REASON_GO_HOME = 10,
	REASON_FOUND_IN_ENEMY_STACK,
	REASON_FROM_DIALOG,
	REASON_FROM_SCRIPT,
	REASON_RUN_AWAY,
	// End
	REASON_SUCCESS = 30,
	REASON_HEX_TOO_FAR,
	REASON_HEX_BUSY,
	REASON_HEX_BUSY_RING,
	REASON_DEADLOCK,
	REASON_TRACE_FAIL,
	REASON_POSITION_NOT_FOUND,
	REASON_FIND_PATH_ERROR,
	REASON_CANT_WALK,
	REASON_TARGET_DISAPPEARED,
	REASON_USE_ITEM_NOT_FOUND,
	REASON_GAG_CRITTER,
	REASON_GAG_ITEM,
	REASON_NO_UNARMED,
	// Run
	REASON_ATTACK_TARGET = 50,
	REASON_ATTACK_WEAPON,
	REASON_ATTACK_DISTANTION,
	REASON_ATTACK_USE_AIM,
};

#endif // __AI__
