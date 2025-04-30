// #define DRAW_SCALE 0x0c // smallest scale printSync can cope with!

// this is quite a "C" optimized version of print_sync
// it is actually FASTER than
// the "not very optimized" assembler include!
// (-O3 and no_frame_pointer)
void cDraw_synced_list(
	signed char *u, 
	signed int y, 
	signed int x, 
	unsigned int scaleMove, 
	unsigned int scaleDraw);

void cMov_Draw_VLc_a(signed char *vList);

// 0 = move
// >1 = draw
// 1 = end
// (-O3 and no_frame_pointer)
void cDraw_VL_mode(signed char *u);

void cDraw_VLc(signed char *vList);

void cDraw_VLcZ(const signed char *vList, signed int y, signed int x, unsigned int scaleMove, unsigned int scaleDraw );