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

********
Firmware
********

Lenlab benötigt eine eigene Firmware auf dem LaunchPad. Das LaunchPad besitzt
ein integriertes Programmiergerät, um die Firmware in den Microcontroller zu
laden. Das Programm `Uniflash <https://www.ti.com/tool/UNIFLASH>`_ von Texas
Instruments kann dieses Programmiergerät ansprechen.

Die Firmware finden Sie unter Sie unter "Releases" auf github:
`Releases kalanzun/red_lenlab <https://github.com/kalanzun/red_lenlab/releases>`_.
Wählen Sie bitte die neueste, stabile Version, welche mit „Latest“ markiert ist.
Die „Pre-releases“ sind zum Testen.

Das Programmiergerät befindet sich auf der Oberseite des Launchpad und hat dort
einen eigenen USB-Anschluss, der mit DEBUG beschriftet ist. Zum Programmieren bitte
den DEBUG-Anschluss an der schmalen Seite des Launchpad mit dem Computer verbinden
und den Power-Schalter ebenfalls auf DEBUG umstellen. Das Launchpad wird dann
über den DEBUG-Anschluss mit Strom versorgt und die grüne Power-LED leuchtet.

lenlab_flash-Paket für Windows
------------------------------

Laden Sie `Uniflash <https://www.ti.com/tool/UNIFLASH>`_ von Texas Instruments
herunter und installieren Sie es im Standard-Pfad. Es fragt bei der Installation,
ob es Treiber installieren darf. Bitte bestätigen.

Laden Sie das Paket ``lenlab_flash-[VERSION]-win.zip`` herunter von
`Releases kalanzun/red_lenlab <https://github.com/kalanzun/red_lenlab/releases>`_.
Es ist ein zip-Archiv, bitte entpacken Sie es.
(Im Windows Explorer, Rechtsklick auf das Archiv, "Alle extrahieren...").

Das Paket enthält die Firmware und ein kleines Programm, welches Uniflash mit den richtigen Argumenten startet.
Das Programm schaut im Standard-Pfad ``C:\ti\`` nach dem installierten Uniflash. Es schlägt fehl, wenn Uniflash nicht
dort installiert ist oder wenn mehr als ein Uniflash installiert ist. (Es führt in diesem Fall alle aus).

* Öffnen Sie das Verzeichnis des entpackten Pakets im Explorer.
* Das Lenlab-Board ist über den DEBUG-Anschluss verbunden, der Schalter steht auf DEBUG, die grüne Power-LED leuchtet.
* Starten Sie die Programmierung mit der Datei ``lenlab_flash.bat``.
* Im Dialog "Der Computer wurde durch Windows geschützt", klicken Sie auf "Weitere Informationen".
  Dann können Sie auf "Trotzdem ausführen" klicken.
* Die Programmierung dauert einen Moment. Am Ende steht in der vorletzten Zeile "Success" wenn die
  Programmierung erfolgreich war oder "Failed" bei Misserfolg.

Flashen mit Uniflash
--------------------

Laden Sie `Uniflash <https://www.ti.com/tool/UNIFLASH>`_ von Texas Instruments
herunter und installieren Sie es.

Uniflash unterstützt derzeit Windows, Mac und Linux, aber nur x86_64.

Laden Sie die Firmware ``lenlab_firmware-[VERSION].out`` herunter von
`Releases kalanzun/red_lenlab <https://github.com/kalanzun/red_lenlab/releases>`_.

* Das Lenlab-Board ist über den DEBUG-Anschluss verbunden, der Schalter steht auf DEBUG, die grüne Power-LED leuchtet.
* Starten Sie Uniflash.
* Wählen Sie EK-TM4C123GXL (Launchpad) bei "Choose your device".
* "Stellaris In-Circuit Debug Interface" ist richtig.
* Klicken Sie auf "Start".
* "Browse", wählen Sie die Firmware-Datei.
* "Binary" darf keinen Haken haben.
* Starten Sie die Programmierung mit "Load Image".
