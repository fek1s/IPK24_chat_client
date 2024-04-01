## IPK Project 1: Client for a chat server using IPK24-CHAT protocol

#### Author: Jakub Fukala (xfukal01)

## Obsah

1. [**Úvod**](#úvod)
2. [**Teoretický úvod**](#teoretický-úvod)
3. [**Implementovaná funkcionalita**](#implementovaná-funkcionalita)
4. [**Testování**](#testování)
5. [**Dodatečná funkcionalita**](#dodatečná-funkcionalita)

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
- Napřiklad pro input `/auth user1 JakubF secret` byl výstup `AUTH user1 AS JakubF USING secret`.
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
- Pro input `/auth user1 JakubF secret` byl výstup `Success: Hi, user1! Successfully authenticated you as JakubF.` což
je odpověď serveru konkrétně zpráva typu REPLY která obsahuje informaci o úspěšném přihlášení.

##### /join
- Pro input `/join ch1`  `Success: Hi, JakubF! Successfully joined you NOWHERE! This server has only one channel.` což 
je odpověď serveru konkrétně zpráva typu REPLY která obsahuje informaci o úspěšném připojení k danému kanálu.

##### /rename 
- Dále při použití příkazu `/rename NotJakubF` a nasledně `/join ch2` byl výstup `Success: Hi, NotJakubF! Successfully 
joined you NOWHERE! This server has only one channel.` což ukazuje uspěšné přejmenování uživatele a následné připojení
k jinem kanálu.







## Dodatečná funkcionalita

... (informace o dodatečné funkcionlitě)
