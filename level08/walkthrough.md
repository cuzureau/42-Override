Le programme nécessite un argument (av[1]) qui doit être un fichier. Ce fichier sera copié dans le dossier `/backups/`.
Pour exploiter cette fonctionnalité, nous allons créer un lien symbolique pointant vers le fichier `/home/users/level09/.pass` et le fournir au programme.

Voici les étapes à suivre :
- On modifie les permissions du dossier courant : `chmod 777 .`.
- On crée un lien symbolique vers le fichier .pass en exécutant la commande : `ln -s /home/users/level09/.pass pass`.
- On execute le binaire avec notre lien : `./level08 pass`
- Pour finir, on visualise le contenu du fichier copié dans /backups/ : `cat backups/pass`.
