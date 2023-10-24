## Comportement
- J'ai des droits `level03` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level04 users   7677 Sep 10  2016 level03*`
En exécutant le binaire (`./level03`), on observe:
- qu'il attend un mot de passe de notre part.
- qu'il ne segfault pas meme si le mot de passe est tres long.
    ```shell
    ***********************************
    *		level03		**
    ***********************************
    Password:123

    Invalid Password
    ```


## Analyse
En analysant le binaire (`gdb ./level03`) on observe:
- Le programme semble complexe à première vue, mais est en réalité assez simple :
    - prend un entier comme entrée
    - le soustrait de 322424845
    - le resultat doit etre inferieur a 22, sinon le resultat prend une valeur aleatoire
    - utilise le résultat pour faire un XOR avec chaque caractère de la chaîne `Q}|u`sfg~sf{}|a3`
    - compare la chaîne résultante à "Congratulations!"
    - s'ils sont égaux, le programme exécute "/bin/sh"


## Résolution
- On pourrait comparer les binaires des deux strings mais il y a encore plus simple : 
    - le resultat de la soustraction doit etre inferieur a 22
    - si on suppose que on s'arrete a 0 et on ne part pas dans les negatifs cela nous donne 22 solutions potentielles
    - on peut facilement bruteforce 22 potentielles solutions : de 0 a 21 (on va envoyer de 322424845 a 322424824)
    ```shell
    for i in {322424824..322424845}; do (python -c  "print $i"; cat -) | ./level03 ; done
    ```
- On tape `whoami` par chaque possibilite pour voir quand apparait `level04`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/users/level04/.pass`)
  - quitter le shell (`exit`)
  - et passer au level04 (`su level04`) en renseignant ce password.
  