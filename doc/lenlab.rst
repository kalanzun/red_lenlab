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

******
Lenlab
******

Lenlab läuft auf Windows, Mac und Linux.

- Windows: Version 10 und 11, nur 64 bit
- Mac: macOS 10.15.7 "Catalina" und neuer
- Linux amd64: Ubuntu 18.04 und neuer
- Linux arm64: Ubuntu 20.04 und neuer

Sie finden den Download unter "Releases" auf github:
`Releases kalanzun/red_lenlab <https://github.com/kalanzun/red_lenlab/releases>`_.
Wählen Sie bitte die neueste, stabile Version, welche mit "Latest" markiert ist. Die "Pre-releases" sind zum Testen.

Launchpad anschließen
=====================

Verbinden Sie den DEVICE USB-Anschluss an der langen Seite des Launchpad mit dem Computer.
Stellen Sie den Power-Schalter auf DEVICE. Die grüne Power-LED leuchtet.

Lenlab sucht nach dem Board und verbindet sich, wenn es eines findet. Es meldet
dann im Nachrichten-Fenster "Lenlab-Board Version [VERSION] verbunden."

Windows
=======

Der Download für Windows heißt ``Lenlab-[VERSION]-win.zip``. Es ist ein zip-Archiv, bitte entpacken Sie es.
(Im Windows Explorer, Rechtsklick auf das Archiv, "Alle extrahieren...").

Die ausführbare Datei heißt ``lenlab.exe`` und ist im Unterordner ``lenlab``.
Starten Sie ``lenlab.exe`` mit einem Doppelklick. Meist zeigt Windows die Dateiendung ``.exe`` nicht an,
sondern nur den Namen ``lenlab``.

Im Dialog "Der Computer wurde durch Windows geschützt", klicken Sie auf "Weitere Informationen".
Dann können Sie auf "Trotzdem ausführen" klicken.

Mac
===

Der Download für Mac heißt ``Lenlab-[VERSION]-mac.dmg``. Öffnen Sie die Datei.
Das Programm darin heißt ``lenlab``, starten Sie es.

Linux
=====

Für Linux stehen verschiedene Downloads zur Verfügung. Ein AppImage, welches die notwendingen Bibliotheken
selbst dabei hat und auf den meisten modernen Linux-Systemen läuft, sowie binaries für Ubuntu 20.04 und neuer
auf den Architekturen x86_64 und arm64.

Ein normaler Benutzer auf Linux hat meist nicht genügend Rechte, dass Lenlab mit der Firmware kommunizieren kann.
Dies zeigt sich durch die Fehlermeldung "Access denied (insufficient permissions)" im Nachrichtenfenster von Lenlab.

Sie können Ihrem Benutzer die Berechtigung geben:

Erstellen Sie (als root) eine Datei ``/etc/udev.d/20-lenlab.rules`` mit dem Inhalt::

  SUBSYSTEM=="usb", ATTR{idVendor}=="1cbe", ATTR{idProduct}=="0270", GROUP="uucp"

Im Terminal::

  sudo echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1cbe", ATTR{idProduct}=="0270", GROUP="uucp"' > /etc/udev.d/20-lenlab.rules

Damit konfigurieren Sie Ihr Linux-System, dass Benutzer der Gruppe ``uucp`` Zugriff über USB auf
die Lenlab-Firmware ``1cbe:0270`` erhalten.

Fügen Sie Ihren Benutzer zu der Gruppe ``uucp`` hinzu. Im Terminal::

  sudo adduser [NAME] uucp

Zum Schluß noch ein logout und login, oder ein Neustart, damit die Gruppenzugehörigkeit neu geladen wird.
Lenlab sollte nun mit der Firmware kommunizieren können.

AppImage
--------

Das AppImage heißt ``Lenlab-[VERSION]-linux-x86_64.AppImage``. Die meisten modernen Distributionen können das AppImage
direkt ausführen. Aktivieren Sie dafür das Dateiattribut für "ausführbar" und starten Sie die Datei.

Im Terminal::

  cd [VERZEICHNIS]
  chmod +x Lenlab*
  ./Lenlab*

Binaries ab Ubuntu 20.04
------------------------

Für x86_64 heißt der Download ``lenlab-[VERSION]-linux-x86_64`` und für arm64 ``lenlab-[VERSION]-linux-arm64``.
Es sind nur die ausführbaren Dateien, die notwendigen Bibliotheken sind nicht dabei.

Installation der Abhängigkeiten::

  sudo apt install qt5-default libqt5charts5 libqt5svg5 libqt5opengl5 libusb-1.0-0

Auf Linux selbst kompilieren
============================

Anbei eine ganz knappe Anleitung, falls Sie lenlab selbst kompilieren möchten.

Abhängigkeiten installieren::

  sudo apt install build-essential git qt5-default libqt5charts5-dev libqt5svg5-dev libqt5opengl5-dev libusb-1.0-0-dev

Quelltext herunterladen::

  git clone https://github.com/kalanzun/red_lenlab.git

In das Projektverzeichnis wechseln::

  cd red_lenlab

Lenlab konfigurieren::

  qmake red_lenlab.pro

Lenlab kompilieren::

  make

Die ausführbare Datei ist::

  lenlab/app/lenlab