/*
 * Include some standard vectrex functions first!
 */
#include <vectrex.h>
#include "controller.h"
#include "drawRoutines.h"

const signed char triangles[] = {
  19,
  3,0,
  -5,3,
  0,-6,
  5,3,
  4,0,
  -6,2,
  1,-7,
  5,4,
  4,1,
  -8,1,
  2,-8,
  5,6,
  5,2,
  -9,0,
  4,-9,
  5,8,
  5,3,
  -11,0,
  6,-9,
  5,10,
  5,5,
  -13,-2,
  8,-10,
  4,12,
  6,7,
  -15,-5,
  12,-10,
  3,15,
  5,9,
  -16,-8,
  15,-10,
  0,18,
  5,11,
  -18,-12,
  20,-9,
  -2,22,
  4,14,
  -18,-18,
  25,-7,
  -6,25,
  2,17,
  -19,-24,
  30,-4,
  -11,28,
  0,20,
  -18,-31,
  36,0,
  -18,31,
  -3,24,
  -15,-39,
  42,6,
  -26,33,
  -8,27,
  -11,-48,
  47,14,
  -36,34,
  -14,30,
  -5,-58,
  53,24,
  -47,34,
  -21,33,
  3,-69,
  58,37,
  -61,31,
  -30,35,
  15,-79,
  61,53,
  -76,26,
  -41,36,
  31,-90,
  62,72,
  -93,18,
  -54,35,
  51,-99,
  60,94,
  -112,5,
  };


/*
 * Some defines, Maximal brightness is $7f, highest not set!
 * Max Scale would be $ff, well here we take only $f0!
 */
#define MAX_BRIGHTNESS (0x7f)
#define MAX_SCALE (0xf0)
#define MOVE_SCALE 0x90

#define EDGE 92
#define BOT 100

/*
 * For variable variables ALLWAYS leave them uninitialized, this way
 * the compiler puts them into the BSS ram section in vectrex ram
 * area from c880 onwards.
 *
 * All non BSS memory should be declared constant!
 * (This is still leaves the option of auto variables in a
 * functiion, which will takes it needed men from the stack)
 *
 */
unsigned char *current_song;

//$07,$80,$00,$02
unsigned char bounce_sound_data[4] = {0x07,0x80,0x00,0x02};

inline static void bounce_sound()
{
  //DP_to_C8();
  Vec_Expl_Flag = 255;
  //DP_to_D0();
}


/*
 * If you declare a function 'static inline' it is pretty much sure that
 * the compiler will inline it completely, the function will not
 * appear in any way in the module.
 *
 * If you leave out the static, the compiler assumes it might be used
 * globally, and thus leaves the function in the code, even if
 * it is inlined everywhere!
 */
/*
 * This Funktion handles all startup code, needed for vectrex to work
 * correctly, recallibrating the beam and making sure that some sound
 * is played (if wanted).
 */
static inline void start_one_vectrex_round(void)
{
  DP_to_C8();                        /* vectrex internal... dp must point */
  Explosion_Snd(bounce_sound_data);
  //Init_Music_chk(current_song);    /* to c800, could make a function which */
  Wait_Recal();                       /* sets this up allright... */
  Do_Sound();
  check_buttons();
}

/*
 * This function sets up a piece of music to be played from the start
 * of the next round on onward...
 */
static inline void play_song(void* song)
{
  Vec_Music_Flag = 1;       /* A makro to write to a specific memory */
  current_song = song;                /* address */
}

/*
 * A simple setup routine, enables/disables the joystick, and makes sure
 * that the button state is read correctly...
 */
void setup(void)
{
  enable_controller_1_x();
  enable_controller_1_y();
  disable_controller_2_x();
  disable_controller_2_y();
  Joy_Digital();
  check_buttons();                       /* last pressed button */
  Wait_Recal();                       /* sets this up allright... */
}

/*
 * Our main function we start of here...
 * we should make sure that we never return from here, or vectrex will
 * be surely bothered!
 */
int main(void)
{
  unsigned char frame;

  frame = 0;
  setup();                            /* setup our program */

  while (1)                        /* never to return... */
  {
    start_one_vectrex_round();        /* start 'de round */
    Intensity_a(MAX_BRIGHTNESS/2);          /* set some brightness */

    //cDraw_VLcTri((void*)(triangles), MOVE_SCALE, _SCALE);
    //cDraw_synced_list((void*)(triangles), MOVE_SCALE, MOVE_SCALE);

    check_buttons();

    frame++;
  } /* while (1) */                    /* joystick information is up to date */
}

/* END OF FILE */
