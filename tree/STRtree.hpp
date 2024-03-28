// https://infolab.usc.edu/csci599/Fall2001/paper/rstar-tree.pdf r* tree
// http://www-db.deis.unibo.it/courses/SI-LS/papers/Gut84.pdf r tree
// https://apps.dtic.mil/sti/tr/pdf/ADA324493.pdf STR tree

#pragma once
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <bits/stdc++.h>

template <typename T>
struct Branch;

template <typename T>
struct Node {
    int count;
    struct Branch<T> **children; // array of ptr to branches

    Node(int M) : count{0} {
        this->children = (struct Branch<T> **)std::malloc(M * sizeof(struct Branch<T>*));
    }

    Node(Node &n){
        this->count = n.count;
        this->children = n.children;
    }
};

template <typename T>
struct Branch {
    int rect[2][2]; // [0] - lower left point, [1] - upper right point
    struct Node<T> *children; // a ptr to an internal node, NULL when leaf
    T data;

    Branch(){}

    Branch(int rect[2][2]){
        std::memcpy(this->rect,rect,4 * sizeof(int));
    }

    Branch<T>(Branch<T> &b){
        std::memcpy(this->rect,b.rect,4 * sizeof(int));
        this->data = b.data;
        this->children = b.children;
    }

    void set_rect(){
        int tmp[2][2];
        assert(this->children != NULL);
        struct Node<T> *nptr = this->children;
        std::memcpy(tmp,nptr->children[0]->rect, 4 * sizeof(int));
        for(int i = 1; i < nptr->count; i++){
            if(nptr->children[i]->rect[0][0] < tmp[0][0])
                tmp[0][0] = nptr->children[i]->rect[0][0];
            if(nptr->children[i]->rect[0][1] < tmp[0][1])
                tmp[0][1] = nptr->children[i]->rect[0][1];
            if(nptr->children[i]->rect[1][0] > tmp[1][0])
                tmp[1][0] = nptr->children[i]->rect[1][0];
            if(nptr->children[i]->rect[1][1] > tmp[1][1])
                tmp[1][1] = nptr->children[i]->rect[1][1];
        }
        std::memcpy(this->rect, tmp, 4 * sizeof(int));
    }
};

template<typename T, int i>
bool comp(struct Branch<T> *a, struct Branch<T> *b){
    return (a->rect[0][i] + a->rect[1][i]) / 2 < (b->rect[0][i] + b->rect[1][i]) / 2;
}

template <typename T, int N>
struct STRTREE {
    struct Node<T> *root;

    // rect is an array of bounding boxes
    STRTREE(int ***rect, T *data, size_t num){
        struct Branch<T> **bbs = (struct Branch<T> **)std::malloc(num * sizeof(struct Branch<T>*));
        for(int i = 0; i < num; i++){
            struct Branch<T> *tmp = new struct Branch<T>(((int (*)[2][2])rect)[i]);
            tmp->data = data[i];
            tmp->children = NULL;
            bbs[i] = tmp;
        }

        int S = std::ceil(std::sqrt(num / ((float)N))); // ceil(sqrt(num_rectangles / num_data_in_leaf)) vertical slices

        std::stable_sort(bbs, bbs + num, comp<T,0>); //sort by x
        for(int i = 1; i < S; i++){
            int upper = (S * N * i < num)? S * N * i : num, lower = S * N * (i - 1);
            std::stable_sort(bbs + lower, bbs + upper, comp<T,1>); // sort by y for each tile
        }

        int num_nodes_in_this_level = std::ceil(num / N), current_num_node = 0, num_branch, branch_count = 0, num_in_leaf = 0;
        struct Node<T> **nodes_in_this_level = (struct Node<T>**)std::malloc(num_nodes_in_this_level * sizeof(struct Node<T>*));
        // constructing the tree
        while(num_nodes_in_this_level > 1){
            // constructing each level
            num_branch = num;
            while(branch_count < num_branch){
                struct Node<T> *tmp = new struct Node<T>(N);
                // pack branches into internal nodes
                for(num_in_leaf = 0; branch_count < num_branch && num_in_leaf < N; branch_count++, num_in_leaf++){
                    tmp->children[num_in_leaf] = bbs[branch_count];
                    tmp->count++;
                }
                nodes_in_this_level[current_num_node] = tmp;
                current_num_node++;
            }
            // create branch for each internal node
            for(int i = 0; i < current_num_node; i++){
                struct Branch<T> *tmp = new struct Branch<T>();
                tmp->children = nodes_in_this_level[i];
                (*tmp).set_rect();
                bbs[i] = tmp;
            }

            num = num_nodes_in_this_level;
            S = std::ceil(std::sqrt(num / ((float)N))); // repeat the process
            std::stable_sort(bbs, bbs + num, comp<T,0>); // sort by x
            for(int i = 1; i < S; i++){
                int upper = (S * N * i < num)? S * N * i : num, lower = S * N * (i - 1);
                std::stable_sort(bbs + lower, bbs + upper, comp<T,1>); // sort by y for each tile
            }
            // set appropriate var
            num_nodes_in_this_level = std::ceil(num / N);
            current_num_node = 0;
            branch_count = 0;
        }

        // creat root
        this->root = new struct Node<T>(N);
        for(int i = 0; i < num; i++){
            this->root->children[i] = bbs[i];
            this->root->count++;
        }

        std::free(nodes_in_this_level);
        std::free(bbs);
    }

    void print_bb(int c, struct Node<T> *nptr){
        for(int i = 0; i < c; i++){
            printf("  ");
        }
        for(int i = 0; i < nptr->count; i++){
            struct Branch<T> *tmp = nptr->children[i];
            printf("%d, %d | %d, %d\n",tmp->rect[0][0], tmp->rect[0][1], tmp->rect[1][0], tmp->rect[1][1]);
            if(nptr->children[i]->children)
                print_bb(c + 1, nptr->children[i]->children);
        }
    }

    void print_tree(){
        print_bb(0, this->root);
    }

    bool is_leaf(struct Node<T> *n){
        return n->children[0]->children == NULL;
    }

    void destroy_node(struct Node<T> *n){
        if(!is_leaf(n)){
            for(int i = 0; i < n->count; i++){
                destroy_node(n->children[i]->children);
                delete n->children[i]->children;
            }
        }
        for(int i = 0; i < n->count; i++){
            delete n->children[i];
        }
        std::free(n->children);
    }

    void destroy(){
        destroy_node(this->root);
        delete this->root;
    }
};

bool intersect(int bb1[2][2], int bb2[2][2]) {
    return bb1[0][0] < bb2[1][0] && bb1[1][0] > bb2[0][0] && bb1[0][1] < bb2[1][1] && bb1[1][1] > bb2[0][1];
}