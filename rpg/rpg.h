#include "../ass.h"
#include "../ass-keys.h"

extern IBios* interface;

extern const TImageFile titlePic;
extern const uint16_t spritepalPal[], fontTiles[], fontPal[], uiBackground[];
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
} MapEntity;

#define MAXENTITIES 16

extern MapEntity entities[MAXENTITIES];

extern int lastInput;
extern void getInput();

extern char *strcpy(char *dest, const char *src);
extern int strlen(const char *str);

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
extern void drawString(int x, int y, const char* string);

extern void runScript(uint8_t* code, int entityID);

