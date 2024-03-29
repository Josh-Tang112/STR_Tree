#include <cstdio>
#include <cstdlib>

#include "tree/STRtree.hpp"

int main(int argc, char *argv[])
{
    int points[5][2] = {{20,6},{99,70},{20,60},{4,70},{70,63}}, bbs[5][2][2], data[5] = {1,2,3,4,5};
    for(int i = 0; i < 5; i++){
        int idx1 = i, idx2 = (i + 1) % 5;
        if(points[idx1][0] < points[idx2][0]){
            bbs[i][0][0] = points[idx1][0]; 
            bbs[i][1][0] = points[idx2][0];
        } else {
            bbs[i][0][0] = points[idx2][0];
            bbs[i][1][0] = points[idx1][0]; 
        }
        if(points[idx1][1] < points[idx2][1]){
            bbs[i][0][1] = points[idx1][1]; 
            bbs[i][1][1] = points[idx2][1];
        } else {
            bbs[i][0][1] = points[idx2][1];
            bbs[i][1][1] = points[idx1][1]; 
        }
    }

    // printf("bounding boxes\n");
    // for(int i = 0; i < 5; i++){
    //     printf("%d, %d | %d, %d\n",bbs[i][0][0], bbs[i][0][1], bbs[i][1][0], bbs[i][1][1]);
    // }
    // printf("--------------------\n");

    struct STRTREE<int,2> tree((int***)bbs, data, 5);
    tree.print_tree();

    int q[2][2] = {{20,30},{60,40}};
    std::vector<struct Branch<int> *> *res = tree.query(q);
    for(auto b : *res){
        printf("%d\n",b->data);
    }

    delete res;
    tree.destroy();

    return 0;
}