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

************
Vorbereitung
************

.. image:: launchpad_usb_ports.svg

Das Launchpad hat zwei USB-Anschlüsse, einen für das Programmieren und Testen
der Firmware (DEBUG) und einen für die Kommunikation zwischen der Firmware und
Lenlab auf dem Computer (DEVICE). Der POWER-Schalter wählt, von welchem
USB-Anschluss die Versorgungsspannung bezogen wird. Wenn die Stromversorgung
eingeschaltet ist, leuchtet die grüne POWER-LED, welche sich ebenfalls in dem
Bereich links oben befindet.

Download des Lenlab-Softwarepakets
==================================

Sie finden die Softwarepakete unter "Releases" auf github:
`Lenlab releases <https://github.com/kalanzun/red_lenlab/releases>`_.

Für Windows ist das Softwarepaket ein zip-Archiv. Bitte entpacken Sie es.
(Im Explorer, Rechtsklick auf das Archiv, "Alle extrahieren...")

Flashen der Firmware
====================

Lenlab benötigt eine eigene Firmware auf dem LaunchPad. Das LaunchPad besitzt
ein integriertes Programmiergerät, um die Firmware in den Microcontroller zu
laden. Das Programm `Uniflash <https://www.ti.com/tool/UNIFLASH>`_ von Texas
Instruments kann dieses Programmiergerät ansprechen.

Das Programmiergerät befindet sich auf der Oberseite des Launchpad und hat dort
einen eigenen USB-Anschluss, der mit DEBUG beschriftet ist. Zum Programmieren bitte
den DEBUG-Anschluss an der schmalen Seite des Launchpad mit dem Computer verbinden
und den Power-Schalter ebenfalls auf DEBUG umstellen. Das Launchpad wird dann
über den DEBUG-Anschluss mit Strom versorgt und die grüne Power-LED leuchtet.

Das Lenlab-Softwarepaket für Windows enthält eine Kopie von Uniflash für
64-bit Systeme (z.B. die meisten Windows 8 und 10). Die Anleitung dafür befindet
im folgenden Abschnitt :ref:`flash_windows_64`. Für andere Betriebssysteme oder
Windows 32-bit Systeme erhalten Sie Uniflash direkt von Texas Instruments und
eine Anleitung zur Verwendung im Abschnitt :ref:`flash_other_systems`.

.. _flash_windows_64:

Windows 64-bit Systeme
----------------------

Im Unterverzeichnis `uniflash_windows_64` im Lenlab-Softwarepaket befindet sich
das Programm zum Flashen der Firmware mit fertig eingestellten Parametern für
die Lenlab Firmware und das LaunchPad.

* Öffnen Sie das Verzeichnis `uniflash_windows_64` im Explorer.
* Installieren Sie die Treiber durch Starten der `one_time_setup.bat`. Das ist
  pro Computer nur einmal nötig.
* Starten Sie die Programmierung mit der Datei `dslite.bat`. Die Programmierung
  dauert einen Moment. Am Ende steht in der vorletzten Zeile "Success" wenn die
  Programmierung erfolgreich war oder "Failed" bei Misserfolg.

.. _flash_other_systems:

Andere Systeme
--------------

Laden Sie `die Firmware <https://github.com/kalanzun/red_lenlab/tree/master/bin>`_ direkt von github herunter.
Die benötigte Datei heißt `lenlab_firmware_[VERSION].out`. Verwenden Sie die selbe Version wie Ihr Release Download.

Laden Sie `Uniflash <https://www.ti.com/tool/UNIFLASH>`_ von Texas Instruments
herunter, Version 5, installieren Sie es und starten Sie es.

* Wählen Sie EK-TM4C123GXL (Launchpad) bei "Choose your device".
* Stellaris In-Circuit Debug Interface ist richtig.
* Klicken Sie auf "Start".
* Browse, wählen Sie die Firmware-Datei.
* Binary darf keinen Haken haben.
* Starten Sie die Programmierung mit "Load Image".

Windows 7
---------

**Windows 7 64 bit**: Flashen Sie die Firmware, wie unter :ref:`flash_windows_64` beschrieben.

**Windows 7 32 bit**: Laden Sie in diesem Fall Version 4 von
`Uniflash v4 Releases <https://software-dl.ti.com/ccs/esd/uniflash/docs/release_archive.html#uniflash-v4-releases>`_
herunter. Version 5 funktioniert auf 32-bit Systemen nicht. Nach der Installation, aber bevor Uniflash gestartet wird,
muss noch der Treiber installiert werden.

* Das Lenlab-Board ist über den DEBUG-Anschluss verbunden, der Schalter steht auf DEBUG, die grüne Power-LED leuchtet.
* Systemsteuerung -> System -> Geräte-Manager.
* Unter "Andere Geräte" gibt es drei "In-Circuit Debug Interface", welche mit einem kleinen, gelben Ausrufezeichen markiert sind.
* Doppelklick auf ein "In-Circuit Debug Interface".
* "Treiber aktualisieren" anklicken.
* "Auf dem Computer nach Treibersoftware suchen".
* Als Ort `C:\\ti` eintragen, oder das Verzeichnis, in dem Sie Uniflash installiert haben.
* Unterordner einbeziehen: Ja.
* Weiter.
* Windows sucht und installiert den Treiber von Uniflash.
* Führen Sie die Treiber-Installation für alle drei "In-Circuit Debug Interface" aus.
* Nach der Installation heißen die Geräte "Stellaris ..." und haben keine gelbe Markierung mehr.

Flashen Sie dann die Firmware mit Version 4, wie unter :ref:`flash_other_systems` beschrieben.
Die Firmware-Datei ist auch im Lenlab-Softwarepaket enthalten, im Unterordner `firmware`. Danach muss noch ein
Treiber installiert werden. Siehe unten.

**Windows 7 ? bit**: Wenn Sie nicht wissen, wie viel Bit Ihr Windows hat, versuchen Sie zunächst
die einfachere Methode für 64 bit.
Falls Sie doch ein 32-bit System haben, dann passiert bei der `one_time_setup.bat` nichts,
es blinkt nur ganz kurz ein Fenster auf, die Fehlermeldung ist nicht lesbar. Die `dslite.bat`
startet und arbeitet eine Weile, bricht dann aber ab. Die Fehlermeldung lautet:
"error: CORTEX_M4_0: Error connecting to the target: Unable to communicate with
the device. Please check your connection. Failed: Operation was aborted"
Versuchen Sie dann die Methode für 32bit. Achten Sie darauf, Version 4 von Uniflash zu verwenden,
Version 5 funktioniert auf 32-bit Systemen nicht.

**Windows 7 64 bit und 32 bit:** Nach dem Flashen ist bei Windows 7 noch die Installation eines weiteren Treibers notwendig
für die Kommunikation zwischen der Lenlab-Software auf dem PC und dem Lenlab-Board.
Das Programm "Zadig" führt die Treiberinstallation aus.

* Laden Sie "Zadig" von `https://zadig.akeo.ie <https://zadig.akeo.ie>`_ herunter.
* Verbinden Sie den DEVICE-Anschluss des Lenlab-Boards mit dem Computer.
* Schalten Sie den Schalter auf DEVICE, so dass die grüne Power-LED leuchtet.
* Starten Sie Zadig.
* Wählen Sie in der Liste das "Lenlab Red Board".
* Wählen Sie in der Zeile "Driver" den WinUSB-Treiber. In der Zeile steht dann:
  "Driver (NONE) -> WinUSB (...)"
* Klicken Sie auf "Install WCID Driver"
* Zadig löst die Treiberinstallation in Windows aus.
* Zadig meldet gleich Erfolg, Windows braucht danach aber noch eine Weile,
  bis der Treiber fertig installiert ist.
* Windows meldet später auch Erfolg, mit einer Meldung unten rechts im Bildschirm über ein "Generic WinUSB Driver".
* Windows fordert zum Neustart auf. Starten Sie Windows neu.

Der Treiber ist nun installiert und Lenlab kann mit dem Board kommunizieren.

Lenlab stürzt auf Windows 7 manchmal ab, wenn es gestartet wird und das Board bereits verbunden ist.
Es funktioniert, wenn Lenlab zuerst gestartet wird und dann das Board verbunden wird.

Lenlab ausführen
================

Verbinden Sie den DEVICE USB-Anschluss an der langen Seite des Launchpad mit dem Computer.
Stellen Sie den Power-Schalter auf DEVICE. Die grüne Power-LED leuchtet.

Lenlab sucht nach dem Board und verbindet sich, wenn es eines findet. Es meldet
dann im Nachrichten-Fenster "Lenlab-Board bereit."

Windows
-------

Das Windows-Programm befindet sich im Unterordner `lenlab` im Lenlab-Softwarepaket. Starten Sie `lenlab.exe`.

Mac
---

Im Lenlab-Softwarepaket befindet sich das Mac-Programm `lenlab`. Starten Sie es.

Linux
-----

Der Download ist ein AppImage. Die meisten modernen Distributionen können die Datei direkt ausführen.
Aktivieren Sie dafür das Dateiattribut für "ausführbar" und starten Sie die Datei.
Einige Distributionen verlangen, dass LenLab als Administrator (Super User) ausgeführt wird.
Dies zeigt sich durch die Fehlermeldung "Access denied (insufficient permissions)" im Nachrichtenfenster.

Im Terminal:

- Wechseln Sie in das Verzeichnis, in dem das AppImage liegt: `cd [Verzeichnisname]`
- Aktivieren Sie das Dateiattribut "ausführbar": `chmod +x Lenlab*`
- Führen Sie die Datei aus: `./Lenlab*`
- Ausführen als Administrator: `sudo ./Lenlab*`

ARM v8 (arm64) Systeme (Raspberry Pi, ...)
------------------------------------------

Der Download ist das Lenlab executable binary. Es gelten die gleichen Anweisungen wie für Linux.

Lenlab für ARM hat keine Programm-Bibliotheken dabei. Unter Umständen müssen diese noch installiert werden. Lenlab benötigt:

- qt5
- libqwt
- libusb

Auf ubuntu lautet der Befehl für die Installation:

`sudo apt install sudo apt install libqwt-qt5-6 libqt5svg5 libqt5opengl5 libusb-1.0-0`
