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

extern void srand(uint32_t seed);
extern uint32_t rand();
extern void print(char* str, int x, int y, int color);
extern int bitTest(int* set, int bit);
extern int bitSet(int* set, int bit);
extern int bitClear(int* set, int bit);

#define MAXENTITIES 128

typedef struct
{
	int16_t type;
	int16_t group;
	int16_t x, y;
	int16_t oldX, oldY;
	uint8_t pal;
	uint8_t obj;
	int8_t state;
	void (*draw)(), (*think)();
	int16_t extra[16];
} tEntity;

extern tEntity entities[];
extern int objectsUsed[];

extern int Spawn(int16_t type, int16_t x, int16_t y);
extern int GetNextObjectIdx(int min, int max);
extern int GetNextObject();

extern const void* const entityFuncs[];

extern void DrawPlayer(int id);
extern void ThinkPlayer(int id);
extern void InitPlayer(int id);
extern void DrawPlayerBullet(int id);
extern void ThinkPlayerBullet(int id);
extern void InitPlayerBullet(int id);
extern void ThinkStarfield(int id);
extern void ThinkStar(int id);
extern void DrawStar(int id);
extern void InitStar(int id);

extern void Draw();
extern void Think();

