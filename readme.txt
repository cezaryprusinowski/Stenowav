Steganografia.

Program zapisuje tekst podany przez uzytkownika w pliku wav metoda LSB (najmniej znaczacy bit).
Aby odszyfrowac tekst ukryty w pliku wav nalezy uruchomic program z parametrem decode.


Jak uzyc:
	Plik tekstowy, wav i exe musza znajdowac sie w jednym folderze.
	Po zakodowaniu utrzowy sie plik wav z litera 'e' na poczatku.
	Po odkowaniu utrzowy sie plik MESSAGE.txt.

	W cmd (wiersz polecen) przechodzimy do folderu w którym znajdują się pliki.

Zakodowanie z pliku txt:
	stenowav.exe -e -t plik.wav plik.txt

Zakodowanie tekstu podanego w linii komend:
	stenowav.exe -e -s plik.wav "Tutaj wpisac tekst"

Odkodowanie:
	stenowav.exe -d eplik.wav


Parametry:
	stenowav.exe - program szyfrujacy
	-e - encode
	-d - decode
	-t - plik .txt
	-s - tekst wpisany w linii komend
	plik.wav - plik wav podany przez uzytkownika
	plik.txt - plik txt podany przez uzytkownika
