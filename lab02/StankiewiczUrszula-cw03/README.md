W celu kompilacji odpowiednich podpunktów należy postępować następująco:
- make zad1a kompiluje podpunkt zadania 1 dotyczący przetwarzania danych po znaku
- make zad1b kompiluje podpunkt zadania 1 dotyczący przetwarzania danych blokami po 1024B
- make zad2 kompiluje zadanie 2
- make all kompiluje wszystkie zadania
  
Testowanie rozwiązań: 
zad1a, zad1b: Uruchamiamy ./build/zad1a arg1 arg2
    gdzie arg1 to plik wejściowy, a arg2 wyjściowy (preferowana nazwa wyjściowego to out*.txt)
zad2: Uruchamiamy ./build/zad2

Można też uruchomić przykładowy test, którego pierwszym argumentem jest plik in.txt. Powstanie plik out.txt, który następnie
zostanie użyty jako pierwszy argument ponownego wywołania programu - wtedy powstanie out2.txt

zad1a: make test1a
zad1b: make test1b