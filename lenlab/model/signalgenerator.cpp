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

#include "lenlab.h"
#include "signalgenerator.h"

namespace model {

Signalgenerator::Signalgenerator(Lenlab *parent) : Component(parent)
{

}

void
Signalgenerator::setSine(uint32_t index)
{
    auto cmd = usb::newCommand(setSignalSine);
    cmd->setByteArray(sine[index], 3); // multiplier, predivider, divider
    lenlab->send(cmd);
}

#define OVERHEAD (16.0/17.5)
#define CHANNELS (2)
#define SAMPLES (500)
#define BITS_PER_SAMPLE (16)
#define SYSCLK (80000000.0)
#define BASE_FREQUENCY (SYSCLK / CHANNELS / SAMPLES / BITS_PER_SAMPLE * OVERHEAD)

double
Signalgenerator::getFrequency(uint32_t index)
{
    return BASE_FREQUENCY  * (double) sine[index][0] / (double) (sine[index][1] * sine[index][2]);
}

} // namespace model
