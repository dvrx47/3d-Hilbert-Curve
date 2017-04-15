/*******************************************************
*             AUTOR: MIKOŁAJ KOWALIK                   *
*           PROJEKT: KRZYWE HILBERTA 3D                *
*             ZADANIE NA PRACOWNIE NR 1                *
********************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define F(r,k,l) for(int r=k; r<l; ++r)



typedef struct punkt
{
    double x;
    double y;
    double z;
} punkt;

typedef struct node
{
    punkt * val;
    struct node *next;
}node;

typedef struct lista
{
    node *poczatek;
    node *koniec;
} lista;



node * createNode();
lista *createList();
bool isEmpty(lista * x);
void dodajVal(lista * l, double x, double y, double z);
void printList(lista * l);
lista * duplikuj(lista * l);
void przesun(int rzad, int dir, lista * l);
void dodajListy(lista * l1, lista * l2);
void obrotA(int rzad, lista *l);
void obrotB(int rzad, lista *l);
void obrotC(int rzad, lista *l);
void obrotFi(double fi, lista*l, int rzad);
void obrotPsi(double fi, lista*l, int rzad);
void rzutuj(FILE * out, double d,lista *l);
void przesunArg(lista *l, double x, double y, double z);
void skaluj(lista *l, int rzad, double u);

int main( int argc, char *argv[] )
{

    int n=0;

    int k = atoi( argv[1] );      // n
    int s = atoi( argv[2] );      // rozmiar obrazka
    double u = atof( argv[3] );   // dlugosc krawedzi szescianu w ktory wpisany jest krzywa
    double d = atof( argv[4] );   // odleglosc obserwatora
    double x = atof( argv[5] );   // poczatek ukladu - x
    double y = atof( argv[6] );   // poczatek ukladu - y
    double z = atof( argv[7] );   // poczatek ukladu - z
    double fi = atof( argv[8] );  //kat fi w radianach
    double psi = atof( argv[9] ); //kat gamma w radianach

    lista * res;

    res = createList();

    dodajVal(res, 0, 0, 1);
    dodajVal(res, 0, 0, 0);
    dodajVal(res, 1, 0, 0);
    dodajVal(res, 1, 0, 1);
    dodajVal(res, 1, 1, 1);
    dodajVal(res, 1, 1, 0);
    dodajVal(res, 0, 1, 0);
    dodajVal(res, 0, 1, 1);




    //---------------
    while(n<k)
    {
        lista * a[7];

        F(i,0,7) a[i] = duplikuj(res);


        obrotB(n,res);
        obrotA(n,res);
        obrotA(n,res);
        obrotA(n,res);
        przesun(n,0,res);

        obrotC(n,a[0]);
        obrotA(n,a[0]);

        obrotC(n,a[1]);
        obrotA(n,a[1]);
        przesun(n,2,a[1]);

        obrotC(n,a[2]);
        obrotC(n,a[2]);
        przesun(n,2,a[2]);
        przesun(n,0,a[2]);

        obrotC(n,a[3]);
        obrotC(n,a[3]);
        przesun(n,2,a[3]);
        przesun(n,0,a[3]);
        przesun(n,1,a[3]);


        obrotC(n,a[4]);
        obrotA(n,a[4]);
        obrotA(n,a[4]);
        obrotA(n,a[4]);
        przesun(n,2,a[4]);
        przesun(n,1,a[4]);

        obrotC(n,a[5]);
        obrotA(n,a[5]);
        obrotA(n,a[5]);
        obrotA(n,a[5]);
        przesun(n,1,a[5]);


        obrotB(n,a[6]);
        obrotB(n,a[6]);
        obrotB(n,a[6]);
        obrotA(n,a[6]);
        przesun(n,1,a[6]);
        przesun(n,0,a[6]);


        F(i,0,7) dodajListy(res,a[i]);
        ++n;
    }

    obrotFi(fi,res,n);
    obrotPsi(psi, res, n);
    skaluj(res, n, u);
    przesunArg(res,x,y,z);


    //---------------

    FILE *out = fopen("hilbert.ps", "w");

    fprintf(out, "\%!PS-Adobe-2.0 EPSF-2.0\n");
    fprintf(out, "\%\%\%BoundingBox 0 0 %d %d\n", s, s);
    fprintf(out, "newpath\n");

    rzutuj(out,d,res);

    fprintf(out, ".4 setlinewidth\n");
    fprintf(out, "stroke\n");
    fprintf(out, "showpage\n");
    fprintf(out, "%%%Trailer\n%");
    fprintf(out, "EOF\n");


    return 0;
}

/*******************************************************
*                                                      *
*               implementacje funkcji                  *
*                                                      *
*                                                      *
*                                                      *
********************************************************/

void przesunArg(lista *l, double x, double y, double z)
{
    node * temp = l->poczatek;

    while( temp != NULL)
    {
        temp->val->x += x;
        temp->val->y += y;
        temp->val->z += z;

        temp = temp->next;
    }
}

void skaluj(lista *l, int rzad, double u)
{
    node * temp = l->poczatek;

    double k = u / ( (1<<(rzad+1)) -1 );

    while( temp != NULL)
    {
        temp->val->x *= k;
        temp->val->y *= k;
        temp->val->z *= k;

        temp = temp->next;
    }
}


void rzutuj(FILE * out, double d,lista *l)
{
    node * temp = l->poczatek;


    fprintf(out, "%.2f %.2f moveto\n", ((temp->val->x*d)/(temp->val->z + d)), ((temp->val->y*d)/(temp->val->z + d)));

    temp = temp->next;

    while(temp != NULL)
    {
        fprintf(out, "%.2f %.2f lineto\n", ((temp->val->x*d)/(temp->val->z + d)), ((temp->val->y*d)/(temp->val->z + d)) );
        temp = temp->next;
    }
}

void obrotFi(double fi, lista*l, int rzad)
{
    double h = (double)(1<<(rzad+1))-1;
    h = h/2;
    node * temp = l->poczatek;
    while(temp != NULL)
    {
        double x = (temp->val->x-h)*cos(fi) - (temp->val->y-h)*sin(fi) + h;
        double y = (temp->val->x-h)*sin(fi) + (temp->val->y-h)*cos(fi) + h;
        temp->val->x = x;
        temp->val->y = y;

        temp = temp->next;
    }
}

void obrotPsi(double psi, lista*l, int rzad)
{
    double h = (double)(1<<(rzad+1))-1;
    h = h/2;
    node * temp = l->poczatek;
    while(temp != NULL)
    {
        double x = (temp->val->x-h)*cos(psi) - (temp->val->z-h)*sin(psi) + h;
        double z = (temp->val->x-h)*sin(psi) + (temp->val->z-h)*cos(psi) + h;
        temp->val->x = x;
        temp->val->z = z;

        temp = temp->next;
    }
}

void obrotA(int rzad, lista *l)
{
    double h = (double)(1<<(rzad+1))-1;
    h = h/2;
    node * temp = l->poczatek;
    while(temp != NULL)
    {
        double x = temp->val->x;
        double y = temp->val->y;
        temp->val->x = y;
        temp->val->y = 2*h-x;

        temp = temp->next;
    }
}

void obrotB(int rzad, lista *l)
{
    double h = (double)(1<<(rzad+1))-1;
    h = h/2;
    node * temp = l->poczatek;
    while(temp != NULL)
    {
        double z = temp->val->z;
        double y = temp->val->y;
        temp->val->y = z;
        temp->val->z = 2*h-y;

        temp = temp->next;
    }
}

void obrotC(int rzad, lista *l)
{
    double h = (double)(1<<(rzad+1))-1;
    h = h/2;
    node * temp = l->poczatek;
    while(temp != NULL)
    {
        double z = temp->val->z;
        double x = temp->val->x;
        temp->val->x = z;
        temp->val->z = 2*h-x;

        temp = temp->next;
    }
}


void dodajListy(lista * l1, lista * l2)
{
    l1->koniec->next = l2->poczatek;
    l1->koniec = l2->koniec;
    free(l2);
}

void przesun(int rzad, int dir, lista * l)
{
    node * temp = l->poczatek;

    int h = 1 << (rzad+1);

    if( dir == 0)
    {
        while(temp != NULL)
        {
            temp->val->z += h;
            temp = temp->next;
        }
    }
    else
    if(dir == 1)
    {
        while(temp != NULL)
        {
            temp->val->y += h;
            temp = temp->next;
        }
    }
    else
    if( dir == 2)
    {
        while(temp != NULL)
        {
            temp->val->x += h;
            temp = temp->next;
        }
    }

}

lista * duplikuj(lista * l)
{
    lista * res = malloc(sizeof(lista));
    node * temp = createNode();

    node * wskaznik = l->poczatek;

    res->poczatek = temp;


    while(wskaznik != NULL)
    {
        temp->val = malloc(sizeof(punkt));
        temp->val->x = wskaznik->val->x;
        temp->val->y = wskaznik->val->y;
        temp->val->z = wskaznik->val->z;

        if( wskaznik->next == NULL)
        {
            temp->next = NULL;
        }
        else
        {
            temp->next = createNode();
            temp = temp->next;
        }

        wskaznik = wskaznik->next;
    }

    res->koniec = temp;
    return res;
}

void printList(lista * l)
{
    node * temp = l->poczatek;
    while(temp != NULL)
    {
        printf("%5.2f, %5.2f, %5.2f\n", temp->val->x, temp->val->y, temp->val->z);
        temp=temp->next;
    }
}

void dodajVal(lista * l, double x, double y, double z)
{
    node * temp = createNode();
    if(isEmpty(l))
    {
        l->poczatek = temp;
        l->poczatek->val = malloc(sizeof(punkt));
        l->poczatek->val->x = x;
        l->poczatek->val->y = y;
        l->poczatek->val->z = z;
        l->koniec = l->poczatek;
    }
    else
    {
        l->koniec->next = temp;

        temp->val = malloc(sizeof(punkt));
        temp->val->x = x;
        temp->val->y = y;
        temp->val->z = z;

        l->koniec = temp;
    }
}

lista *createList()
{
    lista * res = malloc(sizeof(lista));
    res->koniec = NULL;
    res->poczatek = NULL;

    return res;
}

node * createNode()
{
    node *res;
    res = malloc(sizeof(node));
    res->next = NULL;
    res->val = NULL;
    return res;
}

bool isEmpty(lista * x)
{
    if(x->poczatek == NULL) return 1;
    return 0;
}
