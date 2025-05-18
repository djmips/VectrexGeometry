/*
 * Include some standard vectrex functions first!
 */
#include <vectrex.h>
#include "controller.h"
#include "drawRoutines.h"

#include "inward_triangles.h"
#include "box.h"

/*
 * Some defines, Maximal brightness is $7f, highest not set!
 * Max Scale would be $ff, well here we take only $f0!
 */
#define MAX_BRIGHTNESS (0x7f)
#define MAX_SCALE (0xf0)
#define MOVE_SCALE MAX_SCALE

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


  const long numberOfTriangles = (long)triangles[0];
  const long sizeofTriangle =  ((long)sizeof(triangles)-1) / numberOfTriangles;

  long offset = sizeofTriangle*(numberOfTriangles-1);

  while (1)                        /* never to return... */
  {
    start_one_vectrex_round();        /* start 'de round */
    Intensity_a(MAX_BRIGHTNESS/2);          /* set some brightness */
  
    void *triangleStart = ((void*)(triangles+1));
    triangleStart = triangleStart + offset;
    cDraw_synced_list(triangleStart,0,0, MOVE_SCALE, MOVE_SCALE);

    offset -= sizeofTriangle;
    if (offset < 0)
    {
      offset = sizeofTriangle*(numberOfTriangles-1);
    }

    check_buttons();

    frame++;
  } /* while (1) */                    /* joystick information is up to date */
}

/* END OF FILE */
