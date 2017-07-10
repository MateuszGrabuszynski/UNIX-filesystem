#include "FileManagement.h"

using namespace std;

int FileManagement::countBlocksFromSize(int size) {
	if (size % SO_BLOCK != 0) {
		return size / SO_BLOCK + 1;
	}
	else {
		return size / SO_BLOCK;
	}
}


// CONSTRUCTOR&DESTRUCTOR / KONSTRUKTOR I DESKTRUKTOR
FileManagement::FileManagement() {
	cout << "Inicialization. Filling superblock with zeros. / Inicjacja. Wypelniam superblok zerami.\n";
	supB.inodes.reset();	//resets all inodes / resetuje wszystkie i-wezly
	supB.memBlocks.reset();	//resets all memory blocks / resetuje wszystkie bloki pamieci
}

FileManagement::~FileManagement() {
}

// MAIN FUNCTIONS / GLOWNE FUNKCJE
DESCRIPTOR FileManagement::createFile(string name) {
	cout << "-- Creating new " << name << " file / Tworze nowy plik o nazwie " << name << ".\n";
	
	//CHECKERS / SPRAWDZACZE
	if(debug == true) cout << "Checking if file has to be pipe / Sprawdzam, czy plik ma byc potokiem.\n";

	if (name != "") {
		if (debug == true) cout << "Normal file. Checking if there's no name conflict. / Plik normalny. Sprawdzam, czy istnieje juz plik o tej nazwie.\n";

		if (findFile(name) != -1) {
			cerr << "File " << name << " already exists! / Plik o tej nazwie juz istnieje!\n";
			return -1;
		}
		else {
			cout << "Ok. I can create file " << name << ". / Dobrze, moge utworzyc taki plik.\n";
		}
		if (debug == true) cout << "Checking if filename is shorter than 7 chars. / Sprawdzam, czy nazwa pliku jest krotsza niz 7 znakow.\n";
		if (name.length() > 6 || name.length() == 0) {	//it may be deleted cause filenames are not inside memory blocks / moze byc usuniety gdyz nazwy plikow nie sa w blokach pamieci
			cerr << "Filename is too long! / Nazwa pliku jest pusta lub zbyt dluga!\n";
			return -1;
		}
	}
	else {
		cout << "Creating unnamed pipe. / Tworze potok nienazwany.\n";
	}


	if (debug == true) cout << "There's no file with that name. Looking for empty i-nodes. / Nie ma jeszcze pliku o tej nazwie. Sprawdzam, czy sa jeszcze wolne i-wezly.\n";
	//checks if there's a free inode available, if no then returns -1 / sprawdza czy sa wolne i-wezly, jesli nie to zwraca -1
	int inodeNr = supB.assignInode(); //inodeNr is being assigned for created file / numer i-wezla jest przypisywany do stworzonego pliku
	if (inodeNr == -1) {
		cerr << "I can't assign new i-node! / Nie moge przypisac nowego i-wezla!\n";
		return -1;
	}

	if (debug == true) cout << "I assigned i-node to file (in superblock). Looking for empty memory blocks. / Przypisalem i-wezel do pliku (w superbloku). Sprawdzam, czy sa jeszcze wolne bloki w pamieci.\n";
	//checks if there's a free memory block available, if no - returns -1 / sprawdza czy sa wolne bloki pamieci, jesli nie to zwraca -1
	int memBlockNr = supB.assignMemBlock(); //memory block is being assigned for created file / do pliku jest przypisywany blok pamieci
	if (memBlockNr == -1) {
		cerr << "Memory full! / Pamiec zapelniona!\n";
		return -1;
	}

	if (debug == true) cout << "One memory block assigned successfully. Changing values inside i-node. / Udalo sie przypisac jeden blok pamieci do pliku. Ustawiam wartosci w i-wezle.\n";
	inodeTable[inodeNr].setAll(memBlockNr);

	if (name != "") {
		if (debug == true) cout << "Creating directory entry with name and i-node number. / Tworze wpis katalogowy z nazwa i numerem i-wezla.\n";
		newDir(inodeNr, name);
	}

	cout << "-- File " << name << " created. / Stworzono plik " << name << ".\n";
	
	return inodeNr;
}

short FileManagement::openFile(DESCRIPTOR inode, short mode) {
	cout << "-- Opening file in mode " << mode << ". / Otwieram plik w trybie " << mode << ".\n";
	if (debug == true) cout << "Checking if given i-node is empty (in superblock). / Sprawdzam, czy podany i-wezel jest zajety (w superbloku).\n";
	if (inode <= -1) {
		cout << "File does not exist! / Podany plik nie istnieje!";
		return -1;
	}
	if (supB.inodes.test(inode) != 0) {
		if (debug == true) cout << "File with this i-node exists. Increasing file reference counter, changing access and opening times. / Plik o tym numerze i-wezla istnieje. Zwiekszam licznik referencji do pliku, zmieniam czas dostepu i tryb otwarcia.\n";
		inodeTable[inode].refCount++;
		inodeTable[inode].aTimeRefresh();
		inodeTable[inode].setStatus(mode);
		return true;
	}
	else {
		cerr << "File does not exist! / Taki plik nie istnieje!\n";
		return false;
	}
}

int FileManagement::writeFile(DESCRIPTOR inode, string buffer) {
	cout << "-- Adding " << buffer.size() << " chars to file. / Dopisuje " << buffer.size() << " znakow do pliku.\n";
	int noSignsToAdd = buffer.size();
	int noSignsLeft = noSignsToAdd;
	int noSignsAdded = 0;

	/* CHECKERS / SPRAWDZACZE */
	if (noSignsToAdd == 0) {
		cerr << "You cannot add 0 chars! / Nie mozesz zapisac 0 znakow do pliku!\n";
		return -1;
	}
	if (debug == true) cout << "Checking if there is file with given i-node. / Sprawdzam, czy istnieje plik o tym numerze i-wezla.\n";
	if (inode < 0 || supB.inodes[inode] != 1) {
		cerr << "Link to file was not found! / Nie znalazlem dowiazania do pliku!\n";
		return -1;
	}
	if (debug == true) cout << "Link to file was found! Checking if file is opened in suitable mode. / Znalazlem dowiazanie do pliku! Sprawdzam, czy plik jest otwarty w odpowiednim trybie.\n";
	if (inodeTable[inode].refCount < 1) {
		cerr << "File is not opened! / Plik nie jest otwarty!\n";
		return -1;
	}
	if (inodeTable[inode].status == 0) {
		cerr << "File was opened in read-only mode! / Plik otwarto w trybie tylko do odczytu!\n";
		return -1;
	}
	if (debug == true) cout << "File is opened in suitable mode. Checking if file size will not exceed maximum. / Plik jest otwarty w odpowiednim trybie! Sprawdzam, czy dlugosc pliku nie przekroczy maksymalnej.\n";
	if ((inodeTable[inode].fileSize >(2 + SO_BLOCK)*SO_BLOCK) || (inodeTable[inode].fileSize + buffer.size() >(2 + SO_BLOCK)*SO_BLOCK)) {
		cout << "File is too big to add given chars! / Plik jest zbyt duzy by dopisac do niego tyle znakow!\n";
		return -1;
	}

	int offset = inodeTable[inode].offset;
	int fileSize = inodeTable[inode].fileSize;

	if (debug == true) cout << "Refreshing access time. / Uaktualniam date i czas modyfikacji.\n";
	inodeTable[inode].mTimeRefresh();

	/* matching offset / dopasowywanie offsetu */
	if (debug == true) cout << "Offset before: / Offset przed: " << offset << "\n";

	if (offset <= 0) {
		cout << "I will write at the begining of the file. / Zgodnie z podanym offsetem, bede dopisywac do poczatku pliku.\n";
		offset = 0;
	}
	else if (offset > fileSize) {
		cout << "I will write at the end of the file. / Zgodnie z podanym offsetem, bede dopisywac na koncu pliku.\n";
		offset = fileSize;
	}
	else {
		cout << "Writing from " << offset << " (of " << fileSize << "). / Dopisuje od pozycji " << offset << " z " << fileSize << ".\n";
	}
	
	if (offset < SO_BLOCK) { //offset[0..15]
		if (debug == true) cout << "Writing at position given by direct i-node (1st). / Dopisuje na pozycji wskazanej w i-wezle bezposrednio (1.).\n";
		do {
			disk[inodeTable[inode].direct1st][offset] = buffer[noSignsAdded];
			noSignsAdded++;
			noSignsLeft--;
			offset++;
		} while (offset != SO_BLOCK && noSignsLeft != 0);
		//offset is 16 / offset to 16
	}
	if (offset >= SO_BLOCK && offset < 2 * SO_BLOCK && noSignsLeft != 0) { //offset[16..31]
		if (inodeTable[inode].direct2nd == -1) {
			if (debug == true) cout << "I have to seek for free i-node (direct2nd). / Musze szukac wolnego bloku (direct2nd).\n";
			int temp = supB.assignMemBlock();
			if (temp != -1)
				inodeTable[inode].direct2nd = temp;
			else {
				cerr << "There is no more empty memory blocks! / Brakuje wolnych blokow w pamieci!\n";
				return offset;
			}
		}

		if (debug == true) cout << "Writing at position given by direct i-node (2nd). / Dopisuje na pozycji wskazanej w i-wezle bezposrednio (2.).\n";
		do {
			disk[inodeTable[inode].direct2nd][offset - SO_BLOCK] = buffer[noSignsAdded];
			noSignsAdded++;
			noSignsLeft--;
			offset++;
		} while (offset%SO_BLOCK != 0 && noSignsLeft != 0);
		//offset is 32 / offset to 32
	}
	if (offset >= 2 * SO_BLOCK && offset < 3 * SO_BLOCK && noSignsLeft != 0) { //offset[32..47]
		if (inodeTable[inode].indir1st == -1) {
			if (debug == true) cout << "I have to seek for free i-node (indir1st). / Musze szukac wolnego bloku (indir1st).\n";
			int temp = supB.assignMemBlock();
			if (debug == true) cout << "Assigned " << temp << "block as indir1st! / Przypisalem blok " << temp << " jako indir1st!\n";
			int temp2 = supB.assignMemBlock();
			if (temp != -1 && temp2 != -1) {
				inodeTable[inode].indir1st = temp;
				disk[inodeTable[inode].indir1st][0] = temp2;
			}
			else {
				cerr << "There is no more empty memory blocks! / Brakuje wolnych blokow w pamieci!\n";
				return offset;
			}
		}

		if (debug == true) cout << "Writing at position given by indirect i-node (0). / Dopisuje na pozycji wskazanej w posrednio (0).\n";
		do {
			disk[int(disk[inodeTable[inode].indir1st][0])][offset - 2 * SO_BLOCK] = buffer[noSignsAdded];
			noSignsAdded++;
			noSignsLeft--;
			offset++;
		} while (offset%SO_BLOCK != 0 && noSignsLeft != 0);
		//offset is 48 / offset to 48
	}
	/* indirHelper */
	if (debug == true) cout << "indirH: " << inodeTable[inode].fileSize << " + " << noSignsAdded << " / " << SO_BLOCK << " | floored: " << floor((inodeTable[inode].fileSize + noSignsAdded) / SO_BLOCK) << " | floored and -2ed: " << floor((inodeTable[inode].fileSize + noSignsAdded) / SO_BLOCK) - 2 << "\n";
	int indirHelper = floor((inodeTable[inode].offset + noSignsAdded) / SO_BLOCK) - 2;
	if (debug == true) cout << "indirHelper=" << indirHelper << endl;
	if (debug == true) cout << "inodeTable[inode].indir1st=" << inodeTable[inode].indir1st << endl;
	while (offset >= 3 * SO_BLOCK && offset < 18 * SO_BLOCK && noSignsLeft != 0) { //offset[48..287]
		if (disk[inodeTable[inode].indir1st][indirHelper] == 0) {
			if (debug == true) cout << "I have to seek for free i-node. / Musze szukac wolnego bloku (...).\n";
			int temp = supB.assignMemBlock();
			if (temp != -1) {
				disk[inodeTable[inode].indir1st][indirHelper] = temp;
			}
			else {
				cerr << "There is no more empty memory blocks! / Brakuje wolnych blokow w pamieci!\n";
				return offset;
			}
		}

		if (debug == true) cout << "Writing at position given by indirect i-node (" << indirHelper << "). / Dopisuje na pozycji wskazanej w posrednio (" << indirHelper << ").\n";
		do {
			disk[int(disk[inodeTable[inode].indir1st][indirHelper])][offset - (2 + indirHelper) * SO_BLOCK] = buffer[noSignsAdded];
			noSignsAdded++;
			noSignsLeft--;
			offset++;
		} while (offset%SO_BLOCK != 0 && noSignsLeft != 0);
		indirHelper++;
		//offset is 288 if the end of file is reached / offset to 288 gdy koniec pliku zostal osiagniety
	}
	cout << "-- Written successfully. / Zapis zakonczono powodzeniem.\n";
	if (offset > inodeTable[inode].fileSize) inodeTable[inode].fileSize = offset;
	inodeTable[inode].offset = offset;
	return offset;
}

int FileManagement::readFile(DESCRIPTOR inode, string &buffer, int noChars) {
	cout << "-- Trying to read " << noChars << " chars from file. / Probuje odczytac " << noChars << " znakow z pliku.\n";
	int noSignsLeft = noChars;
	int noSignsRead = 0;
	int offset = inodeTable[inode].offset; //holds current offset position / trzyma obecna pozycje kursora
	int indirHelper;
	int fileSize = inodeTable[inode].fileSize;

	if (debug == true) cout << "Checking if more than 0 chars is read. / Sprawdzam, czy odczytywane jest wiecej niz 0 znakow.\n";
	if (noChars == 0) {
		cerr << "You cannot read 0 chars from file! / Nie mozesz wczytac 0 znakow z pliku!\n";
		return -1;
	}

	if (debug == true) cout << "Checking if theres a file with that inode number. / Sprawdzam, czy istnieje plik o tym numerze i-wezla.\n";
	if (inode < 0 || supB.inodes[inode] != 1) {
		cerr << "I can't find link to file! / Nie znalazlem dowiazania do pliku!\n";
		return -1;
	}

	if (debug == true) cout << "Found link to file! Checking if the file is opened in suitable mode. / Znalazlem dowiazanie do pliku! Sprawdzam, czy plik jest otwarty w odpowiednim trybie.\n";
	if (inodeTable[inode].refCount < 1) {
		cerr << "File is not opened! / Plik nie jest otwarty!\n";
		return -1;
	}
	if (inodeTable[inode].status == 1) {
		cerr << "File is opened in write-only mode! / Plik otwarto w trybie tylko do zapisu!\n";
		return -1;
	}

	if (debug == true) cout << "File is opened in suitable mode! Moving cursor, writing zero to buffer. / Plik jest otwarty w odpowiednim trybie! Ustawiam kursor i zeruje bufor.\n";
	buffer.erase();

	/*matching offset / dopasowywanie offsetu */
	if (debug == true) cout << "Offset before: / Offset przed: " << offset << "\n";

	if (offset <= 0) {
		cout << "As given, reading from the beginning of the file. / Zgodnie z podanym offsetem, bede czytac od poczatku pliku.\n";
		offset = 0;
	}
	else if (offset >= inodeTable[inode].fileSize) {
		cout << "Can't read from the end of the file. / Nie mozna czytac od konca pliku.\n";
		return -1;
	}
	else {
		cout << "Reading from " << offset << " of " << inodeTable[inode].fileSize << ". / Czytam od pozycji " << offset << " z " << inodeTable[inode].fileSize << ".\n";
	}
	/*end of matching offset / koniec dopasowywania offsetu */

	if (offset < SO_BLOCK - 1 && offset < fileSize) { //offset[0..15]
		if (debug == true) cout << "Reading from position pointed by 1st direct pointer / Czytam na pozycji wskazanej w i-wezle bezposrednio (1.).\n";
		do {
			buffer.push_back(disk[inodeTable[inode].direct1st][offset]);
			noSignsRead++;
			noSignsLeft--;
			offset++;
		} while (offset != SO_BLOCK && noSignsLeft != 0 && offset < fileSize);
		//offset is 16 / offset to 16
	}
	if (offset >= SO_BLOCK && offset < 2 * SO_BLOCK - 1 && noSignsLeft != 0 && offset <= fileSize) { //offset[16..31]
		if (inodeTable[inode].direct2nd != -1) {
			if (debug == true) cout << "Reading from position pointed by 2nd direct pointer / Czytam na pozycji wskazanej w i-wezle bezposrednio (2.).\n";
			do {
				buffer.push_back(disk[inodeTable[inode].direct2nd][offset - SO_BLOCK]);
				noSignsRead++;
				noSignsLeft--;
				offset++;
			} while (offset%SO_BLOCK != 0 && noSignsLeft != 0 && offset < fileSize);
			//offset is 32 / offset to 32
		}
		else {
			cerr << "File has ended, can't read further. / Nie moge czytac dalej, gdyz plik sie juz skonczyl.\n";
		}
	}
	if (offset >= 2 * SO_BLOCK && offset < 3 * SO_BLOCK - 1 && noSignsLeft != 0 && offset < fileSize) { //offset[32..47]
		if (inodeTable[inode].indir1st != -1) {
			if (debug == true) cout << "Reading from position pointed by indirect pointer / Czytam na pozycji wskazanej posrednio (0).\n";
			do {
				buffer.push_back(disk[int(disk[inodeTable[inode].indir1st][0])][offset - 2 * SO_BLOCK]);
				noSignsRead++;
				noSignsLeft--;
				offset++;
			} while (offset%SO_BLOCK != 0 && noSignsLeft != 0 && offset < fileSize);
			//offset is 48 / offset to 48
		}
		else {
			cerr << "File has ended, can't read further. / Nie moge czytac dalej, gdyz plik sie juz skonczyl. (0)\n";
		}
	}

	indirHelper = floor(offset / SO_BLOCK) - 2;
	while (offset >= 3 * SO_BLOCK && offset < 18 * SO_BLOCK - 1 && noSignsLeft != 0 && offset < fileSize) { //offset[48..287]
		if (disk[inodeTable[inode].indir1st][indirHelper] != 0) {
			if (debug == true) cout << "Reading from position pointed by indirect pointer (" << indirHelper << "). / Czytam na pozycji wskazanej posrednio (" << indirHelper << ").\n";
			do {
				buffer.push_back(disk[int(disk[inodeTable[inode].indir1st][indirHelper])][offset - (2 + indirHelper) * SO_BLOCK]);
				noSignsRead++;
				noSignsLeft--;
				offset++;
				if (debug == true) cout << offset << "/" << fileSize << "\n";
			} while (offset%SO_BLOCK != 0 && noSignsLeft != 0 && offset < fileSize);
			indirHelper++;
			//offset is 288 if the end of file is reached
		}
		else {
			if (debug == true) cerr << "File has ended, can't read further (" << indirHelper << "). / Nie moge czytac dalej, gdyz plik sie juz skonczyl (" << indirHelper << ").\n";
			break;
		}
	}

	cout << "-- Read " << noSignsRead << " signs. / Wczytano " << noSignsRead << " znakow.\n";
	inodeTable[inode].offset = offset;
	return offset;
}

int FileManagement::truncateFile(string fileName, int finalFileSize) {
	if (debug == true) cout << "Checking if file exists. / Sprawdzam czy istnieje plik o podanej nazwie.\n";
	int inode = findFile(fileName);
	if (inode != -1) {
		return truncateFile(inode, finalFileSize);
	}
	return -1;
}

int FileManagement::truncateFile(DESCRIPTOR inode, int finalFileSize) {
	cout << "-- Truncating file to size " << finalFileSize << ". / Przycinam plik do rozmiaru " << finalFileSize << ".\n";
	int fileSize = inodeTable[inode].fileSize;
	int offset = fileSize - 1;
	int noSignsRmvd = 0;
	int noSignsLeft = fileSize - finalFileSize;
	int indirHelper = ceil(float(fileSize) / float(SO_BLOCK)) - 3;
	if (debug == true) cout << "indirHelper @ begininining / indirHelper na poczatku: " << indirHelper << "\n";

	if (inode < 0 || supB.inodes.test(inode) == 0) {
		cerr << "There's no file linked to that i-node. / Z tym i-wezlem nie powiazano zadnego pliku.\n";
		return -1;
	}
	if (debug == true) cout << "Checking filesize. / Sprawdzam rozmiar pliku.\n";
	if (inodeTable[inode].fileSize == 0) {
		cerr << "Can't truncate empty file! / Nie mozesz przyciac pustego pliku!\n";
		return -1;
	}
	if (finalFileSize < 0) {
		cerr << "Unknown number of " << finalFileSize << " was given. / Podano niepoprawna liczbe " << finalFileSize << " znakow!\n";
		return -1;
	}

	//int offset = inodeTable[inode].offset; //current cursor position / obecna pozycja kursora

	if (finalFileSize >= fileSize) {
		cerr << "Can't truncate because finalFileSize >= offset! / Nie moge przyciac do tego miejsca gdyz finalFileSize >= offset!\n";
		return -1;
	}

	if (debug == true) cout << "Everything is OK. Changing modification time and truncating. / Wszystko w porzadku. Odswiezam czas modyfikacji i przechodze do ucinania.\n";
	inodeTable[inode].mTimeRefresh();

	///////////////////////////////////////////////////

	while (fileSize <= 18 * SO_BLOCK && fileSize > 3 * SO_BLOCK && fileSize != finalFileSize) { //fileSize[288..49] //offset[287..48]
		if (disk[inodeTable[inode].indir1st][indirHelper] != 0 && indirHelper > 0) {
			if (debug == true) cout << "Deleting in block pointed by indirect pointer (" << indirHelper << "). / Usuwam w bloku na pozycji wskazanej posrednio (" << indirHelper << ").\n";
			do {
				disk[int(disk[inodeTable[inode].indir1st][indirHelper])][fileSize - 1 - (2 + indirHelper) * SO_BLOCK] = 0;
				noSignsRmvd++;
				noSignsLeft--;
				fileSize--;
				if (debug == true) cout << "noSignsRmvd after/po: " << noSignsRmvd << " | " << fileSize - 1 - (2 + indirHelper) * SO_BLOCK << "\n";
			} while (fileSize - 1 - (2 + indirHelper) * SO_BLOCK != -1 && noSignsLeft != 0);

			if (fileSize % SO_BLOCK == 0) {
				if (debug == true) cout << "supB.memBlocks.reset(" << int(disk[inodeTable[inode].indir1st][indirHelper]) << ")\n";
				supB.memBlocks.reset(int(disk[inodeTable[inode].indir1st][indirHelper]));
				if (debug == true) cout << "disk[inodeTable[inode].indir1st][" << indirHelper << "] = 0\n";
				disk[inodeTable[inode].indir1st][indirHelper] = 0;
			}

			indirHelper--;
		}
		else {
			if (debug == true) cerr << "Going deeper. / Przechodze glebiej. (" << indirHelper << ")\n";
			break;
		}
	}

	//fileSize is 49 if removed all signs / rozmiar pliku jest rowny 49 po usunieciu wszystkich znakow

	if (fileSize <= 3 * SO_BLOCK && fileSize > 2 * SO_BLOCK && fileSize != finalFileSize) { //fileSize[48..33] //offset[47..32]
		if (disk[inodeTable[inode].indir1st][0] != 0) {
			if (debug == true) cout << "Deleting in block pointed by indirect pointer (0). / Usuwam w bloku na pozycji wskazanej posrednio (0).\n";
			do {
				disk[int(disk[inodeTable[inode].indir1st][0])][fileSize - 1 - 2 * SO_BLOCK] = 0;
				noSignsRmvd++;
				noSignsLeft--;
				fileSize--;
				if (debug == true) cout << "noSignsRmvd after/po: " << noSignsRmvd << " | " << fileSize - 1 - 2 * SO_BLOCK << "\n";
			} while (fileSize - 2 * SO_BLOCK != 0 && noSignsLeft != 0);

			if (fileSize - 2 * SO_BLOCK == 0) {
				supB.memBlocks.reset(int(disk[inodeTable[inode].indir1st][0]));
				disk[inodeTable[inode].indir1st][0] = 0;

				if (debug == true) cout << "Pointing indir1st to -1 and reseting it's position in supB. / Ustawiam indir1st na -1 i resetuje jego pozycje w supB\n";
				supB.memBlocks.reset(inodeTable[inode].indir1st);
				inodeTable[inode].indir1st = -1;
			}
		}
		else {
			if (debug == true) cout << "Going deeper. / Przechodze glebiej. (0)\n";
		}
	}

	//fileSize is 32 if removed all signs / rozmiar pliku jest rowny 32 po usunieciu wszystkich znakow

	noSignsRmvd = 0;
	if (fileSize <= 2 * SO_BLOCK && fileSize > SO_BLOCK && fileSize != finalFileSize) { //fileSize[32..17] //offset[31..16]
		if (inodeTable[inode].direct2nd != -1) {
			if (debug == true) cout << "Deleting in block pointed by direct pointer. / Usuwam w bloku na pozycji wskazanej bezposrednio. (direct2nd)\n";
			do {
				disk[inodeTable[inode].direct2nd][fileSize - 1 - SO_BLOCK] = 0;
				noSignsRmvd++;
				noSignsLeft--;
				fileSize--;
				if (debug == true) cout << "noSignsRmvd after/po: " << noSignsRmvd << "\n";
			} while (fileSize - SO_BLOCK != 0 && noSignsLeft != 0 && fileSize >= finalFileSize);

			if (fileSize - SO_BLOCK == 0) {
				supB.memBlocks.reset(inodeTable[inode].direct2nd);
				inodeTable[inode].direct2nd = -1;
			}
		}
		else {
			if (debug == true) cout << "Not interfering with next memory block. / Nie ingeruje w nastepny blok pamieci. (direct2nd)\n";
		}
	}

	//fileSize is 16 if removed all signs / rozmiar pliku jest rowny 16 po usunieciu wszystkich znakow

	if (fileSize >= 0 && fileSize <= SO_BLOCK && fileSize != finalFileSize) { //fileSize[16..1 (0)] //offset[15..0]
		if (inodeTable[inode].direct1st != -1) {
			if (debug == true) cout << "Deleting in block pointed by direct pointer. / Usuwam w bloku na pozycji wskazanej bezposrednio. (direct1st)\n";
			do {
				disk[inodeTable[inode].direct1st][fileSize - 1] = 0;
				noSignsRmvd++;
				noSignsLeft--;
				fileSize--;
				if (debug == true) cout << "noSignsRmvd after/po: " << noSignsRmvd << "\n";
			} while (fileSize != 0 && noSignsLeft != 0 && fileSize >= finalFileSize);
		}
		else {
			if (debug == true) cout << "Not interfering with next memory block. / Nie ingeruje w nastepny blok pamieci. (direct1st)\n";
		}
	}

	//fileSize is 0 if removed all signs / rozmiar pliku jest rowny 0 po usunieciu wszystkich znakow

	cout << "-- Deleted " << inodeTable[inode].fileSize - fileSize << " signs. / Usunieto " << inodeTable[inode].fileSize - fileSize << " znakow.\n";

	return inodeTable[inode].offset = inodeTable[inode].fileSize = fileSize;
}

bool FileManagement::closeFile(DESCRIPTOR desc) {
	cout << "-- Trying to close file. / Probuje zamknac plik.\n";
	if (inodeTable[desc].refCount < 1) {
		cerr << "You can't close file that is not opened! / Nie mozesz zamknac nieotwartego pliku!\n";
		return false;
	}
	else {
		cout << "-- Closing file. / Zamykam plik.\n";
		inodeTable[desc].refCount--;
		inodeTable[desc].setStatus(-1);
		return true;
	}
}

int FileManagement::lseekFile(DESCRIPTOR inode, int displacement, int finalPosition) {
	cout << "-- Moving cursor to " << displacement << ". / Przesuwam pozycje kursora w pliku o " << displacement << ".\n";
	if (inode < 0 || supB.inodes.test(inode) == 0) {
		cerr << "There's no file linked to that i-node! / Z tym i-wezlem nie jest powiazany zaden plik!";
		return -1;
	}
	else {
		if (debug == true) cout << "Changing access time. / Odswiezam czas dostepu.\n";
		inodeTable[inode].aTimeRefresh();

		if (debug == true) cout << "Checking finalPosition parameter. / Sprawdzam parametr finalPosition.\n";
		if (finalPosition != 0) {
			if (finalPosition = -1) {
				if (debug == true) cout << "-- Setting cursor position to the beginning of the file. / Ustawiam offset na poczatek pliku.\n";
				return inodeTable[inode].offset = 0;
			}
			else if (finalPosition = 1) {
				if (debug == true) cout << "-- Setting cursor position to the end of the file. / Ustawiam offset na koniec pliku.\n";
				return inodeTable[inode].offset = inodeTable[inode].fileSize;
			}
		}
		else {
			cout << "-- Setting cursor position to " << inodeTable[inode].offset + displacement << " of " << inodeTable[inode].fileSize << ". / Ustawiam offset na pozycje " << inodeTable[inode].offset + displacement << " z " << inodeTable[inode].fileSize << ".\n";
			return inodeTable[inode].offset += displacement;
		}
	}
}

bool FileManagement::renameFile(string presentName, string finalName) {
	cout << "-- Trying to change file name from " << presentName << " to " << finalName << ". / Probuje zmienic nazwe pliku " << presentName << " na " << finalName << ".\n";
	for (list<dirEnt>::iterator outIter = root.begin(); outIter != root.end(); outIter++) {
		if (outIter->name == presentName) {
			if (debug == true) cout << "Found! Changing name and modification time. / Znalazlem! Zmieniam nazwe oraz odswiezam czas modyfikacji.\n";
			inodeTable[outIter->inodeNumber].mTimeRefresh();
			outIter->name = finalName;
			cout << "-- Finished successfully! / Zakonczono powodzeniem!\n";
			return true;
		}
	}
	cerr << "There's no such file! / Nie znalazlem pliku o podanej nazwie!\n";
	return false;
}

int FileManagement::findFile(string searchedName) {
	cout << "-- Searching for " << searchedName << " file in root catalog. / Szukam pliku " << searchedName << " w katalogu glownym (root).\n";
	for (list<dirEnt>::iterator outIter = root.begin(); outIter != root.end(); outIter++) {
		if (outIter->name == searchedName) {
			cout << "-- File found! / Znalazlem taki plik!\n";
			return outIter->inodeNumber;
		}
	}
	cout << "There's no such file! / Nie znalazlem takiego pliku!\n";
	return -1;
}

void FileManagement::listFiles() {
	cout << "-- Listing files. / Listuje pliki.\n";
	for (list<dirEnt>::iterator outIter = root.begin(); outIter != root.end(); outIter++) {
		cout << outIter->name << " | @: " << outIter->inodeNumber << "\n";
	}
}

bool FileManagement::deleteFile(string fileName) {
	cout << "-- Deleting " << fileName << "'s directory entry. / Usuwam powiazany z plikiem " << fileName << " wpis katalogowy.\n";
	int inode = findFile(fileName);
	if (inode != -1) {
		cout << "-- Deleted successfully. / Usuwanie powiodlo sie.\n";
		delDir(fileName);
		if (deleteFile(inode)) return true;
		else return false;
	}
	else {
		cerr << "Deleting failed! / Usuwanie nie powiodlo sie!\n";
		return false;
	}
}

bool FileManagement::deleteFile(int inode) {
	cout << "-- Deleting file. / Usuwam plik.\n";
	if (inode >= 0) {
		truncateFile(inode, 0);
		if (debug == true) cout << "Deleting links pointing to file. / Usuwam powiazane z plikiem dowiazanie do pierwszego bloku oraz dowiazanie do iwezla w superbloku.\n";
		supB.memBlocks.reset(inodeTable[inode].direct1st);
		supB.inodes.reset(inode);
		return true;
	}
	else { cerr << "Can't delete file! / Nie moge usunac pliku!\n"; }
}

//OTHERS / INNE
short FileManagement::setOffset(DESCRIPTOR inode, short newOffset) {
	if (supB.inodes.test(inode) == 1) {
		return inodeTable[inode].setOffset(newOffset);
	}
	else {
		return -1;
	}
}

short FileManagement::getOffset(DESCRIPTOR inode)
{
	if (supB.inodes.test(inode) == 1) {
		return inodeTable[inode].offset;
	}
	else {
		return -1;
	}
}

//DEBUG
string FileManagement::printTime(time_t time_tbp) {
	struct tm* timeinfo;
	timeinfo = localtime(&time_tbp);
	return asctime(timeinfo);
}

void FileManagement::showSuperBlock() {
	cout << "\n------superblock / superblok------\n";

	cout << "inodes / i-wezly: ";
	for (int i = 0; i < NO_INODES; i++) {
		cout << supB.inodes.test(i);
	}

	cout << "\nmemory blocks / bloki pamieci: ";
	for (int i = 0; i < NO_MEMBLOCKS; i++) {
		cout << supB.memBlocks.test(i);
	}

	cout << "\n---END OF superblock / KONIEC superbloku---\n";
}

void FileManagement::showMemoryBlocks() {
	cout << "\n------memory blocks / bloki pamieci------\n";
	//formatting: (<number>) <sign> / formatowanie: (<liczbowo>) <znak> 
	for (int i = 0; i < SO_BLOCK; i++) {
		cout << "\t" << i;
	}
	cout << "\n";

	for (int x = 0; x < NO_MEMBLOCKS; x++) {
		cout << x << "\t";
		for (int y = 0; y < SO_BLOCK; y++) {
			cout << "(" << int(disk[x][y]) << ")" << disk[x][y] << "\t";
		}
		cout << "\n";
	}

	cout << "\n---END OF memory blocks / KONIEC blokow pamieci---\n";
}

void FileManagement::showTakenInodes() {
	cout << "\n------occupied inodes / zajete i-wezly------\n";

	if (supB.inodes.none()) {
		cout << "[every inode is empty / wszystkie i-wezly sa puste]\n";
	}
	else {
		for (int i = 0; i < NO_INODES; i++) {
			if (supB.inodes.test(i)) {
				cout << "inode " << i << " metadata: / i-wezel o numerze " << i << " zawiera nastepujace dane:\n" <<
					"fileSize: " << inodeTable[i].fileSize << " | dirs: " << inodeTable[i].direct1st << ", " << inodeTable[i].direct2nd << " | indir(s): " << inodeTable[i].indir1st << " ( ";
				for (int j = 0; j < SO_BLOCK; j++) {
					cout << int(disk[inodeTable[i].indir1st][j]) << " ";
				}
				cout << ")\n" <<
					"cTime: RAW[" << inodeTable[i].cTime << "] | CONV: " << printTime(inodeTable[i].cTime) <<
					"aTime: RAW[" << inodeTable[i].aTime << "] | CONV: " << printTime(inodeTable[i].aTime) <<
					"mTime: RAW[" << inodeTable[i].mTime << "] | CONV: " << printTime(inodeTable[i].mTime) <<
					"refCounter: " << inodeTable[i].refCount << endl <<
					"status: " << inodeTable[i].status << "(0 - read / 1 - write / 2 - read&write)" << endl <<
					"offset: " << inodeTable[i].offset << "\n";
			}
		}
	}

	cout << "\n---END OF occupied inodes / KONIEC zajete i-wezly---\n";
}

void FileManagement::showRoot() {
	cout << "\n------root catalog / katalog glowny------\n" <<
		"/Names:\tInodes nums./\n" <<
		"/Nazwy:\tNum. i-w:/\n";

	for (list<dirEnt>::iterator outIter = root.begin(); outIter != root.end(); outIter++) {
		cout << outIter->name << '\t' << outIter->inodeNumber << "\n";
	}

	cout << "\n---END OF root catalog / KONIEC katalogu glownego---\n";
}