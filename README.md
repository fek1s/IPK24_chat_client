## IPK Project 1: Client for a chat server using IPK24-CHAT protocol

#### Author: Jakub Fukala (xfukal01)

## Obsah

1. [**Úvod**](#úvod)
2. [**Teoretický úvod**](#teoretický-úvod)
3. [**Implementovaná funkcionalita**](#implementovaná-funkcionalita)
4. [**Testování**](#testování)
5. [**Dodatečná funkcionalita**](#dodatečná-funkcionalita)
6. [**Zdroje**](#zdroje)

## Úvod

### Účel dokumentace
Tato dokumentace slouží k popisu implementace projektu IPK-CHAT-CLIENT, který se zabývá komunikací mezi klientem a serverem v jazyce C. Obsahuje strukturu programu, popis implementovaných funkcí a informace o testování.

### Struktura dokumentace
Dokumentace je rozdělena do několika sekcí, které postupně popisují teoretický úvod, implementovanou funkcionalitu, detailní popis funkcí a informace o testování a dodatečné funkcionalitě.

## Teoretický úvod

### Princip komunikace klient-server
Klient-server architektura je základním konceptem, ve kterém klienti požadují služby a servery poskytují tyto služby odpovídajícím způsobem. Komunikace mezi klientem a serverem může probíhat pomocí různých protokolů, jako jsou TCP (Transmission Control Protocol) a UDP (User Datagram Protocol).

### Protokoly TCP a UDP
- **TCP (Transmission Control Protocol):** Protokol zajišťující spolehlivou a řízenou komunikaci mezi klientem a serverem. Zahrnuje mechanismy pro kontrolu toku, řízení přetížení a potvrzení přenosu dat.
- **UDP (User Datagram Protocol):** Protokol poskytující neřízenou a nespolehlivou komunikaci mezi klientem a serverem. Data jsou posílána bez záruky doručení nebo zachování pořadí.

## Implementovaná funkcionalita
- Při používání transportního protokolu **TCP** je klient schopen se připojit k serveru, autentizovat se, přejmenovat uživatele, připojit se k kanálu a posílat zprávy.
- Při používání transportního protokolu **UDP** je nutná implementace spolehlivého přenosu zpráv, který zajišťuje doručení zpráv a potvrzení o přijetí. 
Pro tyto učely byli implementovány funkce pro přijímání a odesílaní potvrzení.
- Pokud není na zpravu přijato porvrzení, klient zprávu znovu odešle tolikrát, jak je specifikováno parametrem `-r` (defaultně 3x).
- Klient je schopen zpracovávat zprávy od serveru a reagovat na ně podle specifikace protokolu IPK24-CHAT.
- Klient je schopen zpracovávat argumenty příkazové řádky a zprávy od uživatele.

### Struktura programu
Program je rozdělen do několika souborů:
- **ipkcpc.c:** Hlavní soubor obsahující funkci `main` a směrování programu podle zadaných argumentů.
- **ipkcpc_utils.h:** Hlavičkový soubor obsahující deklarace funkcí a struktur.
- **tcp_utils.c:** Implementační soubor obsahující definice funkcí pro TCP protokol.
- **udp_utils.c:** Implementační soubor obsahující definice funkcí pro UDP protokol.
- **parse.c:** Implementační soubor obsahující definice funkcí pro zpracování argumentů příkazové řádky a zpráv.
- **Makefile:** Soubor obsahující pravidla pro kompilaci programu.
- **server** (adresář): Adresář obsahující zdrojové kódy serveru sloužícího pro testování klienta (TCP)

### Hlavičkové soubory
- `ipkcpc_utils.h` obsahuje deklarace funkcí a struktur potřebných pro projekt.
- `stdint.h`, `arpa/inet.h`, `string.h`, `sys/socket.h`, `netinet/in.h`, `stdio.h`, `unistd.h`, `stdlib.h`, `netdb.h`, `pthread.h`, `stdbool.h`, `sys/time.h` jsou standardní hlavičkové soubory pro manipulaci se sokety, řetězci, vlákny atd.

### Tělo programu
- `ipkcpc.c` obsahuje funkci `main`, která zpracovává argumenty příkazové řádky a směruje program podle zvoleného protokolu
do funkcí `useTCP` nebo `useUDP`.

  
## Testování

### Testovací prostředí
- Program byl testován na operačním systému Fedora 39 s kompilátorem GCC 13.2.0.
- Ke sledování síťové komunikace byl použit Wireshark 4.0.13.

### Testování funkcionality TCP (Lokální testování)
- Pro testování TCP slouži dummy server, v adresáři `server`, který naslouchá na portu 1999 a zobrazuje přijaté zprávy.
- Pro spuštění serveru je třeba přejít do adresáře `server` přeložit pomocí `make` a spustit pomocí `./server`.
- Klient byl spouštěn s následujícími argumenty: `./ipk24chat-client -t tcp -s 127.0.0.1 -p 1999`.
- Nyní je možné posílat zprávy z klienta na server a zobrazovat je v terminálu serveru.
- Testování bylo provedeno s různými zprávami a bylo ověřeno, že server přijímá zprávy od klienta.
#### Kratká demonstrace základních příkazů:
##### /auth
- Napřiklad pro input `/auth user1 secret JakubF` byl výstup `AUTH user1 AS JakubF USING secret`.
##### /join 
- Pro input `/join ch1`  `JOIN ch1 AS JakubF`.
##### /rename
- Dále při použití příkazu `/rename NotJakub` a nasledně `/join ch2` byl výstup `JOIN ch2 AS NotJakub `
##### /help
- Pro input `/help` je vypsán seznam dostupných příkazů.

### Testování funkcionality TCP (Vzdálené testování)

- Pro vzdálené testování byl použit server kolegy Vítka Pavlíka `vitapavlik.cz` který naslouchá na portu 4567.
- Klient byl spouštěn s následujícími argumenty: `./ipk24chat-client -t tcp -s vitapavlik.cz -p 4567`.
- Nyní na poslané zprávy server reaguje a zobrazuje je v terminálu.
- Testování bylo provedeno s různými zprávami a bylo ověřeno, že server přijímá zprávy od klienta.
##### /auth 
- Pro input `/auth user1 secret JakubF` byl výstup `Success: Hi, user1! Successfully authenticated you as JakubF.` což
je odpověď serveru konkrétně zpráva typu REPLY která obsahuje informaci o úspěšném přihlášení.

##### /join
- Pro input `/join ch1`  `Success: Hi, JakubF! Successfully joined you NOWHERE! This server has only one channel.` což 
je odpověď serveru konkrétně zpráva typu REPLY která obsahuje informaci o úspěšném připojení k danému kanálu.

##### /rename 
- Dále při použití příkazu `/rename NotJakubF` a nasledně `/join ch2` byl výstup `Success: Hi, NotJakubF! Successfully 
joined you NOWHERE! This server has only one channel.` což ukazuje uspěšné přejmenování uživatele a následné připojení
k jinem kanálu.

##### /bye 
- Pro input `/bye` je na server odeslána zpráva typu BYE

### Testování funkcionality UDP (Lokální testování)

- Pro lokalní testování UDP byl nejprve použit netcat konkétně `nc -4 -u -l -v 127.0.0.1 4567` naslouchající na portu 4567.

- Klient byl spouštěn s následujícími argumenty: `./ipk24chat-client -t udp -s 127.0.0.1 -p 4567`.

- Nasledně se přešlo na lokalní [UDP Server](https://github.com/okurka12/ipk_proj1_livestream/blob/main/ipk_server.py)

- Nyní je možné posílat zprávy z klienta na lokalní server.
- Spouštění serveru `python3.10 ipk_server.py`
- Klient byl spouštěn s následujícími argumenty: `./ipk24chat-client -t udp -s 127.0.0.1 -p 4567`.

##### /auth
- Napřiklad pro input `/auth user1 secret JakubF` je odelsána zpráva:
- `[|01|10|user1.JakubF.secret.]` kde `.` ke NULL byte. a první tři byty jsou (zkráceně) `01` což je kód pro AUTH. 
a `10` je Sekvenční číslo zprávy.
- Server odpoví zprávou Confirm `|0|10|` kde Sekvenční čísla zpráv se shodují.
- Dále je zpráva `|1|X|` kde `1` je kód pro REPLY a `X` je sekvenční číslo zprávy.
- Klien odešle zprávu `|0|X|` kde `0` je kód pro CONFIRM a `X` je sekvenční číslo zprávy.
- Tato sekvence zpráv znazornuje úspěšné přihlášení uživatele.
##### /join
- Pro input `/join ch1` je odelsána zpráva:
- `[|3|100|ch1.JakubF.]` kde `.` ke NULL byte. a první tři byty jsou (zkráceně) `03` což je kód pro JOIN.
- Server odpoví zprávou Confirm `|0|100|` kde Sekvenční čísla zpráv se shodují.
- Dále je zpráva `|1|X|` kde `1` je kód pro REPLY a `X` je sekvenční číslo zprávy.
- Klien odešle zprávu `|0|X|` kde `0` je kód pro CONFIRM a `X` je sekvenční číslo zprávy.

##### /rename
- Pro input `/rename NotJakubF` a nasledně `/join ch2` je odelsána zpráva:
- `[|3|100|NotJakubF.]` kde `.` ke NULL byte. a první tři byty jsou (zkráceně) `03` což je kód pro RENAME.
- Server odpoví zprávou Confirm `|0|100|` kde Sekvenční čísla zpráv se shodují.
- Dále je zpráva `|1|X|` kde `1` je kód pro REPLY a `X` je sekvenční číslo zprávy.
- Klien odešle zprávu `|0|X|` kde `0` je kód pro CONFIRM a `X` je sekvenční číslo zprávy.
- Tato sekvence zpráv znazornuje úspěšné přejmenování uživatele a následné připojení k jinem kanálu.

##### /bye
- Pro input `/bye` je odelsána zpráva:
- `[|FF|xx|NotJakubF.]` kde `.` ke NULL byte. a první byte je  `FF` což je kód pro BYE.
- Server odpoví zprávou Confirm `|0|xx|` kde Sekvenční čísla zpráv se shodují.

- Pokud na zpravu bye server neodpoví, klient zpravu odešle znovu a to tolika krát 
jak je specifikováno parametrem `-r` (defaultně 3x).
- Toto bylo otestováno tak že byl klient spuštěn takzvaně "na slepo" tedy bez serveru a bylo ověřeno že klient
odesílá zprávu znovu. 

## Dodatečná funkcionalita
- V tomto bodě bych chtěl zmínit pouze příkaz `/bye` který posíla zprávu typu BYE na server a ukončuje spojení.

## Zdroje  
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)
- [Markdown](https://www.markdownguide.org/cheat-sheet/)
- [Wireshark](https://www.wireshark.org/)
- [Makefile writing](https://devhints.io/makefile)
- [Python UDP Server](https://github.com/okurka12/ipk_proj1_livestream/tree/main)
- [Licence](https://choosealicense.com/licenses/)