#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"                // lib for pre-defined class(es)

/* program for eumerating all hamilton cycles of a given graph */

Graph graph;            // instance store information of vertices and arcs 
int d = MAXV;                  // degree of graph: maximum degree of all vertices

int verbose;            // print more details about detection process
                        //
/* DEBUGGING */
char* type[3] = {"bare", "outer", "inner"};
/* @subroutine: print information of a vertex */
void vertex_info(Vertex* u) {
    printf("%s\t%d\t%s\t%s\t%s\t", u->name, u->deg, type[u->type], u->mate->name, u->comate->name);
    for (Arc* a = u->a; a != NULL; a = a->next) printf("%s ", a->v->name);
    printf("\n");
}

void graph_state() {
    printf("name\tdeg\ttype\tmate\tcomate\tadj\n");
    for (Vertex* u = graph.v; u < graph.v+graph.n; u++) {
        vertex_info(u);
    }    
}


/* PREPROCESS */

/* @subroutine: map name to index for graph's vertices */
/*
   int name2idx(const char* name) {
   const int p = 31;
   const int mod = graph->n;
   unsigned long long hash = 0;
   unsigned long long e = 1;
   for (int i = 0; i < strlen(name); i++) {
   hash = (hash + (name[i]*e)) % mod;
   e = (e * p) % mod;
   }
   return 0; 
   }
   */

void add_arc(Vertex *u, Arc *a) {
    a->next = u->a;
    u->a = a;
}

/* @subroutine: init vertices and arcs for detected graph */
void init(const char* fn) {
    FILE *fs;
    fs = fopen(fn, "r");
    if (fs == NULL) {
        perror("INFORM: Error opening file\n");
    }
    else {
        fscanf(fs, "%d", &graph.n);
        for (int i = 0; i < graph.n; i++) {
            fscanf(fs, "%s", graph.v[i].name);
        }
        int na;
        fscanf(fs, "%d", &na);
        for (int i = 0; i < na; i++) {
            int u, v;
            fscanf(fs, "%d %d", &u, &v);

            Arc *au = (Arc*) malloc(sizeof(Arc));
            au->u = graph.v + u;
            au->v = graph.v + v;
            add_arc(graph.v + u, au);

            Arc *av = (Arc*) malloc(sizeof(Arc));
            av->u = graph.v + v;
            av->v = graph.v + u;
            add_arc(graph.v + v, av);

            graph.v[u].deg++;
            graph.v[v].deg++;
        }
        fclose(fs);
    }
}
int lv = 0;                 // traveled level
Vertex* bare[MAXV];         // bare vertice(s) with degree of 2: so-called '2-bare' 
Vertex* bakbare[MAXV];      // store previous 2-bare to restore after branching   
Vertex* outer_hdr;          // header of cycly doubly linked list stored outer vertice(s)     
Vertex* outer_opt;

int nbare = 0;              // 2-bare counter
int prenbare[MAXV];         // marker for next level
int curnbare[MAXV];         // marker for each branch

int narc = 0;               // arc counter for solution: narc==g->n ~ 1 solution
int narclv[MAXV];           // store collected arcs in each level, used in backup

Vertex *src[MAXV], *dest[MAXV];         // two ends of chosen arcs
/* @subroutine: init bare[], get degree 'd' of graph */
void pre_proc() {
    if (verbose) printf("\nPRE-PROCESSING ... done\n");
    for (Vertex* u = graph.v; u < graph.v + graph.n; u++) {
        u->type = BARE;
        if (u->deg == 2) bare[nbare++] = u;
        if (u->deg < d) d = u->deg, outer_opt = u;
        u->rlink = u->llink = u->mate = NULL;
    }
    printf("Degree: %d\n", d);
    prenbare[lv] = 0;
    outer_hdr = (Vertex*) malloc(sizeof(Vertex));
    outer_hdr->rlink = outer_hdr->llink = outer_hdr;
}

/* DETECTION */
#define iso(u) u->rlink->llink = u->llink, u->llink->rlink = u->rlink
#define conn(u) u->rlink->llink = u, u->llink->rlink = u
#define add_outer(u) outer_hdr->rlink->llink = u, u->rlink = outer_hdr->rlink, u->llink = outer_hdr, outer_hdr->rlink = u, u->type = OUTER

/* @subroutine: decrease degree of 'v' which is adjacent of OUTER 'u' with mate 'w' */
void dec_deg(Vertex* v, Vertex* w) {
    if (v->type == BARE) {
        v->deg--;
        if (v->deg == 2) bare[nbare++] = v;
    }
    else if (v != w) v->deg--;
}
/* @subroutine: take outer vertex with minimum degree */
Vertex* get_opt() {
    int tmp = MAXV;
    Vertex* u = NULL;
    for (Vertex* v = outer_hdr->rlink; v != outer_hdr; v = v->rlink) {
        if (v->deg < tmp) {
            u = v;
            tmp = v->deg;
        } 
    }
    return u;
}
/* @subroutine: cloth BARE 'u' with OUTER mate 'v' and BARE comate 'w' */ 
void bob2iio(Vertex* v, Vertex* w) {
    w->deg--;
    add_outer(w);
    w->mate = v->mate;
    v->mate->mate = w;
    // if v->mate and w are adjacent, their degree should be decreased
    for (Arc *a = w->a; a != NULL; a = a->next) {
        if (a->v == v->mate) {
            w->deg--;
            v->mate->deg--;
            break;
        }
    }
    // adjacent vertices of OUTER decrease its degree when OUTER->INNER
    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v->type != INNER) dec_deg(a->v, v->mate);
    }
    v->type = INNER;
    iso(v);
}
/* @subroutine: cloth BARE 'u' with OUTER mate 'v' and OUTER comate 'w' */ 
void boo2iii(Vertex* v, Vertex* w) {
    // adjacent vertices of OUTER decrease its degree when OUTER->INNER
    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v->type != INNER) dec_deg(a->v, v->mate);
    }
    v->type = INNER;
    iso(v);
    for (Arc *a = w->a; a != NULL; a = a->next) {
        if (a->v->type != INNER) dec_deg(a->v, w->mate);
    }
    w->type = INNER;
    iso(w);

    // if 'v' and 'w' are in common segment, it could be a short cycle or full one
    // which we will check later on
    if (v->mate != w) {
        v->mate->mate = w->mate;
        w->mate->mate = v->mate;

        for (Arc *a = v->mate->a; a != NULL; a = a->next) {
            if (a->v == w->mate) {
                v->mate->deg--;
                w->mate->deg--;
                break;
            }
        }
    }

}
/* @subroutine: cloth BARE 'u' with BARE mate 'v' and BARE comate 'w' */ 
void bbb2ioo(Vertex* v, Vertex* w) {
    --v->deg, --w->deg;
    add_outer(v), add_outer(w);
    v->mate = w, w->mate = v;
    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v == w) {
            v->deg--, w->deg--;
            break;
        }
    }
}
/* @subroutine: clothe all 2-bare vertice(s) */
void cloth_2bare() {
    if (verbose) printf("\nACTION: cloth 2-bare explored at level %d: \n", lv);
    for (int i = prenbare[lv]; i < nbare; i++) {
        Vertex* u = bare[i];
        if (u->type != BARE) bakbare[i] = u, bare[i] = NULL; 
        else if (u->deg == 2) {
            // find mate and comate of 2-bare vertex
            Vertex *v, *w;
            Arc *a;
            for (a = u->a; a != NULL; a = a->next) {
                if (a->v->type != INNER) {
                    v = a->v;
                    break;
                }
            }
            for (a = a->next; a != NULL; a = a->next) {
                if (a->v->type != INNER) {
                    w = a->v;
                    break;
                }
            }
            // update 2-bare 'u' 's attributes
            u->type = INNER;
            u->mate = v, u->comate = w;

            // add 2 arcs for current path
            src[narc] = u, dest[narc] = v, ++narc; 
            src[narc] = u, dest[narc] = w, ++narc;

            // update some based on cases of 'v' and 'w'
            if (v->type == BARE) {
                if (w->type == OUTER) bob2iio(w, v); 
                else bbb2ioo(v, w);
            }
            else if (w->type == BARE) bob2iio(v, w);
            else boo2iii(v, w);
        }
    }
    if (verbose) graph_state();    
}
/* @subroutine: uncloth INNER 'u' with INNER mate 'v' and INNER comate 'w' */ 
void iii2boo(Vertex *u, Vertex *v, Vertex *w) {

    if (v->mate != w) {
        v->mate->mate = v, w->mate->mate = w;
        for (Arc *a = v->mate->a; a != NULL; a = a->next) {
            if (a->v == w->mate) {
                v->mate->deg++, w->mate->deg++;
                break;
            }
        }
    }
    conn(v);
    v->type = OUTER;
    // fix degree of adjacent of 'v'
    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v->type != INNER 
                && a->v != v->mate 
                && a->v != u) a->v->deg++;
    }
    conn(w);
    w->type = OUTER;
    // fix degree of adjacent of 'w'
    for (Arc *a = w->a; a != NULL; a = a->next) {
        if (a->v->type != INNER
                && a->v != w->mate
                && a->v != u) a->v->deg++;
    }
}
/* @subroutine: uncloth INNER 'u' with INNER mate 'v' and OUTER comate 'w' */
void iio2bob(Vertex *u, Vertex *v, Vertex *w) {
    // convert OUTER 'w' back to BARE
    iso(w);
    w->type = BARE;
    w->deg++;
    w->mate = NULL;
    for (Arc *a = w->a; a != NULL; a = a->next) {
        if (a->v == v->mate) {
            v->mate->deg++, w->deg++;
            break;
        }
    }
    // convert INNER 'v' back to OUTER
    conn(v);
    v->type = OUTER;
    v->mate->mate = v;
    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v->type != INNER 
                && a->v != v->mate
                && a->v != u) a->v->deg++; 
    }

}
/* @subroutine: uncloth INNER 'u' with OUTER mate 'v' and OUTER comate 'w' */
void ioo2bbb(Vertex *v, Vertex *w) {
    iso(v);
    v->type = BARE;
    v->mate = NULL;
    iso(w);
    w->type = BARE;
    w->mate = NULL;

    v->deg++, w->deg++;

    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v == w) {
            v->deg++, w->deg++;
            break;
        }
    }
}
/* @subroutine: unclothe all 2-bare vertice(s) */
void uncloth_2bare() {
    if (verbose) printf("\nACTION: uncloth 2-bare explored at level %d: \n", lv);
    for (int i = nbare-1; i >= prenbare[lv]; i--) {
        Vertex *u = bare[i];
        if (!u) bare[i] = bakbare[i];
        else {
            Vertex *v = u->mate, *w = u->comate;
            u->type = BARE, u->mate = u->comate = NULL;
            if (v->type == INNER) {
                if (w->type == INNER) iii2boo(u, v, w);
                else iio2bob(u, v, w);
            }
            else if (w->type == INNER) iio2bob(u, w, v);
            else ioo2bbb(v, w);
        }
    }
    if (verbose) graph_state();        
}
/* @subroutine: INNER to OUTER*/
void i2o(Vertex* u) {
    conn(u);
    u->type = OUTER;
    for (Arc *a = u->a; a != NULL; a = a->next) {
        if (a->v->type != INNER && a->v != u->mate) a->v->deg++;
    }
    if (verbose) graph_state();
}
/* @subroutine: OUTER to INNER */
void o2i(Vertex* u) {
    iso(u);
    u->type = INNER;
    for (Arc* a = u->a; a != NULL; a = a->next) {
        Vertex* v = a->v;
        if (v != NULL && v->type < INNER) dec_deg(v, u->mate); 
    }
    u->type = INNER;
    iso(u);
    if (verbose) {
        printf("\nACTION: source %s at level %d\n", u->name, lv);
        graph_state();    
    }
}
/* @subroutine: branch to prepare for next 'cloth_2bare' subroutine */
void branch(Vertex *u, Vertex *v) {
    if (verbose) printf("\nACTION: dest %s at level %d\n", v->name, lv);
    dest[narc++] = v;
    if (v->type == OUTER) {
        for (Arc *a = u->mate->a; a != NULL; a = a->next) {
            if (a->v == v->mate) {
                u->mate->deg--, v->mate->deg--;
                break;
            }
        }
        u->mate->mate = v->mate, v->mate->mate = u->mate;
        iso(v);
        v->type = INNER;
        for (Arc *a = v->a; a != NULL; a = a->next) {
            if (a->v->type != INNER) dec_deg(a->v, v->mate);
        }
    }
    else {
        for (Arc *a = u->mate->a; a != NULL; a = a->next) {
            if (a->v == v) {
                v->deg--, u->mate->deg--;
                break;
            }
        }
        v->type = OUTER;
        v->mate = u->mate, u->mate->mate = v;
        add_outer(v);
    }
    if (verbose) graph_state();
}
/* @subroutine: retreat to prepare for next branch */
void retreat(Vertex *u, Vertex *v) {
    // restore the number of chosen arcs
    if (verbose) printf("\nACTION: retreat\n");
    if (v->type == INNER) {
        for (Arc *a = v->a; a != NULL; a = a->next) {
            if (a->v->type != INNER && a->v != v->mate) a->v->deg++;
        }
        v->type = OUTER;
        conn(v);
        v->mate->mate = v, u->mate->mate = u;

        for (Arc *a = u->mate->a; a != NULL; a = a->next) {
            if (a->v == v->mate) {
                v->mate->deg++, a->v->deg++;
                break;
            }
        }
    }
    else {
        for (Arc *a = u->mate->a; a != NULL; a = a->next) {
            if (a->v == v) {
                u->mate->deg++, v->deg++;
                break;
            }
        }
        v->type = BARE;
        iso(v);
        v->mate = NULL;
        u->mate->mate = u;
    }
    if (verbose) graph_state();
    nbare = curnbare[lv];
}

void cycle_info() {
    printf("\nHAMILTONION CYCLE: ");
    for (int i = 0; i < narc; i++) {
        printf("(%s, %s), ", src[i]->name, dest[i]->name);
    }
    printf("\n");
}


/* @subroutine: detect ('backtrack' as backbone) */
void detect() {
    cloth_2bare();
    lv++;
    prenbare[lv] = nbare;
    narclv[lv] = narc;
    if (narc == graph.n) cycle_info();
    else if (narc == graph.n-1) {
        Vertex *u, *v;
        u = outer_hdr->rlink, v = outer_hdr->llink;
        if (u->mate == v) {
            src[narc] = u, dest[narc++] = v;
            cycle_info();
        }
    }
    else {
        Vertex* u = get_opt();    // optimal outer vertex
        src[narc] = u;
        o2i(u);
        curnbare[lv] = nbare;
        for (Arc* a = u->a; a != NULL; a = a->next) {
            Vertex* v = a->v;
            if (v->type != INNER && v != u->mate) {
                branch(u, v);
                detect(u, v);
                retreat(u, v);
            }
        }
        i2o(u);
        nbare = prenbare[lv];
    }
    lv--;
    narc = narclv[lv];
    uncloth_2bare();
}



/* PROGRAM */

#define TC 5
#define MAX_FN 50
char tc[TC][MAX_FN];

/* main function: 'entry point' 
 * command to run: ./'file_name' [-v] tc
 *      -v: verbose
 *      tc: index of test case from 1 to 3
 * */

int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) printf("INFORM: Incorrect syntax to run program\n");
    else {
        if (strcmp(argv[1], "-v") == 0) verbose = 1;
        
        char fn[40] = "test/tc#";               // prefix file location
        char ff[] = ".txt";                     // file formate
        if (argc == 2) strcat(fn, argv[1]);
        else strcat(fn, argv[2]);
        strcat(fn, ff);
        
        init(fn);
        graph_state();
        
        pre_proc();
        if (d < 2) printf("INFORM: Can not detect any Hamiltonian cycle\n");
        else if (d == 2) detect();
        else {
            outer_opt->type = OUTER;
            outer_opt->deg--;
            add_outer(outer_opt);
            src[narc] = outer_opt;
            for (Arc *a = outer_opt->a; a != NULL; a = a->next) {
                narc = 0;
                printf("\nACTION: Fix arc [%s, %s] in the detecting path(s)\n", a->u->name, a->v->name);
                Vertex *v = a->v;
                outer_opt->mate = v, v->mate = outer_opt;
                v->deg--;
                add_outer(v);
                
                dest[narc++] = v;
                graph_state();
                detect();
                iso(v);
                v->deg++, v->type = BARE;
            }
            iso(outer_opt);
            outer_opt->deg++;
            outer_opt = BARE;
            graph_state();
        }
    }
    return 0;
}
