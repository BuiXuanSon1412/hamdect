#define MAXV 100
#define BARE 0
#define OUTER 1
#define INNER 2
#define MAXC 20

typedef struct Vertex {
    char name[16];
    int deg;
    int type;
    
    struct Arc* a;                  // 'single linked list' of arcs starting from  
    struct Vertex* mate;            // other end of its segment
    struct Vertex* comate;          // 'mate' and 'comate': 2 neighbor of 2-bare vertex
    struct Vertex* rlink;           // right link
    struct Vertex* llink;           // left link
} Vertex;

typedef struct Arc {
    char* name;
    Vertex* u;                      // starting vertex
    Vertex* v;                      // ending vertex
    struct Arc* next;               // pointer to next arc             
} Arc;


typedef struct Graph {
    int n;                          // number of vertices
    struct Vertex v[MAXV];          // each vertex includes its adjacent vertice(s)
} Graph;

/*
void add_arc(Vertex u, Arc *a) {
    a->next = u.a;
    u.a = a;
}
*/

