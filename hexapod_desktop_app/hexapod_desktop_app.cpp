/**
 * S T E W A R T    P L A T F O R M    O N    E S P 3 2
 *
 * Copyright (C) 2019  Nicolas Jeanmonod, ouilogique.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

// USER CHOICES

// Number of intervals
const double nb_intervals = 1;
// Reduce min/max
const double shrink = 3;

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <time.h>
#include "Hexapod_Kinematics.h"

using namespace std;

// Variables.
Hexapod_Kinematics hk; // Stewart platform object.
angle_t servo_angles[NB_SERVOS];
int movOK = -1;
ofstream angle_file;
double cpu_time_used;
double counter = 0;

// Print dimensions
const uint8_t SMALL_WIDTH = 7;
const uint8_t LARGE_WIDTH = 17;
const uint8_t ALL_WIDTH = 151;

void calcAndPrintResults(platform_t coords)
{
     clock_t T1 = clock();
     movOK = hk.calcServoAngles(coords, servo_angles);
     // movOK = hk.calcServoAnglesAlgo1(coords, servo_angles);
     // movOK = hk.calcServoAnglesAlgo2(coords, servo_angles);
     // movOK = hk.calcServoAnglesAlgo3(coords, servo_angles);
     clock_t T2 = clock();
     cpu_time_used += (double)(T2 - T1);
     counter++;

     angle_file << fixed << setprecision(1) << setw(SMALL_WIDTH) << setfill(' ') << coords.hx_x;
     angle_file << fixed << setprecision(1) << setw(SMALL_WIDTH) << setfill(' ') << coords.hx_y;
     angle_file << fixed << setprecision(1) << setw(SMALL_WIDTH) << setfill(' ') << coords.hx_z;
     angle_file << fixed << setprecision(1) << setw(SMALL_WIDTH) << setfill(' ') << degrees(coords.hx_a);
     angle_file << fixed << setprecision(1) << setw(SMALL_WIDTH) << setfill(' ') << degrees(coords.hx_b);
     angle_file << fixed << setprecision(1) << setw(SMALL_WIDTH) << setfill(' ') << degrees(coords.hx_c);
     angle_file << fixed << setw(SMALL_WIDTH) << setfill(' ') << movOK;

     if (movOK >= 0)
     {
          for (uint8_t id = 0; id < NB_SERVOS; id++)
          {
#define WHAT_TO_PRINT 2
#if WHAT_TO_PRINT == 1
               angle_file << fixed << setprecision(6) << setw(LARGE_WIDTH) << setfill(' ') << servo_angles[id].deg;
#elif WHAT_TO_PRINT == 2
               angle_file << fixed << setprecision(6) << setw(LARGE_WIDTH) << setfill(' ') << servo_angles[id].us;
#elif WHAT_TO_PRINT == 3
               angle_file << fixed << setprecision(6) << setw(LARGE_WIDTH) << setfill(' ') << servo_angles[id].rad;
#elif WHAT_TO_PRINT == 4
               angle_file << fixed << setprecision(6) << setw(LARGE_WIDTH) << setfill(' ') << servo_angles[id].debug;
#endif
          }
     }
     angle_file << endl;
}

int main()
{
     // Prepare file for output.
     stringstream ss;
     string file_name_str;
     ss << "../doc/angles_config_" << HEXAPOD_CONFIG << ".txt";
     file_name_str = ss.str();
     char *fname = &file_name_str[0u];
     angle_file.open(fname);

     // Write file header.
     angle_file << "\nSTEWART PLATFORM\n\n";
     angle_file << "COMPILATION DATE AND TIME\n";
     angle_file << __DATE__ << endl;
     angle_file << __TIME__ << endl;
     angle_file << "HEXAPOD_CONFIG = " << HEXAPOD_CONFIG << endl;
     angle_file << "ALGORITHM = " << ALGO << endl;
     angle_file << endl;

     // Print column titles.
     angle_file << fixed << setw(SMALL_WIDTH) << setfill(' ') << "X";
     angle_file << fixed << setw(SMALL_WIDTH) << setfill(' ') << "Y";
     angle_file << fixed << setw(SMALL_WIDTH) << setfill(' ') << "Z";
     angle_file << fixed << setw(SMALL_WIDTH) << setfill(' ') << "A";
     angle_file << fixed << setw(SMALL_WIDTH) << setfill(' ') << "B";
     angle_file << fixed << setw(SMALL_WIDTH) << setfill(' ') << "C";
     angle_file << fixed << setw(SMALL_WIDTH) << setfill(' ') << "movOK";
     angle_file << fixed << setw(LARGE_WIDTH) << setfill(' ') << "ANGLE 1";
     angle_file << fixed << setw(LARGE_WIDTH) << setfill(' ') << "ANGLE 2";
     angle_file << fixed << setw(LARGE_WIDTH) << setfill(' ') << "ANGLE 3";
     angle_file << fixed << setw(LARGE_WIDTH) << setfill(' ') << "ANGLE 4";
     angle_file << fixed << setw(LARGE_WIDTH) << setfill(' ') << "ANGLE 5";
     angle_file << fixed << setw(LARGE_WIDTH) << setfill(' ') << "ANGLE 6";

     // Print separator.
     angle_file << endl;
     angle_file << fixed << setw(ALL_WIDTH) << setfill('=') << "" << endl;

     // 0
     calcAndPrintResults({0, 0, 0, 0, 0, 0});
     calcAndPrintResults({0, 0, HX_Z_MAX, 0, 0, 0});
     calcAndPrintResults({0, 0, HX_Z_MIN, 0, 0, 0});
     angle_file << fixed << setw(ALL_WIDTH) << setfill('=') << "" << endl;

     // Compute and print angles in the respective min/max ranges.
     for (double hx_x = HX_X_MIN / shrink; hx_x <= HX_X_MAX / shrink; hx_x += (HX_X_MAX - HX_X_MIN) / nb_intervals / shrink)
     {
          for (double hx_y = HX_Y_MIN / shrink; hx_y <= HX_Y_MAX / shrink; hx_y += (HX_Y_MAX - HX_Y_MIN) / nb_intervals / shrink)
          {
               for (double hx_z = HX_Z_MIN / shrink; hx_z <= HX_Z_MAX / shrink; hx_z += (HX_Z_MAX - HX_Z_MIN) / nb_intervals / shrink)
               {
                    for (double hx_a = HX_A_MIN / shrink; hx_a <= HX_A_MAX / shrink; hx_a += (HX_A_MAX - HX_A_MIN) / nb_intervals / shrink)
                    {
                         for (double hx_b = HX_B_MIN / shrink; hx_b <= HX_B_MAX / shrink; hx_b += (HX_B_MAX - HX_B_MIN) / nb_intervals / shrink)
                         {
                              for (double hx_c = HX_C_MIN / shrink; hx_c <= HX_C_MAX / shrink; hx_c += (HX_C_MAX - HX_C_MIN) / nb_intervals / shrink)
                              {
                                   calcAndPrintResults({hx_x, hx_y, hx_z, hx_a, hx_b, hx_c});
                              }
                         }
                    }
               }
          }
     }

     // Done.
     angle_file.close();

     // Reopen and print results in the console.
     freopen(fname, "rb", stdin);
     string line;
     while (getline(cin, line))
          cout << line << endl;

     cpu_time_used = cpu_time_used * 1.0E6 / CLOCKS_PER_SEC;
     cout << "\nTime elapsed = " << cpu_time_used << " µs";
     cout << "\nTime per calculation = " << cpu_time_used / counter << " µs\n";
     return 0;
}
