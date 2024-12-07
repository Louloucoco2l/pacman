#include "global.h"
#include "generation_fusion_aleatoire.h"
#include "generation_division_recursive.h"





typedef struct  {
    BITMAP *page; //fond sur lequel on dessine initialement le laby. fait qu une fois car pas vocation a changer
    BITMAP *buffer; //support sur lequel on copie les BITMAPS avant de les mettre à l ecran
    BITMAP *PLAYER[2];// pour version ouvert et fermé
    BITMAP *FANTOM[4][2];  //3 types de FANTOM existants et 2 etats differents selon coup d horloge
    BITMAP *food;  //pommes?
    BITMAP* test;  //pour afficher matrice obtenue avec getpixel
}t_image;// structure unique pour toutes les images
typedef struct{
    int dx;
    int dy;
    bool alive;
    int points;
    int temps_survie;
    bool visuel; //alterner entre les deux visuels pour creer mouvement
}t_player;// structure unique du player
typedef struct {
    int type;
    int dx;
    int dy;
    bool alive;
    bool vulnerable; // true si player a mange un objet qui lui permet de desactiver les phantomes
    int direction;
    bool visuel; //alterner entre les deux visuels pour creer mouvement
}t_fantom;


void initialisation_allegro() {
    allegro_init(); // appel obligatoire (var.globales, recup. infos système ...)
    install_keyboard(); //pour utiliser le clavier
    //install_mouse(); //pour utiliser la souris
    install_timer(); //pour utiliser le timer
    install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
    //pour choisir la profondeur de couleurs (8,16,24 ou 32 bits)
    set_color_depth(desktop_color_depth()); //ici : identique à celle du bureau
    //sélection du mode graphique
    // avec choix d'un driver+mode+résolution de l'écran
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT) != 0) {
        // message et sortie de prog. si échec
        allegro_message("prb gfx mode");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    show_mouse(screen);
}
void load_bitmap_or_exit(BITMAP **bitmap, const char *filename) {
    *bitmap = load_bitmap(filename, NULL);
    if (*bitmap == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(EXIT_FAILURE);
    }
}
void chargement_bitmaps(t_image *mes_images) {
    mes_images->page = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    load_bitmap_or_exit(&mes_images->page, "pictures/percorso.2.bmp");

    mes_images->buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    mes_images->test = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);

    load_bitmap_or_exit(&mes_images->FANTOM[0][0], "pictures/Fantom1.1.bmp");
    load_bitmap_or_exit(&mes_images->FANTOM[0][1], "pictures/Fantom1.2.bmp");
    load_bitmap_or_exit(&mes_images->FANTOM[1][0], "pictures/Fantom2.1.bmp");
    load_bitmap_or_exit(&mes_images->FANTOM[1][1], "pictures/Fantom2.2.bmp");
    load_bitmap_or_exit(&mes_images->FANTOM[2][0], "pictures/Fantom3.1.bmp");
    load_bitmap_or_exit(&mes_images->FANTOM[2][1], "pictures/Fantom3.2.bmp");
    load_bitmap_or_exit(&mes_images->FANTOM[3][0], "pictures/Fantom4.1.bmp");
    load_bitmap_or_exit(&mes_images->FANTOM[3][1], "pictures/Fantom4.2.bmp");

    load_bitmap_or_exit(&mes_images->PLAYER[0], "pictures/vita1.bmp");
    load_bitmap_or_exit(&mes_images->PLAYER[1], "pictures/vita2.bmp");
}

void transpose(int maze[WIDTH][HEIGHT], int matrice[SCREEN_WIDTH][SCREEN_HEIGHT]) {


    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if(maze[i][j] == 1){
                for (int k = 0; k < CELL_SIZE; ++k) {
                    for (int l = 0; l < CELL_SIZE; ++l) {
                        matrice[i * CELL_SIZE + k][j * CELL_SIZE + l] = maze[i][j];
                    }
                }
            }
        }
    }

}
void detection(t_image *mes_images, int matrice[SCREEN_WIDTH][SCREEN_HEIGHT]) {

    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            //on parcourt page (charge depuis percorso) pour remplir la matrice avec les murs
            if (getpixel(mes_images->page, j, i) == makecol(0, 0, 0)) {
                matrice[j][i] = 0;//libre
            } else {
                matrice[j][i] = 1;//murs
            }
        }
    }

    // remplir BITMAP test a partir de matrice pour verif si on a bien detecte les murs et les cases vides
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (matrice[j][i] == 1) {// si mur
                putpixel(mes_images->test, j, i, makecol(255, 0, 0));
            } else {
                putpixel(mes_images->test, j, i, makecol(0, 0, 0));
            }
        }
    }

}

void initialisation_persos(t_player *player, t_fantom fantoms[4]) {
    player->dx = 40;
    player->dy = 40;
    player->alive = 1;
    player->points = 0;
    player->temps_survie = 0;

    for (int i = 0; i < 4-1; i++) {//seulement 3 fantomes
        fantoms[i].dx = 415;
        fantoms[i].dy = 250+40*i;
        fantoms[i].alive = 1; // modifiable a posteriori pour chacun selon niveau choisi
        fantoms[i].vulnerable = 0;
        fantoms[i].direction = 0;
        fantoms[i].visuel = 0; //alterner entre les deux visuels pour creer mouvement
        fantoms[i].type = 0;
    }

    fantoms[3].alive = 0;// sinon ca bug
}


void deplacement_player(t_player *player, int matrice[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    int new_dx = player->dx;
    int new_dy = player->dy;

    if (key[KEY_LEFT] && player->dx - MOVE_SIZE > 0) {
        new_dx = player->dx - MOVE_SIZE;
    }
    if (key[KEY_RIGHT] && player->dx + PLAYER_SIZE + MOVE_SIZE < SCREEN_WIDTH) {
        new_dx = player->dx + MOVE_SIZE;
    }
    if (key[KEY_UP] && player->dy - MOVE_SIZE > 0) {
        new_dy = player->dy - MOVE_SIZE;
    }
    if (key[KEY_DOWN] && player->dy + PLAYER_SIZE + MOVE_SIZE < SCREEN_HEIGHT) {
        new_dy = player->dy + MOVE_SIZE;
    }

    // Va rentrer dans un mur?
    bool can_move = true;
    for (int i = new_dx; i < new_dx + PLAYER_SIZE; i++) {
        for (int j = new_dy; j < new_dy + PLAYER_SIZE; j++) {
            if (matrice[i][j] == 1) {
                can_move = false;
                break;
            }
        }
        if (!can_move) break;
    }

    // pas de collision donc maj position
    if (can_move) {
        player->dx = new_dx;
        player->dy = new_dy;
    }
}
void mettre_a_jour_matrice(t_fantom fantoms[4], int matrice[SCREEN_WIDTH][SCREEN_HEIGHT], bool reset) {
    // Efface ou met à jour la position des fantômes dans la matrice
    for (int i = 0; i < 4; i++) {
        if (fantoms[i].alive) {
            for (int x = fantoms[i].dx; x < fantoms[i].dx + PLAYER_SIZE; x++) {
                for (int y = fantoms[i].dy; y < fantoms[i].dy + PLAYER_SIZE; y++) {
                    matrice[x][y] = reset ? 0 : 2;
                }
            }
        }
    }
}

void deplacement_fantom_aleatoire(t_fantom *fantom, int matrice[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    int directions[4][2] = {{-MOVE_SIZE, 0}, {MOVE_SIZE, 0}, {0, -MOVE_SIZE}, {0, MOVE_SIZE}};
    int valid_directions[4];
    int valid_count = 0;

    // Efface l'ancienne position du fantôme dans la matrice
    for (int i = fantom->dx; i < fantom->dx + PLAYER_SIZE; i++) {
        for (int j = fantom->dy; j < fantom->dy + PLAYER_SIZE; j++) {
            matrice[i][j] = 0;
        }
    }

    // Vérifie quelles directions sont valides
    for (int d = 0; d < 4; d++) {
        int new_dx = fantom->dx + directions[d][0];
        int new_dy = fantom->dy + directions[d][1];

        // Vérifie qu'il ne rentre pas dans un mur ou un autre fantôme
        bool can_move = true;
        for (int i = new_dx; i < new_dx + PLAYER_SIZE; i++) {
            for (int j = new_dy; j < new_dy + PLAYER_SIZE; j++) {
                if (matrice[i][j] != 0) { // 0 = libre, 1 ou 2 = obstacle
                    can_move = false;
                    break;
                }
            }
            if (!can_move) break;
        }

        if (can_move) {
            valid_directions[valid_count++] = d;
        }
    }

    // Si des directions valides existent, choisir une direction aléatoire
    if (valid_count > 0) {
        int chosen_direction = valid_directions[rand() % valid_count];
        fantom->dx += directions[chosen_direction][0];
        fantom->dy += directions[chosen_direction][1];
    }

    // Réinscrit la nouvelle position du fantôme dans la matrice
    for (int i = fantom->dx; i < fantom->dx + PLAYER_SIZE; i++) {
        for (int j = fantom->dy; j < fantom->dy + PLAYER_SIZE; j++) {
            matrice[i][j] = 2;
        }
    }
}
void deplacement_fantom_vers_player(t_fantom *fantom, t_player *player, int matrice[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    int directions[4][2] = {{-MOVE_SIZE, 0}, {MOVE_SIZE, 0}, {0, -MOVE_SIZE}, {0, MOVE_SIZE}};
    int best_dx = fantom->dx, best_dy = fantom->dy;
    int min_distance = INT_MAX;

    // Efface l'ancienne position du fantôme dans la matrice
    for (int i = fantom->dx; i < fantom->dx + PLAYER_SIZE; i++) {
        for (int j = fantom->dy; j < fantom->dy + PLAYER_SIZE; j++) {
            matrice[i][j] = 0;
        }
    }

    // Tente chaque direction pour trouver celle qui se rapproche le plus du joueur
    for (int d = 0; d < 4; d++) {
        int new_dx = fantom->dx + directions[d][0];
        int new_dy = fantom->dy + directions[d][1];

        // Vérifie qu'il ne rentre pas dans un mur ou un autre fantôme
        bool can_move = true;
        for (int i = new_dx; i < new_dx + PLAYER_SIZE; i++) {
            for (int j = new_dy; j < new_dy + PLAYER_SIZE; j++) {
                if (matrice[i][j] != 0) { // 0 = couloir libre, tout autre valeur (1 ou 2) est infranchissable
                    can_move = false;
                    break;
                }
            }
            if (!can_move) break;
        }

        if (can_move) {
            // Calcul de la distance au joueur
            int distance = abs(player->dx - new_dx) + abs(player->dy - new_dy);
            if (distance < min_distance) {
                min_distance = distance;
                best_dx = new_dx;
                best_dy = new_dy;
            }
        }
    }

    // Si un mouvement valide est trouvé, on met à jour la position
    if (min_distance < INT_MAX) {
        // Met à jour les coordonnées du fantôme
        fantom->dx = best_dx;
        fantom->dy = best_dy;

        // Marque la nouvelle position du fantôme dans la matrice
        for (int i = best_dx; i < best_dx + PLAYER_SIZE; i++) {
            for (int j = best_dy; j < best_dy + PLAYER_SIZE; j++) {
                matrice[i][j] = 2;
            }
        }
    } else {
        // Réinscrit l'ancienne position si aucun mouvement valide n'est possible
        for (int i = fantom->dx; i < fantom->dx + PLAYER_SIZE; i++) {
            for (int j = fantom->dy; j < fantom->dy + PLAYER_SIZE; j++) {
                matrice[i][j] = 2;
            }
        }
    }
}
void deplacement_fantoms(t_fantom fantoms[4], t_player *player, int matrice[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    // Réinitialise les positions des fantômes dans la matrice
    mettre_a_jour_matrice(fantoms, matrice, true);

    for (int i = 0; i < 4; i++) {
        if (fantoms[i].alive) {
            if (fantoms[i].type == 0) {
                deplacement_fantom_aleatoire(&fantoms[i], matrice);
            } else if (fantoms[i].type == 1) {
                deplacement_fantom_vers_player(&fantoms[i], player, matrice);
            }
        }
    }

    // Met à jour les positions des fantômes dans la matrice
    mettre_a_jour_matrice(fantoms, matrice, false);
}


void contact_fantoms(t_player *player, t_fantom fantoms[4]) {


    for (int i = 0; i < 4; i++) {
        if (fantoms[i].alive &&
            player->dx < fantoms[i].dx + PLAYER_SIZE &&
            player->dx + PLAYER_SIZE > fantoms[i].dx &&
            player->dy < fantoms[i].dy + PLAYER_SIZE &&
            player->dy + PLAYER_SIZE > fantoms[i].dy) {
            printf("contact ", player->alive, "\n");
            if (fantoms[i].vulnerable) {
                printf("case 1\n");
                fantoms[i].alive = 0;
            } else {
                printf("case 2");
                player->alive = 0;
                printf("Player alive: %d\n", player->alive);
            }
        }
    }
}


void affichageInitial(t_image *mes_images, int matrice[SCREEN_WIDTH][SCREEN_HEIGHT]) {

    vsync();
    blit(mes_images->page, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

}
void affichageMaj(t_image *mes_images, int matrice[SCREEN_WIDTH][SCREEN_HEIGHT], t_player *player, t_fantom fantoms[4]) {

    clear_bitmap(mes_images->buffer);


    blit(mes_images->page,mes_images->buffer, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    //blit(mes_images->test,mes_images->buffer, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    if(player->alive){
        draw_sprite(mes_images->buffer,mes_images->PLAYER[0],player->dx ,player->dy );
    }
    //blit(mes_images->PLAYER[0],mes_images->buffer, 0, 0, player->dx, player->dy, SCREEN_WIDTH, SCREEN_HEIGHT);
    //draw_sprite(mes_images->buffer,mes_images->PLAYER[0],player->dx ,player->dy );
    //blit(mes_images->FANTOM[0][0],mes_images->buffer, 0, 0, fantoms[0].dx, fantoms[0].dy, SCREEN_WIDTH, SCREEN_HEIGHT);
    for (int i  = 0; i < 4; i++) {
        if (fantoms[i].alive){
            draw_sprite(mes_images->buffer,mes_images->FANTOM[i][fantoms[i].visuel],fantoms[i].dx,fantoms[i].dy);
        }
    }

    vsync();
    blit(mes_images->buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


}


void destruction_bitmaps(t_image *mes_images) {
    destroy_bitmap(mes_images->page);
    destroy_bitmap(mes_images->buffer);
    destroy_bitmap(mes_images->PLAYER[0]);
    destroy_bitmap(mes_images->PLAYER[1]);
    destroy_bitmap(mes_images->FANTOM[0][0]);
    destroy_bitmap(mes_images->FANTOM[0][1]);
    destroy_bitmap(mes_images->FANTOM[1][0]);
    destroy_bitmap(mes_images->FANTOM[1][1]);
    destroy_bitmap(mes_images->FANTOM[2][0]);
    destroy_bitmap(mes_images->FANTOM[2][1]);
    destroy_bitmap(mes_images->food);
}

int main() {
    t_image mes_images;
    t_player player;
    t_fantom fantoms[4];
    srand(time(NULL));
    int matrice[SCREEN_WIDTH][SCREEN_HEIGHT];
    int maze[WIDTH][HEIGHT];
    long time = clock();

    initialisation_allegro();
    chargement_bitmaps(&mes_images);
    initialisation_persos(&player, fantoms);


    /*labSansIlots(maze);//(fusion aléatoire de chemins)
    transpose(maze, matrice);
    affichageInitial(&mes_images, matrice);*/



    /*
    initializeMazeWithDFS(maze);//(division récursive)
    transpose(maze, matrice);
    affichageInitial(&mes_images, matrice);
     */
    affichageInitial(&mes_images, matrice);
    detection(&mes_images, matrice);

    while (!key[KEY_ESC]) {
        deplacement_player(&player, matrice);
        deplacement_fantoms(fantoms, &player, matrice);
        contact_fantoms(&player, fantoms);
        affichageMaj(&mes_images, matrice, &player, fantoms);
        time=clock()-time;
        printf("%ld  ", time);
    }

    destruction_bitmaps(&mes_images);


    return 0;
}
END_OF_MAIN();
