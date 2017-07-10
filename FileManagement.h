#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

/* INFO EN
 * Author: Mateusz Grabuszynski
 * mateusz.grabuszynski (at) student.put.poznan.pl
 * mgrabuszynski (at) gmail.com
 *
 * This is part of bigger student project simulating whole UNIX system I have completed with friends (which repository is private on Github).
 * Code development ended in Feb 2017. Only minor changes (including translations) were made since then.
 * Poznan University of Technology 2017
 *
 * IMPORTANT:
 * 1) File can't exceed 10*SO_BLOCK chars because there are only 2 direct indexes (2*SO_BLOCK) and 1 indirect (8*SO_BLOCK).
 * 2) Any negative offset will be treated as position at the beginning of the file.
 *    Any offset that exceeds filesize will be treated as position at the end of the file.
 * 3) Some additional steps are taken while running test programs (showing occupied i-nodes, showing memory blocks etc.).
 */

 /* INFO PL
 * Autor: Mateusz Grabuszynski
 * mateusz.grabuszynski (at) student.put.poznan.pl
 * mgrabuszynski (at) gmail.com
 *
 * Ten kod jest czescia wiekszego projektu symulujacego caly system UNIX, ktory wykonalismy wraz z kolegami (repozytorium prywatne).
 * Rozwoj kodu zakonczono w lutym 2017. Od tego czasu wprowadzono tylko niewielkie zmiany, głównie tłumaczenie.
 * Politechnika Poznanska 2017
 *
 * WAZNE:
 * 1) Rozmiar pliku nie moze przekraczac 10*SO_BLOCK znakow, gdyz sa tylko 2 indeksy bezposrednie (2*SO_BLOCK) i 1 posredni (8*SO_BLOCK).
 * 2) Ujemna pozycja kursora jest traktowana jako poczatek pliku.
 *    Pozycja wieksza od rozmiaru pliku jest traktowana jako koniec pliku.
 * 3) Programy przykladowe wykonuja dodatkowe, nieopisane w komentarzu, kroki (np. pokazywanie zajetych i-wezlow).
 */

/////////////////////////////////////////////////
#include <bitset>
#include <time.h>
#include <iostream>
#include <list>
#include <vector>
#include <math.h>

#define NO_MEMBLOCKS 64	//total number of memory blocks (multiplied by SO_BLOCK gives total disk size in chars) / calkowita liczba blokow pamieci (pomnozona razy SO_BLOCK daje calkowity rozmiar dysku w znakach)
#define NO_INODES 16	//total number of inodes (max. number of files) / calkowita liczba i-wezlow (maksymalna liczba plikow)
#define SO_BLOCK 16		//size of one of memory blocks / rozmiar jednego bloku pamieci

#define DESCRIPTOR int	//descriptor is just inode here, this represents its number in inodeTable / deskryptor jest tutaj i-wezlem, reprezentuje jego number w tablicy i-wezlow
///////////////////////////////////////////////


class FileManagement{
public:
	bool debug = false;

protected:
	int countBlocksFromSize(int);

	struct dirEnt {
		unsigned short inodeNumber;
		std::string name;
	}; std::list<dirEnt> root;
	bool newDir(int inodeNr, std::string name) {
		dirEnt temp;
		temp.inodeNumber = inodeNr;
		temp.name = name;
		root.push_back(temp);
		return true;
	}
	bool delDir(std::string name, int inodeNr = -1) {
		if (inodeNr != -1) {
			for (std::list<dirEnt>::iterator outIter = root.begin(); outIter != root.end(); outIter++) {
				if (outIter->inodeNumber == inodeNr) {
					root.erase(outIter);
					return true;
				}
			}
		}
		else if (name != "") {
			for (std::list<dirEnt>::iterator outIter = root.begin(); outIter != root.end(); outIter++) {
				if (outIter->name == name) {
					root.erase(outIter);
					return true;
				}
			}
		}
		else {
			std::cerr << "Something went wrong while deleting directory entry! / Wystapil problem z usuwaniem wpisu katalogowego!";
		}
		return false;
	}

	// 1 - occupied / 0 - free
	// 1 - zajety / 0 - wolny
	struct superBlock {
		std::bitset<NO_INODES> inodes;
		std::bitset<NO_MEMBLOCKS> memBlocks;

	public:
		int assignInode() {
			if (this->inodes.all()) return -1; //returns -1 if there's no 0 in superblock / zwraca -1 gdy nie ma 0 w superbloku
			else {
				for (int i = 0; i < NO_INODES; i++) {
					if (this->inodes.test(i) == 0) {
						this->inodes.set(i);
						return i;
					}
				}
			}
			std::cerr << "Something went wrong while trying to assign new inode! / Wystapil problem podczas proby przypisania nowego i-wezla!";
			return -1;
		}
		int assignMemBlock() {
			if (this->memBlocks.all()) return -1; //returns -1 if there's no 0 in superblock / zwraca -1 gdy nie ma 0 w superbloku
			else {
				for (int i = 0; i < NO_MEMBLOCKS; i++) {
					if (this->memBlocks.test(i) == 0) {
						this->memBlocks.set(i);
						return i;
					}
				}
			}
			std::cerr << "Something went wrong while looking for a free memory block! / Wystapil problem podczas szukania wolnego bloku pamieci!";
			return -1;
		}
		void print() {
			std::cout << "Printing superBlock: / Drukuje superBlock:\n";
			for (int i = 0; i < NO_INODES; i++) {
				std::cout << this->inodes.test(i);
			}
			std::cout << " ";
			for (int i = 0; i < NO_MEMBLOCKS; i++) {
				std::cout << this->memBlocks.test(i);
			}
		}
	}supB;

	struct inode {
		unsigned short fileSize;	//how many chars does the file contain / ile znakow zawiera plik
		time_t cTime, mTime, aTime; //creationTime, modificationTime, accessTime / czas utworzenia, czas modyfikacji, czas dostepu

		short direct1st, direct2nd;	// Those are memoryBlocks pointers: / Wskazniki na bloki pamieci:
		short indir1st;				// - direct means it points memory blocks directly / - direct oznacza wskazanie bezposrednie
									// - indirect means it points index blocks that points memory blocks / - indirect oznacza wskazanie na blok pamieci ze wskaznikami na bloki pamieci
									// - value -1 means its not used (because of smaller file size) / - wartosc -1 oznacza, ze dane pole jest nieuzywane (plik jest mniejszy)
		
		short refCount;				//reference counter / licznik odniesien
		short status;				//0 - read only, 1 - write only, 2 - read&write / 0 - tylko czytanie, 1 - tylko pisanie, 2 - czytanie i pisanie
		short offset;				//offset number is the number of the sign after which te cursor is / przesuniecie kursora, po tym znaku znajduje sie kursor

	public:
		void setAll(int memBlockNr) {
			this->fileSize = 0;
			time_t timeRightNow = time(nullptr);
			this->cTime = this->mTime = this->aTime = timeRightNow;

			this->direct1st = memBlockNr;
			this->direct2nd = this->indir1st = -1;

			this->refCount = 0; //file is closed / plik jest zamkniety
			this->status = -1; //does not really matter till the file is opened / nie ma znaczenia poki plik jest otwarty
			this->offset = 0; //sets the offset to the beginning of the file / ustawia kursor na poczatku pliku
		}
		void aTimeRefresh() {
			time_t timeRightNow = time(nullptr);
			this->aTime = timeRightNow;
		}
		void mTimeRefresh() {
			time_t timeRightNow = time(nullptr);
			this->mTime = timeRightNow;
			this->aTime = timeRightNow;
		}
		short setStatus(short newStatus) {
			this->aTimeRefresh();
			this->mTimeRefresh();
			return this->status = newStatus;
		}
		short setOffset(short newOffset) {
			this->aTimeRefresh();
			if (newOffset <= 10 * SO_BLOCK) {
				this->mTimeRefresh();
				return this->offset = newOffset;
			}
			else return -1;
		}
	}inodeTable[NO_INODES];

	char disk[NO_MEMBLOCKS][SO_BLOCK];

public:
	// CONSTRUCTOR&DESTRUCTOR / KONSTRUKTOR I DESTRUKTOR
	FileManagement();
	~FileManagement();

	// MAIN FUNCTIONS / GLOWNE FUNKCJE	

	/* Creates file / Tworzy plik */
	DESCRIPTOR createFile(std::string name);

		/* Openes file / Otwiera plik */
		short openFile(DESCRIPTOR, short); //DESCRIPTOR openFile(DESCRIPTOR descriptor, int openingMode);

			/* Writes whatToWrite at the desctiptor->curPos of the file / Pisze do pliku string na pozycji wskazanej przez kursor */
			int writeFile(DESCRIPTOR, std::string);

			/* Reads numberOfChars from the descriptor->curPos of the file / Czyta podana liczbe znakow z pliku od pozycji kursora */
			int readFile(DESCRIPTOR, std::string&, int);

			int truncateFile(std::string filePath, int howManyToCut);
			int truncateFile(DESCRIPTOR descriptor, int howManyToCut);

		bool closeFile(DESCRIPTOR descriptor);

		int lseekFile(DESCRIPTOR, int displacement, int finalPosition = 0);
		bool renameFile(std::string presentName, std::string finalName);

		int findFile(std::string searchedName);
		void listFiles();

	bool deleteFile(int inode);
	bool deleteFile(std::string name);

	// OTHERS / INNE
	short setOffset(DESCRIPTOR inode, short newOffset); //RETURN: new offset if OK; -1 when ERROR / ZWRACA: nowy offset gdy pomyslne; -1 w przypadku bledu
	short getOffset(DESCRIPTOR inode);


	// DEBUG
	std::string printTime(time_t);
	void showSuperBlock();		//shows 01s inside superBlock / pokazuje 0 i 1 w superbloku
	void showMemoryBlocks();	//shows all memory blocks / pokazuje wszystkie bloki pamieci
	void showTakenInodes();		//shows all info inside taken inodes / pokazuje info zawarte w zajetych i-wezlach
	void showRoot();			//shows root catalog / pokazuje katalog glowny
};

#endif