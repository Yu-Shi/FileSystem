//
// Created by Shi on 2017/12/8.
//

#ifndef FILESYSTEM_STRUCTS_H
#define FILESYSTEM_STRUCTS_H

#endif //FILESYSTEM_STRUCTS_H

#include <string>
#include <cstring>
using namespace std;

const string FILEPATH = "hello";

struct super_block
{
    bool inode_bitmap[4096];
    bool block_bitmap[4096];
};

struct inode
{
    int i_id;
    int i_mode;
    int i_file_size;
    int i_blocks[1];
    char i_place_holder[16];
};

struct file_block
{
    char data[4096];
};

struct dir_entry
{
    char name[252];
    int inode_id;
};

struct dir_block
{
    dir_entry dirs[16];
};