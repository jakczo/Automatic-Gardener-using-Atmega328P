# <p align="center"> Automatic-Gardener-using-Atmega328P (originally made in December 2017, updated in September 2018) </p>
C/C++ language program for specified device (based on the Atmega328P microprocessor) that takes basic care of plants.
Below I place a copy of the final report (in Polish language) containing all the information about the Automatic Gardener project.


# <p align="center"> Automatyczny ogrodnik </p>

## Spis treści:

### 1. [Założenia i funkcjonalność projektu](#-1-temat-projektu-): 

### 2. [Użyte elementy i problematyka](#-2-specyfikacja-wewn%C4%99trzna-):

### 3. [Specyfikacja wewnętrzna](#-3-specyfikacja-zewn%C4%99trzna-programu-):
   - [Schemat blokowy algorytmu działania urządzenia](#opis-zasady-dzia%C5%82ania-algorytmu-levenshteina-krok-po-kroku) 
   - [Opis zmiennych w programie](#analiza-zadania-modyfikacja-algorytmu-uzasadnienie)
   - [Opis funkcji w programie](#za%C5%82o%C5%BCenia-projektowe)
   - [Szczegółowy opis wybranej funkcji](#funkcjonalno%C5%9B%C4%87-programu)

### 4. [Specyfikacja zewnętrzna](#-4-uruchamianie-i-testowanie-):
   - [Szczegółowy opis użytych elementów](#testowanie)
   - [Skrócona instrukcja obsługi urządzenia](#wyniki-czasowe)

### 5. [Montaż i wtórna analiza](#-5-analiza-dzia%C5%82ania-programu-)

### 6. [Kod źródłowy](#-6-wnioski-)

### 7. [Wnioski i testowanie](#-7-wnioski-)

## <p align="center"> 1. Założenia i funkcjonalność projektu </p>

Tematem projektu jest „automatyczny ogrodnik”, czyli urządzenie wielofunkcyjne, służące do sprawdzania wilgotności gleby i w przypadku, gdy jest ona niewystarczająco wilgotna, mające podlać roślinę. Poza tym do jego obowiązków należałoby:

- Sprawdzanie, czy pojemnik, z którego pompa pobiera wodę podczas nawadniania gleby, nie jest pusty;
- Badanie aktualnej temperatury panującej w otoczeniu rośliny;
- Wyświetlanie zbadanych wartości – temperatury, wilgotności gleby, poziomu wody w pojemniku oraz aktualnej daty i czasu – na wyświetlaczu;
- Sterowanie diodą powiadamiającą o braku wody w pojemniku;
- Obsługa wielofunkcyjnego przycisku.

Cały projekt opiera się na mikroprocesorze Atmega328P.

## <p align="center"> 2. Użyte elementy i problematyka </p>

Urządzenie, które miałem za zadanie stworzyć, miało badać wilgotność gleby, poziom wody w pojemniku, temperaturę oraz wyświetlać zbadane wartości na wyświetlaczu, razem z aktualną datą i czasem, a następnie jeśli odpowiednie warunki były spełnione, miało przepompować wodę z pojemnika do doniczki, toteż projekt zdecydowałem się wykonać przy użyciu:

-	Mikroprocesora Atmega328P do sterowania całym układem;
-	Czujnika wilgotności gleby FC 28 do badania wilgotności gleby;
-	Czujnika wody Water Sensor A-MCW do badania poziomu wody;
-	Modułu RTC DS3231 do odczytu daty i czasu;
-	Wyświetlacza LCD 2x16 QC1602B do wyświetlania informacji;
- Pompy wodnej RS 360SH do nawadniania gleby;
- Przekaźnika SRD-05VDC-SL-C sterowania zasilaniem pompy;

Pozostałe elementy użyte w projekcie, to:

- Kondensatory ceramiczne 22pF i 100nF;
- Rezystory 100Ω i 10kΩ;
- Koszyk na cztery baterie AA 1.5V;
- Dioda świecąca na czerwono;
- Dwa przyciski;
- Wtyk jack DC 2.1/5.5mm;
- Gniazdo DC 2.1/5.5mm z zaciskami;

Narzędzia, które użyłem wykonując projekt, to:

- Programator USBASP;
- Lutownica;
- Cyna;
- Kalafonia i topnik;
- Powerbank;

Jako, że czujniki w moim projekcie mają stały kontakt z wodą, będąc pod napięciem, postanowiłem włączać je i wyłączać z poziomu mikroprocesora, aby zapobiec efektowi elektrolizy (co prowadzi do utleniania się metalowych elementów, a w efekcie do rdzy). Tak więc zdecydowałem się sterować zasilaniem czujników przy użyciu wyjść cyfrowych, na które jest podawany wysoki stan tylko, gdy zachodzi potrzeba dokonania pomiaru. Sam czas działania czujników to ok. 10ms, gdyż ich szybsze wyłączenie powodowało, że mikroprocesor nie nadążał z pobraniem danych z czujnika, zanim ten się wyłączył. W ten prosty sposób udało mi się znacznie wydłużyć żywotność czujników.

Mając na uwadze mobilny charakter mojego projektu, zdecydowałem się na zamontowanie między pompą a przekaźnikiem gniazda DC 2.1/5.5mm z zaciskami, aby mieć możliwość na wybranie wersji zasilania – gdy jest taka możliwość, pompa może być zasilana zwykłym zasilaczem spełniającym wymagania działania pompy, a gdy nie ma dostępnego źródła, do którego można podpiąć zasilacz, to można skorzystać z zasilania bateryjnego – w tym celu baterie umieściłem w koszyku, którego wyjścia przylutowałem do wtyku jack DC 2.1/5.5mm, aby móc podłączyć to do układu.

## <p align="center"> 3. Specyfikacja wewnętrzna </p>

#### Schemat blokowy algorytmu działania urządzenia:

![Alt text](images/1.png?raw=true "Schemat blokowy algorytmu działania urządzenia. Źródło - wykonanie własne.")

#### Opis zmiennych globalnych w programie:

- Wyświetlacz:
  * rs, en, d4, d5, d6, d7 – zmienne const typu int służące do obsługi wyświetlacza, przechowujące przypisane im numery wyjść mikroprocesora (kolejno 12, 11, 5, 4, 3, 2).
  * lcd – obiekt klasy LiquidCrystal (z biblioteki LiquidCrystal.h) służący do inicjalizacji wyświetlacza i jego wyjść.

- Moduł RTC:
  * clock – obiekt klasy DS3231 (z biblioteki DS3231.h) służący do inicjalizacji modułu RTC DS3231.
  * dt – obiekt klasy RTCDateTime (z biblioteki DS3231.h) służący do przechowywania sczytanych z modułu RTC DS3231 wartości. 

- Czujnik wilgotności:
  * atmegaPinoutMoistureSensorAnalogPin – zmienna const typu int służąca do obsługi czujnika wilgotności, przechowująca przypisany jej numer wyjścia analogowego Atmegi328P, z którego będą brane odczyty (A0).
  * moistureSensorValue – zmienna typu int służąca do przechowywania wartości sczytanej z czujnika wilgotności.
  * moistureSensorValuePercentage – zmienna typu float służąca do przechowywania wartości sczytanej z czujnika wilgotności w procentach.
  * atmegaPinoutMoistureSensorVcc – zmienna const typu int służąca do obsługi czujnika wilgotności, przechowująca przypisany jej numer wyjścia cyfrowego Atmegi328P (8), służącego do zasilania czujnika (włączania i wyłączania go).

- Czujnik wody:
  * atmegaPinoutWaterSensorAnalogPin – zmienna const typu int służąca do obsługi czujnika wody, przehowująca przypisany jej numer wyjścia analogowego Atmegi328P, z którego będą brane odczyty (A1).
  * waterSensorValue – zmienna typu int służąca do przechowywania wartości sczytanej z czujnika wody.
  * atmegaPinoutWaterSensorVcc – zmienna const typu int służąca do obsługi czujnika wody, przechowująca przypisany jej numer wyjścia cyfrowego Atmegi328P (7), służącego do zasilania czujnika (włączania i wyłączania go).

- Przekaźnik:
  * atmegaPinoutRelayIn – zmienna const typu int służąca do obsługi przekaźnika sterującego zasilaniem pompy wodnej, przechowująca przypisany jej numer wyjścia cyfrowego Atmegi328P (6), służącego do sterowania przekaźnikiem przy użyciu jego wejścia IN.
  
- Dioda:
* atmegaPinoutDiodeAnode – zmienna const typu int służąca do obsługi diody, przechowująca przypisany jej numer wyjścia cyfrowego Atmegi328P (9), służącego do włączania i wyłączania diody.

- Przycisk:
  * atmegaPinoutButton – zmienna const typu int służąca do obsługi przycisku, przechowująca przypisany jej numer wyjścia cyfrowego Atmegi328P (13), służącego w tym przypadku jako wejście oczekujące na pojawienie się stanu wysokiego (przepływ prądu - zwarty obwód - naciśnięty przycisk).
  * buttonUseTime – zmienna typu long double służąca do przechowywania czasu trwania naciśnięcia przycisku.

- Inne:
  * lastTest – zmienna typu int przechowująca w sekundach czas (mierzony od uruchomienia urządzenia), kiedy zostały ostatnio uruchomione czujniki.
  * presentTime – zmienna typu int przechowująca w sekundach aktualny czas (mierzony od uruchomienia urządzenia).
  * sensorsInterval – zmienna typu int przechowująca czas, po którym urządzenie ma wykonać kolejny pomiar z czytników i ewentualnie podjąć daną akcję.
  * diodeCounter – zmienna typu int przechowująca czas, przez który będzie migać dioda powiadomień oraz czas między wykonaniem kolejnych pomiarów z czujnika wody, podczas braku wody w pojemniku.
  * thresholdNo1 – zmienna const typu int przechowująca wymagany czas, przez który musi być naciśnięty przycisk, w celu wywołania pierwszej opcji (zmiennej jest przypisana wartość 100, co oznacza 100ms).
  * thresholdNo2 – zmienna const typu int przechowująca wymagany czas, przez który musi być naciśnięty przycisk, w celu wywołania drugiej opcji (zmiennej jest przypisana wartość 2000, co oznacza 2000ms).
  * lastWatering – obiekt klasy RTCDateTime (z biblioteki DS3231.h) służący do przechowywania daty i czasu ostatniego nawodnienia gleby.
  * lastRefilling – obiekt klasy RTCDateTime (z biblioteki DS3231.h) służący do przechowywania daty i czasu ostatniego uzupełnienia wodą pojemnika.

#### Opis funkcji w programie:

- viewDefault() – funkcja ekranu głównego. Wyświetla aktualną datę i czas, a także jeśli są spełnione dane warunki, to wywołuje funkcje dokonujące pomiarów, a także uaktualnia dane wyświetlane na ekranie.
- considerData() – funkcja decyzyjna, która na podstawie wcześniejszych pomiarów, sprawdza, czy nie zaszła potrzeba uzupełnienia wodą pojemnika, bądź nawodnienia gleby.
- diodeOn() – funkcja włączająca stopniowo diodę.
- diodeOff() – funkcja wyłączająca stopniowo diodę.
- buttonFunction() – funkcja sprawdzająca, czy nie został naciśnięty przycisk i jeśli tak było, to na podstawie długości trwania naciśnięcia, podejmuje decyzję, która funkcja programu powinna zostać wywołana.
- viewNo1() – funkcja, która jest wywoływana, gdy przycisk jest naciśnięty przez 100ms – 1999ms. Funkcja ta co sekundę, przez 5 sekund dokonuje pomiaru wilgotności gleby oraz poziomu wody w pojemniku i otrzymane wartości wyświetla na wyświetlaczu. Po upłynięciu czasu, następuje powrót do ekranu głównego.
- viewNo2() – funkcja, która jest wywoływana, gdy przycisk jest naciśnięty dłużej, niż 2000ms. Funkcja ta przez 5 sekund wyświetla datę i czas ostatniego nawodnienia gleby, a przez kolejne 5 sekund datę i czas ostatniego uzupełnienia wodą pojemnika. Po upłynięciu czasu, następuje powrót do ekranu głównego.
- moistureSensorFunction() – funkcja odpowiedzialna za włączenie, zapisanie otrzymanej wartości oraz wyłączenie czujnika wilgotności.
- waterSensorFunction() – funkcja odpowiedzialna za włączenie, zapisanie otrzymanej wartości oraz wyłączenie czujnika wody.
- motorRelayFunction() – funkcja odpowiedzialna za włączanie i wyłączanie przekaźnika sterującego zasilaniem pompy.

#### Szczegółowy opis wybranej funkcji – ekran główny:

```C++

void vievDefault() {
    lcd.clear();                              //wywołanie metody czyszczącej wyświetlacz
    presentTime = millis()/1000;              //pobranie aktualnego czasu (w sekundach) mierzonego od uruchomienia urządzenia

    if (presentTime - lastTest>= sensorInterval) {    //różnica między obecnym, a poprzednim czasem jest co najmniej godzinna
        moistureSensorFunction();                     //sprawdź poziom wilgotności gleby 
        waterSensorFunction();                        //sprawdź poziom wody
        considerData();                               //rozpatrz otrzymane dane   
        lcd.clear();                                  //wyczyść ekran
    }
    dt = clock.getDateTime();                         //zapisz w dt aktualną datę i czas
    
    lcd.setCursor(1, 0);                              //ustaw wskaźnik kursora na pierwszym miejscu zerowego wiersza
    lcd.print(dt.day);    lcd.print(".");             //wypisz na wyświetlaczu kolejno dzień
    lcd.print(dt.month);  lcd.print(".");             //wypisz na wyświetlaczu kolejno miesiąc
    lcd.print(dt.year);   lcd.print("");              //wypisz na wyświetlaczu kolejno rok

    lcd.setCursor(12, 0);                             //ustaw wskaźnik kursora na dwunastym miejscu zerowego wiersza
    lcd.print((int)moistureSensorValuePrecentage);    //wypisz na wyświetlaczu (zrzutowaną na typ int) wartość wilgotności gleby
    lcd.print("%");                                   //dopisz znak “%” za wartością
    
    lcd.setCursor(0, 1);                              //ustaw wskaźnik kursora na zerowym miejscu pierwszego wiersza
    lcd.print(dt.hour);   lcd.print(":");             //wypisz godzinę
    lcd.print(dt.minute); lcd.print(":");             //wypisz minutę
    lcd.print(dt.second); lcd.print("");              //wypisz sekundę
    
    lcd.setCursor(9, 1);                      //ustaw wskaźnik kursora na dziewiątym miejscu pierwszego wiersza
    lcd.print(clock.readTemperature());       //wypisz temperaturę (z miernika temperatury zamontowanego w module RTC)
    lcd.print((char)0xDF);                    //zrzutuj na char wartość “stopnia” zapisaną szesnastkowo z zestawu znaków HD44780
    lcd.print("C");                           //wypisz „C”
}

```
	
## <p align="center"> 4. Specyfikacja zewnętrzna </p>

#### Szczegółowy opis użytych elementów:

![Alt text](images/2.png?raw=true "Wyjścia mikroprocesora Atmega328P (powyższy schemat 168 jest kompatybilny z 328) – źródło: arduino.cc/en/Hacking/PinMapping168")

Mikroprocesor Atmega328P oferuje szeroką gamę bibliotek i jest stosunkowo łatwy w programowaniu. W projekcie wykorzystałem jego wyjścia analogowe ADC0 i ADC1 do pobierania próbek w pomiarach, cyfrowe PB5, PB4, PB3, PB1, PB0, PD7, PD6, PD5, PD4, PD3 i PD2 do obsługi elementów oraz magistralę I2C, czyli wyjścia ADC4/SDA i ADC5/SCL do komunikacji z modułem RTC. Do zaprogramowania Atmegi wykorzystałem programator USBASP oraz środowisko programistyczne Atmel Studio 7. Przed wgraniem programu zmieniłem fusebity Atmegi tak, aby działa na zewnętrznym kwarcu o częstotliwości 16MHz (fabrycznie Atmega miała taktowanie 1MHz, gdzie do tego wykorzystywała wewnętrzny kwarc 8MHz i fusebit CKDIV8 (divide clock by 8 internally). Do zmiany fusebitów wykorzystałem program AvrDude, wykorzystując polecenie avrdude –c usbasp –p atmega328p -U lfuse:w:0xF3:m, przeprogramowując tylko rejestr LOW (pozostałe – HIGH, EXTENDED oraz LOCKBIT pozostawiając nienaruszone). Układ zasilany jest powerbankiem o pojemności 2200mAh i napięciu wyjściowym 5V.

![Alt text](images/3.png?raw=true "Czujnik wilgotności FC-28 – źródło: abc-rc.pl/arduino-fc-28")

Czujnika wilgotności FC-28 używam do pomiaru wilgotności gleby. Jest to prosty układ porównujące rezystancję sondy z wcześniej ustaloną wartością. Woda powoduje spadek rezystancji pomiędzy ścieżkami sondy – co jest odczytywane przez układ sterownika, innymi słowy – im mniej wody w glebie, tym większa rezystancja pomiędzy ścieżkami sondy. Czujnik do prawidłowego działania potrzebuje prądu nie większego niż 20mA o napięciu od 3V – 5V, dlatego zasilam go prądem o napięciu 5V z wyjścia cyfrowego D8. Na sterowniku znajduje się potencjometr oraz diody sygnalizujące zasilanie i stan zadziałania modułu. Czujnik posiada dwa wyjścia: analogowe A0 oraz cyfrowe D0. Na wyjściu cyfrowym pojawia się sygnał 0 lub 1 w zależności od ustawienia progu zadziałania potencjometrem na płytce. Moduł posiada cztery wyprowadzenia, tj. VCC – zasilanie 3V – 5V, GND – masa, D0 – wyjście cyfrowe TTL sygnału przełączającego 5V oraz A0 – wyjście analogowe 0V–5V. W moim projekcie zdecydowałem się na wykorzystanie wyjścia analogowego A0, które pozwala na dokonanie dokładniejszego pomiaru.

![Alt text](images/4.png?raw=true "Czujnik wody Water Sensor A-MCW – źródło: abc-rc.pl/Water-Sensor")

Czujnik poziomu wody Water Sensor A-MCW wykorzystuję do badania stanu napełnienia wodą pojemnika, z którego będzie pobierać wodę pompa podczas nawadniania gleby. Jest to prosty układ wykrywające rezystancję sondy. Woda powoduje spadek rezystancji pomiędzy ścieżkami sondy, co po wzmocnieniu jest wykrywane przez mikroprocesor. Moduł posiada trzy wyjścia: S – wyjście analogowe, „+” - zasilanie 3V – 5V oraz „-” – masa. Czujnik nie pobiera prądu większego, niż 20mA o napięciu od 3V do 5V, dlatego zasilam go prądem z wyjścia cyfrowego D7.

![Alt text](images/5.png?raw=true "Moduł RTC DS3231 – źródło: abc-rc.pl/RTC-zegar-czasu-DS3231")

Moduł RTC DS3231 jest zegarem czasu rzeczywistego z własnym zasilaniem bateryjnym. Pozwala on na odczyt czasu w postaci godziny, minuty i sekundy oraz daty w postaci rok, miesiąc i dzień. Interfejsem komunikacyjnym tu jest magistrala I2C (linie SDA i SCL). Częstotliwość odświeżania danych wynosi 1Hz (1 sekunda). Moduł ten posiada wbudowany termometr służący do kalibracji zegara – gdy temperatura otoczenia wzrasta, to kwarc zwiększa taktowanie, natomiast podczas spadku temperatury, kwarc spowalnia taktowanie. Termometr służy do utrzymywania stałego taktowania i zapobiegania w ten sposób przekłamaniom w podawaniu aktualnego czasu. Zwykły moduł, przykładowo DS1307 może się opóźniać po rocznym użytkowaniu aż o 9 minut, natomiast deklarowana maksymalna różnica między aktualnym czasem, a czasem wskazywanym przez DS3231 wynosi 1 minutę, ale po 9 latach użytkowania, co jest więcej, niż zadowalającym efektem. Moduł do prawidłowego działania potrzebuje prądu nie większego niż 20mA o napięciu od 2.3V – 5.5V, dlatego zasilam go prądem z wyjścia Atmegi o napięciu 3.3V. Wyjścia modułu to: „+” – zasilanie 2.3V – 5.5V, D – linia danych, C – linia zegarowa, NC – no connection oraz „-” – masa. Do obsługi modułu wykorzystuję funkcje zawarte w bibliotece Wire.h oraz DS3231.h, z których ta pierwsza jest dostarczana wraz ze środowiskiem uruchomieniowym Arduino, natomiast druga jest autorstwa Pana Korneliusza Jarzębskiego i została przeze mnie pobrana z [jego profilu](github.com/jarzebski/Arduino-DS3231) w serwisie GitHub.com.

![Alt text](images/6.png?raw=true "Wyświetlacz LCD 2x16 QC1602B – źródło: abc-rc.pl/LCD-1602-Yellow")

Wyświetlacz alfanumeryczny LCD 2x16 QC1602B ze sterownikiem kompatybilnym z HD44780 służący do wyświetlania danych informacji użytkownikowi. Do obsługi wyświetlacza wykorzystuję funkcje zawarte w bibliotece LiquidCrystal.h dostarczonej wraz ze środowiskiem uruchomieniowym Arduino. Wyświetlacz zasilany jest prądem średnio 10mA o napięciu 5V. Wejścia wyświetlacza to: VSS – masa, VDD – zasilanie 5V, V0 – kontrast, RS – wybór rejestru instrukcji wyświetlacza (stan niski) albo rejestru danych (stan wysoki), RW – odczyt (Read – stan niski) lub zapis (Write – stan wysoki), E – odblokowanie wyświetlacza, 8 linii magistrali danych od DB0 do DB7, LEDA – (anoda) zasilanie podświetlenia 5V, LEDK – (katoda) masa podświetlenia. Sterowanie wyświetlaczem ustawiłem na tryb 4-bitowy bez odczytu flagi zajętości – do transmisji wkorzystuję cztery linie magistrali danych (D4, D5, D6 i D7) podłaczone kolejno do cyfrowych wyjść Atmegi (~5, 4, ~3 i 2) i dwie linie sterujące: RS podłączone do 12 cyfrowego wyjścia i E podłączone do ~11 wyjścia cyfrowego (w tym trybie RW jest podłączone do masy).

![Alt text](images/7.png?raw=true "Pompa wodna RS360SH – źródło: abc-rc.pl/pompa-wodna-z-igla-3-12v-5w")

Pompa wodna RS 360SH o mocy 5W z zakończeniem w postaci igły umożliwia wtłaczanie wody przez wężyk akwarystyczny do doniczki bez zanużania pompy w pojemniku z wodą, dzięki zastosowaniu silnika klasy 360. Mimo deklarowanego przez producenta prądu 1.2A o napięciu od 4V do 12V potrzebnego do pracy pompki, nie działa ona poprawnie, co może być spowodowane podaniem wartości pracy „na sucho”. Do pompowania wody, wykorzystuje ona prąd oscylujący między 1.8A – 2A o napięciu 6V, natomiast podczas rozruchu pompa potrzebuje dostarczenia aż 3A prądu, co zostało sprawdzone przeze mnie za pomocą miernika i dopiero w takich warunkach gwarantuje ona poprawne działanie. Nie należy podawać wyższego napięcia niż 7.5V przez dłuższy czas, gdyż może to zepsuć silniczek, dlatego postanowiłem do zasilania jej użyć czterech baterii AA 1.5V podpiętych szeregowo, co daje łącznie napięcie rzędu 6V. Pompa ta miałaby być sterowana przez Atmegę, aczkolwiek nie jest możliwym podłączenie jej bezpośrednio do wyjścia mikroprocesora, gdyż maksymalny prąd wyjściowy nie może przekroczyć 40mA, stąd postanowiłem w celu załączania i wyłączania pompy wykorzystać przekaźnik, o którym mowa poniżej.

![Alt text](images/8.png?raw=true "Przekaźnik SRD-05VDC-SL-C – źródło: abc-rc.pl/modul-przekaznika-1-kanal-5v-10a")

Przekaźnik SRD-05VDC-SL-C pozwala w moim projekcie sterować pracą pompy wodnej, bez konieczności bezpośredniego podłączania pompy do mikroprocesora. Jego zadaniem jest zwieranie i rozwieranie obwodu zawierającego pompę wodną i jej zasilanie, zależnie od sygnału otrzymanego z wyjścia Atmegi. Moduł ten może obsługiwać nawet 10A, a maksymalne napięcie styków to 250VAC. Zasilany jest on prądem nieprzekraczającym 20mA o napięciu 5V. Przekaźnik ten posiada trzy wyprowadzenia wejściowe: DC+ – zasilanie 5V, DC- – masa zasilania oraz IN – sygnał wejściowy, sterujący. Po drugiej stronie mamy trzy wejścia – NO, COM i NC, co kolejno oznacza Normally Opened, Common i Normally Closed, a to zaś można przetłumaczyć na „zazwyczaj otwarte”, „zwyczajne” i „zazwyczaj zamknięte”. Do wejścia COM podłączę przewód idący od pompy, a do wejścia NO przewód idący od zasilania – baterii (oczywiście baterie z drugiej strony są podłączone z pompą), natomiast NC zostawiam niepodłączone. Ustawiając stan niski na wyjściu cyfrowym 6 połączonym z wejściem IN załączam przekaźnik, którego wewnętrzny elektromagnes rozłącza połączenie COM z NC w celu złączenia COM z NO, co w tym przypadku powoduje przepływ prądu. Innymi słowy przekaźnik to switch, który ma dwa stany: COM zwarte z NC i NO „wiszące” oraz po podaniu stanu niskiego na wejście IN zwarte COM z NO i NC „wiszące”.

#### Skrócona instrukcja obsługi urządzenia:

Po podłączeniu zasilania do urządzenia, zostanie ono automatycznie uruchomione. Na samym początku zostanie sprawdzona wilgotność gleby oraz poziom wody w pojemniku i jeśli zajdzie potrzeba, to urządzenie nawodni ziemię, natomiast jeśli nie będzie wody w pojemniku, to ukaże się na ekranie odpowiedni komunikat oraz zapalona zostanie dioda powiadamiająca o zdarzeniu. Po napełnieniu wodą pojemnika urządzenie wyświetli na wyświetlaczu aktualną datę i czas, a także wynik pomiaru wilgotności gleby (w procentach) w górnym, prawym rogu, a pod nim aktualną temperaturę uaktualnianą co 60 sekund. Kolejne pomiary wilgotności gleby i poziomu wody będą wykonywane automatycznie co godzinę, a o zdarzeniu zostaniemy tak samo poinformowani, jak przy starcie urządzenia. Użytkownik ma możliwość wymuszenia dokonania pomiaru w dowolnej chwili poprzez przyciśnięcie przycisku na czas od 100ms do 1999ms. Po tym czasie wyświetlane informacje zostaną zastąpione aktualnymi danymi, tj. wilgotnością gleby i poziomem wody w pojemniku uaktualnianymi co sekudnę, przez 5 sekund. Po tym czasie urządzenie powróci do wyświetlania ekranu głównego. Przytrzymanie przycisku na dłużej, niż 2000ms spowoduje wywołanie drugiej funkcji, jaką jest wyświetlenie na ekranie daty i czasu ostatniego nawadniania gleby, a po 5 sekundach wyświetlenie daty i czasu ostatniego uzupełnienia wodą pojemnika. Po kolejnych 5 sekundach urządzenie powróci do wyświetlania ekranu głównego. Urządzenie będzie w podany sposób działać dopóki nie zostanie odcięte zasilanie – urządzenia bądź pompy. Pompa jest zasilana bateryjnie, ale istnieje możliwość podłączenia zasilacza. Aby zresetować układ (przywrócić do stanu początkowego), należy przycisnąć przycisk reset.

## <p align="center"> 5. Montaż i wtórna analiza </p>

Celem prezentacji projektu, układ został przeniesiony z płytki stykowej na płytkę uniwersalną, gdzie komponenty układu zostały trwale przylutowane do płytki z zachowaniem odpowiedniej dla tego typu elementów ostrożności.

Cały schemat wygląda następująco:

![Alt text](images/9.png?raw=true "Schemat techniczny projektu wykonany w programie Eagle – źródło: wykonanie własne")

![Alt text](images/10.png?raw=true "Schemat połączeń elementów z wyjściami Atmega328P – źródło: wykonanie własne")

Napięcie 5V oraz masę podłączyłem we wskazane miejsca na płytce stykowej, aby ułatwić sobie późniejszy dostęp, gdyż w zasadzie każde urządzenie peryferyjne z nich korzystało. 

Schematy podłączania czujników wilgotności i wody prezentują się następująco:

![Alt text](images/11.png?raw=true "Schemat podłączania wyjść czujnika wilgotności z Atmega328P – źródło: wykonanie własne")

![Alt text](images/12.png?raw=true "Schemat podłączania wyjść czujnika wody z Atmega328P – źródło: wykonanie własne")

Moduł RTC DS3231 zdecydowałem się zasilać napięciem 3.3V z wyjścia Atmegi, a do komunikacji między tym modułem, a mikroprocesorem wykorzystać magistralę I2C, gdzie wyjście D podłączyłem do wyjścia ADC4/SDA(PC4), a wyjście C do wyjścia ADC5/SCL(PC5):

![Alt text](images/13.png?raw=true "Schemat podłączania wyjść modułu RTC DS3231 z Atmega328P – źródło: wykonanie własne")

Sterowanie wyświetlaczem (tak jak już wspomniałem) ustawiłem na tryb 4-bitowy bez odczytu flagi zajętości – do transmisji wkorzystuję cztery linie magistrali danych (AD4, ADC5, ADC6 i ADC7) podłaczone kolejno do cyfrowych wyjść Atmegi (~5, 4, ~3 i 2) i dwie linie sterujące: RS podłączone do 12 cyfrowego wyjścia i E podłączone do ~11 wyjścia cyfrowego (w tym trybie RW jest podłączone do masy). Wyjście V0 jest zwarte z masą poprzez potencjometr – regulując rezystancję, jestem w stanie zwiększać, bądź zmniejszać kontrast wyświetlacza. Schemat połączeń wygląda następująco:

![Alt text](images/14.png?raw=true "Schemat podłączania wyjść wyświetlacza  LCD 2x16 QC1602B z Atmega328P – źródło: wykonanie własne")

Pompa wodna jest zasilana czterema bateriami AA 1.5V połączonymi szeregowo, aby uzyskać 6V napięcia. Do sterowania zasilaniem pompy używam przekaźnika, który podłączony jest do wyjść Atmegi jak na schemacie:

![Alt text](images/15.png?raw=true "Schemat podłączania pompy wodnej RS360SH z przekaźnikiem SRD-05VDC-SL-C  z Atmega328P – źródło: wykonanie własne")

Gdy stan niski jest podany na wejście IN przekaźnika, COM i NO jest zwierane ze sobą, a co za tym idzie pojawia się przepływ prądu między zasilaniem, a pompą, natomiast gdy na IN jest podany stan wysoki (domyślnie), NO i COM są rozwarte (jak na powyższym schemacie), co powoduje wyłączenie pompy wodnej.

Co do diody, to jej katoda jest zwarta do masy, natomiast anoda jest podłączona do wyjścia cyfrowego Atmegi poprzez dwa, szeregowo podpięte rezystory, z których każdy ma po 100Ω rezystancji.

![Alt text](images/16.png?raw=true "Schemat podłączenia diody z Atmega328P – źródło: wykonanie własne")

Ostatnim elementem układu jest przycisk. Z jednej strony podłączony jest do 5V, natomiast z drugiej do wyjścia cyfrowego Atmegi ustawionego na tryb INPUT. Jako, że wyjścia Atmegi są trójstanowe – stan wysoki, stan niski oraz stan wysokiej impedancji – to potrzebne było podłączenie równoległe rezystora 10kΩ zwartego do masy. W ten sposób wciskając przycisk powodujemy przepływ prądu, co zostanie wykryte przez wyjście Atmegi, natomiast gdy przycisk jest niewciśnięty, to wyjście jest zwarte jedynie z masą. Rezystor (w tym przypadku tzw. pulldown resistor) zapobiega przepływowi prądu do masy (potrzebnej do uzyskania stanu niskiego podczas niewciśniętego przycisku), gdy jest wciśnięty przycisk.

![Alt text](images/17.png?raw=true "Schemat podłączenia przycisku z Atmega328P – źródło: wykonanie własne")

## <p align="center"> 6. Kod źródłowy </p>

```C++

/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <LiquidCrystal.h>
#include <Wire.h>
#include "DS3231.h"
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio
#ifndef F_CPU																              // if F_CPU was not defined in Project -> Properties
#define F_CPU 16000000UL													        // define it now as 16 MHz unsigned long
#endif

/*FUNCTIONS*/
void viewDefault();
void considerData();
void diodeOn();
void diodeOff();
void buttonFunction();
void viewNo1();
void viewNo2();
void moistureSensorFunction();
void waterSensorFunction();
void motorRelayFunction();

/*LCD*/
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;					//lcd pinouts to atmega ports
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);									        //lcd variable
/*RTC*/
DS3231 clock;																                        //RTC variable
RTCDateTime dt;																                      //date and time format variable
/*MS*/
const int atmegaPinoutMoistureSensorAnalogPin = A0;							    //moisture sensor analog pin
const int atmegaPinoutMoistureSensorVcc = 8;								        //moisture sensor vcc
int moistureSensorValue = 0;												                //value measured by moisture sensor
float moistureSensorValuePercentage = 0;									          //value measured by moisture sensor but in percentage
/*WS*/
const int atmegaPinoutWaterSensorAnalogPin = A1;							      //water sensor analog pin
const int atmegaPinoutWaterSensorVcc = 7;									          //water sensor vcc
int waterSensorValue = 0;													                  //value measured by water sensor
/*RELAY*/
const int atmegaPinoutRelayIn = 6;											            //relay input signal pinout (in the water pump circuit)
/*DIODE*/
const int atmegaPinoutDiodeAnode = 9;										            //diode
/*BUTTON*/
const int atmegaPinoutButton = 13;											            //button
long double buttonUseTime = 0;												              //time of button being used [ms]
/*OTHER*/
int presentTime = 0;														                    //present time [s]
int lastTest = 0;															                      //time of last check of the sensors [s]
const int sensorsInterval = 14;												              //time interval (which is one loop) between measurements [s]
const int diodeCounter = 6;													                //diode loop counter
const int thresholdNo1 = 100;												                //time threshold of entering into viewNo1 [ms]
const int thresholdNo2 = 2000;												              //time threshold of entering into viewNo2 [ms]
RTCDateTime lastWatering;													                  //date and time of the last watering
RTCDateTime lastRefilling;													                //date and time of the last refilling

void setup() {
	clock.begin();
	clock.setDateTime(__DATE__, __TIME__);
	pinMode(atmegaPinoutMoistureSensorVcc, OUTPUT);							      //setting digital pin No. 8 as output
	pinMode(atmegaPinoutWaterSensorVcc, OUTPUT);							        //setting digital pin No. 7 as output
	pinMode(atmegaPinoutRelayIn, OUTPUT);									            //setting digital pin No. 6 as output
	pinMode(atmegaPinoutDiodeAnode, OUTPUT);								          //setting digital pin No. 9 as output
	pinMode(atmegaPinoutMoistureSensorAnalogPin, INPUT);					    //setting analog pin No. A0 as input
	pinMode(atmegaPinoutWaterSensorAnalogPin, INPUT);						      //setting analog pin No. A1 as input
	pinMode(atmegaPinoutButton, INPUT);										            //setting digital pin No. 13 as input
	digitalWrite(atmegaPinoutRelayIn, HIGH);								          //pin state is low as default, but it triggers relay... 
                                                                    //...so we set it as high to prevent that
	lcd.begin(16, 2);														                      //setting lcd
	lcd.clear();															                        //clearing (just in case)
	delay(100);
	lcd.setCursor(2, 0);
	lcd.print("Automatyczny");
	lcd.setCursor(4, 1);
	lcd.print("Ogrodnik");
	delay(4000);
	moistureSensorFunction();
	waterSensorFunction();
	considerData();
}

void loop() {
	buttonFunction();
	viewDefault();
	delay(90);
}

void viewDefault() {
	lcd.clear();
	presentTime = millis()/1000;

	if (presentTime - lastTest >= sensorsInterval) {
		moistureSensorFunction();
		waterSensorFunction();
		considerData();
		lcd.clear();
	}

	dt = clock.getDateTime();
	lcd.setCursor(1, 0);
	lcd.print(dt.day);    lcd.print(".");
	lcd.print(dt.month);  lcd.print(".");
	lcd.print(dt.year);   lcd.print("");
	lcd.setCursor(12, 0);
	lcd.print((int)moistureSensorValuePercentage);
	lcd.print("%");
	lcd.setCursor(0, 1);
	lcd.print(dt.hour);   lcd.print(":");
	lcd.print(dt.minute); lcd.print(":");
	lcd.print(dt.second); lcd.print("");
	lcd.setCursor(9, 1);
	lcd.print(clock.readTemperature());
	lcd.print((char)0xDF);
	lcd.print("C");
}

void moistureSensorFunction() {
	digitalWrite(atmegaPinoutMoistureSensorVcc, HIGH);
	delay(10);
	moistureSensorValue = 1023 - analogRead(atmegaPinoutMoistureSensorAnalogPin);
	delay(10);
	digitalWrite(atmegaPinoutMoistureSensorVcc, LOW);

	if (moistureSensorValue <= 60) {
		moistureSensorValue = 0;														 //sensor calibration - measurement error of size 60 (value starts at 0)
	} else if (moistureSensorValue >= 620) {
		moistureSensorValue = 720;													 //sensor calibration - measurement error of size 60 (value ends at 740)
	}
	moistureSensorValuePercentage = ((float)moistureSensorValue / 720) * 100;
}

void waterSensorFunction() {
	digitalWrite(atmegaPinoutWaterSensorVcc, HIGH);
	delay(10);
	waterSensorValue = analogRead(atmegaPinoutWaterSensorAnalogPin);
	delay(10);
	digitalWrite(atmegaPinoutWaterSensorVcc, LOW);
}

void considerData() {
	if (moistureSensorValuePercentage < 31) {											//if soil is dry
		if (waterSensorValue > 220) {													      //there is water in the container
			motorRelayFunction();
			lastWatering = clock.getDateTime();
			moistureSensorFunction();
		} else {																		                //there is no water in the container
			do {
				for (int i = 0; i < diodeCounter; ++i) {
					lcd.clear();
					diodeOn();
					diodeOff();
					lcd.setCursor(1, 0);
					lcd.print("ZBIORNIK PUSTY");
					lcd.setCursor(3, 1);
					lcd.print("NALEJ WODE");
					diodeOn();
					diodeOff();
				}
				waterSensorFunction();
			} while(waterSensorValue <=220);
			motorRelayFunction();
			lastWatering = clock.getDateTime();
			lastRefilling = clock.getDateTime();
			moistureSensorFunction();
	  }
  } else if (moistureSensorValuePercentage >= 31 && waterSensorValue <= 220) {	//if soil is moist...
                                                                                //...but there is no water in the container
		do {
			for (int i = 0; i < diodeCounter; ++i) {
				lcd.clear();
				diodeOn();
				diodeOff();
				lcd.setCursor(1, 0);
				lcd.print("ZBIORNIK PUSTY");
				lcd.setCursor(3, 1);
				lcd.print("NALEJ WODE");
				diodeOn();
				diodeOff();
			}
			lastRefilling = clock.getDateTime();
			waterSensorFunction();
		} while(waterSensorValue <=220);
	}
	presentTime = millis()/1000;
	lastTest = presentTime;
}

void motorRelayFunction() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Pompa wlaczona!");
	lcd.setCursor(0, 1);
	lcd.print("Pozostalo: ");
	digitalWrite(atmegaPinoutRelayIn, LOW);
	for (int i=0; i<6;) {
		++i;
		lcd.setCursor(11, 1);
		lcd.print(6-i);
		delay(1000);
	}
	digitalWrite(atmegaPinoutRelayIn, HIGH);
	delay(1000);
}

void diodeOn() {
	for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
		analogWrite(atmegaPinoutDiodeAnode, fadeValue);
		delay(10);
	}
}

void diodeOff() {
	for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
		analogWrite(atmegaPinoutDiodeAnode, fadeValue);
		delay(10);
	}
}

void buttonFunction() {
	while(digitalRead(atmegaPinoutButton) == HIGH) {
		delay(100);
		buttonUseTime += 100;
	}
	if(buttonUseTime >= thresholdNo2) {
		viewNo2();
		} else if(buttonUseTime>=thresholdNo1) {
		viewNo1();
	}
	buttonUseTime = 0;
}

void viewNo1() {
	lcd.clear();
	for(int i = 0; i < 6; ++i) {
		lcd.setCursor(0, 0);
		lcd.print("Wilgotnosc: ");
		waterSensorFunction();
		moistureSensorFunction();
		lcd.print((int)moistureSensorValuePercentage);
		lcd.print("%");
		lcd.setCursor(0, 1);
		if (waterSensorValue <=220) {
			lcd.print("Uzupelnij zrodlo");
			} else {
			lcd.print("Zrodlo pelne");
		}
		delay(1000);
	}
}

void viewNo2() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Ost. podlewanie");
	lcd.setCursor(0, 1);
	lcd.print(lastWatering.day);    lcd.print(".");
	lcd.print(lastWatering.month);  lcd.print(".");
	lcd.print(lastWatering.year);   lcd.print("");
	lcd.setCursor(11, 1);
	lcd.print(lastWatering.hour);   lcd.print(":");
	lcd.print(lastWatering.minute);
	delay(3500);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Ost. dolewanie");
	lcd.setCursor(0, 1);
	lcd.print(lastRefilling.day);    lcd.print(".");
	lcd.print(lastRefilling.month);  lcd.print(".");
	lcd.print(lastRefilling.year);   lcd.print("");
	lcd.setCursor(11, 1);
	lcd.print(lastRefilling.hour);   lcd.print(":");
	lcd.print(lastRefilling.minute);
	delay(3500);
	lcd.clear();
}

```

## <p align="center"> 7. Wnioski i testowanie </p>

Na każdym etapie wykonywania projektu – czy to było wstępne projektowanie i założenia, kompletowanie części, podłączanie modułów i czujników, obliczanie wartości, obmyślanie algorytmu, programowanie mikroprocesora, czy też montaż końcowy i testowanie gotowego urządzenia – towarzyszyły mi liczne problemy. Obmyślając całość na papierze wszystko wydaje się idealne, natomiast dopiero mając fizyczne części i testując je, można dostrzec niezgodności w odczytach. Czujnik wilgotności nie bada wilgotności w procentach od 0 do 100, a rezystancję między wyjściami sondy w skali od 1023 do 0 przy całkowitym zwarciu. Gdy przyszło do testów, okazało się, że czujnik w powietrzu uzyskuje rezystancję ok. 700, a będąc zanurzonym w wodzie, rezystancja spada do ok. 150. Zamieniając to na żądaną wilgotność w procentach, wychodziło mi np. że w wodzie wilgotność wynosi 75%. To zmusiło mnie do kaliborwania czujnika, zarówno wiglotności, jak i wody oraz wykonywania wielu testów celem oszacowania błędu pomiarowego, aby wartości były jak najbardziej zbliżone do oczekiwanych.
Innym razem podłączyłem do pompy wodnej zasilanie, tak jak mówił producent – 1.2A o napięciu  8V (producent podał 4V-12V). Pompa działała poprawnie do chwili, gdy nie kazałem jej pompować wody.Wówczas nie była ona w stanie działać poprawnie i po początkowym spadku ciśnienia wody, pompa zaczęła zwalniać obroty, aż na końcu pozostał tylko terkoczący silniczek niezdolny do pracy. Jak się później okazało, pompa do prawidłowego nawadniania potrzebowała 3A na starcie i ok. 2A do utrzymania pracy.
Oprócz problemów, było też miłe zaskoczenie, gdyż dowiedziałem się, że zakupiony przeze mnie moduł RTC DS3231 ma wbudowany termometr do kalibracji kwarcu. Wystarczyło tylko znaleźć bibliotekę zawierającą funkcje wykorzystujące go i byłem w stanie wyświetlać dokładną temperaturę panującą w otoczeniu, tak naprawdę jedynie dzięki „rzuceniu” okiem na dokumentację techniczną dołączoną do zakupionego modułu.

Wykonując projekt nabrałem trochę praktyki w programowaniu mikroprocesorów. Dzięki szerokiemu wachlarzowi bibliotek oraz dostępnym narzędziom dla mikroprocesora Atmega328P, projekt ten, mimo bycia praco- i czasochłonnym, to uważam za warty poświęcenia każdej chwili. Satysfakcja płynąca z „ożywiania” martwych komponentów i tworzenia w ten sposób urządzenia funkcjonalnego rekompensuje chwile zwątpienia podczas odczytywania niezrozumiałych lub błędnych wyników pomiarów.
