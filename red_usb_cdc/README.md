red_usb_cdc
===========

This firmware uses the USB CDC driver to appear as a virtual serial port (COM port on Windows).
It sends data as fast as possible.

The pytest receives that data and measures the transfer speed. I achieved about 500 kB/s.
The processor itself seems to be the limiting factor, it writes as fast as it can.
I did not get DMA to work with the CDC driver.
