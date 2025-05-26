
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define RED   1
#define BLACK 0

#define MAXQ     200000
#define MAXNODE  200000
#define MAXD     60          /* height < 40, leave margin             */
#define STKSIZE  64          /* height-bounded explicit stacks        */

/* ————————————————— GLOBAL AGGREGATES ————————————————— */
static int64_t TOT_P   = 0;                     /* Σ p              */
static int64_t SUM_DP  = 0;                     /* Σ depth·p        */
static int64_t SUM_D2P = 0;                     /* Σ depth²·p       */
static int64_t POP[MAXD][2] = {{0}};            /* by depth & colour*/

/* ————————————————— NODE DEFINITION ————————————————— */
typedef struct Node {
    int64_t key;           /* Greenhouse ID               */
    int64_t pop;           /* Population                  */
    int     depth;         /* Depth (root = 0)            */
    char    colour;        /* RED / BLACK                 */
    struct Node *left, *right, *parent;
} Node;
typedef struct { Node *n; } StackItem;    


static Node pool[MAXNODE + 5];
static int  poolCnt = 0;

/* NIL sentinel (single instance) */
static Node NIL_NODE = { .key = 0, .pop = 0, .depth = 0,
                         .colour = BLACK, .left = NULL, .right = NULL, .parent = NULL };
static Node *NIL  = &NIL_NODE;
static Node *ROOT = NULL;

/* ——————————— STATISTIC HELPERS ——————————— */
static inline void stat_add(Node *x) {
    int d = x->depth, c = x->colour;
    TOT_P      += x->pop;
    SUM_DP     += (int64_t)d * x->pop;
    SUM_D2P    += (int64_t)d * d * x->pop;
    POP[d][c]  += x->pop;
}
static inline void stat_sub(Node *x) {
    int d = x->depth, c = x->colour;
    TOT_P      -= x->pop;
    SUM_DP     -= (int64_t)d * x->pop;
    SUM_D2P    -= (int64_t)d * d * x->pop;
    POP[d][c]  -= x->pop;
}
static inline void stat_move_depth(Node *x,int oldd,int newd){
    if (oldd==newd) return;
    int64_t diff1 = (int64_t)newd - oldd;
    int64_t diff2 = (int64_t)newd*newd - (int64_t)oldd*oldd;
    SUM_DP  += diff1 * x->pop;
    SUM_D2P += diff2 * x->pop;
    POP[oldd][(int)x->colour] -= x->pop;
    POP[newd][(int)x->colour] += x->pop;
}
static inline void stat_flip_colour(Node *x,int newc){
    if (x->colour==newc) return;
    POP[x->depth][(int)x->colour]-=x->pop;
    POP[x->depth][newc]          +=x->pop;
    x->colour=(char)newc;
}

/* ——————————— NEW NODE ——————————— */
static Node* new_node(int64_t key,int64_t pop,int depth){
    Node *x = &pool[++poolCnt];
    x->key=key; x->pop=pop; x->depth=depth;
    x->colour=RED;
    x->left=x->right=x->parent=NIL;
    stat_add(x);
    return x;
}

static void shift_depth(Node *root,int delta){
    if(root==NIL) return;
    StackItem st[STKSIZE]; int top=0;
    st[top++].n=root;
    while(top){
        Node *n=st[--top].n;
        if(n==NIL) continue;
        stat_move_depth(n,n->depth,n->depth+delta);
        n->depth+=delta;
        st[top++].n=n->right;
        st[top++].n=n->left;
    }
}

/* ------------------- LEFT rotation -------------------
         x                     y
        / \                   / \
       α   y      --->       x   γ
          / \               / \
         β   γ             α   β
   depth shifts: x +1, y -1, α +1, γ -1
------------------------------------------------------- */
static void rotate_left(Node *x)
{
    Node *y = x->right;                    /* pivot */

    /* standard pointer moves */
    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL) ROOT = y;
    else if (x == x->parent->left) x->parent->left = y;
    else                          x->parent->right = y;
    y->left = x;
    x->parent = y;

    /* pivot depths */
    int odx = x->depth, ody = y->depth;
    x->depth++;                    /* down  */
    y->depth--;                    /* up    */
    stat_move_depth(x, odx, x->depth);
    stat_move_depth(y, ody, y->depth);

    /* α down (+1), γ up (-1) */
    if (x->left  != NIL) shift_depth(x->left,  +1);   /* α */
    if (y->right != NIL) shift_depth(y->right, -1);   /* γ */
}

/* ------------------- RIGHT rotation -------------------
         y                     x
        / \                   / \
       x   γ      --->       α   y
      / \                       / \
     α   β                     β   γ
   depth shifts: y +1, x -1, γ +1, α -1
------------------------------------------------------- */
static void rotate_right(Node *y)
{
    Node *x = y->left;                     /* pivot */

    /* standard pointer moves */
    y->left = x->right;
    if (x->right != NIL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NIL) ROOT = x;
    else if (y == y->parent->left) y->parent->left = x;
    else                          y->parent->right = x;
    x->right = y;
    y->parent = x;

    /* pivot depths */
    int ody = y->depth, odx = x->depth;
    y->depth++;                    /* down  */
    x->depth--;                    /* up    */
    stat_move_depth(y, ody, y->depth);
    stat_move_depth(x, odx, x->depth);

    /* γ down (+1), α up (-1) */
    if (y->right != NIL) shift_depth(y->right, +1);   /* γ */
    if (x->left  != NIL) shift_depth(x->left,  -1);   /* α */
}




/* ——————————— INSERT FIXUP ——————————— */
static void insert_fix(Node *z){
    while(z->parent->colour==RED){
        Node *gp=z->parent->parent;
        if(z->parent==gp->left){
            Node *u=gp->right;
            if(u->colour==RED){
                stat_flip_colour(z->parent,BLACK);
                stat_flip_colour(u,BLACK);
                stat_flip_colour(gp,RED);
                z=gp;
            }else{
                if(z==z->parent->right){ z=z->parent; rotate_left(z); }
                stat_flip_colour(z->parent,BLACK);
                stat_flip_colour(gp,RED);
                rotate_right(gp);
            }
        }else{
            Node *u=gp->left;
            if(u->colour==RED){
                stat_flip_colour(z->parent,BLACK);
                stat_flip_colour(u,BLACK);
                stat_flip_colour(gp,RED);
                z=gp;
            }else{
                if(z==z->parent->left){ z=z->parent; rotate_right(z); }
                stat_flip_colour(z->parent,BLACK);
                stat_flip_colour(gp,RED);
                rotate_left(gp);
            }
        }
    }
    stat_flip_colour(ROOT,BLACK);
}

/* ——————————— SEARCH ——————————— */
static Node* tree_search(int64_t key){
    Node *x=ROOT;
    while(x!=NIL && key!=x->key)
        x=(key<x->key)?x->left:x->right;
    return x;
}

/* ——————————— INSERT (with empty-tree fix) ——————————— */
static void rb_insert(int64_t key,int64_t pop){
    if(ROOT==NIL){                       /* first real node */
        ROOT=new_node(key,pop,0);
        ROOT->parent=NIL;
        stat_flip_colour(ROOT,BLACK);
        return;
    }
    Node *y=NIL,*x=ROOT; int depth=0;
    while(x!=NIL){
        if(key==x->key) return;          /* duplicate */
        y=x; depth++;
        x=(key<x->key)?x->left:x->right;
    }
    Node *z=new_node(key,pop,depth);
    z->parent=y;
    if(key<y->key) y->left=z; else y->right=z;
    insert_fix(z);
}

/* ——————————— TRANSPLANT ——————————— */
static void transplant(Node *u,Node *v){
    if(u->parent==NIL) ROOT=v;
    else if(u==u->parent->left) u->parent->left=v;
    else                        u->parent->right=v;
    v->parent=u->parent;
}


/* ——————————— TREE MINIMUM ——————————— */
static Node* tree_minimum(Node *x){
    while(x->left!=NIL) x=x->left;
    return x;
}

/* ——————————— DELETE FIXUP ——————————— */
static void delete_fix(Node *x){
    while(x!=ROOT && x->colour==BLACK){
        if(x==x->parent->left){
            Node *w=x->parent->right;
            if(w->colour==RED){
                stat_flip_colour(w,BLACK);
                stat_flip_colour(x->parent,RED);
                rotate_left(x->parent);
                w=x->parent->right;
            }
            if(w->left->colour==BLACK && w->right->colour==BLACK){
                stat_flip_colour(w,RED);
                x=x->parent;
            }else{
                if(w->right->colour==BLACK){
                    stat_flip_colour(w->left,BLACK);
                    stat_flip_colour(w,RED);
                    rotate_right(w);
                    w=x->parent->right;
                }
                w->colour=x->parent->colour;
                stat_flip_colour(x->parent,BLACK);
                stat_flip_colour(w->right,BLACK);
                rotate_left(x->parent);
                x=ROOT;
            }
        }else{
            Node *w=x->parent->left;
            if(w->colour==RED){
                stat_flip_colour(w,BLACK);
                stat_flip_colour(x->parent,RED);
                rotate_right(x->parent);
                w=x->parent->left;
            }
            if(w->right->colour==BLACK && w->left->colour==BLACK){
                stat_flip_colour(w,RED);
                x=x->parent;
            }else{
                if(w->left->colour==BLACK){
                    stat_flip_colour(w->right,BLACK);
                    stat_flip_colour(w,RED);
                    rotate_left(w);
                    w=x->parent->left;
                }
                w->colour=x->parent->colour;
                stat_flip_colour(x->parent,BLACK);
                stat_flip_colour(w->left,BLACK);
                rotate_right(x->parent);
                x=ROOT;
            }
        }
    }
    stat_flip_colour(x,BLACK);
}

/* ——————————— DELETE ——————————— */
static void rb_delete(int64_t key){
    Node *z=tree_search(key);
    if(z==NIL) return;
    Node *y=z,*x; int ycol=y->colour;
    stat_sub(z);
    if(z->left==NIL){
        x=z->right;
        transplant(z,x);
        if(x!=NIL) shift_depth(x,-1);
    }else if(z->right==NIL){
        x=z->left;
        transplant(z,x);
        if(x!=NIL) shift_depth(x,-1);
    }else{
        y=tree_minimum(z->right); ycol=y->colour; x=y->right;
        if(y->parent==z){
            x->parent=y;
        }else{
            transplant(y,x);
            if(x!=NIL) shift_depth(x,-1);
            y->right=z->right; y->right->parent=y;
        }
        transplant(z,y);
        int delta=z->depth - y->depth;
        if(delta) shift_depth(y,delta);
        y->left=z->left; y->left->parent=y;
        stat_flip_colour(y,z->colour);
        //stat_add(y);
    }
    if(ycol==BLACK) delete_fix(x);
}

/* ——————————— OP 3  RELOCATE ——————————— */
static void relocate(int64_t u_id,int64_t v_id,int64_t p){
    Node *u=tree_search(u_id),*v=tree_search(v_id);
    if(u==NIL||v==NIL||u==v) return;
    int64_t mv=(u->pop<p)?u->pop:p;
    if(mv==0) return;
    stat_sub(u); u->pop-=mv; stat_add(u);
    stat_sub(v); v->pop+=mv; stat_add(v);
}

/* ——————————— SUBTREE HELPERS ——————————— */
static int count_desc(Node *x){
    if(x==NIL) return 0;
    Node *st[STKSIZE]; int top=0, cnt=0;
    st[top++]=x;
    while(top){
        Node *n=st[--top];
        if(n==NIL) continue;
        if(n!=x) ++cnt;
        st[top++]=n->left;
        st[top++]=n->right;
    }
    return cnt;
}
static void add_pop_subtree(Node *x,int64_t delta){
    if(x==NIL) return;
    Node *st[STKSIZE]; int top=0;
    st[top++]=x;
    while(top){
        Node *n=st[--top];
        if(n==NIL) continue;
        stat_sub(n); n->pop+=delta; stat_add(n);
        st[top++]=n->left;
        st[top++]=n->right;
    }
}

/* ——————————— OP 4  EVACUATE  ———————————
   Move all people out of x, giving ⌊px/(|S|+1)⌋ to each *descendant*
   (NOT to x itself) and the remainder to the parent (or lost if root). */
static void evacuate(int64_t id)
{
    Node *x = tree_search(id);
    if (x == NIL || x->pop == 0) return;

    int  desc   = count_desc(x);                 /* |S|  (excluding x) */
    int64_t px  = x->pop;
    int64_t share = px / (desc + 1);             /* ⌊ px / (|S|+1) ⌋   */

    /* give “share” to every real descendant, but **not** to x itself */
    if (share) {
        if (x->left  != NIL) add_pop_subtree(x->left,  share);
        if (x->right != NIL) add_pop_subtree(x->right, share);
    }

    int64_t distributed = share * desc;          /* total sent to S     */
    int64_t remain      = px - distributed;      /* R in the spec       */

    /* parent gets the remainder, unless x is the root */
    if (x->parent != NIL) {
        stat_sub(x->parent);
        x->parent->pop += remain;
        stat_add(x->parent);
    }

    /* x is now empty */
    stat_sub(x);
    x->pop = 0;
    stat_add(x);
}


/* ——————————— DISTANCE  (OP 5) ——————————— */
static inline int64_t distance_nodes(Node *u,Node *v){
    if(u==NIL||v==NIL) return -1;
    if(u==v) return 0;
    if(u->depth==v->depth)
        return (u->colour==v->colour)?0:2;
    return llabs((int64_t)u->depth - v->depth);
}

/* ——————————— ACCESS INDEX (OP 6) ——————————— */
static int64_t access_index(Node *x){
    if(x==NIL) return -1;
    int d=x->depth;
    int64_t base = SUM_D2P - 2LL*d*SUM_DP + 1LL*d*d*TOT_P;
    int64_t extra= 4LL*POP[d][1-x->colour];
    return base+extra;
}

/* ——————————— MAIN ——————————— */
int main(void){
    int Q; if(scanf("%d",&Q)!=1) return 0;
    ROOT=NIL;

    while(Q--){
        int op; scanf("%d",&op);
        if(op==1){                    /* Add */
            int64_t x,p; scanf("%"SCNd64"%"SCNd64,&x,&p);
            rb_insert(x,p);
        }else if(op==2){              /* Remove */
            int64_t x;  scanf("%"SCNd64,&x); rb_delete(x);
        }else if(op==3){              /* Relocate */
            int64_t u,v,p; scanf("%"SCNd64"%"SCNd64"%"SCNd64,&u,&v,&p);
            relocate(u,v,p);
        }else if(op==4){              /* Evacuate */
            int64_t x;  scanf("%"SCNd64,&x); evacuate(x);
        }else if(op==5){              /* Distance */
            int64_t u,v; scanf("%"SCNd64"%"SCNd64,&u,&v);
            printf("%"PRId64"\n",distance_nodes(tree_search(u),tree_search(v)));
        }else if(op==6){              /* Access Index */
            int64_t x;  scanf("%"SCNd64,&x);
            printf("%"PRId64"\n",access_index(tree_search(x)));
        }
    }
    return 0;
}
