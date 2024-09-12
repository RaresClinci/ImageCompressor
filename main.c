/*CLINCI Rares-Mihail - 315CC*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NR_SONS 4
#define TYPE_LEN 3

// structura pixel
typedef struct pix {
    unsigned char R, G, B;
} Pixel;

// structura imagine ppm
typedef struct ppm {
    char tip[TYPE_LEN];
    int w, h;
    unsigned int max;
    Pixel **pix;
} Image;

// structura arbore cuaternar
typedef struct nod {
    Pixel color;
    unsigned char type;
    struct nod *child[NR_SONS];
} Node, *Tree;

// structura coada BFS
typedef struct qnode {
    Tree elem;
    struct qnode *next;
} QCell, *Queue;

typedef struct q {
    Queue rear, front;
} Coada;

// initializare coada
Coada initQueue() {
    Coada que;
    que.front = que.rear = NULL;
    return que;
}

// este coada goala?
int EmptyQueue(Coada que) {
    return (que.front == NULL);
}

// adaugare la coada
Coada enqueue(Coada que, Tree elem) {
    Queue aux;
    aux = (Queue)malloc(sizeof(QCell));
    aux->elem = elem;
    if (EmptyQueue(que)) {
        // primul element adaugat
        que.front = que.rear = aux;
    } else {
        // element normal
        que.rear->next = aux;
        que.rear = aux;
    }
    return que;
}

// eliminare din coada
Tree dequeue(Coada *que) {
    Queue aux;
    Tree elem;
    if (!EmptyQueue(*que)) {
        elem = que->front->elem;
        if (que->front == que->rear) {
            // exista doar un element
            free(que->front);
            que->front = que->rear = NULL;
        } else {
            // element normal
            aux = que->front;
            que->front = que->front->next;
            free(aux);
        }
        return elem;
    }
    // nu mai exista elemente
    return 0;
}

// initializare arbore
Tree InitTree() {
    Tree aux;
    int i;
    aux = (Tree)malloc(sizeof(Node));
    // initializare fii
    for (i = 0; i < NR_SONS; i++) {
        aux->child[i] = NULL;
    }
    return aux;
}

// este frunza?
int isLeaf(Tree quad) {
    return (quad->child[0] == NULL);
}

// inaltimea arborelui
int height(Tree quad) {
    int i, h, max = 0;
    if (isLeaf(quad)) {
        return 1;
    } else {
        // determinare subarbore cu lungime maxima
        for (i = 0; i < NR_SONS; i++) {
            h = height(quad->child[i]);
            if (h > max) {
                max = h;
            }
        }
        return max + 1;
    }
}

// numarul de frunze
int leaf_number(Tree quad) {
    int i, nr = 0;
    if (isLeaf(quad)) {
        // s-a ajuns la o frunza
        return 1;
    } else {
        // numarul frunzelor subarborilor
        for (i = 0; i < NR_SONS; i++) {
            nr += leaf_number(quad->child[i]);
        }
        return nr;
    }
}

// nivelul cu cea mai apropiata frunza
int closest_leaf(Tree quad) {
    int i, h, min;
    if (isLeaf(quad)) {
        //radacina (sub)arborelui este frunza
        return 1;
    } else {
        //minimul inaltimii fiilor
        min = closest_leaf(quad->child[0]);
        for (i = 1; i < NR_SONS; i++) {
            h = closest_leaf(quad->child[i]);
            if (h < min) {
                min = h;
            }
        }
        return min + 1;
    }
}

// functie eliberare arbore
Tree free_tree(Tree quad) {
    int i;
    if (!isLeaf(quad)) {
        for (i = 0; i < NR_SONS; i++) {
            free_tree(quad->child[i]);
        }
    }
    free(quad);
    return quad;
}

// functie eliberare imagine
Image free_image(Image img) {
    int i;
    for (i = 0; i < img.h; i++) {
        free(img.pix[i]);
    }
    free(img.pix);
    return img;
}

// functie citire
Image read_ppm(FILE *in) {
    Image img;
    int i, j;
    // citire informatii antet
    fscanf(in, "%2s", img.tip);
    fseek(in, 1, SEEK_CUR);
    fscanf(in, "%d", &(img.h));
    fseek(in, 1, SEEK_CUR);
    fscanf(in, "%d", &(img.w));
    fseek(in, 1, SEEK_CUR);
    fscanf(in, "%d", &(img.max));
    fseek(in, 1, SEEK_CUR);
    // citire imagine propriu-zisa
    img.pix = (Pixel **)malloc(img.h * sizeof(Pixel *));
    for (i = 0; i < img.h; i++) {
        img.pix[i] = (Pixel *)malloc(img.w * sizeof(Pixel));
        for (j = 0; j < img.w; j++) {
            fread(&(img.pix[i][j]), sizeof(Pixel), 1, in);
        }
    }
    return img;
}

// calculare medie aritmetica per culoare
Pixel avarege_RGB(Image img, int x, int y, int size) {
    Pixel col;
    int i, j;
    unsigned long long red = 0, blue = 0, green = 0;
    // suma valorilor pixelilor per culoare
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {
            red += img.pix[i][j].R;
            blue += img.pix[i][j].B;
            green += img.pix[i][j].G;
        }
    }
    // media propriu-zisa
    col.R = (unsigned char)(red / (size * size));
    col.B = (unsigned char)(blue / (size * size));
    col.G = (unsigned char)(green / (size * size));
    return col;
}

// calculare scor al similaritatii
int mean(Image img, int x, int y, int size, Pixel col) {
    int i, j;
    unsigned long long sum = 0;
    // suma patratelor diferentelor
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {
            sum += (col.R - img.pix[i][j].R) * (col.R - img.pix[i][j].R);
            sum += (col.G - img.pix[i][j].G) * (col.G - img.pix[i][j].G);
            sum += (col.B - img.pix[i][j].B) * (col.B - img.pix[i][j].B);
        }
    }
    // calculare scor propriu-zis
    return sum / (3 * size * size);
}

// formare arbore de compresie
Tree compression_tree(Image img, int x, int y, int size, int tol) {
    Pixel col;
    Tree comp = InitTree();
    if (size == 1) {
        // s-a ajuns la un pixel
        comp->color = img.pix[x][y];
        comp->type = 1;
    } else {
        col = avarege_RGB(img, x, y, size);
        if (mean(img, x, y, size, col) <= tol) {
            // portiunea poate deveni frunza
            comp->color = col;
            comp->type = 1;
        } else {
            // portiunea trebuie divizata
            comp->child[0] = compression_tree(img, x, y, size / 2, tol);
            comp->child[1] = compression_tree(img, x, y + size / 2,
                                                size / 2, tol);
            comp->child[2] = compression_tree(img, x + size / 2, y + size / 2,
                                                size / 2, tol);
            comp->child[3] = compression_tree(img, x + size / 2, y,
                                                size / 2, tol);
            comp->type = 0;
        }
    }
    return comp;
}

// apel functie de compresie
Tree compress(Image img, int tol) {
    return compression_tree(img, 0, 0, img.h, tol);
}

// printare prin parcurgere BFS
void BFS_print(Tree comp, FILE *out) {
    Coada visited = initQueue();
    Tree current;
    int i;
    current = comp;
    // afisare root
    fwrite(&(current->type), sizeof(char), 1, out);
    if (current->type == 1) {
        fwrite(&(current->color), sizeof(Pixel), 1, out);
    }
    while (current != NULL) {
        // afisare si adaugare la coada a fiilor
        if (!isLeaf(current)) {
            for (i = 0; i < NR_SONS; i++) {
                fwrite(&(current->child[i]->type), sizeof(char), 1, out);
                if (current->child[i]->type == 1) {
                    fwrite(&(current->child[i]->color), sizeof(Pixel), 1, out);
                } else {
                    visited = enqueue(visited, current->child[i]);
                }
            }
        }

        // extragere element nou
        current = dequeue(&visited);
    }
}

// citire fisier comprimat
Tree BFS_read(FILE *in) {
    Tree comp = InitTree(), current;
    Coada needson = initQueue();
    int i;

    // citim radacina
    fread(&(comp->type), sizeof(char), 1, in);
    if (comp->type == 0) {
        current = comp;
        while (current != NULL) {
            for (i = 0; i < NR_SONS; i++) {
                current->child[i] = InitTree();
                // citim tipul fiilor si culoarea daca este nod terminal
                fread(&(current->child[i]->type), sizeof(char), 1, in);
                if (current->child[i]->type == 0) {
                    needson = enqueue(needson, current->child[i]);
                } else {
                    fread(&(current->child[i]->color), sizeof(Pixel), 1, in);
                }
            }
            current = dequeue(&needson);
        }
    } else {
        // imaginea are o singura culoare
        fread(&(comp->color), sizeof(Pixel), 1, in);
    }

    return comp;
}

// functie umplere zona cu o culoare
void fill_area(Image *img, int x, int y, int size, Pixel color) {
    int i, j;
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {
            img->pix[i][j] = color;
        }
    }
}

// functie restaurare imagine din arbore
void reconstruct_image(Image *img, Tree comp, int lenght, int x, int y) {
    // imaginea propriu-zisa
    if (comp->type == 0) {
        // portiunea nu are o singura culoare
        reconstruct_image(img, comp->child[0], lenght / 2, x, y);
        reconstruct_image(img, comp->child[1], lenght / 2, x, y + lenght / 2);
        reconstruct_image(img, comp->child[2], lenght / 2, x + lenght / 2, 
                                    y + lenght / 2);
        reconstruct_image(img, comp->child[3], lenght / 2, x + lenght / 2, y);
    } else {
        // potiunea are o singura culoare
        fill_area(img, x, y, lenght, comp->color);
    }
}

// subprogram reconstruire imagine
Image reconstruct(Tree comp, int lenght) {
    Image img;
    int i;

    // antetul imagini
    strcpy(img.tip, "P6");
    img.h = img.w = lenght;
    img.max = 255;

    // alocare imagine
    img.pix = (Pixel **)malloc(img.h * sizeof(Pixel *));
    for (i = 0; i < img.w; i++) {
        img.pix[i] = (Pixel *)malloc(img.w * sizeof(Pixel));
    }

    // reconstructia propriu-zisa
    reconstruct_image(&img, comp, img.h, 0, 0);

    return img;
}

// functie printare ppm
void print_ppm(Image img, FILE *out) {
    int i, j;

    // printare antet
    fprintf(out, "%s\n%d %d\n%d\n", img.tip, img.h, img.w, img.max);

    // printare imagine propriu-zisa
    for (i = 0; i < img.h; i++) {
        for (j = 0; j < img.w; j++) {
            fwrite(&(img.pix[i][j]), sizeof(Pixel), 1, out);
        }
    }
}

// cerinta 1
void task1(int tol, char *in_file, char *out_file) {
    Tree comp;
    int div;
    FILE *in, *out;
    Image img;

    // citire
    in = fopen(in_file, "rb");
    img = read_ppm(in);
    fclose(in);

    // compresie a imaginii
    comp = compress(img, tol);

    out = fopen(out_file, "w");
    // afisare inaltime
    fprintf(out, "%d\n", height(comp));
    // afisare numar blocuri
    fprintf(out, "%d\n", leaf_number(comp));
    // afisare cel mai mare bloc
    div = closest_leaf(comp) - 1;
    fprintf(out, "%d\n", img.h / (1 << div));
    fclose(out);

    // eliberare memorie
    img = free_image(img);
    comp = free_tree(comp);
}

void task2(int tol, char *in_file, char *out_file) {
    FILE *out, *in;
    Tree comp;
    Image img;

    // citire
    in = fopen(in_file, "rb");
    img = read_ppm(in);
    fclose(in);

    // compresie a imagini
    comp = compress(img, tol);

    // afisare
    out = fopen(out_file, "wb");
    fwrite(&(img.h), sizeof(int), 1, out);
    BFS_print(comp, out);
    fclose(out);

    // eliberare memorie
    img = free_image(img);
    comp = free_tree(comp);
}

void task3(char *in_file, char *out_file) {
    FILE *in, *out;
    Tree comp;
    Image img;
    int size;

    // citire
    in = fopen(in_file, "rb");
    fread(&size, sizeof(int), 1, in);
    comp = BFS_read(in);
    fclose(in);

    // creare imagine
    img = reconstruct(comp, size);

    // afisare imagine
    out = fopen(out_file, "wb");
    print_ppm(img, out);
    fclose(out);

    // eliberare memorie
    img = free_image(img);
    comp = free_tree(comp);
}

int main(int argc, char **argv) {
    int tol;
    // procesare argumente in linia de comanda
    if (strcmp(argv[1], "-c1") == 0) {
        // cerinta 1
        sscanf(argv[2], "%d", &tol);
        task1(tol, argv[3], argv[4]);
    } else if (strcmp(argv[1], "-c2") == 0) {
        // cerinta 2
        sscanf(argv[2], "%d", &tol);
        task2(tol, argv[3], argv[4]);
    } else if (strcmp(argv[1], "-d") == 0) {
        // cerinta 3
        task3(argv[2], argv[3]);
    }
    return 0;
}