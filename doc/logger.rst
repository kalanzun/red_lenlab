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
Logger
******

Der Logger misst die Spannung an vier Pins wiederholt in einem festen Abtastintervall
für eine lange Zeit. Die Messung beginnt nach einem Klick auf "Start" und endet bei einem Klick auf "Stop".
Bei einem erneuten Klick auf "Start" wird die Messung fortgesetzt. Es entsteht keine Lücke in den Daten.

Bei einem Klick auf "Zurücksetzen" werden die aktuellen Messwerte gelöscht und es kann ein neues Abtastintervall
eingestellt werden.

Eigenschaften
=============

.. list-table:: Pin-Belegung

    * - Kanal 1
      - PD0
      - ADC0
    * - Kanal 2
      - PD2
      - ADC0
    * - Kanal 3
      - PD1
      - ADC1
    * - Kanal 4
      - PD3
      - ADC1

Messbereich Spannung
    0 bis 3,3 V

Analog-Digital-Converter
    Das Modul ADC0 erfasst abwechselnd Kanal 1 und 2.
    und das Modul ADC1 erfasst abwechselnd Kanal 3 und 4.
    Beide Module arbeiten mit einer Samplerate von 1 MHz.

Auflösung
    12 bit = 4096 für 3,3 V. Spannungsauflösung 0,8 mV.

Mittelung
    Für einen Messwert erfassen die ADCs 32 samples,
    berechnen den Mittelwert und runden das Ergebnis auf 12 bit.

Abtastintervall
    100 ms, 200 ms, 500 ms, 1 s, 2 s oder 5 s

Langzeitmessung
===============

Die Firmware auf dem Launchpad erfasst die Messwerte und sendet sie sofort an den Computer. Wenn der Computer einschläft
oder überlastet ist und die Messwerte nicht mehr annehmen kann bricht die Messung ab und der Logger stoppt.
Das passiert, wenn der Computer nicht bedient wird und wegen Energieeinstellungen in einen Ruhezustand geht
oder wenn Updates installiert werden. Windows entscheidet offenbar auch manchmal spontan,
die Übertragungsgeschwindigkeit zum Launchpad stark zu reduzieren. Es überträgt dann noch Daten, aber zu langsam.

Lenlab meldet in diesem Fall im Nachrichtenfenster "Die Logger-Messung wurde abgebrochen weil die USB-Verbindung
zu langsam war." Speichern Sie Ihre Daten ab oder verwenden Sie die Funktion "Automatisch Speichern". Sie können
die Messung mit "Start" fortsetzen, doch oft tritt der Fehler bald wieder auf. Schalten Sie in diesem Fall
das Launchpad mit dem Schalter aus- und wieder ein. Verwenden Sie ein größeres Abtastintervall.

Schließen Sie das Netzteil Ihres Laptops an.

Energieeinstellungen in Windows
-------------------------------

Im Programm "Einstellungen" unter "System", "Strom & Akku".

In den "Energieoptionen", wählen Sie "Nie" für den automatischen Wechsel in einen "Ruhezustand".

Das automatische Ausschalten des Bildschirms ist kein Problem.
