//
// Created by Shi on 2017/12/8.
//

#ifndef FILESYSTEM_OPERATIONS_H
#define FILESYSTEM_OPERATIONS_H

#endif //FILESYSTEM_OPERATIONS_H

#include "structs.h"

//将内存中的内容写入文件
void writeMemoryToFile()
{
    ofstream fout(FILEPATH, ios::binary);
    fout.write((char *)super_block.inode_bitmap, sizeof(super_block.inode_bitmap));
    fout.write((char *)super_block.block_bitmap, sizeof(super_block.block_bitmap));
    for (int i = 0; i < 4096; i++)
    {
        fout.write((char *)&inodes[i].i_id, 4);
        fout.write((char *)&inodes[i].i_mode, 4);
        fout.write((char *)&inodes[i].i_file_size, 4);
        fout.write((char *)inodes[i].i_blocks, sizeof(inodes[i].i_blocks));
        fout.write((char *)inodes[i].i_place_holder, 16);
    }
    fout.write(emptyspaces, sizeof(emptyspaces));
    fout.close();
}

//初始化（“格式化”）硬盘
void initialize()
{
    super_block.inode_bitmap[0] = true;
    super_block.block_bitmap[0] = true;
    for (int i = 0; i < 4096; i++)
    {
        inodes[0].i_id = i;
        inodes[0].i_mode = 0;
        inodes[0].i_file_size = 0;
        inodes[0].i_blocks[0] = 0;
    }
    writeMemoryToFile();
}

//pwd
void getLocation()
{

}

//cd $path
int setLocation(string path)
{

}

//mkdir $path
int newItem(string path)
{

}

//ls $path
int getChildItem(string path)
{

}

//rmdir $path
int removeDirectory(string path)
{

}

//echo $str $path
int writeOutput(string str, string path)
{

}

//cat $path
int getContent(string path)
{

}

//rm $path
int removeFile(string path)
{

}