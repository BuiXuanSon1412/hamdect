#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "graph.h"                // lib for pre-defined class(es)

/* program for eumerating all hamilton cycles of a given graph */

Graph graph;                                    // instance store information of vertices and arcs 
int d = MAXV;                                   // degree of graph: maximum degree of all vertices
int cnt = 0;                                    // counter for solution
int sc = 0;                                     // flag for short cycle
int _v_mode = 0, _o_mode = 0;                                // option program mode
FILE *fp;                                       // output file descriptor

int lv = 0;                                     // traveled level
Vertex* bare[MAXV];                             // bare vertice(s) with degree of 2: so-called '2-bare' 
Vertex* bakbare[MAXV];                          // store previous 2-bare to restore after branching   
Vertex* outer_hdr;                              // header of cycly doubly linked list stored outer vertice(s)     
Vertex* outer_opt;

int nbare = 0;                                  // 2-bare counter
int prenbare[MAXV];                             // marker for next level
int curnbare[MAXV];                             // marker for each branch

int narc = 0;                                   // arc counter for solution: narc==g->n ~ 1 solution
int narclv[MAXV];                               // store collected arcs in each level, used in backup

Vertex *src[MAXV], *dest[MAXV];                 // two ends of chosen arcs


/*  DEBUGGING: utility routine  */
char* type[3] = {"bare", "outer", "inner"};

void vertex_info(Vertex* u);                    /* @util: print information of a vertex */
void graph_state();                             /* @util: print current state of graph  */
void outer_list();                              /* @util: print OUTER list */
void arc_list();                                /* @util: print arcs in current occupied path */
void cycle_info();                              /* @util: print solution */

/*  PRE-PROCESS: prepare for detection  */
int init(const char* fn);
void pre_proc();

/*  DETECTION: */
#define iso(u) u->llink->rlink = u->rlink, u->rlink->llink = u->llink
#define conn(u) u->llink->rlink = u, u->rlink->llink = u
#define add_outer(u) u->rlink = outer_hdr->rlink, outer_hdr->rlink->llink = u, u->llink = outer_hdr, outer_hdr->rlink = u, u->type = OUTER

void dec_deg(Vertex* v, Vertex* w);             /* @subroutine: decrease degree of 'v' which is adjacent of OUTER 'u' with mate 'w' */
Vertex* get_opt();                              /* @subroutine: take outer vertex with minimum degree */

void bob2iio(Vertex* v, Vertex* w);             /* @subroutine: cloth BARE 'u' with OUTER mate 'v' and BARE comate 'w' */ 
void boo2iii(Vertex* v, Vertex* w);             /* @subroutine: cloth BARE 'u' with OUTER mate 'v' and OUTER comate 'w' */ 
void bbb2ioo(Vertex* v, Vertex* w);             /* @subroutine: cloth BARE 'u' with BARE mate 'v' and BARE comate 'w' */ 
void cloth_2bare();                             /* @subroutine: clothe all 2-bare vertice(s) */

void iii2boo(Vertex *u, Vertex *v, Vertex *w);  /* @subroutine: uncloth INNER 'u' with INNER mate 'v' and INNER comate 'w' */ 
void iio2bob(Vertex *u, Vertex *v, Vertex *w);  /* @subroutine: uncloth INNER 'u' with INNER mate 'v' and OUTER comate 'w' */
void ioo2bbb(Vertex *v, Vertex *w);             /* @subroutine: uncloth INNER 'u' with OUTER mate 'v' and OUTER comate 'w' */
void uncloth_2bare();                           /* @subroutine: unclothe all 2-bare vertice(s) */

void i2o(Vertex* u);                            /* @subroutine: INNER to OUTER*/
void o2i(Vertex* u);                            /* @subroutine: OUTER to INNER */

void branch(Vertex *u, Vertex *v);              /* @subroutine: branch to prepare for next 'cloth_2bare' subroutine */
void retreat(Vertex *u, Vertex *v);             /* @subroutine: retreat to prepare for next branch */

void bdetect();                                 /* @subroutine: detecting procedure ('backtrack' as backbone) when graph exists OUTER */
void detect();                                  /* @subroutine: prepare if no 2-BARE */


/* PROGRAM */

#define TC 5
#define MAX_FN 50
char tc[TC][MAX_FN];

/* main function: 'entry point' 
 * command to run: ./'file_name' [-v, o] tc
 *      -v: verbose mode
 *      tc: index of test case from 1 to 3
 * */

int main(int argc, char **argv) {
    
    if (argc < 2 || argc > 3) printf("ERROR: Incorrect syntax ./filename [-v/o] tc\n");
    else {
        // check if file exists
        char fn[40] = "test/tc#";               // prefix file location
        if (argc == 3) {
            if (strcmp(argv[1], "-v") == 0) _v_mode = 1;
            else if (strcmp(argv[1], "-o") == 0) _o_mode = 1;
            else if (strcmp(argv[1], "-ov") == 0
                    || strcmp(argv[1], "-vo") == 0) _v_mode = _o_mode = 1;
            else {
                printf("ERROR: Incorrect syntax ./filename [-v/o] tc\n");
                return 0;
            }
            strcat(fn, argv[2]);
        }
        else strcat(fn, argv[1]);

        strcat(fn, ".txt");
        if (access(fn, F_OK) != 0) {
            printf("ERROR: File not found\n");
            return 0;
        } 

        // check and start
        if (init(fn)) {
            graph_state();
            pre_proc();
            detect();
        }
    }
    return 0;
}



/* @subroutine: print information of a vertex */
void vertex_info(Vertex* u) {
    fprintf(fp,"%s\t%d\t%s\t%s\t%s\t", u->name, u->deg, type[u->type], u->mate->name, u->comate->name);
    for (Arc* a = u->a; a != NULL; a = a->next) fprintf(fp,"%s ", a->v->name);
    fprintf(fp,"\n");
}

void graph_state() {
    fprintf(fp,"name\tdeg\ttype\tmate\tcomate\tadj\n");
    for (Vertex* u = graph.v; u < graph.v+graph.n; u++) {
        vertex_info(u);
    }    
}

/* @subroutine: init vertices and arcs for detected graph */
int init(const char* fn) {
    if (_o_mode == 0) fp = stdout;
    else {
        fp = fopen("test/outp.txt", "w");
        if (fp == NULL) {
            printf("ERROR: Can not open verbose storage file\n");
            return 0;
        }
    }
    FILE *fs;
    fs = fopen(fn, "r");
    if (fs == NULL) {
        printf("ERROR: Can not open input file\n");
        return 0;
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
        return 1;
    }
}
/* @subroutine: init bare[], get degree 'd' of graph */
void pre_proc() {
    if (_v_mode) fprintf(fp,"\nPRE-PROCESSING ... done\n");
    for (Vertex* u = graph.v; u < graph.v + graph.n; u++) {
        u->type = BARE;
        if (u->deg == 2) bare[nbare++] = u;
        if (u->deg < d) d = u->deg, outer_opt = u;
        u->rlink = u->llink = u->mate = NULL;
    }
    fprintf(fp,"Degree: %d\n", d);
    prenbare[lv] = 0;
    outer_hdr = (Vertex*) malloc(sizeof(Vertex));
    outer_hdr->rlink = outer_hdr->llink = outer_hdr;
}

void outer_list() {
    fprintf(fp,"list of outer: ");
    fprintf(fp,"(%s %s %s) ", outer_hdr->llink->name, outer_hdr->name, outer_hdr->rlink->name);
    for (Vertex *u = outer_hdr->rlink; u != outer_hdr; u = u->rlink) {
        printf ("(%s %s %s) ", u->llink->name, u->name, u->rlink->name);
    }
    fprintf(fp,"\n");
}

void arc_list() {
    fprintf(fp,"traveled arcs: ");
    for (int i = 0; i < narc; i++) {
        fprintf(fp,"(%s %s), ", src[i]->name, dest[i]->name);
    }
    fprintf(fp,"\n");
}
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
    v->type = INNER, w->type = INNER;
    iso(v), iso(w);
    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v->type != INNER) dec_deg(a->v, v->mate);
    }
    
    for (Arc *a = w->a; a != NULL; a = a->next) {
        if (a->v->type != INNER) dec_deg(a->v, w->mate);
    }

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
    if (_v_mode) fprintf(fp,"\nACTION: cloth 2-bare explored at level %d:\n", lv);
    for (int i = prenbare[lv]; i < nbare; i++) {
        Vertex* u = bare[i];
        if (u->type != BARE || u->deg != 2) bakbare[i] = u, bare[i] = NULL; 
        else {
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

            // update based on cases of 'v' and 'w'
            if (v->type == BARE) {
                if (w->type == OUTER) bob2iio(w, v); 
                else bbb2ioo(v, w);
            }
            else if (w->type == BARE) bob2iio(v, w);
            else {
                boo2iii(v, w);
                if (v == w->mate && (narc < graph.n || outer_hdr->rlink != outer_hdr)) sc = 1;
            }
        }
    }
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
    
    // fix degree of adjacent of 'v'
    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v->type != INNER 
                && a->v != v->mate 
                && a->v != u) a->v->deg++;
    }
    
    // fix degree of adjacent of 'w'
    for (Arc *a = w->a; a != NULL; a = a->next) {
        if (a->v->type != INNER
                && a->v != w->mate
                && a->v != u) a->v->deg++;
    }
    conn(v), conn(w);
    v->type = OUTER, w->type = OUTER;
}
/* @subroutine: uncloth INNER 'u' with INNER mate 'v' and OUTER comate 'w' */
void iio2bob(Vertex *u, Vertex *v, Vertex *w) {
    
    // convert INNER 'v' back to OUTER
    conn(v);
    v->type = OUTER;
    v->mate->mate = v;
    for (Arc *a = v->a; a != NULL; a = a->next) {
        if (a->v->type != INNER 
                && a->v != v->mate
                && a->v != u) a->v->deg++; 
    }
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
    if (_v_mode) fprintf(fp,"\nACTION: uncloth 2-bare explored at level %d:\n", lv);
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
}
/* @subroutine: INNER to OUTER*/
void i2o(Vertex* u) {
    conn(u);
    u->type = OUTER;
    for (Arc *a = u->a; a != NULL; a = a->next) {
        if (a->v->type != INNER && a->v != u->mate) a->v->deg++;
    }
    if (_v_mode) fprintf(fp, "\nACTION: turn %s back to OUTER at level %d\n", u->name, lv);
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
    if (_v_mode) fprintf(fp,"\nACTION: source %s at level %d\n", u->name, lv);
}
/* @subroutine: branch to prepare for next 'cloth_2bare' subroutine */
void branch(Vertex *u, Vertex *v) {
    if (_v_mode) fprintf(fp,"\nACTION: branch [%s, %s] at level %d\n", u->name, v->name, lv);
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
}
/* @subroutine: retreat to prepare for next branch */
void retreat(Vertex *u, Vertex *v) {
    // restore the number of chosen arcs
    if (_v_mode) fprintf(fp,"\nACTION: retreat [%s, %s] at level %d\n", u->name, v->name, lv);
    if (v->type == INNER) {
        for (Arc *a = v->a; a != NULL; a = a->next) {
            if (a->v->type != INNER && a->v != v->mate) a->v->deg++;
        }
        v->type = OUTER;
        conn(v);
        v->mate->mate = v, u->mate->mate = u;

        for (Arc *a = u->mate->a; a != NULL; a = a->next) {
            if (a->v == v->mate) {
                v->mate->deg++, u->mate->deg++;
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
}

void cycle_info() {
    int v[MAXV];
    int w[MAXV];

    memset(v, -1, sizeof(v));
    memset(w, -1, sizeof(w));
    
    for (int i = 0; i < narc; i++) {
        int j = src[i] - graph.v;
        int k = dest[i] - graph.v;
        if (v[j] != -1) w[j] = k;
        else v[j] = k;
        if (v[k] != -1) w[k] = j;
        else v[k] = j; 
    }
     
    fprintf(fp,"%d-th Hamitonian cycle: ", ++cnt);
    fprintf(fp,"%s %s", graph.v[0].name, graph.v[v[0]].name);
    int pre = 0, cur = v[0];
    while (1) {
        int tmp = cur;
        cur = (v[cur] == pre) ? w[cur] : v[cur];
        fprintf(fp," %s", graph.v[cur].name);
        if (cur == 0) break;
        pre = tmp;
    }
    fprintf(fp,"\n");
}


/* @subroutine: detecting procedure ('backtrack' as backbone) 
 *              when graph already exists OUTER */
void bdetect() {
    cloth_2bare();
    if (_v_mode) graph_state(), arc_list();
    lv++;
    prenbare[lv] = nbare;
    narclv[lv] = narc;
    Vertex* u = get_opt();
    if (sc) {
        if (_v_mode) fprintf(fp,"INFORM: there must exist short cycle\n");
        sc = 0;
    }
    else if (!u) {               // there doesn't exist any OUTER vertex
        if (narc == graph.n) {
            cycle_info();
        }
    }
    else if (u->deg == 0) { // there exists a OUTER vextex which MUST connect to its mate
        if (narc == graph.n-1) {
            Vertex *u, *v;
            u = outer_hdr->rlink, v = outer_hdr->llink;

            if (u != v && u->mate == v) { // condition: u and v are OUTER and be other's mate
                src[narc] = u, dest[narc++] = v;
                cycle_info();
            }
        }
    }
    else {                  // there still exists OUTER vertex being able to branch
        src[narc] = u;
        o2i(u);
        if (_v_mode) graph_state(), arc_list();
        curnbare[lv] = nbare;
        for (Arc* a = u->a; a != NULL; a = a->next) {
            Vertex* v = a->v;
            if (v->type != INNER && v != u->mate) {
                branch(u, v);
                if (_v_mode) graph_state(), arc_list();
                bdetect(u, v);
                retreat(u, v);
                nbare = curnbare[lv];
                if (_v_mode) graph_state(), arc_list();
            }
        }
        i2o(u);
        if (_v_mode) graph_state(), arc_list();
    }
    nbare = prenbare[lv];
    lv--;
    narc = narclv[lv];
    uncloth_2bare();
    if (_v_mode) graph_state(), arc_list();
}

void detect() {
    if (d < 2) fprintf(fp,"INFORM: Can not detect any Hamiltonian cycle\n");
    else if (d == 2) bdetect();
    else {
        outer_opt->type = OUTER;
        outer_opt->deg--;
        add_outer(outer_opt);
        src[narc] = outer_opt;
        for (Arc *a = outer_opt->a; a != NULL; a = a->next) {
            narc = 0;
            if (_v_mode) fprintf(fp,"\nACTION: Fix arc [%s, %s] in the detecting path(s)\n", a->u->name, a->v->name);
            // update for branching vertex 'v'
            Vertex *v = a->v;
            outer_opt->mate = v, v->mate = outer_opt;
            v->deg--;
            add_outer(v);
            dest[narc++] = v;

            if (_v_mode) graph_state();

            bdetect();
            // retreat branching vertex 'v'
            iso(v);
            v->deg++, v->type = BARE;
            outer_opt->mate = NULL, v->mate = NULL;
        }
        iso(outer_opt);
        outer_opt->deg++;
        outer_opt->type = BARE;
        if (_v_mode) graph_state();
    }
}
