/*************************************************************************
	> File Name: csttree.h
	> Author: QYT
	> Mail: ytqiang@dsclab.sn.ac.kr
	> Created Time: Wed 20 Sep 2017 01:55:14 PM KST
 ************************************************************************/

#ifndef _CSTTREE_H
#define _CSTTREE_H

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <libpmem.h>
#include <malloc.h>
#include <libvmalloc.h>

#define DEBUG

/* pmem file address */
#define PATH "/home/qyt/csttree_nvml/cst_data"

/* pmem length */
#define PMEM_LEN (1 << 31)

/* decide the key node level and the node size and the intial level number is zero*/
#define KEY_LEVEL 0x02 

/* decide data node size */ 
#define LISTS 0x04

/* key and value type is the same */
#define KV_T uint64_t

/* initialized pmem node number */
#define NODE_NUM (1 << 20)

/* one single key node size */
#define KEY_NODE_SIZE sizeof(_key_node)

/* one single data node size */
#define DATA_NODE_SIZE sizeof(_data_node)

/* one key-data node pair size */
#define KEY_DATA_SIZE sizeof(sin_node_pair)

/* one key-data array size */
#define NODE_ARRAY_SZIE sizeof(node_arry)

#define PRINT()\
        printf("THE FUNCTION IS %s at the line %d \n",__func__, __LINE__);

namespace stx{

typedef struct
{
    KV_T _key;
    KV_T _value;
}kv_pair;

typedef struct
{
   char* node_arry_addr; 
   bool is_empty;
}pm_meta;

typedef struct
{
    kv_pair kv_data[KEY_NUM][LISTS];
}_data_node;

typedef struct
{
    KV_T maxarr[MAXARR_KEY_NUM];
    node_arry* child_node_array;
}_key_node;

typedef struct
{
    _key_node key_node;
    _data_node data_node;
}sin_node_pair;
    
typedef structure
{
    sin_node_pair node_org[2 << KEY_LEVEL];
}node_array;

class cst_tree
{

private:

uint8_t KEY_NUM;

node_arry* root;

//pm_meta nvm_meta_root;

pm_meta nvm_meta[NODE_NUM];

uint32_t node_id;

public:

cst_tree()
{
    uint8_t count;
    for(count = 0; count <= KEY_LEVEL; key++)
    {
        KEY_NUM += (1 << count);
    }
}

~cst_tree();

inline int8_t keycompare(KV_T key1, KV_T key2)
{
    return (key1) < (key2) ? (0xff) : (key1) == (key2) ? (0x00) : (0x01);
}

char* Init_pm()
{
    int8_t is_pmem;
    uint32_t mapped_len;
    char* node_pool;
    uint32_t node_id;

    if(char* pmem_addr = (char*)(pmem_map_file(PATH, PMEM_LEN, PMEM_FILE_CREATE, 0666, &mapped_len, &is_pmem)) == NULL)
    {
        perror("pmem initialize false!");
        exit(1);
    }
/
    root_node = pmem_addr + sizeof(pm_meta) * NODE_NUM;

    node_pool = root_node + NODE_ARRAY_SZIE * NODE_NUM;

    for(node_id = 0; node_id < NODE_NUM; node_id++)
    {
        nvm_meta[node_id].node_array_addr = node_pool + (NODE_ARRAY_SZIE * node_id);
        nvm_meta[node_id].is_empty = true;
    }
    return node_pool;    

}

bool Insert(KV_T key, KV_T value)
{
    if(root == NULL)
    {
        root = malloc(sizeof(node_array));
        pmem_memset_persist(root, 0x00, sizeof(node_array));
        root->node_org[0].key_node.maxarr[0] = key;
        root->node_org[0].data_node.kv_data[0][LISTS - 1]._key = key;
        root->node_org[0].data_node.kv_data[0][LISTS - 1]._value = value;
        root->node_org[0].key_node.child_node_array = NULL;
        pmem_persist(root, KEY_NODE_SIZE);
        printf("root insert!\n");
        return true;
    }
    else
    {
        node_array* tempnode = root;
        static uint32_t node_offset = 0;
        bool ret = _insert(tempnode, node_offset, key, value);
        printf("the ret value %d \n", ret);
        return ret;
    }
}

inline void insertion_sort(uint32_t* unsorted, uint32_t length)
{
    for(uint32_t i = 1; i <length; i++ )
    {
        int j = i -1;
        int temp = unsorted[i];
        while(j > 0 && unsorted[j] > temp)
        {
            unsorted[j + 1] = unsorted[j];
            j--;
        }
        if(j != i - 1)
        {
            unsorted[j + 1] = temp;
        }
    }
}

uint32_t max(uint32_t a, uint32_t b)
{
    if(a < b)
    {
        return b;
    }
    else if(a >= b)
    {
        return a;
    }
}

uint32_t exter_odd_even(uint32_t num)
{
    if((num % 2) == 0)
    {
        return ((num - 1 + KEY_NUM) >> 1);
    }
    else
    {
        return ((num - 2 + KEY_NUM) >> 1);
    }
}

uint32_t inter_odd_even(uint32_t num)
{
    if((num % 2) == 0)
    {
        return ((num - 2) >> 1);
    }
    else
    {
        return ((num - 1) >> 1);
    }
}

uint32_t tree_depth(node_array* pnode, uint32_t offset, uint32_t inter_node)
{
    uint32_t rightdepth = 0;
    uint32_t leftdepth = 0;
    if(pnode->node_org[offset].key_node.maxarr[inter_node] == NULL)
    {
        return -1;
    }
    if((pnode->node_org[offset].key_node.maxarr[(2<<inter_node) + 1])&&((2<<inter_node) + 1 <= KEY_NUM))
    {
        leftdepth = tree_depth(pnode, offset, (2<<inter_node) + 1);
    }
    if(!(pnode->node_org[offset].key_node.maxarr[(2<<inter_node) + 1])&&((2<<inter_node) + 1 <= KEY_NUM))
    {
        leftdepth = -1;
    }
    if(pnode->node_org[offset].key_node.maxarr[(2<<inter_node) + 2]&&((2<<inter_node) + 2 <= KEY_NUM))
    {
        rightdepth = tree_depth(pnode, offset, (2<<inter_node) + 2);
    }
    if(!(pnode->node_org[offset].key_node.maxarr[(2<<inter_node) + 2])&&((2<<inter_node) + 2 <= KEY_NUM))
    {
        rightdepth = -1;
    }
    if(inter_node > KEY_NUM)
    {
        pnode = pnode->node_org[offset].key_node.child_node_array;
        offset = inter_count - KEY_NUM;
        leftdepth = rightdepth = tree_depth(pnode, offset, 0);
    }
    return rightdepth > leftdepth ? rightdepth + 1 : leftdepth + 1;
}


/* a new way for rotation which we can test the new inserted node's grandpa node have another child node.
 * If take the tree is balanced tree. And drop the old way of rotation. */
 
bool Ggrandpa_balance(sin_node_pair* parent_node, node_array* pnode, uint32_t offset, uint32_t inter_offset)
{
    uint32_t inter_index = inter_offset;
    uint32_t counter = 3;
    sin_node_pair* monitor_node = &pnode->node_org[offset];
    while(counter)
    {
        if(!inter_index)
        {
            inter_index = offset + KEY_NUM;
            monitor_node = parent_node;
            if(inter_index % 2 == 0)
            {
                inter_index = inter_index - 2 >> 1;
                counter--;
            }
            else
            {
                inter_index = inter_index - 1 >> 1;
                counter--;
            }
        }
        else if(!(inter_index % 2))
        {
            inter_index = inter_index - 2 >> 1;
            counter--;
        }
        else if(inter_index % 2)
        {
            inter_index = inter_index - 1 >> 1;
            counter--;
        }
    }
    uint32_t left_h = 0;
    uint32_t right_h = 0;


}






















    uint32_t tmp_num = 0x00;
    uint32_t tmp_index = 0x00;
    if(inter_num == 0x00)
    {
        tmp_index = offset + KEY_NUM;
        if((tmp_index % 2) == 0)
        {
            tmp_index = (tmp_index - 2) >> 1;
            if(tmp_index % 2 == 0)
            {
                tmp_index = tmp_index - 2 >> 1;
                if(parent_node.key_node.maxarr[(tmp_index << 1) + 1] == 0)
                {
                        
                }
                else
                {
                    return true;
                }
            }
            else
            {
                tmp_index = tmp_index - 1 >> 1;
                if(pnode->node_org[offset].key_node.maxarr[(tmp_index << 1) + 2] == 0)
                {
                    
                }
                else
                {
                    return true;
                }
            }
        }
        else
        {
            tmp_index = (tmp_index - 1) >> 1;
            if(tmp_index % 2 == 0)
            {
                tmp_index = tmp_index - 2 >> 1;
                if(parent_node.key_node.maxarr[(tmp_index << 1) - 1] == 0)
                {
                     
                }
                else
                {
                    return true;
                }
            }
            else
            {
                tmp_index = tmp_index - 1 >> 1;
                if(parent_node.key_node.maxarr[(tmp_index << 1) - 2] == 0)
                {
                    
                }
                else
                {
                    return true;
                }
            }
        }
    else if(inter_num % 2 == 0 && inter_num)
    {
        tmp_index = inter_num - 2 >> 1;
        if(tmp_index == 0)
        {
            tmp_index = offset + KEY_NUM;
            if(tmp_index % 2 == 0)
            {
                tmp_index = tmp_index - 2 >> 1;
                if(pnode->node_org[(tmp_index << 1) + 1 - KEY_NUM].maxarr[0] == 0)
                {

                }
                return true;
            }
            else
            {
                tmp_index = tmp_index - 1 >> 1;
                if(pnode->node_org[(tmp_index << 1) + 2 - KEY_NUM].maxarr[0] == 0)
                {
                    
                }
                else
                {
                    return true;
                }
            }
        }
        else if(tmp_index != 0 && tmp_index % 2 == 0)
        {
            tmp_index = tmp_index - 2 >> 1;
            if(pnode->node_org[offset].key_node.maxarr[(tmp_index << 1) - 1] == 0)
            {
                
            }
            else
            {
                return true;
            }
        }
        else if(tmp_index != 0 && tmp_index % 2 != 0)
        {
            tmp_index = tmp_index - 1 >> 1;
            if(pnode->node_org[offset].key_node.maxarr[(tmp_index << 1) - 2] == 0)
            {
                
            }
            else
            {
                return true;
            }
        }
    }
    else if(inter_num % 2 != 0 && inter_num)
    {
        tmp_index = inter_num - 1 >> 1;
        if(tmp_index == 0)
        {
            tmp_index = offset + KEY_NUM;
            if(tmp_index % 2 == 0)
            {
                tmp_index = tmp_index - 2 >> 1;
                if(pnode->node_org[(tmp_index << 1) + 1 - KEY_NUM].maxarr[0] == 0)
                {
                    
                }
                else
                {
                    return true;
                }
            }
            else
            {
                tmp_index = tmp_index - 1 >> 1;
                if(pnode->node_org[(tmp_index << 1) + 2 - KEY_NUM].maxarr[0] == 0)
                {
                    
                }
                else
                {
                    return true;
                }
            }
        }
        else if(tmp_index != 0 && tmp_index % 2 == 0)
        {
            tmp_index = tmp_index - 2 >> 1;
            if(pnode->node_org[offset].key_node.maxarr[(tmp_index << 1) + 1] == 0)
            {
                
            }
            else
            {
                return true;
            }
        }
        else if(tmp_index != 0 && tmp_index % 2 != 0)
        {
            tmp_index = tmp_index - 1 >> 1;
            if(pnode->node_org[offset].key_node.maxarr[(tmp_index << 1) + 2] == 0)
            {
                
            }
            else
            {
                return true;
            }
        }
    }

bool Balance_cst_tree(node_array* pnode, uint32_t offset)
{
    uint32_t bound = (2 << KEY_LEVEL + 1);
    typedef struct
    {
        uint32_t height;
        uint32_t index;
    }depth;
    depth depth_org[bound];
    memset(depth_org, 0x00, sizeof(depth) * bound);
    uint32_t inter;
    node_array* tmpnode = pnode;
    pnode = pnode->node_org[offset].key_node.child_node_array;
    for(inter = 0; inter < bound; inter++)
    {
        depth_org[inter].depth = tree_depth(pnode, inter, 0);
        depth_org[inter].index = inter; 
     }
    uint32_t max, min, max_count, min_count, rotate_count;
    max = depth_org[0].depth;
    max_count depth_org[0].index;
    uint32_t rotate_count = 0;
    for(rotate_count = 0; rotate_count < bound; rotate_count++)
    {
        if(depth_org[rotate_count].depth > max)
        {
            max = height[rotate_count].depth;
            max_count = depth_org[rotate_count].index;
        }
    }
    min = depth_org[0].depth;
    min_count = depth_org[0].index;
    for(rotate_count = 0; rotate_count < bound; rotate_count++)
    {
        if(depth_org[rotate_count].depth < min)
        {
            min = depth_org[rotate_count].depth;
            min_count = depth_org[rotate_count].index;
        }
    }
    if((max - min) >= 2)
    {
        if(max_count > min_count)
        {
            sin_node_pair* q_node = &pnode->node_org[max_count];
            uint32_t counter = 0;
            if((max_count - min_count) != 1)
            {
                uint32_t from = min_count;
                uint32_t to = max_count - 1;
                for(counter = from; counter < to; counter++)
                {
                    ij_rotation(tmpnode, offset, counter, counter + 1);
                }
            }
            node_array* q_children = q_node.key_node->child_node_array;
            depth qnode_depth_org[bound];
            memset(qnode_depth_org, 0x00, sizeof(depth) * bound);
            for(inter = 0; inter < bound; inter++)
            {
                depth_org[inter].depth = tree_depth(q_children, inter, 0);
                depth_org[inter].index = inter; 
            }
            uint32_t qnode_max, qnode_min, qnode_max_count, qnode_min_count, qnode_rotate_count;
            qnode_max = qnode_depth_org[0].depth;
            qnode_max_count qnode_depth_org[0].index;
            for(qnode_rotate_count = 0; qnode_rotate_count < bound; qnode_rotate_count++)
            {
                if(qnode_depth_org[qnode_rotate_count].depth > max)
                {
                    qnode_max = height[qnode_rotate_count].depth;
                    qnode_max_count = qnode_depth_org[qnode_rotate_count].index;
                }
            }
            qnode_min = qnode_depth_org[0].depth;
            qnode_min_count = qnode_depth_org[0].index;
            for(qnode_rotate_count = 0; qnode_rotate_count < bound; qnode_rotate_count++)
            {
                if(qnode_depth_org[qnode_rotate_count].depth < qnode_min)
                {
                    qnode_min = qnode_depth_org[qnode_rotate_count].depth;
                    qnode_min_count = qnode_depth_org[qnode_rotate_count].index;
                }
            }
            if(qnode_max_count < bound - 1)
            {
                for(counter = max_count; counter < bound - 1; counter++)
                {
                    ij_rotation(q_node, offset, counter, counter + 1);
                }
            }
            ij_rotation(tmpnode, max_count, min_count);
        }
        else
        {
            //rotation to right                 
        }
    if(tmp->parent is not null)//?
    cst_balace();
    }
    else
    {
        return true;
    }
}


void ij_rotation(node_array* pnode, uint32_t offset, uint32_t i, uint32_t j)
{
    sin_node_pair* tmp_sinnode = (sin_node_pair*)malloc(sizeof(sin_node_pair));
    node_array* child_node = pnode->node_org[offset].key_node.child_node_array;
    memcpy(tmp_sinnode, child_node->node_org[j], sizeof(sin_node_pair));
    uint32_t a = 0x00;
    uint32_t max_num_j = child_node->node_org[i].maxarr[a];
    while(child_node->node_org[i].maxarr[(a << 1) + 2] != 0)
    {
        max_num_j = child_node->node_org[i].maxarr[(a << 1) + 2];
        a = (a << 1) + 2;
    }
    kv_pair tmp_list_i[LISTS];
    memcpy(tmp_list_i, child_node->node_org[i].data_node.kv_data[a][], sizeof(kv_pair) * LISTS);
    uint32_t tmp_i = exter_odd_even(i);
    uint32_t tmp_j = exter_odd_even(j);
    while(tmp_i != tmp_j)
    {
        tmp_i = inter_odd_even(tmp_i);
        tmp_j = inter_odd_even(tmp_j);
    }
    if(tmp_i != tmp_j)
    {
        #ifdef DEBUG
        PRINT()
        #endif
        return;
    }
    uint32_t tmp_num = pnode->node_org[offset].key_node.maxarr[tmp_i];
    kv_pair tmp_list_j[LISTS];
    memcpy(tmp_list_j, pnode->node_org[offset].data_node.kv_data[tmp_i][], sizeof(kv_pair) * LISTS);
    memcpy(pnode->node_org[offset].data_node.kv_data[tmp_i][], tmp_list_i, sizeof(kv_pair) * LISTS);
    pnode->node_org[offset].key_node.maxarr[tmp_i] = max_num_j;

    

    
}*/



bool _insert(node_array* pnode, uint32_t node_offset, KV_T key, KV_T value)
{
    uint32_t count_inter = 0;//p
    KV_T tempkey, tempvalue;
    while(count_inter <= KEY_NUM)
    {
        uint32_t data_count;
        uint32_t cir;
        uint32_t i = 0;
        if(pnode->node_org[node_offset].key_node.maxarr[count_inter] == key)
        {
            if(pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS-1]._value == value)
            {
                printf("repeat key value pair\n");
                return true;
            }
            else
            {
                data_count = 0;
                for(cir = 0; cir < LISTS; cir++)
                {
                    if(pnode->node_org[node_offset].data_node.kv_data[count_inter][cir]._key != 0)
                    {
                        data_count++;
                    }
                }
                if(data_count < LISTS)
                {
                    while(data_count != 0)
                    {
                        pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count - 1]._key = pnode->node_org[node_offset].data_node.kv_data[conut_inter][LISTS - data_count]._key;
                        pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count - 1]._value = pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count]._value;
                        data_count--;
                    }
                    pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - 1]._value = value;
                    pmem_persist(&pnode->node_org[node_offset].data_node.kv_data[count_inter][0], LISTS * sizeof(kv_pair));//persistent
                    #if DEBUG
                    PRINT()
                    #endif
                    return true;
                }
                else if(data_count == LISTS)
                {
                    tempkey = pnode->node_org[node_offset].data_node.kv_data[count_inter][0]._key;                 
                    tempvalue = pnode->node_org[node_offset].data_node.kv_data[count_inter][0]._value;
                    while(data_count != 0)
                    {
                        pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count]._key = pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count + 1]._key;
                        pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count]._value = pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count + 1]._value;
                        data_count--;
                    }
                    pnode->node_org[node_offset].data_node.kv_data[inter_count][LISTS - 1]._value = value;
                    pmem_persist(&pnode->node_org[node_offset].data_node.kv_data[count_inter][0], LISTS * sizeof(kv_pair));//persistent
                    key = tempkey;
                    value = tempvalue;
                    count_inter = (count_inter << 1) + 1;
                    continue;
                }
                else
                {
                    #if DEBUG
                    PRINT()
                    #endif
                    printf(" data number overflow\n");
                    return false;
                }
            }
        }
        else if(pnode->node_org[node_offset].key_node.maxarr[count_inter] > key)
        {
            if(pnode->node_org[node_offset].key_node.maxarr[(count_inter<<1) + 1] < key)
            {
                data_count = 0;
                for(cir = 0; cir < LISTS; cir++)
                {
                    if(pnode->node_org[node_offset].data_node.kv_data[count_inter][cir]._key != 0)
                    {
                        data_count++;
                    }
                }
                uint32_t l = LISTS - data_count;
                uint32_t r = LISTS - 2;
                while(l < r)
                {
                    uint32_t d = (l + r) / 2;
                    KV_T itemkey = pnode->node_org[node_offset].data_node.kv_data[inter_count][d]._key;
                    int8_t ndiff = keycompare(key, itemkey);
                    if(ndiff == 0)
                    {
                        break;
                    }
                    else if(ndiff < 0)
                    {
                        r = d;
                    }
                    else 
                    {
                        l = d + 1;
                    }
                }
                if(data_count < LISTS)
                {
                    for(i = 0; i < r; i++)
                    {
                        pnode->node_org[node_offset].data_node.kv_data[inter_count][LISTS - data_count + i - 1]._key = pnode->node_org[node_offset].data_node.kv_data[inter_count][LISTS - data_count + i]._key;
                        pnode->node_org[node_offset].data_node.kv_data[inter_count][LISTS - data_count + i - 1]._value = pnode->node_org[node_offset].data_node.kv_data[inter_count][LISTS - data_count + i]._value;
                    }
                    pnode->node_org[node_offset].data_node.kv_data[inter_count][r - 1]._key = key;
                    pnode->node_org[node_offset].data_node.kv_data[inter_count][r - 1]._value = value;
                    pmem_persist(&pnode->org[node_offset].data_node.kv_data[inter_count][0], LISTS * sizeof(kv_pair));
                    #if DEBUG
                    PRINT()
                    #endif
                    return true;
                }
                else if(data_count == LISTS)
                {
                    if()
                    tempkey = pnode->node_org[node_offset].data_node.kv_data[inter_count][0]._key;
                    tempvalue = pnode->node_org[node_offset].data_node.kv_data[inter_count][0]._value;
                    for(; i< r; i++)
                    {
                        pnode->node_org[node_offset].data_node.kv_data.kv_data[inter_count][i]._key = pnode->node_org[node_offset].data_node.kv_data.[inter_count][i + 1]._key;
                        pnode->node_org[node_offset].data_node.kv_data.kv_data[inter_count][i]._value = pnode->node_org[node_offset].data_node.kv_data.[inter_count][i + 1]._value;
                    }
                    pnode->node_org[node_offset].data_node.kv_data[inter_count][r - 1]._key = key;
                    pnode->node_org[node_offset].data_node.kv_data[inter_count][r - 1]._value = value;
                    pmem_persist(&pnode->node_org[node_offset].data_node.kv_data[inter_count][0], LISTS * sizeof(kv_pair));
                    inter_count = (inter_count<<1) + 1;
                    key = tempkey;
                    value = tempvalue;
                    continue;
                }
            }
            else if(pnode->node_org[node_offset].key_node.maxarr[(count_inter<<1) + 1] <= key)
            {
                count_inter = (count_inter<<1) + 1;
                continue;
            }
        }
        else if(pnode->node_org[node_offset].key_node.maxarr[count_inter] < key)
        {
            data_count = 0;
            for(cir = 0; cir < LISTS; cir++)
            {
                if(pnode->node_org[node_offset].data_node.kv_data[count_inter][cir]._key != 0)
                {
                    data_count++;
                }
            }
            if(data_count < LISTS)
            {
                for(i = 0; i < data_count; i++)
                {
                    pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count - 1 + i]._key = pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count + i]._key;
                    pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count - 1 + i]._value = pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - data_count + i]._value;
                }
                pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - 1]._key = key;
                pnode->node_org[node_offset].data_node.kv_data[count_inter][LISTS - 1]._value = value;
                pnode->node_org[node_offset].key_node.maxarr[count_inter] = key;
                Balance_cst_tree(pnode, node_offset, count_inter);
                pmem_persist(&pnode->node_org[node_offset].data_node.kv_data[count_inter][0], LISTS * sizeof(KV_T));
                pmem_persist(&pnode->node_org[node_offset].key_node.maxarr[0], sizeof(_key_node));//flush size
                return true;
            }
            else if(data_count = LISTS)
            {
                count_inter = (count_inter<<1) + 2;
                continue;
            }
        }
    }
    if(count_inter > KEY_NUM)
    {
        if(pnode->node_org[node_offset].key_node->child_node_array == NULL) 
        {
            pnode->node_org[node_offset].key_node.child_node_array = (node_array*)malloc(sizeof(node_array));
            pmem_memset_persist(pnode->node_org[node_offset].key_node.child_node_array, 0x00, sizeof(node_array));
        }
        pnode = pnnode->node_org[node_offset].key_node.child_node_array;
        node_offset = count_inter - KEY_NUM;
        _insert(pnode, node_offset, key, value);
    }
}









































/*bool Find_pmem(KV_T key)
{
    node_array* curr_node;
    uint32_t i = 0;
    uint32_t node_num = 0;
    uint8_t node_array_num = 0;
    pm_meta* curr_meta;
    curr_meta = &nvm_meta[0];
    char* curr_pm_knode = curr_meta->pkey_node_org_addr + sizeof(key_node) * node_num;
    KV_T com_key = 0;
    while(curr_meta->pkey_node_addr != NULL)
    {
        com_key = ((KV_T*)(curr_pm_knode))[i];
        if(com_key == 0)
        {
            printf("have not found the key-value \n");
            return false;
        }
        else
        {
            if(i <= TRAN_BOUND){
                if(key <= com_key)
                {
                    com_key = ((KV_T*)(curr_pm_knode))[2i+1];
                    if(key > com_key)
                    {
                        return data_node_find(curr_pm_dnode, i, key);
                    }
                    else
                    {
                        i = 2i+1;
                    }
                }
                else
                {
                    i = 2i+2;
                }
            else
            {
                if((key <= com_key)&&(key >= ((KV_T*)(curr_pm_dnode))[i][0][0]))
                {
                    return data_node_find(curr_pm_dnode, i, key);
                }
                else if(key < ((KV_T*)(curr_pm_dnode))[i][0][0])
                {
                    curr_meta = (pm_meta*)()
                }

            }
        }


    }
}

bool data_node_find(char* data_add, uint32_t row_num, KV_T key)
{
    uint32_t count = 0;
    for(; count < LISTS; count++)
    {
        if(key == ((KV_T*)(data_add))[count][LISTS][0]
        {
            return true;
        }
    }
    printf("have not find the key-value\n");
    return false;
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
}

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
