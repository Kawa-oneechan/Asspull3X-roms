#include "../ass.h"
#include "../ass-keys.h"

extern IBios* interface;

extern const TImageFile titlePic;
extern const uint16_t spritepalPal[], uiTiles[], uiPal[], uiBackground[];
extern const uint8_t testMap[], spritePals[];
extern const uint32_t sprites[], portraits[];

#define WIDTH 32
#define HEIGHT 32

#define KEY_ACTION KEYSCAN_X

#define OBJECTA_BUILD(t,b,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((b) &  3) <<  9) |		\
	(((t) & 0x1FF) << 0)		\
)
#define OBJECTB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
(												\
	(((pr) & 3) << 29) |						\
	(((ds) & 1) << 28) |						\
	(((vf) & 1) << 27) |						\
	(((hf) & 1) << 26) |						\
	(((dh) & 1) << 25) |						\
	(((dw) & 1) << 24) |						\
	(((vp) & 0x3FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)


enum facing
{
	faceDown, faceLeft, faceUp, faceRight
};
enum state
{
	stateIdle, stateStep, stateTsk, stateWave, stateLaugh, stateSad
};

typedef struct
{
	uint8_t* graphics;
	int16_t* palette;
	int16_t* metatiles;
} Tileset;

typedef struct
{
	uint8_t width, height;
	uint8_t* map;
	Tileset* tileset;
	uint8_t* entities;
} Map;

typedef struct
{
	uint8_t x, y;
	uint8_t oid;
	uint8_t facing;
	uint8_t state;
	uint8_t counter[4];
	uint8_t palette;
	int8_t* tileset;
	uint8_t* script;
	void* motor;
	bool offscreen;
} MapEntity;

#define MAXENTITIES 16

extern MapEntity entities[MAXENTITIES];
extern MapEntity* playerEntity;

#define BIT_FROZEN		0x00010000
#define BIT_BERSERK		0x00020000
#define BIT_POISONED	0x00040000
#define BIT_CANTFREEZE	0x00100000
#define BIT_CANTBERSERK	0x00200000
#define BIT_CANTPOISON	0x00400000
#define BIT_AI_LEADER	0x01000000
#define BIT_AI_FOLLOWER	0x02000000
#define BIT_PLAYER		0x80000000

#define BIT_FACTIONMASK	0x00000007
enum Factions
{
	factFelin, factNation, factSunyellow, factNeutral
};

#define SHARED_BATTLE_STUFF \
	uint32_t bits; \
	uint16_t hp, hpMax; \
	uint16_t pp, ppMax; \
	uint8_t attack, defense;

typedef struct
{
	SHARED_BATTLE_STUFF
} BattleParticipant;

typedef struct
{
	SHARED_BATTLE_STUFF
	char name[12];
	uint8_t titleID;
} PartyMember;

typedef struct
{
	SHARED_BATTLE_STUFF
	uint16_t monsterID;
	void* work;
} BattleOpponent;

#undef SHARED_BATTLE_STUFF

#define GETFACTION(x) (x->bits & BIT_FACTIONMASK)

extern PartyMember party[8];

typedef struct
{
	uint32_t bits; //copied to BattleOpponent
	uint16_t hp, pp; //copied to BattleOpponent
	uint8_t attack, defense;
	const char name[16];
	void(*handler)(void*, BattleOpponent*, int);
} BattleOpponentDB;

extern const BattleOpponentDB opponentDB[];
extern const uint8_t formationsDB[][6];

extern bool tryToFreeze(BattleParticipant* target);
extern void swapPartyMembers(int a, int b);

extern int lastInput;
extern void getInput();

extern void drawTile(int x, int y, int tileNum);
extern void aimCamera(int x, int y);
extern void drawMap();

extern void drawEntity(MapEntity *entity);
extern void updateEntity(MapEntity *entity);
extern int entityIsBlocked(MapEntity *entity);
extern void entityWalk(MapEntity *entity, int facing);
extern void entityPlayerMotor(MapEntity *entity);
extern void id_sort_shell(int keys[], uint8_t ids[], int count);
extern void updateAndDraw();
extern void loadMap(Map* map);

extern void waitForActionKey();
extern void drawWindow(int l, int t, int w, int h);
extern void eraseWindow(int l, int t, int w, int h);
extern void drawBar(int x, int y, int v, int m, int l);

extern int doMenu(int left, int top, int width, int height, char* options, int num);

extern void runScript(uint8_t* code, int entityID);

extern int drawChar(char ch, int x, int y, int col, int font);
extern int drawString(const char* str, int x, int y, int col, int font);
extern int measureString(const char* str, int font);
