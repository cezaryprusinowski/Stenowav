#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <sstream>
#include <vector>
#include <climits>

using namespace std;

struct wavHEADER
{
public:
	char riff[4];
	uint32_t chunkSize;
	char wave[4];
	char fmt[4];
	uint32_t subchunk1Size;
	uint16_t audioFormat;
	uint16_t numChannels;

	uint32_t sampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;

	char SubChunk2ID[4];
	uint32_t subchunk2Size;
	char* data;
};

void readDataWav(string filename, wavHEADER *wavFile); //wczytanie danych z pliku (lokalizacja pliku, zmienna w której zostaną zapisane dane)
void printInfoHeaderWav(wavHEADER *wavFile); //wypisanie nagłówku pliku .wav (zmienna przechowująca dane dźwiękowe)
void makeOutputWav(string filename, wavHEADER *wavFile); //stworzenie pliku wav (lokalizacja pliku wychodzącego, zmienna przechowująca dane dźwiękowe)		//można zmienić app na to żeby nadpisywało plik
void compareWavAndTxtLenght(string text, wavHEADER wavFile); //porównuje długość (tekstu zapisanego w zmiennej,zmienna przechowująca dane dźwiękowe)
void txtToString(string *txtInString); //zamiana danych z pliku txt na string (zmienna w której ma zostać zapisany tekst z pliku)
void printAvailableOptions(); //wypisuje opcje które są dostępne
void makeFlag(wavHEADER *wavFile); //tworzy flagę na początku zapisywanego tekstu i na końcu, aby później wiedzieć jak to zdekodować (dźwięk do zakodowania)
void Encode(wavHEADER *wavFile); //zakodowuje tekst w pliku
void Decode(wavHEADER wavFile); //odkodowuje zakodowany tekst
void getNumberOfLettersToDecode(wavHEADER wavFile); //sprawdza ile znaków zostało zakodowanych pomiędzy flagą początkową i końcową

string INPUTFILENAME;  //lokalizacja pliku dźwiękowego wchodzacego
string OUTPUTFILENAME;  //lokalizacja pliku dźwiękowego wychodzącego
string TXTTOENCODE; //  //plik .txt, który ma zostać zakodowany
string TXTAFTERDECODE = "MESSAGE.txt"; //plik .txt, który ma zostać zakodowany
const string STRINGTOENCODE = "text"; //string, który ma zostać zakodowany
const int NUMBERSAVAILABLEOPTIONS = 4; //liczba dostępnych opcji
const int FLAGTODECODE = 24; //przy zakodowaniu musze ustawić flagę, żeby wiedziec jakim sposobem jest zakodowane
string TXTINSTRING; //string, w którym powinnien się znajdować cały tekst z pliku .txt
char selectedOption; //opcja zakodowania wybrana przez użytkownika
int placeInFile = 0; //zmienna do trzymania pozycji w pliku
int endFirsFlag = 0; //binarny koniec pierwszej flagi
int beginSecondFlag = 0; //binarny początek drugiej flagi
char numberOfFlag = '0';

bool availableOptions[NUMBERSAVAILABLEOPTIONS]; // tablica, do której zapisuje czy opcja jest dostępna czy nie w metodzie printAvailableOptions()

int main(int argc, char *argv[])
{

	//-e - encode
	//-t - from txt
	//-s - from string
	//-d - decode

	//stenowav.txt -e -t *.wav *.txt - zakodowanie, tekst z pliku .txt
	//stenowav.txt -e -s *.wav "Text here" - zakodowanie, tekst wpisany w lini komend
	//stenowav.exe -d *.wav - odkodowanie zakodowanego pliku


	//////////////////	//główny program
	wavHEADER wavFile;

	if (argv[1][0] == '-') {
		switch (argv[1][1])
		{
		case 'e':

			if (argv[2][0] == '-')
			{
				switch (argv[2][1])
				{
				case 't':
					INPUTFILENAME = argv[3];
					TXTTOENCODE = argv[4];
					OUTPUTFILENAME = "e" + INPUTFILENAME;
					readDataWav(INPUTFILENAME, &wavFile);
					cout << "" << endl;
					txtToString(&TXTINSTRING);
					compareWavAndTxtLenght(TXTINSTRING, wavFile);
					printAvailableOptions();
					cout << "" << endl;
					cin >> &selectedOption;
					Encode(&wavFile);
					makeOutputWav(OUTPUTFILENAME, &wavFile);
					break;
				case 's':
					INPUTFILENAME = argv[3];
					OUTPUTFILENAME = "e" + INPUTFILENAME;
					readDataWav(INPUTFILENAME, &wavFile);
					TXTINSTRING = argv[4];
					compareWavAndTxtLenght(TXTINSTRING, wavFile);
					printAvailableOptions();
					cout << "" << endl;
					cin >> &selectedOption;
					Encode(&wavFile);
					makeOutputWav(OUTPUTFILENAME, &wavFile);
					break;
				default:
					cout << "Blad drugiego parametru" << endl;
				}
			}
			else
			{
				cout << "Blad drugiego parametru" << endl;
			}
			break;
		case 'd':
			OUTPUTFILENAME = argv[2];
			readDataWav(OUTPUTFILENAME, &wavFile);
			getNumberOfLettersToDecode(wavFile);
			Decode(wavFile);
			break;
		default:
			cout << "Bledny pierwszy parametr" << endl;
			break;
		}
	}
	else
	{
		cout << "Bledny pierwszy parametr" << endl;
	}
	system("pause");
}


void readDataWav(string filename, wavHEADER *wavFile) {
	ifstream inFile(filename, ios::in | ios::binary);

	if (inFile.good()) {
		inFile.read(wavFile->riff, 4);
		inFile.read((char*)&wavFile->chunkSize, 4);
		inFile.read(wavFile->wave, 4);
		inFile.read(wavFile->fmt, 4);
		inFile.read((char*)&wavFile->subchunk1Size, 4);
		inFile.read((char*)&wavFile->audioFormat, 2);
		inFile.read((char*)&wavFile->numChannels, 2);
		inFile.read((char*)&wavFile->sampleRate, 4);
		inFile.read((char*)&wavFile->ByteRate, 4);
		inFile.read((char*)&wavFile->BlockAlign, 2);
		inFile.read((char*)&wavFile->BitsPerSample, 2);

		//wczytanie identyfikatora bloku "data"
		do {
			memset(wavFile->SubChunk2ID, 0, sizeof(char*)); //czyszczę zaalokowaną pamięć
			inFile.read(wavFile->SubChunk2ID, 4); //wczytuję danę do identyfikatora

												  //sprawdzam czy identyfikator = "data", jeśli nie, przesuwam pozycję w pliku o jedno miejsce w prawo do poprzedniego odczytu
			if (memcmp(wavFile->SubChunk2ID, "data", 4)) {
				inFile.seekg(-3, ios::cur);
			}
			else { //jeśli identyfikator bloku = "data" to pobieram resztę danych
				inFile.read((char*)&wavFile->subchunk2Size, 4);
				wavFile->data = new char[wavFile->subchunk2Size];               //data
				inFile.read(wavFile->data, wavFile->subchunk2Size);            //data
			}
		} while (memcmp(wavFile->SubChunk2ID, "data", 4));
	}
	else {
		cout << "Blad przy odtwarzaniu pliku" << endl;
		system("pause");
		exit(0);
	}
	inFile.close();
	cout << "Dane z pliku wczytano prawidlowo" << endl;
}

void printInfoHeaderWav(wavHEADER *wavFile) {
	cout << "RIFF: " << wavFile->riff[0] << wavFile->riff[1] << wavFile->riff[2] << wavFile->riff[3] << endl;
	cout << "chunkSize: " << wavFile->chunkSize << endl;
	cout << "wave: " << wavFile->wave[0] << wavFile->wave[1] << wavFile->wave[2] << wavFile->wave[3] << endl;
	cout << "fmt: " << wavFile->fmt[0] << wavFile->fmt[1] << wavFile->fmt[2] << wavFile->fmt[3] << endl;
	cout << "subchunk1Size: " << wavFile->subchunk1Size << endl;
	cout << "audioFormat: " << wavFile->audioFormat << endl;
	cout << "numChannels: " << wavFile->numChannels << endl;
	cout << "sampleRate: " << wavFile->sampleRate << endl;
	cout << "ByteRate: " << wavFile->ByteRate << endl;
	cout << "BlockAlign: " << wavFile->BlockAlign << endl;
	cout << "BitsPerSample: " << wavFile->BitsPerSample << endl;
	cout << "data subchunk2: " << wavFile->SubChunk2ID[0] << wavFile->SubChunk2ID[1] << wavFile->SubChunk2ID[2] << wavFile->SubChunk2ID[3] << endl;
	cout << "subchunk2Size: " << wavFile->subchunk2Size << endl;
	//for (size_t i = 0; i < wavFile->subchunk2Size; i++) cout << wavFile->data[i];	// Wypisanie wszystkich sampli
}

void makeOutputWav(string filename, wavHEADER *wavFile) {
	ofstream outFile(filename, ios::binary);
	if (outFile.good())
	{
		outFile.write(wavFile->riff, 4);
		outFile.write((char*)&wavFile->chunkSize, 4);
		outFile.write(wavFile->wave, 4);
		outFile.write(wavFile->fmt, 4);
		outFile.write((char*)&wavFile->subchunk1Size, 4);
		outFile.write((char*)&wavFile->audioFormat, 2);
		outFile.write((char*)&wavFile->numChannels, 2);
		outFile.write((char*)&wavFile->sampleRate, 4);
		outFile.write((char*)&wavFile->ByteRate, 4);
		outFile.write((char*)&wavFile->BlockAlign, 2);
		outFile.write((char*)&wavFile->BitsPerSample, 2);
		outFile.write(wavFile->SubChunk2ID, 4);
		outFile.write((char*)&wavFile->subchunk2Size, 4);
		outFile.write(wavFile->data, wavFile->subchunk2Size);
	}
	outFile.close();
	cout << "Plik .wav zostal zapisany poprawnie" << endl;
}

void compareWavAndTxtLenght(string text, wavHEADER wavFile) {
	int textLenght = text.length() * 8;
	int spaceFromBegin = 0;  //ta opcja ma być podawana przez użytkownika

	if (textLenght <= (wavFile.subchunk2Size) - FLAGTODECODE) //przy zmianie wszystkich ostatnich bitów
	{
		availableOptions[0] = true;
	}
	if (textLenght <= ((wavFile.subchunk2Size) / 2) - FLAGTODECODE) //przy zmianie co drugiego bitu
	{
		availableOptions[1] = true;
	}
	if (textLenght < ((wavFile.subchunk2Size) * 2) - FLAGTODECODE) //przy zmianie dwóch bitów we wszystkich samplach
	{
		availableOptions[2] = true;
	}
	if (textLenght < ((wavFile.subchunk2Size) - spaceFromBegin) - FLAGTODECODE) // przy odstępie od początku
	{
		availableOptions[3] = true;
	}
}

void txtToString(string *txtInString) {
	ifstream txtFile(TXTTOENCODE, ios::in | ios::binary);
	stringstream strStream;

	if (txtFile.good()) {
		strStream << txtFile.rdbuf();
		*txtInString = strStream.str();
	}
	else
	{
		cout << "Blad przy otwieraniu pliku txt" << endl;
		txtFile.close();
		system("pause");
		exit(0);
	}
	txtFile.close();
}

void printAvailableOptions() {
	if (availableOptions[0] == 1) cout << "1. Zmien tylko najmniej znaczace bity." << endl;
	if (availableOptions[1] == 1) cout << "2. Zmien co drugi najmniej znaczacy bit." << endl;
	if (availableOptions[2] == 1) cout << "3. Zmien dwa najmniej znaczace bity." << endl;
	if (availableOptions[3] == 1) cout << "4. Zmien tylko najmniej znaczace bity przy odstepie od poczatku." << endl;
	if (availableOptions[0] == 0 && availableOptions[1] == 0 && availableOptions[2] == 0 && availableOptions[3] == 0) {
		cout << "Zadna opcja nie jest dostepna. Wybierz wiekszy plik .wav lub zmniejsz tekst." << endl;
		system("pause");
		exit(0);
	}
}

void makeFlag(wavHEADER *wavFile) {

	char flag[3] = { 'F','L',selectedOption };
	int i = placeInFile;

	while (i != (placeInFile + 24)) {
		for (size_t j = 0; j < 3; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				bitset<8> caBit(wavFile->data[i]);
				caBit[0] = bitset<8>(flag[j])[k];//flag[j];
				wavFile->data[i] = char(caBit.to_ulong());
				i++;
			}
		}
	}

	placeInFile = placeInFile + 24;
}

void Encode(wavHEADER *wavFile) {

	size_t k = 0;

	switch (selectedOption)
	{

	case '1':
		makeFlag(wavFile);
		k = placeInFile;

		for (size_t i = 0; i < TXTINSTRING.length(); i++)
		{
			for (int j = 0; j < 8; j++)
			{
				bitset<8> caBit(wavFile->data[k]);
				caBit[0] = bitset<8>(TXTINSTRING[i])[j];
				wavFile->data[k] = char(caBit.to_ulong());
				k++;
				if (k >= (TXTINSTRING.length() * 8) + placeInFile) {
					break;
				}
			}
			if (k >= (TXTINSTRING.length() * 8) + placeInFile) {
				break;
			}
		}
		placeInFile = k;
		makeFlag(wavFile);
		break;

	case '2':

		makeFlag(wavFile);
		k = placeInFile;

		for (size_t i = 0; i < TXTINSTRING.length(); i++)
		{
			for (int j = 0; j < 8; j++)
			{
				bitset<8> caBit(wavFile->data[k]);
				caBit[0] = bitset<8>(TXTINSTRING[i])[j];
				wavFile->data[k] = char(caBit.to_ulong());
				k = k + 2;
				if (k >= ((TXTINSTRING.length() * 8) + placeInFile) * 2) {
					break;
				}
			}
			if (k >= ((TXTINSTRING.length() * 8) + placeInFile) * 2) {
				break;
			}
		}
		placeInFile = k;
		makeFlag(wavFile);
		break;

	case '3':
		makeFlag(wavFile);
		k = placeInFile;

		for (size_t i = 0; i < TXTINSTRING.length(); i++)
		{
			for (int j = 0; j < 8; j++)
			{
				bitset<8> caBit(wavFile->data[k]);
				caBit[0] = bitset<8>(TXTINSTRING[i])[j];
				j++;
				caBit[1] = bitset<8>(TXTINSTRING[i])[j];
				wavFile->data[k] = char(caBit.to_ulong());
				k++;
				if (k >= (TXTINSTRING.length() * 8) + placeInFile) {
					break;
				}
			}
			if (k >= (TXTINSTRING.length() * 8) + placeInFile) {
				break;
			}
		}

		while ((k % 8) != 0)
		{
			k++;
		}

		placeInFile = k;

		makeFlag(wavFile);
		break;

	case '4':
		cout << "Podaj ilosc bajtow odstepu: " << endl;
		cin >> placeInFile;

		while ((placeInFile % 8) != 0)
		{
			placeInFile++;
		}

		makeFlag(wavFile);
		k = placeInFile;

		for (size_t i = 0; i < TXTINSTRING.length(); i++)
		{
			for (int j = 0; j < 8; j++)
			{
				bitset<8> caBit(wavFile->data[k]);
				caBit[0] = bitset<8>(TXTINSTRING[i])[j];
				wavFile->data[k] = char(caBit.to_ulong());
				k++;
				if (k >= (TXTINSTRING.length() * 8) + placeInFile) {
					break;
				}
			}
			if (k >= (TXTINSTRING.length() * 8) + placeInFile) {
				break;
			}
		}
		placeInFile = k;
		makeFlag(wavFile);
		break;
	default:
		cout << "Wybrana została zła opcja" << endl;
		break;
	}
	cout << "Zakodowanie zostalo wykonane" << endl;
}

void getNumberOfLettersToDecode(wavHEADER wavFile) { //  + number of flag
	string example1 = "010001100100110000110001"; //=FL1
	string example2 = "010001100100110000110010"; //=FL2
	string example3 = "010001100100110000110011"; //=FL3
	string example4 = "010001100100110000110100"; //=FL4

	bitset<8> caBit[3];
	endFirsFlag = 0;
	beginSecondFlag = 0;
	bool close = false;

	while ((close != true) && (endFirsFlag < wavFile.subchunk2Size))
	{
		string flagInString = "";
		for (size_t i = 0; i < 3; i++) //wczytanie 3 znaków z pliku .wav
		{
			for (size_t j = 0; j < 8; j++)
			{
				caBit[i][j] = bitset<8>(wavFile.data[endFirsFlag])[0];
				endFirsFlag++;
			}
		}
		for (size_t i = 0; i < 3; i++) //dodanie do jednego ciągu wszystkich znaków
		{
			flagInString = flagInString + caBit[i].to_string();
		}

		if ((flagInString == example1) || (flagInString == example2) || (flagInString == example3) || (flagInString == example4)) {
			close = true;
			numberOfFlag = char(caBit[2].to_ulong());
		}
		else {
			endFirsFlag = endFirsFlag - 16;
		}
	}

	close = false;
	beginSecondFlag = endFirsFlag;

	while ((close != true) && (beginSecondFlag < wavFile.subchunk2Size))
	{
		string flagInString = "";
		for (size_t i = 0; i < 3; i++) //wczytanie 3 znaków z pliku .wav
		{
			for (size_t j = 0; j < 8; j++)
			{
				caBit[i][j] = bitset<8>(wavFile.data[beginSecondFlag])[0];
				beginSecondFlag++;
			}
		}
		for (size_t i = 0; i < 3; i++) //dodanie do jednego ciągu wszystkich znaków
		{
			flagInString = flagInString + caBit[i].to_string();
		}

		if ((flagInString == example1) || (flagInString == example2) || (flagInString == example3) || (flagInString == example4)) {
			close = true;
			//return k;
		}
		else {
			beginSecondFlag = beginSecondFlag - 16;
		}
	}
	beginSecondFlag = beginSecondFlag - 24;
}

void Decode(wavHEADER wavFile) {

	int bitsFromFlagToFlag = beginSecondFlag - endFirsFlag;
	int lettersFromFlagToFlag = (beginSecondFlag - endFirsFlag) / 8;

	bitset<8> caBit[USHRT_MAX]; //maksymalna ilość znaków w txt 65535
	int i = endFirsFlag;

	ofstream outFile(TXTAFTERDECODE, ios::binary);

	switch (numberOfFlag)
	{
	case '1':

		for (size_t j = 0; j < lettersFromFlagToFlag; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				caBit[j][k] = bitset<8>(wavFile.data[i])[0];//flag[j];
				i++;
				if (i >= (beginSecondFlag)) { //zmienić na ilość bitów od flagi do flagi //zmienione
					break;
				}
			}
			if (i >= (beginSecondFlag)) { //zmienić na ilość bitów od flagi do flagi //zmienione
				break;
			}
		}

		for (size_t m = 0; m < lettersFromFlagToFlag; m++) // zmienić na ilość znaków ASCII od flagi do flagi //zmienione
		{
			outFile << char(caBit[m].to_ulong());
		}
		cout << endl;
		break;

	case '2':
		for (size_t j = 0; j < lettersFromFlagToFlag; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				caBit[j][k] = bitset<8>(wavFile.data[i])[0];//flag[j];
				i = i + 2;
				if (i >= (beginSecondFlag)) { //zmienić na ilość bitów od flagi do flagi //zmienione
					break;
				}
			}
			if (i >= (beginSecondFlag)) { //zmienić na ilość bitów od flagi do flagi //zmienione
				break;
			}
		}

		for (size_t m = 0; m < (lettersFromFlagToFlag / 2); m++) // zmienić na ilość znaków ASCII od flagi do flagi //zmienione
		{
			outFile << char(caBit[m].to_ulong());
		}
		cout << endl;
		break;

	case '3':
		for (size_t j = 0; j < (lettersFromFlagToFlag * 2); j++)
		{
			for (int k = 0; k < 8; k++)
			{
				caBit[j][k] = bitset<8>(wavFile.data[i])[0];
				k++;
				caBit[j][k] = bitset<8>(wavFile.data[i])[1];
				i++;
				if (i >= (beginSecondFlag)) { //zmienić na ilość bitów od flagi do flagi //zmienione
					break;
				}
			}
			if (i >= (beginSecondFlag)) { //zmienić na ilość bitów od flagi do flagi //zmienione
				break;
			}
		}

		for (size_t m = 0; m < (lettersFromFlagToFlag * 2); m++) // zmienić na ilość znaków ASCII od flagi do flagi //zmienione
		{
			outFile << char(caBit[m].to_ulong());
		}
		cout << endl;
		break;

	case '4':
		for (size_t j = 0; j < lettersFromFlagToFlag; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				caBit[j][k] = bitset<8>(wavFile.data[i])[0];//flag[j];
				i++;
				if (i >= (beginSecondFlag)) { //zmieniæ na iloœæ bitów od flagi do flagi //zmienione
					break;
				}
			}
			if (i >= (beginSecondFlag)) { //zmieniæ na iloœæ bitów od flagi do flagi //zmienione
				break;
			}
		}

		for (size_t m = 0; m < lettersFromFlagToFlag; m++) // zmieniæ na ilo znaków ASCII od flagi do flagi //zmienione
		{
			outFile << char(caBit[m].to_ulong());
		}
		cout << endl;
		break;
	default:
		cout << "Błąd przy fladze podczas dekodowania." << endl;
		break;
	}
	cout << "Odkodowanie powiodlo sie. Plik output.txt zostal utworzony." << endl;
}



//finish