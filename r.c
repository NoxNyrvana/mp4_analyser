#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

void print_menu(WINDOW *menu_win, int highlight, const char *choices[], int n_choices);
void print_button(WINDOW *button_win, const char *label);
void execute_program(const char *program_name, const char *file_name, const char *mdp);

int main(int argc, char *argv[]) {
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);

    int startx = 0;
    int starty = 0;
    int width = 150;
    int height = 20;
    int highlight = 1;
    int choice = 0;
    int c;

    const char *choices[] = {
        "cryptage",
        "meta donnée",
        "analyse",
    };
    int n_choices = sizeof(choices) / sizeof(char *);

    WINDOW *menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);
    mvprintw(0, 0, "Utilisez les flèches pour naviguer et Entrée pour sélectionner");
    refresh();
    print_menu(menu_win, highlight, choices, n_choices);

    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                highlight = (highlight == 1) ? n_choices : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == n_choices) ? 1 : highlight + 1;
                break;
            case 10:
                choice = highlight;
                break;
            default:
                break;
        }
        print_menu(menu_win, highlight, choices, n_choices);
        if (choice != 0)
            break;
    }

    WINDOW *button_win = newwin(3, 30, starty + height + 1, startx + (width - 30) / 2);
    print_button(button_win, "Exécuter");

    while (1) {
        c = wgetch(button_win);
        if (c == 10) {
            break;
        }
    }

    clear();
    refresh();
    char file_name[256] = {0};
    char mdp[256] = {0};
    int index = 0;

    mvprintw(0, 0, "Entrez le nom du fichier (Appuyez sur Entrée pour valider) :");
    refresh();

    while ((c = getch()) != '\n') {
        if (c == KEY_BACKSPACE || c == 127 || c == '\b') {
            if (index > 0) {
                index--;
                file_name[index] = '\0';
                move(1, index);
                delch();
                refresh();
            }
        } else if (index < 255 && c >= 32 && c <= 126) {
            file_name[index++] = c;
            addch(c);
            refresh();
        }
    }

    file_name[index] = '\0';
    clear();

    switch (choice) {
        case 1:
            mvprintw(0, 0, "Entrez un mot de passe pour crypter (puis appuyez sur Entrée) : ");
            refresh();
            mvprintw(1, 0, "Entrez le mot de passe pour décrypter (puis appuyez sur Entrée) : ");
            refresh();
            execute_program("./cryptage", file_name, mdp);
            break;
        case 2:
            execute_program("./meta", file_name, mdp);
            break;
        case 3:
            execute_program("./analyse", file_name, mdp);
            break;
        default:
            mvprintw(3, 0, "Programme non défini pour cette option.");
            break;
    }

    mvprintw(0, 0, "Nom de fichier saisi : %s", file_name);
    refresh();

    WINDOW *quit_button_win = newwin(3, 30, LINES - 4, (COLS - 30) / 2);
    print_button(quit_button_win, "Quitter");

    while (1) {
        c = wgetch(quit_button_win);
        if (c == 10) {
            break;
        }
    }

    endwin();
    return 0;
}

void print_menu(WINDOW *menu_win, int highlight, const char *choices[], int n_choices) {
    int x = 2, y = 2;
    box(menu_win, 0, 0);
    for (int i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } else {
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        }
        ++y;
    }
    wrefresh(menu_win);
}

void print_button(WINDOW *button_win, const char *label) {
    box(button_win, 0, 0);
    mvwprintw(button_win, 1, (30 - strlen(label)) / 2, "%s", label);
    wrefresh(button_win);
}

void execute_program(const char *program_name, const char *file_name, const char *mdp) {
    char command[256];
    snprintf(command, sizeof(command), "%s %s %s > output.txt", program_name, file_name, mdp);
    system(command);

    FILE *file = fopen("output.txt", "r");
    if (file) {
        char line[256];
        int y = 3;
        while (fgets(line, sizeof(line), file)) {
            mvprintw(y++, 0, "%s", line);
        }
        fclose(file);
    } else {
        mvprintw(3, 0, "Erreur lors de l'exécution du programme.");
    }
    refresh();
}
