..  Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
    Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
    
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

Lenlab red (Version 7)
======================

Lenlab ist eine Software für den PC und eine Firmware für das TI Tiva C series
Launchpad EK-TM4C123GXL für den Workshop Elektrotechnik und Informationstechnik
am Karlsruher Institut für Technologie.

Homepage: `https://github.com/kalanzun/red_lenlab <https://github.com/kalanzun/red_lenlab>`_

Download: `https://github.com/kalanzun/red_lenlab/releases <https://github.com/kalanzun/red_lenlab/releases>`_

Dokumentation: `https://red-lenlab.readthedocs.io <https://red-lenlab.readthedocs.io>`_

Installation
============

Das Lenlab-Softwareprojekt besteht aus zwei Teilen: Eine Firmware, welche auf dem Microcontroller des Launchpads läuft
und eine Software für den Computer, welche die Messdaten vom Launchpad empfängt.

Die Firmware läuft auf dem Launchpad EK-TM4C123GXL (Texas Instruments), Lenlab läuft auf Windows, Mac und Linux.

Zur Verwendung der Software installieren Sie bitte einmalig die Firmware in den Speicher des Mikrocontrollers.
Die Anleitung befindet sich im Unterpunkt :doc:`Firmware <firmware>`. Laden Sie außerdem die passende Variante
der Software für Ihr Betriebssystem herunter und führen Sie diese aus. Die Details werden im Unterpunkt
:doc:`Lenlab <lenlab>` erklärt.

USB-Anschlüsse des Launchpads
=============================

.. image:: launchpad_usb_ports.svg

Das Launchpad hat zwei USB-Anschlüsse, einen für das Programmieren und Testen
der Firmware (DEBUG) und einen für die Kommunikation zwischen der Firmware und
Lenlab auf dem Computer (DEVICE). Der POWER-Schalter wählt, von welchem
USB-Anschluss die Versorgungsspannung bezogen wird. Wenn die Stromversorgung
eingeschaltet ist, leuchtet die grüne POWER-LED, welche sich ebenfalls in dem
Bereich links oben befindet.

.. toctree::
    :maxdepth: 2
    :caption: Inhaltsverzeichnis:

    firmware
    lenlab
    logger
    oscilloscope
    signal
    frequency
    authors
    license

Verzeichnisse
=============

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

Lizenz
======

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL

Copyright (C) 2017-2019 Christoph Simon und das :doc:`Lenlab-Entwicklerteam <authors>`

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

:doc:`license`

You should have received a copy of the GNU General Public License
along with this program.  If not, see `https://www.gnu.org/licenses/ <https://www.gnu.org/licenses/>`_.
