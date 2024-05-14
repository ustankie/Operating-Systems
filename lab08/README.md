Kompilacja: make all
Uruchomienie: ./life arg1, gdzie arg1 to liczba wątków, na które chcemy podzielić program.

Działanie: program dzieli mapę gry na arg1 części, każdy wątek jest odpowiedzialny za jedną z nich. Wątek główny tylko tworzy wątki potomne i rysuje mapę.