#include "../ass.h"
#define sprintf(b,f,rest...) TEXT->Format(b,f, ## rest)
IBios* interface;

extern const uint16_t fontTiles[], fontPal[];
extern const uint16_t playerTiles[], playerPal[];

#define SPRITEA_BUILD(t,b,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((b) &  3) <<  9) |		\
	(((t) & 0x1FF) << 0)		\
)
#define SPRITEB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
(												\
	(((pr) & 3) << 30) |						\
	(((ds) & 1) << 28) |						\
	(((vf) & 1) << 27) |						\
	(((hf) & 1) << 26) |						\
	(((dh) & 1) << 25) |						\
	(((dw) & 1) << 24) |						\
	(((vp) & 0x7FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)

void print(char* str, int x, int y)
{
	unsigned short *t = &MAP1[(y * 64) + x];
	char *b = str;
	while (*b)
	{
		*t++ = (*b - 32);
		b++;
	}
}



//==== STARTING OFF =====

#define MAXENTITIES 64

typedef struct tEntity
{
	short type;
	short group;
	short x, y;
	short oldX, oldY;
	char pal;
	char spr;
	char state;
	void (*draw)(), (*think)();
	short extra[16];
} tEntity;

tEntity entities[MAXENTITIES];

int Spawn(int type, int x, int y);

unsigned int scroll = 0;
char debugBuffer[256];



//===== PLAYER =====

typedef struct tPlayer
{
	short type;
	short group;
	short x, y;
	short oldX, oldY;
	char pal;
	char spr;
	char state;
	void (*draw)(), (*think)();
	short flameTick, swerve, intro, shotTimer;
} tPlayer;

void DrawPlayer(int id)
{
	tPlayer* p = (tPlayer*)&entities[id];

	int frame = 0;

	if (p->y < p->oldY)
		p->swerve--;
	else if (p->y > p->oldY)
		p->swerve++;
	else
	{
		if (p->swerve > 0) p->swerve--;
		else if (p->swerve < 0) p->swerve++;
	}
	p->oldY = p->y;
	if (p->swerve < 0) frame = 1;
	if (p->swerve < -16) { frame = 2; p->swerve = -16; }
	if (p->swerve > 0) frame = 3;
	if (p->swerve > 16) { frame = 4; p->swerve = 16; }

	SPRITES_A[(int)p->spr] = SPRITEA_BUILD(128 + (frame * 8), 0, 1, p->pal);
	SPRITES_B[(int)p->spr] = SPRITEB_BUILD(p->x, p->y, 1, 0, 0, 0, 1, 0);
	SPRITES_A[(int)p->spr+1] = SPRITEA_BUILD(128 + 40 + ((p->flameTick >> 2) % 2), 1, 1, 2);
	SPRITES_B[(int)p->spr+1] = SPRITEB_BUILD(p->x - 8, p->y + 4, 0, 0, 0, 0, 0, 0);
}

void ThinkPlayer(int id)
{
	tPlayer* p = (tPlayer*)&entities[id];

	if (p->state == 0) //flying in
	{
		if (p->intro < 64)
		{
			p->intro++;
			p->x++;
		}
		else
		{
			p->state = 1; //player control
		}
	}
	else
	{
		if (REG_KEYIN == 0xD0) p->y++;
		else if (REG_KEYIN == 0xC8) p->y--;
		else if (REG_KEYIN == 0xCD) p->x++;
		else if (REG_KEYIN == 0xCB) p->x--;

		if (REG_KEYIN == 0x2E)
		{
			if (p->shotTimer == 0)
			{
				Spawn(2, p->x + 12, p->y + 4);
				p->shotTimer = 5;
			}
		}
		else if (p->shotTimer > 0)
		{
			p->shotTimer--;
		}
	}

	p->flameTick++;
}

void InitPlayer(int id)
{
	tPlayer* p = (tPlayer*)&entities[id];

	p->pal = 2;
	p->x -= 64; //to fly in
}



//===== PLAYER'S BULLETS =====

void DrawPlayerBullet(int id)
{
	tEntity* p = &entities[id];

	SPRITES_A[(int)p->spr] = SPRITEA_BUILD(128 + 42, 0, 1, p->pal);
	SPRITES_B[(int)p->spr] = SPRITEB_BUILD(p->x, p->y, 1, 0, 0, 0, 0, 0);
}

void ThinkPlayerBullet(int id)
{
	tEntity* p = &entities[id];
	p->x += 4;
	if (p->x > 320)
		p->type = 0; //die
}

void InitPlayerBullet(int id)
{
	tEntity* p = &entities[id];
	p->pal = 2;
	p->spr = 8 + id;
}



//==== PUTTING IT TOGETHER =====

const void* const entityFuncs[] =
{
	0, 0, 0, 0,
	ThinkPlayer, DrawPlayer, InitPlayer, 0,
	ThinkPlayerBullet, DrawPlayerBullet, InitPlayerBullet, 0
};

void Think()
{
	for (int i = 0; i < MAXENTITIES; i++)
	{
		if (entities[i].type == 0)
			continue;
		if (entities[i].think != 0)
			entities[i].think(i);
	}
}

void Draw()
{
	for (int i = 0; i < MAXENTITIES; i++)
	{
		if (entities[i].type == 0)
			continue;
		if (entities[i].draw != 0)
			entities[i].draw(i);
	}
}

int Spawn(int type, int x, int y)
{
	for (int i = 0; i < MAXENTITIES; i++)
	{
		if (entities[i].type != 0)
			continue;
		tEntity* e = &entities[i];
		e->type = type;
		e->x = e->oldX = x;
		e->y = e->oldY = y;
		e->think = entityFuncs[(type * 4) + 0];
		e->draw = entityFuncs[(type * 4) + 1];
		void (*init)(int) = (void*)entityFuncs[(type * 4) + 2];
		if (init != 0)
			init(i);
		return i;
	}
	return -1;
}

int main(void)
{
	REG_SCREENMODE = SMODE_TILE;
	REG_SCREENFADE = 0;

	intoff();

	REG_MAPSET = 0x10;
	MISC->DmaClear(MAP1, 0, 64 * 32, DMA_SHORT);

	MISC->DmaClear(entities, 0, sizeof(tEntity) * 64, DMA_BYTE);

	MISC->DmaCopy(TILESET, (int8_t*)&fontTiles, 512, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&fontPal, 16, DMA_SHORT);

	MISC->DmaCopy(TILESET + 0x1000, (int8_t*)&playerTiles, 512, DMA_INT);
	MISC->DmaCopy(PALETTE + 32, (int8_t*)&playerPal, 32, DMA_SHORT);

	int e = Spawn(1, 64, 128);
	//e = Spawn(1, 48, 64);
	//entities[e].spr = 4;
	//entities[e].pal = 3;

	print("SOKONYAN INDEV", 0, 0);

	for(;;)
	{
		sprintf(debugBuffer, "SCROLL %d  REG_KEYIN $%02X ", scroll, REG_KEYIN);
		print(debugBuffer, 0, 1);

		Think();
		Draw();
		scroll++;

		vbl();
	}
}
