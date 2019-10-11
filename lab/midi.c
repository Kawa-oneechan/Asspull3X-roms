#include "../ass.h";


uint8_t* midiFile;
uint8_t* midiTrackOneStart;
uint8_t* midiTrackOnePos;
uint8_t* midiTrackOneEnd;
//int32 midiTrackOneDelay;
int32_t midiTempo;
int32_t midiPPQN;
int32_t midiPSecPerTick;
int32_t midiLastEventTime, midiNextEventTime, midiEventTimeNow;
uint8_t midiRunningStatus;
int32_t playing;
int32_t midiRow;

uint8_t GetMIDIByte(int32_t track)
{
	uint8_t ret = *midiTrackOnePos;
	//dpf("[GetMIDIByte: %x]", ret);
	midiTrackOnePos++;
	return ret;
}

uint32_t GetDelta()
{
	uint8_t val;
	uint8_t c;
	if ((val = GetMIDIByte(0)) & 0x80)
	{
		val &= 0x7F;
		do
		{
			val = (val << 7) + ((c = GetMIDIByte(0)) & 0x7F);
		} while (c & 0x80);
	}
	//dpf("[delta= %d]", val);
	return val;
}

void UpdateMIDI()
{
	if (!playing)
		return;
	if (midiTrackOnePos >= midiTrackOneEnd)
	{
		playing = 0;
		printf("Finished.");
		return;
	}

	interface->textLibrary->SetCursorPosition(0, 7);
	printf("%p / %p\n", midiTrackOnePos - midiFile, midiTrackOneEnd - midiFile);
	printf("%8d / %-8d\n", midiNextEventTime, midiLastEventTime);

	if (midiNextEventTime > 0)
	{
		midiNextEventTime -= midiPSecPerTick;
		return;
	}

	while (1)
	{
		interface->textLibrary->SetCursorPosition(0, midiRow);
		//printf("!");
		//midiTrackOneDelay = GetDelta(midiTrackOnePos) * 10;
		int32_t delta = GetDelta();
		midiLastEventTime = delta;
		midiNextEventTime = delta;
		printf("%8d ", delta);

		//printf("%8d ", midiTrackOneDelay);
		printf("%p ", midiTrackOnePos - midiFile);
		uint8_t command = GetMIDIByte(0);
		printf("%02x ", command);
		//printf("[Command: %x]", command);
		switch (command)
		{
			case 0xFF:
			{
				command = GetMIDIByte(0);
				printf("%02x ", command);
				switch (command)
				{
					case 0x02:
						//printf("copyright name\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x03:
						//printf("track name\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x20:
						//printf("channel prefix\n");
						midiTrackOnePos+=2;
						break;
					case 0x2F:
						printf("EOT");
						playing = 0;
						break;
					case 0x51:
						//printf("tempo\n");
						interface->textLibrary->SetCursorPosition(0, 14);
						GetMIDIByte(0);
						midiTempo = (GetMIDIByte(0) << 16) | (GetMIDIByte(0) << 8) | GetMIDIByte(0);
						printf("tempo %d\n", midiTempo);
						midiPSecPerTick = (midiTempo + (midiPPQN >> 2)) / midiPPQN;
						printf("pSecPerTick %d", midiPSecPerTick);
						//midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x58:
						//printf("time signature\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x59:
						//printf("key signature\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x7F:
						//printf("sequencer-specific\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					default:
						printf("*unhandled FF: %02x*\n", command);
						playing = 0;
						break;
				}
				break;
			}
			default:
			{
				if (command < 0x80 && midiRunningStatus > 0)
					command = midiRunningStatus;
				if (command >= 0x80 && command <= 0x8F)
				{
					//Key On
					int32_t channel = command & 0x0F;
					int32_t pitch = GetMIDIByte(0);
					int32_t velocity = GetMIDIByte(0);
					printf("KOF %3d %3d %3d", channel, pitch, velocity);
					MIDI_KEYOFF(channel, pitch, velocity);
				}
				else if (command >= 0x90 && command <= 0x9F)
				{
					//Key On
					int32_t channel = command & 0x0F;
					int32_t pitch = GetMIDIByte(0);
					int32_t velocity = GetMIDIByte(0);
					printf("KON %3d %3d %3d", channel, pitch, velocity);
					MIDI_KEYON(channel, pitch, velocity);
				}
				else if (command >= 0xB0 && command <= 0xBF)
				{
					//Control Change
					int32_t channel = command & 0x0F;
					int32_t control = GetMIDIByte(0);
					int32_t value = GetMIDIByte(0);
					printf("CTL %3d %3d %3d", channel, control, value);
					MIDI_CONTROL(channel, control, value);
				}
				else if (command >= 0xC0 && command <= 0xCF)
				{
					//Program Change
					int32_t channel = command & 0x0F;
					int32_t voice = GetMIDIByte(0);
					printf("PRG %3d %3d %3d", channel, voice);
					MIDI_PROGRAM(channel, voice);
				}
				else if (command >= 0xE0 && command <= 0xEF)
				{
					//Pitch Bend
					int32_t channel = command & 0x0F;
					int32_t control = GetMIDIByte(0);
					int32_t value = GetMIDIByte(0);
					printf("PIT %3d %3d %3d", channel, control, value);
					REG_MIDIOUT = (0xE0 | channel) | (control << 8) | (value << 16);
				}
				else
				{
					printf("*unhandled: %02x*\n", command);
					playing = 0;
					break;
				}
				midiRunningStatus = command;
			}
		}
		printf("              \n");
		midiRow++;
		if (delta) break;
		printf("...");
	}
	//interface->textLibrary->SetCursorPosition(0, 0);
	//printf("%d\n", midiTrackOneDelay);
	//midiTrackOneDelay -= (deltaTicks);
}

int32_t main()
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetCursorPosition(0, 0);
	TEXT->SetTextColor(0, 7);

	FILEINFO nfo;
	FILE file;
	int32_t ret = DISK->FileStat("FOO.MID", &nfo);
	if (ret != 0)
	{
		printf("Something fucked up: %d\n", ret);
		while(1);
	}
	midiFile = (uint8_t*)malloc(nfo.fsize);
	printf("MIDI File allocated at %p\n", midiFile);
	DISK->OpenFile(&file, "FOO.MID", FA_READ);
	DISK->ReadFile(&file, (void*)midiFile, nfo.fsize);
	DISK->CloseFile(&file);

	midiTempo = 120;
	midiPPQN = 96;

	midiRow = 10;

	//deltaTicks = *(uint16*)(midiFile + 0xC);
	//printf("deltaTicks is %d/%#x\n", deltaTicks, deltaTicks);
	//midiPPQN = *(uint16_t*)(midiFile + 0xC);
	midiTrackOneStart = midiFile + 0x16;
	printf("Track one starts at %p\n", midiTrackOneStart - midiFile);
	uint32_t len = *(uint32_t*)(midiFile + 0x12);
	midiTrackOneEnd = midiTrackOneStart + len;
	printf("Track one is %#x long, ends at %p\n", len, midiTrackOneEnd);
	midiTrackOnePos = midiTrackOneStart;
	//midiTrackOneDelay = 0;
	midiEventTimeNow = midiLastEventTime = midiNextEventTime = 0;
	midiRunningStatus = 0;

	midiPSecPerTick = 60000 / (midiTempo * midiPPQN);
	printf("PSecPerTick is 60000 / (%d * %d) = %d\n", midiTempo, midiPPQN, midiPSecPerTick);

	playing = 1;

	while(playing)
	//for (int32_t i = 0; i < 4; i++)
	{
		UpdateMIDI();
		vbl();
	}

	while(1);
}
