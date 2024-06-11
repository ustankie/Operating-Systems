W celu kompilacji odpowiednich podpunktów należy postępować następująco:
- make zad1 kompiluje zadanie 1 (klienta oraz serwer)

# Testowanie rozwiązań
zad1: Uruchamiamy ./build/zad1server z argumentami ADRES i PORT. Jeśli uruchomimy z argumentami NULL NULL, serwer sam wybierze adres i port, które zostaną wyświetlone w terminalu. Następnie w kilku terminalach uruchamiamy ./build/zad1client ADRES PORT_SERWERA NAZWA. ADRES i PORT_SERWERA powinny być takie same jak w serwerze. Serwer zapamięta nazwy klientów. Komenda `list` zwróci klientowi na jego standardowe wyjście listę wszystkich aktywnych klientów (numer indeksu w tablicy i nazwa). Komenda `2all STRING` wyśle do wszystkich klientów wiadomość STRING, `2one ID STRING` wyśle do klienta o indeksie ID wiadomość STRING, `STOP` wyrejestruje klienta z serwera i zakończy jego działanie, `ALIVE` rozpocznie wysyłanie do klienta informacji o zarejestrowanych klientach, `END_ALIVE` zakończy wysyłanie tych wiadomości. Przykładowe działanie:
# a) komenda LIST
![alt text](images/image-3.png)  ![alt text](images/image.png) ![alt text](images/image-2.png)  ![alt text](images/image-1.png)     

# b) komenda 2all

- wysyłający cl2  
  ![alt text](images/image-4.png) 
- odbiera wiadomość cl1  
  ![alt text](images/image-5.png)  
- odbiera wiadomość cl3  
  ![alt text](images/image-6.png) 

# c) komenda 2one
- cl3 wysyła wiadomość def do cl1:     
  ![alt text](images/image-8.png)
- cl1 odbiera wiadomość:    
  ![alt text](images/image-23.png)
- cl2 nie odbiera wiadomości:    
  ![alt text](images/image-10.png)

# d) Komenda alive
- cl1, cl2 wysyłają  komendę alive - w jej wyniku są co 5 sekund powiadamiani o statusie  aktywności klientów zarejestrowanych    
  ![alt text](images/image-11.png)
  ![alt text](images/image-12.png)
- komenda end_alive w cl1 zatrzymuje wysyłanie updatów o klientach do cl1, ale nie do cl2:  
  - cl1:   
  ![alt text](images/image-13.png)
  - cl2:   
    ![alt text](images/image-14.png)
- zatrzymam teraz cl1 komendą ctrl+Z:       
  ![alt text](images/image-15.png)
- w cl2 zaobserwowaliśmy usunięcie cl1:    
  ![alt text](images/image-16.png)
- zobaczmy co się stanie jeśli wykonamy komendę STOP w cl3:   
  - cl3:  
    ![alt text](images/image-17.png)  
  - cl2:   
    ![alt text](images/image-18.png)    
    Jak widać, tym razem klient sam się wyrejestrował z serwera, dlatego nie otrzymaliśmy komunikatu o jego usunięciu.
# e) Ctrl+C:   
Uruchommy klientów cl2, cl3 jeszcze raz i zobaczmy co się stanie po wciśnięciu ctrl+c:
- przed ctrl+c   
  ![alt text](images/image-19.png)
  ![alt text](images/image-20.png)
- po ctrl+c na kliencie cl3 - działa tak samo jak STOP:   
  ![alt text](images/image-24.png)
  ![alt text](images/image-22.png)

