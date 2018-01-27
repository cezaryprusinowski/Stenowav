Steganografia.

Program zapisuje tekst podany przez u¿ytkownika w pliku wav metod¹ LSB (najmniej znacz¹cy bit).
Aby odszyfrowaæ tekst ukryty w pliku wav nale¿y uruchomiæ program z parametrem decode.


Jak u¿yæ:
	Plik tekstowy, wav i exe musz¹ znajdowaæ siê w jednym folderze.
	Po zakodowaniu utrzowy siê plik wav z liter¹ 'e' na pocz¹tku.
	Po odkowaniu utrzowy siê plik MESSAGE.txt.

	W cmd (wiersz poleceń) przechodzimy do folderu w którym znajdują się pliki.

Zakodowanie z pliku txt:
	stenowav.exe -e -t plik.wav plik.txt

Zakodowanie tekstu podanego w linii komend:
	stenowav.exe -e -s plik.wav "Tutaj wpisaæ tekst"

Odkodowanie:
	stenowav.exe -d eplik.wav


Parametry:
	stenowav.exe - program szyfruj¹cy
	-e - encode
	-d - decode
	-t - plik .txt
	-s - tekst wpisany w linii komend
	plik.wav - plik wav podany przez u¿ytkownika
	plik.txt - plik txt podany przez u¿ytkownika
