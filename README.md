> **Note:** By no means is this code secure or validated. Treat as unchecked copy archived due to no changes planned.

# UNIX-filesystem
This project is simulation of filesystem based on i-nodes.

## English
This repository contains public copy of UNIX-based file system simulation in C++. My part of bigger student project "PenguinOS" simulating whole UNIX os.

### Important information
This code is not "memory-leakage-free"! There should be lines added, that would be destined for deleting specific allocated blocks.
1) File can't exceed 10 times SO_BLOCK chars because there are only 2 direct indexes (2 times SO_BLOCK) and 1 indirect (8 times SO_BLOCK).
2) Any negative offset will be treated as position at the beginning of the file. Any offset that exceeds filesize will be treated as position at the end of the file.
3) Some additional steps are taken while running test programs (showing occupied i-nodes, showing memory blocks etc.).

This is part of bigger student project simulating whole UNIX system I have completed with friends (which repository is private on Github).  
Code development ended in Feb 2017. Only minor changes (including translations) were made since then.  
Poznan University of Technology 2017

### How-to
Compile with C++ compiler (originally Microsoft Visual C++) and use menu from tests.cpp.

### Author
Mateusz Grabuszyński  
mateusz.grabuszynski (at) student.put.poznan.pl (for students)  
mgrabuszynski (at) gmail.com (for other contact)

### License
This project is publicly available on MIT License. Full text contained inside LICENSE file.

### Last README change
2018-04-13

## polski 
To repozytorium zawiera publiczną kopię symulacji systemu plików imitującego zastosowany w UNIXie napisaną w języku C++. Jest to moja część większego projektu studenckiego "PingwinOS" symulującego cały system UNIX.

### Ważne informacje
W kodzie występują tzw. "wycieki pamięci"! Należałoby dodać odpowiednie linie zapewniające usuwanie danych.
1) Rozmiar pliku nie może przekraczać 10 razy SO_BLOCK znaków, gdyż są tylko 2 indeksy bezpośrednie (2 razy SO_BLOCK) i 1 pośredni (8 razy SO_BLOCK).
2) Ujemna pozycja kursora jest traktowana jako początek pliku. Pozycja większa od rozmiaru pliku jest traktowana jako koniec pliku.
3) Programy przykładowe wykonują dodatkowe, nieopisane w komentarzu, kroki (np. pokazywanie zajętych i-węzłów).

Ten kod jest czescią większego projektu symulujacego cały system UNIX, ktory wykonalismy wraz z kolegami (repozytorium prywatne).  
Rozwój kodu zakonczono w lutym 2017. Od tego czasu wprowadzono tylko niewielkie zmiany, głównie tłumaczenie.  
Politechnika Poznanska 2017

### Uruchomienie
Skompiluj przy użyciu kompilatora języka C++ (pierwotnie Microsoft Visual C++) i użyj menu zawartego w tests.cpp.

### Autor
Mateusz Grabuszyński  
mateusz.grabuszynski (at) student.put.poznan.pl (dla studentów)  
mgrabuszynski (at) gmail.com (inne)

### Licencja
Projekt jest publicznie dostępny pod licencją MIT, której tekst jest zawarty w pliku LICENSE (język angielski).

### Ostatnia zmiana README
13 kwietnia 2018
