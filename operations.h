//
// Created by Shi on 2017/12/8.
//

#ifndef FILESYSTEM_OPERATIONS_H
#define FILESYSTEM_OPERATIONS_H

#endif //FILESYSTEM_OPERATIONS_H

#include "structs.h"

File_Block * getAddressByLocation_File(int location)
{
    File_Block * p = (File_Block *)emptyspaces;
    return p + location;
}

dir_block * getAddressByLocation_Folder(int location)
{
    dir_block * p = (dir_block *)emptyspaces;
    return p + location;
}

//将路径打碎成各级文件夹的名字，存放在seperated_names中。返回值是seperated_names中内容的项数。
//例如：parsePath("/home/foldera/folderb/file") = 4
void parsePath(string path)
{
    int last_slash = 0;
    int i, j = 0;
    int s = path.size();
    for (i = 1; i < s; i++)
    {
        if (path[i] == '/')
        {
            seperated_names[j] = path.substr(last_slash + 1, i - last_slash - 1);
            last_slash = i;
            j++;
        }
    }
    num_of_layers = j;
}

//检查文件/文件夹是否存在
bool doesExist()
{
    int i;
    int current = 0;//当前所在索引结点
    for (i = 0; i < num_of_layers; i++)
    {
        int mode = inodes[current].i_mode;
        if (mode == A_FOLDER)//文件夹
        {
            int blocks = inodes[current].i_blocks[0];
            dir_block * p = getAddressByLocation_Folder(blocks);
            for (int j = 0; j < 15; j++)
            {
                char t[252];
                strcpy(t, seperated_names[i].c_str());//将打碎的路径的第i项转化为c风格字符串
                if (strcmp(t, p->dirs->name) == 0)
                {
                    break;
                }
            }
        }
        else//文件
        {
            if (i != num_of_layers - 1)
            {

            }
            else
            {

            }
        }
    }
}

//将内存中的内容写入文件
void writeMemoryToFile()
{
    ofstream fout(FILEPATH, ios::binary);
    fout.write((char *)super_block.inode_bitmap, sizeof(super_block.inode_bitmap));
    fout.write((char *)super_block.block_bitmap, sizeof(super_block.block_bitmap));
    /*for (int i = 0; i < 4096; i++)
    {
        fout.write((char *)&inodes[i].i_id, 4);
        fout.write((char *)&inodes[i].i_mode, 4);
        fout.write((char *)&inodes[i].i_file_size, 4);
        fout.write((char *)inodes[i].i_blocks, sizeof(inodes[i].i_blocks));
        fout.write((char *)inodes[i].i_place_holder, 16);
    }*/
    fout.write((char *)inodes, sizeof(inodes));
    fout.write(emptyspaces, sizeof(emptyspaces));
    fout.close();
}

//初始化（“格式化”）硬盘，并写入文件
void initialize()
{
    super_block.inode_bitmap[0] = true;
    super_block.block_bitmap[0] = true;
    inodes[0].i_mode = A_FOLDER;
    for (int i = 0; i < 4096; i++)
    {
        inodes[i].i_id = i;
        inodes[i].i_file_size = 0;
        inodes[i].i_blocks[0] = 0;
    }
    location = "";
    writeMemoryToFile();
}

//pwd
string getLocation()
{
    return location;
}

//cd $path
int setLocation(string path)
{
    location = path;
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