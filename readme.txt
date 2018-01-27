Steganografia.

Program zapisuje tekst podany przez u�ytkownika w pliku wav metod� LSB (najmniej znacz�cy bit).
Aby odszyfrowa� tekst ukryty w pliku wav nale�y uruchomi� program z parametrem decode.


Jak u�y�:
	Plik tekstowy, wav i exe musz� znajdowa� si� w jednym folderze.
	Po zakodowaniu utrzowy si� plik wav z liter� 'e' na pocz�tku.
	Po odkowaniu utrzowy si� plik MESSAGE.txt.

Zakodowanie z pliku txt:
	stenowav.exe -e -t plik.wav plik.txt

Zakodowanie tekstu podanego w linii komend:
	stenowav.exe -e -s plik.wav "Tutaj wpisa� tekst"

Odkodowanie:
	stenowav.exe -d eplik.wav


Parametry:
	stenowav.exe - program szyfruj�cy
	-e - encode
	-d - decode
	-t - plik .txt
	-s - tekst wpisany w linii komend
	plik.wav - plik wav podany przez u�ytkownika
	plik.txt - plik txt podany przez u�ytkownika
