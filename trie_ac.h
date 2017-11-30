//
// Created by muki on 11/30/17.
//

#ifndef LAB2_MATRIXPART_TRIE_AC_H
#define LAB2_MATRIXPART_TRIE_AC_H


typedef struct actreenode {
    char ch;
    int matchid;
    struct actreenode *children, *sibling;
} ACTREE_NODE, *AC_TREE;


typedef struct {
    AC_TREE tree;
    char *T;
    int N, c;
} AC_STRUCT;


AC_STRUCT *ac_alloc(void);
int ac_add_string(AC_STRUCT *node, char *P, int M, int *id);
void ac_free(AC_STRUCT *node);

#endif //LAB2_MATRIXPART_TRIE_AC_H
