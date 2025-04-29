#include <vectrex.h>

// this is quite a "C" optimized version of print_sync
// it is actually FASTER than
// the "not very optimized" assembler include!
// (-O3 and no_frame_pointer)

//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//U = address of vectorlist
//X = (y,x) position of vectorlist (this will be point 0,0), positioning on screen
//scaleMove = scalefactor "Move" (after sync)
//scaleDraw = scalefactor "Draw" (vectors in vectorlist)
//
//     mode, rel y, rel x,                                             
//     mode, rel y, rel x,                                             
//     .      .      .                                                
//     .      .      .                                                
//     mode, rel y, rel x,                                             
//     0x02
// where mode has the following meaning:         
// negative: draw line                    
// $00:        move to specified endpoint     
// $FF:        draw relative line to specified endpoint
// $01:        sync (and move to list start and than to place in vectorlist)      
// $02:        end
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


void cDraw_synced_list(
const signed char *u,
signed int y,
signed int x,
unsigned int scaleMove,
unsigned int scaleDraw)
{
	#define ZERO_DELAY 5
	
	do
	{
		// resnyc / startsync
		
		dp_VIA_shift_reg = 0;		// all output is BLANK
		dp_VIA_cntl = 0xcc;	// zero the integrators
		dp_VIA_port_a = 0;    		// reset integrator offset
		dp_VIA_port_b = (int)0b10000010;
		
		dp_VIA_t1_cnt_lo = scaleMove;
		// delay, till beam is at zero
		// volatile - otherwise delay loop does not work with -O
		for (volatile signed int b=ZERO_DELAY; b>0; b--);
		dp_VIA_port_b= (int)0b10000011;
		
		// move to "location"
		dp_VIA_port_a = y;			// y pos to dac
		dp_VIA_port_b = 0;			// mux enable, dac to -> integrator y (and x)
		dp_VIA_cntl = 0xce;	// disable zero, disable all blank
		dp_VIA_port_b = 1;			// mux disable, dac only to x
		dp_VIA_port_a = x;			// dac -> x
		dp_VIA_t1_cnt_hi=0;		// start timer
		
		// this can be done before the wait loop
		// since it only fills the latch, not the actual timer!
		dp_VIA_t1_cnt_lo = scaleDraw;
		u+=3;
		
		// moveing test for yx== 0 into the move delay
		if ((*(u-2)!=0) || (*(u-1)!=0))
		{
			while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
			
			// internal moveTo
			dp_VIA_port_a = *(u-2);	// y pos to dac
			dp_VIA_port_b = 0;			// mux enable, dac to -> integrator y (and x)
			dp_VIA_cntl = 0xce;	// disable zero, disable all blank
			dp_VIA_port_b = 1;			// mux disable, dac only to x
			dp_VIA_port_a = *(u-1);	// dac -> x
			dp_VIA_t1_cnt_hi=0;		// start timer
			while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		}
		else
		{
			while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		}
		
		while (1)
		{
			if (*u<0) // draw line
			{
				// draw a vector
				dp_VIA_port_a = *(1+u);	// y pos to dac
				dp_VIA_port_b = 0;			// mux enable, dac to -> integrator y (and x)
				dp_VIA_port_b=1;			// mux disable, dac only to x
				dp_VIA_port_a = *(2+u);	// dac -> x
				dp_VIA_t1_cnt_hi=0;		// start timer
				dp_VIA_shift_reg = 0xff;		// draw complete line
				while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
				dp_VIA_shift_reg = 0;		// all output is BLANK
			}
			else if (*u == 0) // moveTo
			{
				if ((*(u+1)!=0) || (*(u+2)!=0))
				{
					// internal moveTo
					dp_VIA_port_a = *(1+u);	// y pos to dac
					dp_VIA_port_b = 0;			// mux enable, dac to -> integrator y (and x)
					dp_VIA_cntl = 0xce;	// disable zero, disable all blank
					dp_VIA_port_b =1 ;			// mux disable, dac only to x
					dp_VIA_port_a = *(2+u);	// dac -> x
					dp_VIA_t1_cnt_hi=0;		// start timer
					while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
				}
			}
			else
			{
				break;
			}
			u+=3;
		}
	} while (*u != 2);
}

void cMov_Draw_VLc_a(const signed char *vList)
{
	register int count = *(vList)-1;	// count in list
	dp_VIA_port_a = *(1+vList);	// y pos to dac
	dp_VIA_port_b = 0;			// mux enable, dac to -> integrator y (and x)
	dp_VIA_cntl = 0xce;	// disable zero, disable all blank
	dp_VIA_port_b = 1;			// mux disable, dac only to x
	dp_VIA_port_a = *(2+vList);	// dac -> x
	dp_VIA_t1_cnt_hi=0;		// start timer
	vList+=3;
	while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
	
	do 
	{
		dp_VIA_port_a = *(vList);	// first y coordinate to dac
		dp_VIA_port_b = 0;				// mux enable, dac to -> integrator y (and x)
		dp_VIA_port_b = 1;				// mux disable, dac only to x
		dp_VIA_port_a = *(vList+1);	// dac -> x
		dp_VIA_shift_reg = 0xff; // full "unblank" output
		dp_VIA_t1_cnt_hi = 0;			// start timer
		vList+=2;
		while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		dp_VIA_shift_reg = 0;			// output full blank
	} while (--count >=0);			// loop thru all vectors
}

void cDraw_VLc(const signed char *vList)
{
	register int count = *(vList++);	// count in list
	do 
	{
		dp_VIA_port_a = *(vList);	// first y coordinate to dac
		dp_VIA_port_b = 0;				// mux enable, dac to -> integrator y (and x)
		dp_VIA_port_b++;				// mux disable, dac only to x
		dp_VIA_port_a = *(vList+1);	// dac -> x
		dp_VIA_shift_reg = (unsigned int)0xff; // full "unblank" output
		dp_VIA_t1_cnt_hi = 0;			// start timer
		vList+=2;
		while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		dp_VIA_shift_reg = 0;			// output full blank
	} while (--count >=0);			// loop thru all vectors
}


void cDraw_VLcZ(const signed char *vList, signed int y, signed int x, unsigned int scaleMove, unsigned int scaleDraw )
{

	{
		dp_VIA_shift_reg = 0;		// all output is BLANK
		dp_VIA_cntl = 0xcc;	// zero the integrators
		dp_VIA_port_a = 0;    		// reset integrator offset
		dp_VIA_port_b = (int)0b10000010;
		
		dp_VIA_t1_cnt_lo = scaleMove;
		// delay, till beam is at zero
		// volatile - otherwise delay loop does not work with -O
		for (volatile signed int b=ZERO_DELAY; b>0; b--);
		dp_VIA_port_b= (int)0b10000011;
		
		// move to "location"
		dp_VIA_port_a = y;			// y pos to dac
		dp_VIA_port_b = 0;			// mux enable, dac to -> integrator y (and x)
		dp_VIA_cntl = 0xce;	// disable zero, disable all blank
		dp_VIA_port_b = 1;			// mux disable, dac only to x
		dp_VIA_port_a = x;			// dac -> x
		dp_VIA_t1_cnt_hi=0;		// start timer
		
		// this can be done before the wait loop
		// since it only fills the latch, not the actual timer!
		dp_VIA_t1_cnt_lo = scaleDraw;

		while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes	
	}


	register int count = *(vList++);	// count in list
	do 
	{
		dp_VIA_port_a = *(vList);	// first y coordinate to dac
		dp_VIA_port_b = 0;				// mux enable, dac to -> integrator y (and x)
		dp_VIA_port_b++;				// mux disable, dac only to x
		dp_VIA_port_a = *(vList+1);	// dac -> x
		dp_VIA_shift_reg = (unsigned int)0xff; // full "unblank" output
		dp_VIA_t1_cnt_hi = 0;			// start timer
		vList+=2;
		while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		dp_VIA_shift_reg = 0;			// output full blank

		count--;

		if (((count & (int)0x07) == (int)0x07))
		{
		//	dp_VIA_shift_reg = 0;		// all output is BLANK
			dp_VIA_cntl = 0xcc;	// zero the integrators
			dp_VIA_port_a = 0;    		// reset integrator offset
			dp_VIA_port_b = (int)0b10000010;
			
			dp_VIA_t1_cnt_lo = scaleMove;
			// delay, till beam is at zero
			// volatile - otherwise delay loop does not work with -O
			for (volatile signed int b=ZERO_DELAY; b>0; b--);
			dp_VIA_port_b= (int)0b10000011;
			
			// move to "location"
			dp_VIA_port_a = y;			// y pos to dac
			dp_VIA_port_b = 0;			// mux enable, dac to -> integrator y (and x)
			dp_VIA_cntl = 0xce;	// disable zero, disable all blank
			dp_VIA_port_b = 1;			// mux disable, dac only to x
			dp_VIA_port_a = x;			// dac -> x
			dp_VIA_t1_cnt_hi=0;		// start timer
			
			// this can be done before the wait loop
			// since it only fills the latch, not the actual timer!
			dp_VIA_t1_cnt_lo = scaleDraw;

			while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes	
		}

	} while (count >=0);			// loop thru all vectors
}

/*
void cDraw_VLTri(const signed char *vList, unsigned int scaleMove, unsigned int scaleDraw )
{

	register int count = *(vList++);	// count of triangles in list

	do 
	{
		// move to "location"
		dp_VIA_port_a = *(vList++);		// y -> dac
		dp_VIA_port_b = 0;				// mux enable, dac to -> integrator y (and x)
		dp_VIA_port_b = 0;				// mux enable, dac to -> integrator y (and x)
		dp_VIA_cntl = 0xce;				// disable zero, disable all blank
		dp_VIA_port_b = 1;				// mux disable, dac only to x
		dp_VIA_port_a = *(vList++);		// x -> dac

		// this can be done before the wait loop
		// since it only fills the latch, not the actual timer!
		dp_VIA_t1_cnt_lo = scaleDraw;

		while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes	

		dp_VIA_port_a = *(vList);		// y -> dac
		dp_VIA_port_b = 0;				// mux enable, dac to -> integrator y (and x)
		dp_VIA_port_b++;				// mux disable, dac only to x
		dp_VIA_port_a = *(vList+1);		// x -> dac
		dp_VIA_shift_reg = (unsigned int)0xff; // full "unblank" output
		dp_VIA_t1_cnt_hi = 0;			// start timer
		vList+=2;
		while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		dp_VIA_shift_reg = 0;			// output full blank

		dp_VIA_port_a = *(vList);		// y -> dac
		dp_VIA_port_b = 0;				// mux enable, dac to -> integrator y (and x)
		dp_VIA_port_b++;				// mux disable, dac only to x
		dp_VIA_port_a = *(vList+1);		// x -> dac
		dp_VIA_shift_reg = (unsigned int)0xff; // full "unblank" output
		dp_VIA_t1_cnt_hi = 0;			// start timer
		vList+=2;
		while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		dp_VIA_shift_reg = 0;			// output full blank

		dp_VIA_port_a = *(vList);		// y coordinate to dac
		dp_VIA_port_b = 0;				// mux enable, dac to -> integrator y (and x)
		dp_VIA_port_b++;				// mux disable, dac only to x
		dp_VIA_port_a = *(vList+1);		// x -> dac
		dp_VIA_shift_reg = (unsigned int)0xff; // full "unblank" output
		dp_VIA_t1_cnt_hi = 0;			// start timer
		vList+=2;
		while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		dp_VIA_shift_reg = 0;			// output full blank

	} while (--count >=0);	

}
*/

// 0 = move
// >1 = draw
// 1 = end
// (-O3 and no_frame_pointer)
void cDraw_VL_mode(const signed char *u)
{
	while (1)
	{
		if (*u>1) // draw line
		{
			// draw a vector
			dp_VIA_port_a = *(1+u);	// y pos to dac
			dp_VIA_port_b = 0;		// mux enable, dac to -> integrator y (and x)
			dp_VIA_port_b=1;			// mux disable, dac only to x
			dp_VIA_port_a = *(2+u);	// dac -> x
			dp_VIA_t1_cnt_hi=0;		// start timer
			dp_VIA_shift_reg = 0xff;		// draw complete line
			while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
			dp_VIA_shift_reg = 0;		// all output is BLANK
		}
		else if (*u == 0) // moveTo
		{
			// internal moveTo
			dp_VIA_port_a = *(1+u);	// y pos to dac
			dp_VIA_port_b = 0;		// mux enable, dac to -> integrator y (and x)
			dp_VIA_cntl = 0xce;	// disable zero, disable all blank
			dp_VIA_port_b =1 ;		// mux disable, dac only to x
			dp_VIA_port_a = *(2+u);	// dac -> x
			dp_VIA_t1_cnt_hi=0;		// start timer
			while ((dp_VIA_int_flags & 0x40) == 0); // wait till timer finishes
		}
		else
		{
			break;
		}
		u+=3;
	}
}

