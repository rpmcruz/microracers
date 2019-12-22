/***************************************************************************
           sin_table.h  -  cache of the expensive sin and cos cals
                             -------------------
    begin                : Thu Nov 10 2005
    copyright            : (C) 2005 by Ricardo Cruz
    email                : rpmcruz@clix.pt
 ***************************************************************************/

#ifndef SIN_TABLE
#define SIN_TABLE

#ifndef M_PI
  #define M_PI 3.141592654
#endif

inline double deg2rad(int deg)
  { return (deg * M_PI) / 180.0;  }

#define keep_angle_in_range(deg) \
  { while(deg < 0)    deg += 360; \
    while(deg >= 360) deg -= 360; }

double sin(int angle_deg);
double cos(int angle_deg);

/* Other math stuff. */
#define ABS(x)  (x >= 0 ? x : -x)
#define SIGN(x) (x >= 0 ? 1 : -1)

#endif /*SIN_TABLE*/
