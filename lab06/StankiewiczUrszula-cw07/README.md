W celu kompilacji odpowiednich podpunktów należy postępować następująco:
- make zad1 kompiluje zadanie 1 
- make zad2 kompiluje zadanie 2
- make zad2_counter_ kompiluje program liczący całkę (zad2_counter.c) z zadania 2
- make zad2sender kompiluje program tworzący fifo i printujący wynik (zad2sender.c) z zadania 2
- make all kompiluje wszystkie zadania
  
Testowanie rozwiązań: 
zad1: Uruchamiamy ./build/zad1 arg1 arg2
    gdzie arg1 to szerokość prostokąta, a arg2 liczba procesów do utworzenia
    Można też uruchomić ./build/zad1_logger, który wykona zad1 dla przykładowych danych

zad2: 
- Uruchamiamy ./build/zad2sender
- Wpisujemy granice przedziału oraz szerokość prostokąta
- w nowym terminalu uruchamiamy ./build/zad2_counter
- Otrzymujemy wynik w pierwszym terminalu
