/*************************************************************************
	> File Name: csttree.h
	> Author: 
	> Mail: 
	> Created Time: Wed 20 Sep 2017 01:55:14 PM KST
 ************************************************************************/

#ifndef _CSTTREE_H
#define _CSTTREE_H

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ROWS 7
#define LISTS 4
#define LEVL_NODE_NUM_MAX
#define KV_T 
#define NODE_KV_NUM
#define MAXARR_KEY_NUM 7 

namespace stx{

typedef struct
{

}dm_meta;

typedef struct
{

}pm_meta;

typedef struct
{
    KV_T key;
    KV_T value;
}kv_pair;

typedef struct
{
    KV_T maxarr[MAXARR_KEY_NUM];
    key_node* child_node_array[8];
    kv_list* data_node_addr;//double dimensional array with structure or something...
    int bf;
    bool exist;
}key_node;

/*typedef struct
{
    key_node key_array[8];        
}key_node_array;*/

typedef struct
{
    kv_pair kv_data[ROWS][LISTS];
}kv_list;
 
class csttree
{
public:
    
keycompare(KV_T key1, KV_T key2)
{
    return key1 < key2 ? -1 : key1 == key2 ? 0 : 1;
}



bool Insert(KV_T key, KV_T value)
{
    if(root = NULL)
    {
        root =  (key_node*)malloc(sizeof(key_node));
        memset(root, 0, sizeof(key_node));
        root->kv_list = (kv_list*)malloc(sizeof(kv_list));
        memset(root->kv_list, 0, sizeof(kv_list));
        root->maxarr[0] = key;
        root->child_node_array = NULL;
        root->data_node_addr->kv_data[0][0].key = key;
        root->data_node_addr->kv_data[0][0].value = value;
        root->bf = 0;
        root->exist = true;
        return true;
    }
    else
    {
        key_node* tnode = root;
        bool ret = _insert(key, value, tnode);
        return ret;
    }
}

char array_find(char index)
{
    
}

bool _insert(KV_T key, KV_T value, key_node* tnode)
{
    KV_T keymin = Findmin(tnode, 0);
    KV_T keymax = Findmax(tnode, 0);
    char diff1 = keycompare(key, keymin);
    char diff2 = keycompare(key, keymax);
    if(diff1 <= 0)
}

KV_T Findmin(key_node* tnode, char count)
{
    if(tnode->exist = true)
    {
        if(tnode->maxarr[0] == 0)
        {
            perror("error : the root is 0 but the exist is true");
            return NULL;
        }
        else if(tnode->maxarr[count*2+1] == 0)
        {
            return tnode->data_node_addr->kv_data[0][count].key;
        }
        else if((tnode->maxarr[count*2+1] > 0)&&(count < 3))
        {
            return Findmin(tnode, count*2+1)
        }
        else
        {
            return tnode->data_node_addr->kv_data[0][count].key;
        }
    }
    else
    {
        return NULL;
    }
}

KV_T Findmax(key_node* tnode, char count)
{
    count = 0;
    if(tnode->exist = true)
    {
        if(tnode->maxarr[0] == 0)
        {
            perror("error : the root is 0 but the exist is true");
            return NULL;
        }
        else if(tnode->maxarr[count*2+2] == 0)
        {
            return tnode->maxarr[count];
        }
        else if(tnode->maxarr[count*2+2] > 0)&&(count < 3)
        {
            return Findmax(tnode, count*2+2)
        }
        else
        {
            return tnode->maxarr[count];
        }
    }
    else
    {
        return NULL;
    }
}

void BalanceTree(key_node* tnode)
{
    int minheght
    
}


int Balancefactor(key_node* tnode)
{
    int height[8];
    memset(height,0,sizeof(height));
    int count;
    for(count = 0; count < 8; count++){
    while(tnode->child_node_array[count].maxarr[0] != 0)
    {
        height[count]++;
        if(tnode->child_node_array[count]->maxarr[1] || tnode->child_node_array[count]->maxarr[2])
        {
            h0++;
            if(tnode->child_node_array[count]->maxarr[3] || tnode->child_node_array[count]->maxarr[4] || tnode->child_node_array[count]->maxarr[5] || tnode->child_node_array[count]->maxarr[6])
            {
                h0++;
                tnode = tnode->child_node_array[count];
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    quicksort(height, 0, 7);
    
}

void quicksort(int *a, int left, int right)
{
    if(left >= right)
    {
        return;
    }
    int i = left;
    int j = right;
    int key = a[left];
    while(i < j)
    {
        while(i < j && key <= a[j])
        {
            j--;
        }
        a[i] = a[j];
        while(i < j && key >= a[i])
        {
            i++;
        }
        a[j] = a[i];
    }
    a[i] = key;
    quicksort(a, left, i-1)
    quicksort(a, i+1, right);
}



/*key_node* FindMax(key_node* tnode)
{
    if(tnode != NULL)
    {
        if(tnode->rightchild == NULL)
        {
            return tnode;
        }
        else
        {
            return FindMax(tnode->rightchild);
        }
    }
    fprintf(stdout,"FUNCTION %s : error!\n",__func__);
    return NULL;
}*/

/*KV_T FindMinKey(key_node* tnode)
{
    if(tnode->maxarr[3] > 0)
    {
        return tnode->maxarr[3];
    }
    else if(tnode->maxarr[4] > 0)
    {
        return tnode->maxarr[4];
    }
    else if(tnode->maxarr[1] > 0)
    {
        return tnode->maxarr[1];
    }
    else if(tnode->maxarr[0] > 0)
    {
        return tnode->maxarr[0];
    }
    else
    {
        return false;
    }
}

KV_T FindMaxKey(key_node* tnode)
{
    if(tnode->maxarr[6] > 0)
    {
        return tnode->maxarr[6];
    }
    else if(tnode->maxarr[5] > 0)
    {
        return tnode->maxarr[5];
    }
    else if(tnode->maxarr[2] > 0)
    {
        return tnode->maxarr[2];
    }
    else if(tnode->maxarr[0] > 0)
    {
        return tnode->maxarr[0];
    }
    else
    {
        return false;

    }
}*/



KV_T Find(KV_T key);
{
    key_node* tnode = root;
    while(tnode != NULL)
    {
        for(char count = 1; count < (MAXARR_KEY_NUM-1);)
        {
            if(key < tnode->maxarr[count-1])
            {
                if(count == 4)
                {
                    if((key > tnode->leftchild->maxarr[0]) == 1)
                    {
                        for(int inter_count = 0; inter_count < (LISTS-1); inter_count++)
                        {
                            if(tnode->data_node_addr->kv_data[count-1][inter_count].key == key)
                            {
                                return tnode->data_node_addr->kv_data[count-1][inter_count].value;
                            }
                            else
                            {
                                fprintf(stdout,"FUNCTION %s : Can not find the key-value\n");
                                return false;
                            }
                        }
                    }
                    else if
                    {
                        tnode = tnode->leftchild;
                        break;
                    }
                }
                else
                {    
                    if(((key <= tnode->maxarr[count*2-1])&&(count < 4)) == 1)
                    {
                        count *= 2;
                    }
                    else
                    {
                        for(int inter_count = 0; inter_count < (LISTS-1); inter_count++)
                        {
                            if(tnode->data_node_addr->kv_data[count-1][inter_count].key == key)
                            {
                                return tnode->data_node_addr->kv_data[count-1][inter_count].value;
                            }
                            else
                            {
                                fprintf(stdout,"FUNCTION %s : Can not find the key-value\n");
                                return false;
                            }
                        }
                    }
                }
            }
            else if(key = tnode->maxarr[count-1])
            {
                return tnode->maxarr[count-1];
            }
            else if(key > tnode->maxarr[count-1])
            {
                if(count < MAXARR_KEY_NUM)
                {
                    count = count * 2 + 1;
                }
                else
                {
                    tnode = tnode->rightchild;
                    break;
                }
            }
        }
    }
    fprintf(stdout,"FUNCTION %s : Did not find the key-value\n");
    return -1;
}

int BalanceTree(key_node* tnode)
{
    
}

void insert(int key, int value)
{
    if(root == NULL)
    {
        root = (key_node*)malloc(sizeof(key_node));
    }
}














































































































/*    {
        if(tnode->maxarr[0] => 0)
        {
            for(int co xunt = 1; count <= 4;)
            {
                if(tnode->maxarr[count*2-1] < 0)
                {
                    if(tnode->maxarr[count*2] < 0)
                    {
                        return tnode->maxarr[count-1];
                    }
                    else
                    {
                        return tnode->maxarr[count*2];
                    }
                }
            
            count += 2;
    
        
    }*/
    
}

}




}






















#endif
