#ifndef __RESOURCE_MANAGER__
#define __RESOURCE_MANAGER__

#include "Defines.h"
#include "SpriteManager.h"
#include "FileManager.h"

enum RES_TYPES
{
	RES_NONE = 0,
	RES_IFACE,
	RES_CRITTERS,
	RES_ITEMS,
	RES_SCRIPT,
	RES_SPLASH,
	RES_GLOBAL_MAP,
	RES_IFACE_EXT,

	RES_TYPE_MAX
};

class SpriteManager;
struct SpriteInfo;
struct AnyFrames;

struct LoadedAnim
{
    int        ResType;
    AnyFrames* Anim;
    LoadedAnim( int res_type, AnyFrames* anim ): ResType( res_type ), Anim( anim ) {}
};
typedef map< uint, LoadedAnim, less< uint > > LoadedAnimMap;

class ResourceManager
{
private:
    PtrVec         processedDats;
    UIntStrMap     namesHash;
    LoadedAnimMap  loadedAnims;
    AnimMap        critterFrames;
    Animation3dVec critter3d;
    StrVec         splashNames;
    StrMap         soundNames;

    void       AddNamesHash( StrVec& names );
    AnyFrames* LoadFalloutAnim( uint crtype, uint anim1, uint anim2, int dir );
    AnyFrames* LoadFalloutAnimSpr( uint crtype, uint anim1, uint anim2, int dir );

public:
    void Refresh();
    void Finish();
    void FreeResources( int type );

    AnyFrames* GetAnim( uint name_hash, int dir, int res_type );
    AnyFrames* GetIfaceAnim( uint name_hash )         { return GetAnim( name_hash, 0, RES_IFACE ); }
    AnyFrames* GetInvAnim( uint name_hash )           { return GetAnim( name_hash, 0, RES_IFACE_EXT ); }
    AnyFrames* GetSkDxAnim( uint name_hash )          { return GetAnim( name_hash, 0, RES_IFACE_EXT ); }
    AnyFrames* GetItemAnim( uint name_hash )          { return GetAnim( name_hash, 0, RES_ITEMS ); }
    AnyFrames* GetItemAnim( uint name_hash, int dir ) { return GetAnim( name_hash, dir, RES_ITEMS ); }

    AnyFrames*   GetCrit2dAnim( uint crtype, uint anim1, uint anim2, int dir );
    Animation3d* GetCrit3dAnim( uint crtype, uint anim1, uint anim2, int dir, int* layers3d = NULL );
    uint         GetCritSprId( uint crtype, uint anim1, uint anim2, int dir, int* layers3d = NULL );

    AnyFrames* GetRandomSplash();

    StrMap& GetSoundNames() { return soundNames; }
};

extern ResourceManager ResMngr;


#define SKILLDEX_PARAM( index )             ( index )
enum SKILLDEX_TYPES
{
	SKILLDEX_PERKS = 1000,
	SKILLDEX_KILLS,
	SKILLDEX_KARMA,
	SKILLDEX_TRAITS,
	SKILLDEX_REPUTATION,
	SKILLDEX_SKILLS,
	SKILLDEX_NEXT_LEVEL,
	SKILLDEX_DRUG_ADDICT,
	SKILLDEX_ALCOHOL_ADDICT,

	SKILLDEX_TYPE_MAX
};
#define SKILLDEX_REPUTATION_RATIO( val )    ( 2100 + ( ( val ) >= GameOpt.ReputationLoved ? 0 : ( ( val ) >= GameOpt.ReputationLiked ? 1 : ( ( val ) >= GameOpt.ReputationAccepted ? 2 : ( ( val ) >= GameOpt.ReputationNeutral ? 3 : ( ( val ) >= GameOpt.ReputationAntipathy ? 4 : ( ( val ) >= GameOpt.ReputationHated ? 5 : 6 ) ) ) ) ) ) )

#endif // __RESOURCE_MANAGER__
