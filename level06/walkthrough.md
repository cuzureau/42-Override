## Comportement
- J'ai des droits `level05` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level07 users   7907 Sep 10  2016 level06*`
En exécutant le binaire (`./level06`), on observe:
- qu'il attend un login/serial number de notre part.
- rien ne se passe apparement sans le bon combo
- qu'il ne segfault pas meme si l'input est tres long.
    ```shell
    level06@OverRide:~$ ./level06 
    ***********************************
    *		level06		  *
    ***********************************
    -> Enter Login: hjk
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: 123
    ```


## Analyse
En analysant le binaire (`gdb ./level06`) on observe:
- Un hash est applique au login que l'on fournit pour creer une clef. Cette clef est comparee au serial number que nous fournissons egalement.
- En cas de succes, la fonction `auth()` renverra 0 et le programme ouvrira un shell via un appel a `system()`
- On va donc chercher a recuperer le hash afin de savoir quel serial number rentrer pour ouvrir le shell. 
- Un appel a `ptrace()` bloque gdb. 


## Résolution
- Nous allons tout d'abord coutourner `ptrace()` en le faisant retourner 0 au lieu de 1 peu importe la situation : 
    ```shell
    (gdb) catch syscall ptrace
    Catchpoint 1 (syscall 'ptrace' [26])
    (gdb) commands 1
    Type commands for breakpoint(s) 1, one per line.
    End with a line saying just "end".
    >set $eax=0
    >continue
    >end
    ```
- Ensuite on place un breakpoint pour imprimer le hash apres sa creation (il est egal a `0x005f236f` = `6234991`) :
```shell
(gdb) b *0x08048866
Breakpoint 2 at 0x8048866
(gdb) run
Starting program: /home/users/level06/level06 
***********************************
*		level06		  *
***********************************
-> Enter Login: cuzureau
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 0000

Catchpoint 1 (call to syscall ptrace), 0xf7fdb440 in __kernel_vsyscall
    ()

Catchpoint 1 (returned from syscall ptrace), 0xf7fdb440 in __kernel_vsyscall ()

Breakpoint 2, 0x08048866 in auth ()
(gdb) x/x $ebp-0x10
0xffffd5a8:	0x005f236f
```
- Il ne nous reste plus qu'a renseigner les bonnes informations : `cuzureau` en login et `6234991` en serial number : 
    ```shell
    level06@OverRide:~$ ./level06 
    ***********************************
    *		level06		  *
    ***********************************
    -> Enter Login: cuzureau
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: 6234991
    Authenticated!
    ```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level07`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/users/level07/.pass`)
  - quitter le shell (`exit`)
  - et passer au level07 (`su level07`) en renseignant ce password.
  