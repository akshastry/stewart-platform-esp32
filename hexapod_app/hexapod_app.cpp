/**
 * S T E W A R T    P L A T F O R M    O N    E S P 3 2
 *
 * Based on
 * 6dof-stewduino
 * Copyright (C) 2018  Philippe Desrosiers
 * https://github.com/xoxota99/stewy
 *
 * Derived from the work of Daniel Waters
 * https://www.youtube.com/watch?v=1jrP3_1ML9M
 *
 * Modified by Nicolas Jeanmonod
 * ouilogique.com
 * March 2019
 *
 *
 * Copyright (C) 2019  Nicolas Jeanmonod
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
const int nb_intervals = 2;

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include "Hexapod_Kinematics.h"

using namespace std;

// Variables.
Hexapod_Kinematics hk; // Stewart platform object.
angle_t servo_angles[NB_SERVOS];
int movOK = -1;
ofstream angle_file;

// Print dimensions
const uint8_t SMALL_WIDTH = 7;
const uint8_t LARGE_WIDTH = 17;
const uint8_t ALL_WIDTH = 151;

void calcAndPrintResults(platform_t coords)
{
     movOK = hk.calcServoAngles(coords, servo_angles);
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
#define WHAT_TO_PRINT 1
#if WHAT_TO_PRINT == 1
               angle_file << fixed << setprecision(6) << setw(LARGE_WIDTH) << setfill(' ') << servo_angles[id].deg;
#elif WHAT_TO_PRINT == 2
               angle_file << fixed << setprecision(6) << setw(LARGE_WIDTH) << setfill(' ') << servo_angles[id].pw;
#elif WHAT_TO_PRINT == 3
               angle_file << fixed << setprecision(6) << setw(LARGE_WIDTH) << setfill(' ') << servo_angles[id].rad;
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
     for (double sway = HX_X_MIN; sway <= HX_X_MAX; sway += (HX_X_MAX - HX_X_MIN) / nb_intervals)
     {
          for (double surge = HX_Y_MIN; surge <= HX_Y_MAX; surge += (HX_Y_MAX - HX_Y_MIN) / nb_intervals)
          {
               for (double heave = HX_Z_MIN; heave <= HX_Z_MAX; heave += (HX_Z_MAX - HX_Z_MIN) / nb_intervals)
               {
                    for (double pitch = HX_A_MIN; pitch <= HX_A_MAX; pitch += (HX_A_MAX - HX_A_MIN) / nb_intervals)
                    {
                         for (double roll = HX_B_MIN; roll <= HX_B_MAX; roll += (HX_B_MAX - HX_B_MIN) / nb_intervals)
                         {
                              for (double yaw = HX_B_MIN; yaw <= HX_B_MAX; yaw += (HX_B_MAX - HX_B_MIN) / nb_intervals)
                              {
                                   calcAndPrintResults({sway, surge, heave, pitch, roll, yaw});
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

     return 0;
}
