//
// Created by muki on 11/30/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "trie_ac.h"
#define MAXSIZE 100000

int main() {
    int begin_create_tree=clock();//��¼����ʼʱ��
    char buffer[MAXSIZE] = {0};//�ļ�������
    memset(buffer, 0, sizeof(buffer));
    FILE *fp = fopen("pattern.txt", "r");
    if(NULL == fp)
    {
        printf("failed to open pattern.txt\n");
        return 1;
    }
    AC_STRUCT *tree= ac_alloc();
    int id=1;
    while(fgets(buffer,MAXSIZE, fp)!=NULL)//���ж�ȡ
    {
        if(buffer[strlen(buffer)-1]=='\n') {//���������лس�
            ac_add_string(tree, buffer, strlen(buffer) - 1, id);//����ȥ���س�
        }
        else{
            ac_add_string(tree, buffer, strlen(buffer), id);
        }
        id++;
    }
    fclose(fp);
    int finish_create_tree=clock();//��¼��������ʱ��
    printf("create tree runtime:%d\n",finish_create_tree-begin_create_tree);
    ac_prep(tree);//����ʧЧ���������
    fp = fopen("string.txt", "r");
    if(NULL == fp)
    {
        printf("failed to open string.txt\n");
        return 1;
    }
    int match_length;
    int match_id;
    char *s;
    long int match_time=0;
    long int pos=0;
    FILE *out=fopen("data\\result.txt", "w");
    char key[1000]={0};
    while(fgets(buffer,100000, fp)!=NULL)//���ж�ȡ
    {
        pos+=strlen(buffer);//����ƫ����
        ac_search_init(tree,buffer,strlen(buffer));
        s = ac_search(tree, &match_length, &match_id);
        while (s != NULL){
            match_time++;
            strncpy(key, s, (size_t) match_length);//��¼��ǰƥ�䵽��ģʽ��
            key[match_length]='\0';
            fprintf(out, "%s %ld\n", key, (long) (pos - strlen(s)));//���ƥ���ģʽ����ƫ��
            s = ac_search(tree, &match_length, &match_id);
        }
        memset(buffer, 0, sizeof(buffer));
    }
    int finish_search=clock();//��¼��������ʱ��
    printf("search runtime:%d\n",finish_search-finish_create_tree);
    printf("match times:%ld\n",match_time);
    fclose(fp);
    fclose(out);
    ac_free(tree);
    return 0;
}

/*
 * ac_alloc
 *��ʼ���ֵ���������ͷ�ڵ��ڴ�
 */
AC_STRUCT *ac_alloc(void)
{
    AC_STRUCT *node;

    if ((node = malloc(sizeof(AC_STRUCT))) == NULL)
        return NULL;
    memset(node, 0, sizeof(AC_STRUCT));

    if ((node->tree = malloc(sizeof(ACTREE_NODE))) == NULL) {
        free(node);
        return NULL;
    }
    memset(node->tree, 0, sizeof(ACTREE_NODE));

    return node;
}


/*
 * ac_add_string
 *�����ֵ�����ÿ�ν�һ��ģʽ����ӽ��ֵ���
 * Parameters:   node      -  �ֵ���ͷ�ڵ�
 *               P         -  ģʽ��
 *               M         -  ģʽ������
 *               id        -  ģʽ��ID
 *
 * Returns:  ��0Ϊ�ɹ���0Ϊʧ��
 */
int ac_add_string(AC_STRUCT *node, char *P, int M, int id)
{
    int i, j, new_size;
    AC_TREE tnode, child, back, new_node, list, tail;


    if (node->errorflag || id == 0)
        return 0;
    P--;
    /*
     * ���� Plengths�����ڴ�
     */
    if (node->Psize <= id) {//���Plengths���鲻������
        if (node->Psize == 0) {//��ʼ���ȷ���16���ֽ�
            new_size = (id >= 16 ? id + 1 : 16);
            node->Plengths = malloc(new_size * sizeof(int));
        }
        else {//����Plengths����ռ�����Ϊԭ��������
            new_size = node->Psize + id + 1;
            node->Plengths = realloc(node->Plengths, new_size * sizeof(int));
        }
        if (node->Plengths == NULL) {
            node->errorflag = 1;
            return 0;
        }
        for (i=node->Psize; i < new_size; i++)//��ʼ���µ�Plengths�ռ�
            node->Plengths[i] = 0;
        node->Psize = new_size;//����Plengths���鳤����Ϣ
    }

    if (node->Plengths[id] != 0) {
        fprintf(stderr, "Error in Aho-Corasick preprocessing."
                "Duplicate identifiers\n");
        return 0;
    }
    /*
     * ��ģʽ����ӽ��ֵ���
     */
    tnode = node->tree;
    for (i=1; i <= M; i++) {
        back = NULL;
        child = tnode->children;//���մ�С˳����ҵ�ǰ�ַ��Ƿ������ֵ�������ƥ��
        while (child != NULL && child->ch < P[i]) {
            back = child;
            child = child->sibling;
        }
        if (child == NULL || child->ch != P[i])//ֱ����ƥ���λ��
            break;

        tnode = child;//��¼��ƥ���λ��
    }
    /*
     * ���ģʽ��ֻ�в��������У������ʣ�µĲ���
     */
    if (i <= M) {
        list = tail = NULL;
        for (j=i; j <= M; j++) {
            if ((new_node = malloc(sizeof(ACTREE_NODE))) == NULL)//�½��ڵ�
                break;
            memset(new_node, 0, sizeof(ACTREE_NODE));
            new_node->ch = P[j];//�����½ڵ���ַ�

            if (list == NULL)//�·�֧��ͷ�ڵ�
                list = tail = new_node;
            else{
                tail->children=new_node;//�½ڵ�����·�֧��ĩ��
                tail=tail->children;//�����·�֧��ĩ��
            }
        }
        if (j <= M) {//����ʧ��
            while (list != NULL) {//�н������������·�֧�������ͷŵ�
                tail = list->children;
                free(list);
                list = tail;
            }
            return 0;//���ش���
        }

        list->sibling = child;//���·�֧��������
        if (back == NULL)
            tnode->children = list;
        else
            back->sibling = list;

        tnode = tail;
    }

    tnode->matchid = id;//���Ľڵ�Ϊ����ƥ��Ľڵ㣬��¼ģʽ��ID
    node->Plengths[id] = M;
    node->ispreprocessed = 0;

    return 1;
}

/*
 * ac_prep
 *
 * Ϊ�ֵ�������ʧЧ���������
 *
 * Parameters:  node  -  �ֵ���ͷ�ڵ�
 *
 * Returns: ��0Ϊ�ɹ���0Ϊʧ��
 */
int ac_prep(AC_STRUCT *node)
{
    char x;
    AC_TREE v, vprime, w, wprime, root, front, back, child;

    if (node->errorflag)
        return 0;

    /*
     * ���� `faillink'�� `outlink'���ֵ������й�����ȱ���
     *`faillink'ָ�뽫��ʱ��¼�����������Ľڵ㡣
     * `outlink'ָ�뽫��ʱ��¼�ýڵ�ĸ��ڵ㡣
     */
    root = node->tree;

    front = back = root;
    front->faillink = NULL;
    front->outlink = NULL;

    while (front != NULL) {
        v = front;
        x = v->ch;
        vprime = v->outlink;

        /*
         * ���ֵ����Ľڵ������
         */
        for (child=v->children; child != NULL; child=child->sibling) {
            child->outlink = v;//��¼���ڵ�
            back->faillink = child;//Ϊ��һ���ڵ��¼����һ��Ҫ�����Ľڵ�
            back = child;
        }
        back->faillink = NULL;

        front = front->faillink;
        v->faillink = v->outlink = NULL;

        /*
         * ����ʧЧ���������
         */
        if (v == root)//��ǰ�ڵ�Ϊ���ڵ���ʲô������
            ;
        else if (vprime == root)//���ڵ�Ϊ���ڵ�
            v->faillink = root;//ʧЧ�ڵ�ֱ��Ϊ���ڵ�
        else {
            wprime = NULL;
            w = vprime->faillink;//���ڵ��ʧЧ�ڵ�

            while (1) {
                wprime = w->children;
                while (wprime != NULL && wprime->ch < x)//���ڵ��ʧЧ�ڵ���ӽڵ���Ѱ��ƥ��
                    wprime = wprime->sibling;

                if ((wprime != NULL && wprime->ch == x) || w == root)//�ҵ���ƥ��Ľڵ� ���� �Ѿ����ص����ڵ�
                    break;

                w = w->faillink;//ƥ��ʧ������������ʧЧ�ڵ�

            }
            if (wprime != NULL && wprime->ch == x)//�ҵ���ƥ��Ľڵ�
                v->faillink = wprime;//����ʧЧ�ڵ�
            else
                v->faillink = root;

            if (v->matchid != 0) {//�����ǰ�ڵ���ģʽ������ڵ�
                if (v->faillink->matchid != 0)//���ʧЧ�ڵ�Ҳ��ƥ��
                    v->outlink = v->faillink;//����ڵ�ΪʧЧ�ڵ�
                else
                    v->outlink = v->faillink->outlink;//����ڵ�ΪʧЧ�ڵ������ڵ�
            }
        }
    }

    node->ispreprocessed = 1;
    node->initflag = 0;

    return 1;
}


/*
 * ac_search_init
 *���ҳ�ʼ��
 * Parameters:  node  -  �ֵ���ͷ�ڵ�
 *              T     -  ��ƥ����ַ���
 *              N     -  ��ƥ����ַ����ĳ���
 *
 * Returns:  ��
 */
void ac_search_init(AC_STRUCT *node, char *T, int N)
{
    if (node->errorflag)
        return;
    else if (!node->ispreprocessed) {
        fprintf(stderr, "Error in Aho-Corasick search.  The preprocessing "
                "has not been completed.\n");
        return;
    }

    node->T = T - 1;
    node->N = N;
    node->c = 1;
    node->w = node->tree;//�Ӹ��ڵ㿪ʼ
    node->output = NULL;
    node->initflag = 1;
    node->endflag = 0;
}


/*
 * ac_search
 *ͨ���ֵ�������ƥ���ģʽ��ID�ͳ���
 * Parameters:  node           -  Ԥ����õ��ֵ���
 *              length_out     -  ƥ���ģʽ�����ȵ�ָ��
 *              id_out         -  ƥ���ģʽ����ָ��
 *
 * Returns:  ƥ�䴮��ƥ�䵽��ƫ��λ�ú�ƥ�䳤��
 */
char *ac_search(AC_STRUCT *node, int *length_out, int *id_out)
{
    int c, N, id;
    char *T;
    AC_TREE w, wprime, root;

    if (node->errorflag)//֮ǰ�����д�
        return NULL;
    else if (!node->ispreprocessed) {//�ֵ���δ��Ԥ����
        fprintf(stderr, "Error in Aho-Corasick search.  The preprocessing "
                "has not been completed.\n");
        return NULL;
    }
    else if (!node->initflag) {//�޳��β��ҵļ�¼
        fprintf(stderr, "Error in Aho-Corasick search.  ac_search_init was not "
                "called.\n");
        return NULL;
    }
    else if (node->endflag)//�ô�ƥ���ѽ���
        return NULL;

    T = node->T;
    N = node->N;
    c = node->c;
    w = node->w;//�ϴ����е��Ľڵ�
    root = node->tree;

    /*
     * ����ϴ���ƥ�䣬���˳��������Ӳ����Ƿ�������ƥ��
     */
    if (node->output != NULL) {//�ϴ���ƥ��
        node->output = node->output->outlink;//����������ڵ�

        if (node->output != NULL) {//����Ҳ��ƥ��
            id = node->output->matchid;//�������
            if (id_out)
                *id_out = id;
            if (length_out)
                *length_out = node->Plengths[id];

            return &T[c] - node->Plengths[id];
        }
    }

    /*
     * ƥ���㷨
     */
    while (c <= N) {//������
        wprime = w->children;
        while (wprime != NULL && wprime->ch != T[c])//����ƥ��
            wprime = wprime->sibling;

        if (wprime == NULL) {//ƥ��ʧ��
            if (w == root)//��ǰ�ڵ�Ϊ���ڵ㣨��һ���ַ��Ͳ�ƥ�䣩
                c++;//ƥ����һ���ַ�
            else {//��֮ǰ��ƥ�䵽���ַ���
                w = w->faillink;//��ת��ʧЧ�ڵ�
            }
        }
        else {//ƥ��ɹ�
            c++;//��һ���ַ�
            w = wprime;//�ֵ���������һ���ڵ�

            if (w->matchid != 0)//��ǰ�ڵ���ģʽ��ƥ��
                node->output = w;//����ڵ�Ϊ��ǰ�ڵ�
            else if (w->outlink != NULL) {//��ǰ�ڵ㲻��ƥ��ڵ㵫������ڵ�
                node->output = w->outlink;//��������ڵ�
            }

            if (node->output != NULL) {//�����
                id = node->output->matchid;
                if (id_out)
                    *id_out = id;
                if (length_out)
                    *length_out = node->Plengths[id];

                node->w = w;//���µ�ǰ�ڵ�
                node->c = c;//����ƫ��

                return &T[c] - node->Plengths[id];
            }
        }
    }

    node->c = c;
    node->endflag = 1;

    return NULL;
}


/*
 * ac_free
 * �ͷ��ֵ�����Plengths����Ŀռ�
 *
 * Parameters:   node  -  �ֵ���ͷ�ڵ�
 *
 * Returns:  ��
 */
void ac_free(AC_STRUCT *node)
{
    AC_TREE front, back, next;

    if (node == NULL)
        return;

    if (node->tree != NULL) {//������
        front = back = node->tree;
        while (front != NULL) {//������
            back->sibling = front->children;
            while (back->sibling != NULL)
                back = back->sibling;

            next = front->sibling;
            free(front);//�ͷ����ڵ�
            front = next;
        }
    }

    if (node->Plengths != NULL)
        free(node->Plengths);//�ͷ� Plengths����ռ�

    free(node);
}
