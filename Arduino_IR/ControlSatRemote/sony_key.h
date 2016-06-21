#define B_prefix 0x0000
#define REMOTE_MODEL "RM-952"
const uint16_t RemoteCode[] = {
	0x5D0, // 00 TeletextInfo
	0xDD0, // 01 JUMP
	0x910, // 02 Num0
	0x010, // 03 Num1
	0x810, // 04 Num2
	0x410, // 05 Num3
	0xC10, // 06 Num4
	0x210, // 07 Num5
	0xA10, // 08 Num6
	0x610, // 09 Num7
	0xE10, // 10 Num8
	0x110, // 11 Num9
	0x6D6, // 12 ExitTeletext
	0x090, // 13 Up
	0x890, // 14 Down
	0x490, // 15 SoundLevelUp
	0xC90, // 16 SoundLevelDown
	0xA90, // 17 Power
	0x290, // 18 Mute
	0x150,  // 19 SoundMode
	0xFD0, // 20 TeletextOpen
	0xA50, // 21 AVSource
	0x1D0, // 22 TVSource
	0xB90, // 23 1-2DigitSwitcher
	0xE90, // 24 LanguageChange
	0x3D0, // 25 TimeToOpen
	0x6D0, // 26 TimeToClose
	0x37EE, // 27 Favorite
	0x2F0, // 28 MenuPlus
	0xAF0, // 29 MenuMinus
	0x3F0, // 30 Select
	0x270 // 31 Picture
};

short bitMode(int key){
	if(key == 27)
		return 15;
	return 12;
}
