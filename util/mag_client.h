/* AT-SPI : Assistive Technology Service Provider Interface
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef MAG_CLIENT_H_
#define MAG_CLIENT_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

Accessibility_Magnifier get_magnifier(void);
void magnifier_set_roi (int x1, int y1, int x2, int y2);
void magnifier_set_magnification (float mag_factor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MAG_CLIENT_H_ */

