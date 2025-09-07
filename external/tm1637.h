#ifndef TM1637_H
#define TM1637_H

#ifdef __cplusplus
extern "C" {
#endif

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Public functions
//
// Initialize the tm1637 by specifying the digital pins used to communicate
// returns 0 for success, -1 for failure
//
int tm1637Init(unsigned char bClock, unsigned char bData);

//
// Set brightness (0-8)
//
void tm1637SetBrightness(unsigned char b);

//
// Display a string of 4 digits and optional colon
// by passing a string such as "12:34" or "45 67"
//
void tm1637ShowDigits(const char *pString);

#ifdef __cplusplus
}
#endif

#endif // TM1637_H
