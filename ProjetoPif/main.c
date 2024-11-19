#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

enum {
  KEY_ESC = 27,
  KEY_W = 119,
  KEY_S = 115,
  KEY_A = 97,
  KEY_D = 100
};

struct noparacobra {
  int Xno;
  int Yno;
  struct noparacobra *next;
};

struct ranking {
  int score;
  struct ranking *next;
};

void printembaixo(int placar, int recorde, int tempo) {
  screenSetColor(LIGHTCYAN, DARKGRAY); 
  int offsetX = MAXX + 5;  
  int offsetY = MINY + 3; 

  screenGotoxy(offsetX, offsetY);
  printf("| Placar : %d", placar);

  screenGotoxy(offsetX, offsetY + 2);
  printf("| Recorde : %d", recorde);

  screenGotoxy(offsetX, offsetY + 4);
  printf("| Tempo vivo : %d", tempo);
}

void addcobra(struct noparacobra **head, int x, int y) {
  if (*head == NULL) {
    *head = (struct noparacobra *)malloc(sizeof(struct noparacobra));
    (*head)->Xno = x;
    (*head)->Yno = y;
    (*head)->next = NULL;
  } 
  else {
    struct noparacobra *temp2 = *head;
    struct noparacobra *novo = (struct noparacobra *)malloc(sizeof(struct noparacobra));
    novo->Xno = x;
    novo->Yno = y;
    while (temp2->next != NULL) {
      temp2 = temp2->next;
    }
    novo->next = NULL;
    temp2->next = novo;
  }
}

void printcobra(struct noparacobra *head) {
  struct noparacobra *temp2 = head;
  while (temp2 != NULL) {
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(temp2->Xno, temp2->Yno);
    printf("0");
    temp2 = temp2->next;
  }
}

void atualizarcobra(struct noparacobra *head) {
  struct noparacobra *temp2 = head;
  while (temp2 != NULL) {
    screenGotoxy(temp2->Xno, temp2->Yno);
    printf(" ");
    temp2 = temp2->next;
  }
}

void freecobra(struct noparacobra **head) {
  struct noparacobra *temp2 = *head;
  while (temp2 != NULL) {
    struct noparacobra *temp1 = temp2;
    temp2 = temp2->next;
    free(temp1);
  }
}

void cobrandando(struct noparacobra **head, int x, int y) {
  struct noparacobra *newHead = (struct noparacobra *)malloc(sizeof(struct noparacobra));

  newHead->Xno = x;
  newHead->Yno = y;
  newHead->next = *head;
  *head = newHead;

  struct noparacobra *temp1 = *head;
  while (temp1->next->next != NULL) {
    temp1 = temp1->next;
  }

  free(temp1->next);
  temp1->next = NULL;
}

void printmaca(int x, int y) {
  screenSetColor(RED, DARKGRAY);
  screenGotoxy(x, y);
  printf("*");
}

int baternocorpo(struct noparacobra *head, int x, int y) {
  struct noparacobra *temp2 = head;
  while (temp2 != NULL) {
    if (temp2->Xno == x && temp2->Yno == y) {
      return 1;
    }
    temp2 = temp2->next;
  }
  return 0;
}

void randonmaca(int *x, int *y) {
  // Gera posições evitando as bordas
  *x = rand() % (MAXX - MINX - 2) + MINX + 1;
  *y = rand() % (MAXY - MINY - 2) + MINY + 1;
}

void rankingemordem(struct ranking **head, int score) {
  if (*head == NULL || score > (*head)->score) {
    struct ranking *novo = (struct ranking *)malloc(sizeof(struct ranking));
    novo->score = score;
    novo->next = *head;
    *head = novo;
  } else {
    rankingemordem(&((*head)->next), score);
  }
}

void addnoranking(struct ranking *head, FILE *in) {
  struct ranking *temp = head;
  while (temp != NULL) {
    int score = temp->score;
    if (fwrite(&score, sizeof(int), 1, in) != 1) {
      break;
    }
    temp = temp->next;
  }
}

void printranking(struct ranking *head) {
  struct ranking *temp = head;
  int cont = 0;
  while (temp != NULL && (cont < 3)) {
    printf("%d° colocado: %d pontos\n", cont + 1, temp->score);
    temp = temp->next;
    cont++;
  }
}

void freeranking(struct ranking **head) {
  struct ranking *temp2 = *head;
  while (temp2 != NULL) {
    struct ranking *temp1 = temp2;
    temp2 = temp2->next;
    free(temp1);
  }
}

void jogoLoop(struct noparacobra *head, int *dirX, int *dirY, int *placar, int *recorde, time_t tempoinicial, int PosMacaX, int PosMacaY) {
  int ch = 0;
  while (ch != KEY_ESC) {
    if (keyhit()) {
      ch = readch();
      switch (ch) {
        case KEY_W:
          if (*dirY != 1) {
            *dirX = 0;
            *dirY = -1;
          }
          break;
        case KEY_S:
          if (*dirY != -1) {
            *dirX = 0;
            *dirY = 1;
          }
          break;
        case KEY_A:
          if (*dirX != 1) {
            *dirX = -1;
            *dirY = 0;
          }
          break;
        case KEY_D:
          if (*dirX != -1) {
            *dirX = 1;
            *dirY = 0;
          }
          break;
      }
      screenUpdate();
    }
    if (timerTimeOver() == 1) {
      int newX = head->Xno + *dirX;
      int newY = head->Yno + *dirY;
      if (newX >= (MAXX - 1) || newX <= MINX || newY >= (MAXY - 1) || newY <= MINY) {
        break;
      }
      if (baternocorpo(head, newX, newY) == 1) {
        break;
      }
      if (newX == PosMacaX && newY == PosMacaY) {
        addcobra(&head, PosMacaX, PosMacaY);
        randonmaca(&PosMacaX, &PosMacaY);
        printmaca(PosMacaX, PosMacaY);
        (*placar)++;
      }
      atualizarcobra(head);
      cobrandando(&head, newX, newY);
      printcobra(head);
      screenUpdate();
      printembaixo(*placar, *recorde, (int)difftime(time(NULL), tempoinicial));
    }
  }
}

int main() {
  printf("          JOGO DA COBRINHA   \n\n\n              Carregando...\n");
  sleep(3);
  struct noparacobra *head = NULL;
  int placar = 0;
  int dirX = 1, dirY = 0;
  int recorde = 0;

  screenInit(1);
  keyboardInit();
  timerInit(80);

  addcobra(&head, 25, 7);

  srand((unsigned int)time(NULL));
  int PosMacaX, PosMacaY;
  randonmaca(&PosMacaX, &PosMacaY);
  printmaca(PosMacaX, PosMacaY);
  screenUpdate();

  FILE *in = fopen("rankfile.txt", "r");
  if (in != NULL) {
    fread(&recorde, sizeof(int), 1, in); 
    fclose(in);
  }

  time_t tempoinicial = time(NULL);
  jogoLoop(head, &dirX, &dirY, &placar, &recorde, tempoinicial, PosMacaX, PosMacaY);

  if (placar > recorde) {
    recorde = placar;
  }

  in = fopen("rankfile.txt", "w"); 
  if (in != NULL) {
    fwrite(&recorde, sizeof(int), 1, in);
    fclose(in);
  }

  freecobra(&head);
  keyboardDestroy();
  screenDestroy();
  timerDestroy();

  printf("\n\nPlacar final: %d\n", placar);
  printf("Recorde atual: %d\n", recorde);

  return 0;
}

