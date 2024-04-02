W celu kompilacji odpowiednich podpunktów należy postępować następująco:
- make zad1 kompiluje zadanie 1 
- make zad2 kompiluje zadanie 2
- make zad2catcher kompiluje program catcher (zad2catcher.c) z zadania 2
- make zad2sender kompiluje program sender (zad2sender.c) z zadania 2
- make all kompiluje wszystkie zadania
  
Testowanie rozwiązań: 
zad1: Uruchamiamy ./build/zad1 arg1 
    gdzie arg1 to opcja uruchomienia programu (none, handler, ignore, mask)

zad2: 
- Uruchamiamy ./build/zad2catcher
- W konsoli zostaje wyświetlony PID - kopiujemy go
- w nowym terminalu uruchamiamy ./build/zad2sender arg1 arg2
    gdzie arg1 to PID skopiowane wcześniej, a arg2 to tryb pracy (1,2 lub 3)
