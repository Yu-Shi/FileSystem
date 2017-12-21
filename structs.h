//
// Created by Shi on 2017/12/8.
//

#ifndef FILESYSTEM_STRUCTS_H
#define FILESYSTEM_STRUCTS_H

#endif //FILESYSTEM_STRUCTS_H

#include <string>
#include <cstring>
#include <fstream>
using namespace std;

const string FILEPATH = "hello";

const int A_FOLDER = 1;
const int A_FILE = 2;

const int ABSOLUTE_PATH = 1;
const int RELATIVE_PATH = 2;

struct Super_Block
{
    bool inode_bitmap[4096];
    bool block_bitmap[4096];
}super_block;

struct inode
{
    int i_id;
    int i_mode;
    int i_file_size;
    int i_blocks[1];
    char i_place_holder[16];
}inodes[4096];

struct File_Block
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

char emptyspaces[4096 * 4096];

//当前所在目录的信息
string location = "/";//current directory
int current_node = 0;//当前索引节点编号