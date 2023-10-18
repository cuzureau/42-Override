## Comportement
- Je dispose des droits `level00` que je peux vérifier en tapant `whoami`.
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level01 users   7280 Sep 10  2016 level00*`.
- En exécutant le binaire (`./level00`), on constate : 
    ```shell
    ***********************************
    * 	     -Level00 -		  *
    ***********************************
    Password:123

    Invalid Password!
    ```

→ On en déduit que le programme attend un mot de passe. Reste à connaître lequel.

## Analyse
En analysant le binaire (`gdb ./level00`), on observe :
  - La fonction appelle `scanf()` puis compare son résultat avec `0x149c` (`5276` en hexadécimal) :
    ```shell
    (gdb) disas main
    ...
    0x080484d3 <+63>:	lea    0x1c(%esp),%edx
    0x080484d7 <+67>:	mov    %edx,0x4(%esp)
    0x080484db <+71>:	mov    %eax,(%esp)
    0x080484de <+74>:	call   0x80483d0 <__isoc99_scanf@plt>
    ...
    ```
  - Si le paramètre est égal à `5276`, alors un appel à `execv()` sera fait. Cette fonction permet d'exécuter une commande.
    ```shell
    (gdb) disas main
    ...
    0x080484fa <+102>:	movl   $0x8048649,(%esp)
    0x08048501 <+109>:	call   0x80483a0 <system@plt>
    ...
    ```
  - Il suffit d'examiner l'adresse du paramètre envoyé à la fonction `execv()` (située une ligne au-dessus) pour constater qu'on exécute `/bin/sh`. C'est une technique courante pour lancer un shell interactif. Bingo Mango !
    ```shell
    (gdb) x/s 0x8048649
    0x80c5348:	 "/bin/sh"
    ```

## Résolution
- On exécute donc le binaire avec le bon argument (`5276`) : 
    ```shell
    ***********************************
    * 	     -Level00 -		  *
    ***********************************
    Password:5276

    Authenticated!
    ``` 
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level01`.
- Il ne reste plus qu'à :
  - récupérer le mot de passe (`cat /home/users/level01/.pass`)
  - quitter le shell (`exit`)
  - et passer au niveau 1 (`su level01`) en renseignant ce mot de passe.
