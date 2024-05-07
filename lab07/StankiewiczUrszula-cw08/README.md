# Zadanie 8

Wykorzystując semafory i pamięć wspólną z IPC Systemu V lub standardu POSIX napisz program symulujący działanie systemu wydruku: 

System składa się z N użytkowników oraz M drukarek. Każdy z użytkowników może wysłać do systemu zadanie wydruku tekstu składającego się z 10 znaków. Drukarka, która nie jest aktualnie zajęta, podejmuje się zadania "wydruku" tekstu danego zadania. Wydruk w zadaniu polega na wypisaniu na standardowym wyjściu znaków wysłanych wcześniej do wydruku w ten sposób, że każdy następny znak wpisywany jest co jedną sekundę. Jeżeli wszystkie drukarki są zajęte to zlecenia wydruku są kolejkowane w opisywanym systemie wydruku. Jeżeli kolejka jest pełna to użytkownik chcący zlecić zadanie wydruku czeka do momentu gdy będzie można zlecenie wpisać do kolejki.

Każdy z N użytkowników powinien przesyłać do systemu wydruku zadanie wydruku 10 losowych znaków (od 'a' do 'z') a następnie odczekać losową liczbe sekund. Zadania zlecenia wydruku i odczekania powinny być wykonane w nieskończonej pętli. 
Należy zsynchronizować prace użytkowników oraz drukarek. Należy użyć mechanizmów System V lub POSIX.

# Kompilacja
W celu kompilacji odpowiednich podpunktów należy postępować następująco:
- make zad1 kompiluje zadanie 1 

# Testowanie rozwiązań
Uruchamiamy ./build/zad1printer arg1 arg2, a następnie ./build/zad1client arg1 arg2. arg1 i arg2 muszą być takie same dla obu uruchomień, arg1 oznacza liczbę klientów, a arg2 liczbę drukarek. Następnie w konsoli klienta zostanie wyświetlona informacja o liczbie drukarek i klientów, a także informacja o początkowych wartościach utworzonych semaforów i pid procesów potomnych. Później zaczną być wyświetlane informacje o wysyłanych przez klientów wiadomościach, ewnetualnie o wpisaniu wiadomości do kolejki. W konsoli serwera pojawi się podobnie, informacja o liczbie drukarek i klientów, a także informacja o początkowych wartościach utworzonych semaforów i pid procesów potomnych, a następnie zaczną być drukowane znaki z informacją, czy wiadomość pochodzi z kolejki oraz która drukarka ją drukuje i który klient ją wysłał. Można zmieniać zakres losowości czasu czekania klientów `sleep(rand() % 15 + 2);` w celu uzyskania większego bądź mniejszego korzystania z kolejki klientów.

