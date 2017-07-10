#include "FileManagement.h"

void main() {
	system("mode 650"); //forces the command line window to be fullscreened for better visibility / otwiera okno terminala w pelnym oknie dla lepszej widocznosci

	FileManagement fm; //main constructor / glowny konstruktor
	system("cls"); //cleans whole command line window (init message) / czysci okno (wiadomosc o inicjalizacji)

	int menu = 0; //user menu choice / wybor uzytkownika (menu)
	std::string param1, param2; //helpers / pomocnicze
	std::string continueMessage = "\nPress ENTER to continue. / Nacisnij ENTER aby kontynuowac.\n";

	/* Debug mode switcher / Przelaczanie trybu debuggera */
	std::cout << "Debug mode? / Wlaczyc debugowanie?\n"
	<< "0 - OFF / WYLACZ | 1 - ON / WLACZ\n";
	std::cin >> menu;
	
	if(menu >= 1) fm.debug = 1;
	else if (menu == 0) fm.debug = 0;

	menu = 1; //prevents from not looping into main menu when no-debug mode / zapobiega ominieciu petli menu glownego dla trybu bez debugowania

	/* Main menu / Menu glowne */
	while (menu != 0) {
		std::cout << "\nFile management module in Penguin: / Modul obslugi plikow w Pingwinie:\n" <<
			"0 - EXIT / OPUSC TO MENU\n" <<
			"1 - 1st TEST PROGRAM / PROGRAM TESTOWY 1\n" <<
			"2 - 2nd TEST PROGRAM / PROGRAM TESTOWY 2\n" <<
			"3 - 3rd TEST PROGRAM / PROGRAM TESTOWY 3\n" <<
			"-----------------------------\n" <<
			"4 - create file / stworz plik\n" <<
			"5 - open file / otworz plik\n" <<
			"6 - write to file / pisz do pliku\n" <<
			"7 - read file / czytaj plik\n" <<
			"8 - truncate file / obetnij plik od konca\n" <<
			"9 - close file / zamknij plik\n" <<
			"10 - delete file / usun plik\n" <<
			"11 - move cursor / przesun pozycje kursora w pliku\n" <<
			"12 - change filename / zmien nazwe pliku\n" <<
			"13 - check if file exists / sprawdz czy plik o podanej nazwie istneje\n" <<
			"14 - list files / wylistuj pliki\n" <<
			"-----------------------------\n" <<
			"15 - SHOW SUPERBLOCK / POKAZ SUPERBLOK\n" <<
			"16 - SHOW MEMORY / POKAZ PAMIEC\n" <<
			"17 - SHOW OCCUPIED I-NODES / POKAZ ZAJETE I-WEZLY\n" <<
			"18 - SHOW MAIN CATALOG / POKAZ KATALOG GLOWNY\n";
		std::cin >> menu;
		
		switch (menu) {
		case 0:
			//exit / wyjscie
			std::cout << "-----------------------------------------------------------------\n"
				<< "    _\n"
				<< "  ('v')\t\tAuthor: / Autor:\n"
				<< " //-=-\\\\\tMateusz Grabuszynski\n"
				<< " (\\_=_/)\tmateusz.grabuszynski@student.put.poznan.pl\n"
				<< "  ^^ ^^\n"
				<< "-----------------------------------------------------------------\n";
			break;

		case 1:
			//1st program: creates 2 files, opens it, writes at the end, sets new offset, reads and closes second one, lists files, changes name and lists again
			//1. program: tworzy 2 pliki, otwiera je, dopisuje, ustala nowe przesuniecie kursora, odczytuje i zamyka drugi, listuje pliki, zmienia nazwe i listuje ponownie

			fm.createFile("tes11"); fm.showRoot(); //create tes11 file / stworz plik tes11
			std::cout << continueMessage; std::cin.ignore(2);

			fm.createFile("tes12"); fm.showRoot(); //create tes12 file / stworz plik tes12
			std::cout << continueMessage; std::cin.ignore(2);

			fm.openFile(fm.findFile("tes12"), 2); fm.showTakenInodes(); //open tes12 file in rw mode / otworz plik tes12 w trybie czytania i pisania
			std::cout << continueMessage; std::cin.ignore(2);

			fm.writeFile(fm.findFile("tes12"), "Hello world :)"); fm.showMemoryBlocks(); fm.showTakenInodes(); //write to tes12 / pisz w tes12
			std::cout << continueMessage; std::cin.ignore(2);

			fm.lseekFile(fm.findFile("tes12"), -14); fm.showTakenInodes(); //change offset / zmien pozycje kursora
			std::cout << continueMessage; std::cin.ignore(2);

			fm.readFile(fm.findFile("tes12"), param1, 14); std::cout << "Loaded: / Wczytano: " << param1 <<"\n"; fm.showTakenInodes(); //read file / czytaj plik
			std::cout << continueMessage; std::cin.ignore(2);

			fm.closeFile(fm.findFile("tes12")); fm.showTakenInodes(); //close tes12 / zamknij plik tes12
			std::cout << continueMessage; std::cin.ignore(2);

			fm.listFiles(); //list files / listuj pliki
			std::cout << continueMessage; std::cin.ignore(2);

			fm.renameFile("tes12", "abcd"); //rename tes12 to abcd / zmien nazwe tes12 na abcd
			std::cout << continueMessage; std::cin.ignore(2);

			fm.listFiles(); //list files / listuj pliki
			break;

		case 2:
			//2nd program: creates file, opens it, writes, moves cursor, reads, truncates, moves cursor, reads and closes
			//2. program: tworzy plik, otwiera, pisze, (przesuwa,) czyta go, ucina i (przesuwa,) czyta znowu, zamyka
			
			fm.createFile("test2"); fm.showRoot(); //create test2 file / stworz plik test2
			std::cout << continueMessage; std::cin.ignore(2);

			fm.openFile(fm.findFile("test2"), 2); fm.showTakenInodes(); //open test2 file in rw mode / otworz plik test2 w trybie czytania i pisania
			std::cout << continueMessage; std::cin.ignore(2);

			fm.writeFile(fm.findFile("test2"), "This is some kind of random text to fill some more space than in 1st test program"); fm.showMemoryBlocks(); fm.showTakenInodes(); //write to test2 / pisz w test2
			std::cout << continueMessage; std::cin.ignore(2);

			fm.lseekFile(fm.findFile("test2"), -100); fm.showTakenInodes(); //change offset / zmien pozycje kursora
			std::cout << continueMessage; std::cin.ignore(2);

			fm.readFile(fm.findFile("test2"), param1, 100); std::cout << "Loaded: / Wczytano: " << param1 << "\n"; fm.showTakenInodes(); //read file / czytaj plik
			std::cout << continueMessage; std::cin.ignore(2);

			fm.truncateFile("test2", 8); fm.showMemoryBlocks(); fm.showTakenInodes(); //truncate file at the end, so it has 8 signs / przytnij plik do wielkosci 8 znakow
			std::cout << continueMessage; std::cin.ignore(2);

			fm.lseekFile(fm.findFile("test2"), -7); fm.showTakenInodes(); //change offset / zmien pozycje kursora
			std::cout << continueMessage; std::cin.ignore(2);

			fm.readFile(fm.findFile("test2"), param1, 7); std::cout << "Loaded: / Wczytano: " << param1 << "\n"; fm.showTakenInodes(); //read file / czytaj plik
			std::cout << continueMessage; std::cin.ignore(2);

			fm.closeFile(fm.findFile("test2")); fm.showTakenInodes(); //close test2 / zamknij plik test2
			break;

		case 3:
			//3rd program: creates file, opens it, writes, closes, lists files, seeks for it, deletes, lists files, seeks for it again, seeks for other file
			//3. program: tworzy plik, otwiera, pisze, zamyka; listuje pliki, szuka pliku, kasuje, listuje, szuka go ponownie, szuka innego pliku

			fm.createFile("test3"); fm.showRoot(); //create test3 file / stworz plik test3
			std::cout << continueMessage; std::cin.ignore(2);

			fm.openFile(fm.findFile("test3"), 2); fm.showTakenInodes(); //open test3 file in rw mode / otworz plik test3 w trybie czytania i pisania
			std::cout << continueMessage; std::cin.ignore(2);

			fm.writeFile(fm.findFile("test3"), "SampleSample"); fm.showMemoryBlocks(); fm.showTakenInodes(); //write to test3 / pisz w test3
			std::cout << continueMessage; std::cin.ignore(2);

			fm.closeFile(fm.findFile("test3")); fm.showTakenInodes(); //close test3 / zamknij plik test3
			std::cout << continueMessage; std::cin.ignore(2);

			fm.listFiles(); //list files / listuj pliki
			std::cout << continueMessage; std::cin.ignore(2);

			fm.findFile("test3"); //seek for test3 (successful) / szukaj pliku test3 (powodzenie)
			std::cout << continueMessage; std::cin.ignore(2);

			fm.deleteFile("test3"); fm.showRoot(); fm.showMemoryBlocks(); //delete test3 / usun plik test3
			std::cout << continueMessage; std::cin.ignore(2);

			fm.findFile("test3"); //seek for test3 (fails) / szukaj pliku test3 (niepowodzenie)
			std::cout << continueMessage; std::cin.ignore(2);

			fm.findFile("skjk"); //seek for skjk file
			break;

		case 4:
			//create file / stworz plik
			std::cout << "Name new file: / Podaj nazwe pliku do stworzenia: ";
			std::cin >> param1;
			fm.createFile(param1);
			break;

		case 5:
			//open file / otworz plik
			std::cout << "Name the file you want to open: / Podaj nazwe pliku do otwarcia: ";
			std::cin >> param1;
			std::cout << "\nWhich mode? (0 - read, 1 - write, 2 - both)" <<
				"\nW jakim trybie otworzyc plik? (0 - czytanie, 1 - pisanie, 2 - czytanie i pisanie) ";
			std::cin >> menu;
			if(menu == 0 || menu == 1 || menu == 2) fm.openFile(fm.findFile(param1), menu);
			else std::cout << "\nIncorrect mode number! / Podano niepoprawny numer trybu!";
			menu = 5;
			break;

		case 6:
			//write to file / pisz do pliku
			std::cout << "Name the file you want to write in: / Podaj nazwe pliku, do ktorego mam pisac: ";
			std::cin >> param1;
			std::cout << "\nAnd data to write: / I dane do zapisania: ";
			std::cin >> param2;
			std::cout << fm.writeFile(fm.findFile(param1), param2) << "\n";
			break;

		case 7:
			//read file / czytaj plik
			std::cout << "Name the file you want to read from: / Podaj nazwe pliku, z ktorego mam czytac: ";
			std::cin >> param1;
			std::cout << "\nHow many signs? / Podaj ilosc znakow do odczytania: ";
			std::cin >> menu;
			fm.readFile(fm.findFile(param1), param2, menu);
			std::cout << param2;
			menu = 7; //prevents from not looping back into main menu / zapobiega pominieciu petli glownego menu
			break;

		case 8:
			//truncate file / obetnij plik od konca
			std::cout << "Name the file you want to truncate: / Podaj nazwe pliku do przyciecia: ";
			std::cin >> param1;
			std::cout << "\nHow many signs do you want to leave untouched? / Ile znakow w pliku chcesz zachowac? ";
			std::cin >> menu;
			fm.truncateFile(param1, menu);
			menu = 8; //prevents from not looping back into main menu / zapobiega pominieciu petli glownego menu
			break;

		case 9:
			//close file / zamknij plik
			std::cout << "Name the file you want to close: / Podaj nazwe pliku do zamkniecia: ";
			std::cin >> param1;
			fm.closeFile(fm.findFile(param1));
			break;

		case 10:
			//delete file / usun plik
			std::cout << "Name the file you want to delete: / Podaj nazwe pliku, ktory chcesz usunac: ";
			std::cin >> param1;
			fm.deleteFile(param1);
			break;

		case 11:
			//move cursor / przesun pozycje kursora
			std::cout << "Name the file in which you want to move cursor: / Podaj nazwe pliku, w ktorym chcesz przesunac offset: ";
			std::cin >> param1;
			std::cout << "\nOffset? (negative - left, positive - right)" <<
				"\nPodaj przesuniecie (ujemne w lewo, dodatnie w prawo): ";
			std::cin >> menu;
			fm.lseekFile(fm.findFile(param1), menu);
			menu = 10; //prevents from not looping back into main menu / zapobiega pominieciu petli glownego menu
			break;

		case 12:
			//change filename / zmien nazwe pliku
			std::cout << "Which file do you want to rename? / Nazwe ktorego pliku chcesz zmienic? ";
			std::cin >> param1;
			std::cout << "\nTo...? / Na...? ";
			std::cin >> param2;
			fm.renameFile(param1, param2);
			break;

		case 13:
			//find file / znajdz plik
			std::cout << "Name the file you want to find / Podaj nazwe pliku, ktory chcesz odnalezc ";
			std::cin >> param1;
			std::cout << "Returned: / Zwrocono: " << fm.findFile(param1);
			break;

		case 14:
			//list files / wylistuj pliki
			fm.listFiles();
			break;

		case 15:
			//show superblock / pokaz superblok
			fm.showSuperBlock();
			break;

		case 16:
			//show memory blocks / pokaz bloki pamieci
			fm.showMemoryBlocks();
			break;

		case 17:
			//show occupied i-nodes / pokaz zajete i-wezly
			fm.showTakenInodes();
			break;

		case 18:
			//show root catalog / pokaz katalog glowny
			fm.showRoot();
			break;

		default:
			std::cout << "Incorrect number! / Podano niepoprawny numer!";
			break;
		}
		std::cout << continueMessage;
		std::cin.ignore(2);
		system("cls");
	}
}