W celu kompilacji odpowiednich podpunktów należy postępować następująco:
- make zad1 kompiluje zadanie 1 

Testowanie rozwiązań: 
zad1: Uruchamiamy ./build/zad1server z argumentami ADRES i PORT. Jeśli uruchomimy z argumentami NULL NULL, serwer sam wybierze adres i port, które zostaną wyświetlone w terminalu. Następnie w kilku terminalach uruchamiamy ./build/zad1client ADRES PORT NAZWA. ADRES i PORT powinny być takie same jak w serwerze. Serwer zapamięta nazwy klientów. Komenda `list` zwróci klientowi na jego standardowe wyjście listę wszystkich aktywnych klientów (numer deskryptora i nazwa). Komenda `2all STRING` wyśle do wszystkich klientów wiadomość STRING, `2one ID STRING` wyśle do klienta o deskryptorze ID wiadomość STRING, `STOP` wyrejestruje klienta z serwera i zakończy jego działanie, `ALIVE` rozpocznie wysyłanie do klienta informacji o zarejestrowanych klientach, `END_ALIVE` zakończy wysyłanie tych wiadomości. 
