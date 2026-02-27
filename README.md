gcc snake.c snake.o -o snake -Wall
./snake map.txt


Ce projet implémente un Snake autonome en C avec une vision restreinte et une IA simple pour décider de sa direction. Le projet utilise un fichier objet fourni (snake.o) pour gérer l’affichage, et un code source pour la logique du serpent.

📂 Structure du projet
├── player3.c           # Code de décision du serpent
├── player_restreint.c  # Code de décision du serpent avec vision restreinte
├── snake.h             # Définition de la structure Snake et de la vision
├── snake.o             # Code objet fourni pour l'affichage
├── level 20*10         # Carte utilisée pour les simulations
├── README.md           # Ce fichier
└── Makefile            # (optionnel) Pour compiler facilement
