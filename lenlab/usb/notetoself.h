/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef NOTETOSELF_H
#define NOTETOSELF_H

#include "active.h"
#include "activeguard.h"
#include "message.h"
#include "transfer.h"

namespace usb {

    class NoteToSelf
    {
    public:
        NoteToSelf(Transfer *transfer, Active *active, pMessage message);

        Transfer *transfer;
        resource::ActiveGuard active_guard;
        pMessage message;
    };

}

#endif // NOTETOSELF_H
