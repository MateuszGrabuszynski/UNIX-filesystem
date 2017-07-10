# UNIX-filesystem

## English
This repository contains public copy of UNIX-based file system simulation in C++. My part of bigger student project "PenguinOS" simulating whole UNIX os.

### Important informations
1) File can't exceed 10*SO_BLOCK chars because there are only 2 direct indexes (2*SO_BLOCK) and 1 indirect (8*SO_BLOCK).
2) Any negative offset will be treated as position at the beginning of the file.
Any offset that exceeds filesize will be treated as position at the end of the file.
3) Some additional steps are taken while running test programs (showing occupied i-nodes, showing memory blocks etc.).

This is part of bigger student project simulating whole UNIX system I have completed with friends (which repository is private on Github).
Code development ended in Feb 2017. Only minor changes (including translations) were made since then.
Poznan University of Technology 2017

### Author
Mateusz Grabuszynski
mateusz.grabuszynski (at) student.put.poznan.pl
mgrabuszynski (at) gmail.com

## polski 
To repozytorium zawiera publiczną kopię symulacji systemu plików imitującego zastosowany w UNIXie napisaną w języku C++. Jest to moja część większego projektu studenckiego "PingwinOS" symulującego cały system UNIX.

### Ważne informacje
1) Rozmiar pliku nie moze przekraczac 10*SO_BLOCK znakow, gdyz sa tylko 2 indeksy bezposrednie (2*SO_BLOCK) i 1 posredni (8*SO_BLOCK).
2) Ujemna pozycja kursora jest traktowana jako poczatek pliku.
Pozycja wieksza od rozmiaru pliku jest traktowana jako koniec pliku.
3) Programy przykladowe wykonuja dodatkowe, nieopisane w komentarzu, kroki (np. pokazywanie zajetych i-wezlow).

Ten kod jest czescia wiekszego projektu symulujacego caly system UNIX, ktory wykonalismy wraz z kolegami (repozytorium prywatne).
Rozwoj kodu zakonczono w lutym 2017. Od tego czasu wprowadzono tylko niewielkie zmiany, głównie tłumaczenie.
Politechnika Poznanska 2017

### Autor
Mateusz Grabuszynski
mateusz.grabuszynski (at) student.put.poznan.pl
mgrabuszynski (at) gmail.com