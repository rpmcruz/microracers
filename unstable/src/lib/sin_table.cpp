/***************************************************************************
          sin_table.cpp  -  cache of the expensive sin and cos cals
                             -------------------
    begin                : Thu Nov 10 2005
    copyright            : (C) 2005 by Ricardo Cruz
    email                : rpmcruz@clix.pt
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sin_table.h"

static const double sin_table[91] = {
  0.00000000, 0.01745241, 0.03489950, 0.05233596,
  0.06975647, 0.08715574, 0.10452846, 0.12186934,
  0.13917310, 0.15643447, 0.17364818, 0.19080900,
  0.20791169, 0.22495105, 0.24192190, 0.25881905,
  0.27563736, 0.29237170, 0.30901699, 0.32556815,
  0.34202014, 0.35836795, 0.37460659, 0.39073113,
  0.40673664, 0.42261826, 0.43837115, 0.45399050,
  0.46947156, 0.48480962, 0.50000000, 0.51503807,
  0.52991926, 0.54463904, 0.55919290, 0.57357644,
  0.58778525, 0.60181502, 0.61566148, 0.62932039,
  0.64278761, 0.65605903, 0.66913061, 0.68199836,
  0.69465837, 0.70710678, 0.71933980, 0.73135370,
  0.74314483, 0.75470958, 0.76604444, 0.77714596,
  0.78801075, 0.79863551, 0.80901699, 0.81915204,
  0.82903757, 0.83867057, 0.84804810, 0.85716730,
  0.86602540, 0.87461971, 0.88294759, 0.89100652,
  0.89879405, 0.90630779, 0.91354546, 0.92050485,
  0.92718385, 0.93358043, 0.93969262, 0.94551858,
  0.95105652, 0.95630476, 0.96126170, 0.96592583,
  0.97029573, 0.97437006, 0.97814760, 0.98162718,
  0.98480775, 0.98768834, 0.99026807, 0.99254615,
  0.99452190, 0.99619470, 0.99756405, 0.99862953,
  0.99939083, 0.99984770, 1.00000000
  };

double sin(int angle)
  {
  keep_angle_in_range(angle);

  if(angle >= 0 && angle <= 90)
    return sin_table[angle];
  if(angle > 90 && angle <= 180)
    return sin_table[180-angle];
  if(angle > 180 && angle <= 270)
    return - sin_table[angle-180];
//if(angle > 270 && angle <= 360)
    return - sin_table[360-angle];
  }

double cos(int angle)
  {
  keep_angle_in_range(angle);

  if(angle >= 0 && angle <= 90)
    return sin_table[90-angle];
  if(angle > 90 && angle <= 180)
    return - sin_table[angle-90];
  if(angle > 180 && angle <= 270)
    return - sin_table[270-angle];
//if(angle > 270 && angle <= 360)
    return sin_table[angle-270];
  }

#if 0
/* Prints a sin table. */
#include <stdio.h>
#include <math.h>

int main()
  {
  int i;
  double angle_rad;

  printf("static const double sin_table[91] = {\n  ");
  for(i = 0; i < 91; i++)
    {
    angle_rad = (i * M_PI) / 180.0;
    printf("%1.6f", sin(angle_rad));

    if((i+1) % 4 == 0)
      printf(",\n  ");
    else
      printf(", ");
    }
  printf("\n  };\n\n");

  return 0;
  }
#endif
