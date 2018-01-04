//
// Created by Shi on 2017/12/8.
//

//写到吐血

#ifndef FILESYSTEM_OPERATIONS_H
#define FILESYSTEM_OPERATIONS_H

#endif //FILESYSTEM_OPERATIONS_H

#include "structs.h"

int getPathType(string path)
{
    if (path[0] == '/' && !path.empty())
        return ABSOLUTE_PATH;
    return RELATIVE_PATH;
}

string convertToAbsolutePath(string path)
{
    if (path[path.size() - 1] != '/')
        path += '/';
    return location + path;
}

File_Block * getAddressByLocation_File(int location)
{
    auto * p = (File_Block *)emptyspaces;
    return p + location;
}

dir_block * getAddressByLocation_Folder(int location)
{
    auto * p = (dir_block *)emptyspaces;
    return p + location;
}

//将路径打碎成各级文件夹的名字，存放在names中。
//例如：parsePath("/home/foldera/folderb/file") = 4
int parsePath(string path, string * names)
{
    if (path.empty())
    {
        names[0] = "";
        return 0;
    }
    int last_slash = 0;
    int i, j = 0;
    int s;
    s = static_cast<int>(path.size());
    if (path[s - 1] != '/')
        path = path + "/";
    if (path[0] != '/')
        path = "/" + path;
    s = static_cast<int>(path.size());
    cout << path << endl;
    for (i = 1; i < s; i++)
    {
        if (path[i] == '/')
        {
            names[j] = path.substr(static_cast<unsigned long>(last_slash + 1),
                                   static_cast<unsigned long>(i - last_slash - 1));
            cout << names[j] << endl;
            last_slash = i;
            j++;
        }
    }
    cout << "层数是：" << j << endl;
    return j;
}

//检查文件/文件夹是否存在，返回存在的最深层
int doesExist(string * names, int num_of_layers)
{
    int i;
    int current = current_node;//当前所在索引结点
    for (i = 0; i < num_of_layers; i++)
    {
        int mode = inodes[current].i_mode;
        if (mode == A_FOLDER)//文件夹
        {
            int blocks = inodes[current].i_blocks[0];
            dir_block * p = getAddressByLocation_Folder(blocks);
            bool flag = false;
            for (auto &dir : p->dirs) 
            {
                char t[252];
                strcpy(t, names[i].c_str());//将打碎的路径的第i项转化为c风格字符串
                if (strcmp(t, dir.name) == 0)//存在
                {
                    flag = true;
                    current = dir.inode_id;
                    break;
                }
            }
            if (!flag)//没有相等的entry
                return i;
        }
        else//路径中间是一个文件
        {
            cout << "No such file or directory.\n";
            return -1;
        }
    }
    return num_of_layers;
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
    inodes[0].i_mode = A_FOLDER;//0号索引节点指示根目录
    for (int i = 0; i < 4096; i++)
    {
        inodes[i].i_id = i;
        inodes[i].i_file_size = 0;
        inodes[i].i_blocks[0] = 0;
    }
    auto * p = (dir_block *)emptyspaces;
    p->dirs[0].inode_id = 0;//inode_id指示当前目录
    super_block.inode_bitmap[0] = true;
    strcpy(p->dirs[0].name, ".");//指当前目录
    location = "";
    writeMemoryToFile();
}

//pwd
string getLocation()
{
    return location;
}

//查找特定文件的结点编号（inode_id）。在检查文件夹存在时调用。
int getId(string * names, int layers)
{
    int current = current_node;
    for (int i = 0; i < layers; i++)
    {
        int blocks = inodes[current].i_blocks[0];
        dir_block * p = getAddressByLocation_Folder(blocks);
        for (auto &dir : p->dirs) 
        {
            char t[252];
            strcpy(t, names[i].c_str());//将打碎的路径的第i项转化为c风格字符串
            if (strcmp(t, dir.name) == 0)//存在
            {
                current = dir.inode_id;
                break;
            }
        }
    }
    return current;
}

//cd $path
void setLocation(string path)
{
    string names[100];
    int layers = parsePath(path, names);
    if (doesExist(names, layers) == layers)//存在
    {
        int id = getId(names, layers);
        current_node = id;
        if (getPathType(path) == RELATIVE_PATH)
        {
            path = convertToAbsolutePath(path);
        }
        location = path;
    }
    else
    {
        cout << path << ": No such file or directory.\n";
    }
}

//（不公开调用！）根据索引节点序号创建名为name的子文件夹。返回子文件夹的id。
int makeDirectoryById(int id, string name)
{
    dir_block * p = getAddressByLocation_Folder(inodes[id].i_blocks[0]);
    int i;
    for (i = 0; i < 16; i++)
    {
        if (strcmp(p->dirs[i].name, "") == 0)//找到空的目录项
        {
            char t[252];
            strcpy(t, name.c_str());
            strcpy(p->dirs[i].name, t);//名字
            break;
        }
    }
    int j;
    for (j = 0; j < 4096; j++)
    {
        if (!super_block.inode_bitmap[j])//inode_bitmap中j号为空
        {
            p->dirs[i].inode_id = j;
            inodes[j].i_mode = A_FOLDER;//inode的j号标记为folder
            super_block.inode_bitmap[j] = true;
            break;
        }
    }
    int k;
    for (k = 0; k < 4096; k++)
    {
        if (!super_block.block_bitmap[k])
        {
            inodes[j].i_blocks[0] = k;
            super_block.block_bitmap[k] = true;
            break;
        }
    }
    dir_block * new_folder = getAddressByLocation_Folder(k);
    strcpy(new_folder->dirs[0].name, ".");
    strcpy(new_folder->dirs[1].name, "..");
    new_folder->dirs[0].inode_id = j;
    new_folder->dirs[1].inode_id = id;
    return j;
}

//mkdir $path
void newItem(string path)
{
    string names[100];
    int layers = parsePath(path, names);
    //cout << names[0] << endl;
    int exist_layers;
    exist_layers = doesExist(names, layers);
    cout << "exist_layers = " << exist_layers << endl;
    if (exist_layers == layers)//执行mkdir却发现已经存在
    {
        cout << path << " already exists.\n";
        return;
    }
    int id = getId(names, exist_layers);
    cout << names[0] << endl;
    cout << "id = " << id << endl;
    for (int i = exist_layers; i < layers; i++)
    {
        cout << "names[" << i << "] = " << names[i] << endl;
        id = makeDirectoryById(id, names[i]);
        cout << "id = " << id << endl;
    }
    writeMemoryToFile();
}

//ls $path
void getChildItem(string path)
{
    string cur_path = location;
    int cur_inode = current_node;
    string names[100];
    int layers = parsePath(path, names);
    if (doesExist(names, layers) != layers)
    {
        cout << path << ": No such file or directory.\n";
        return;
    }
    cur_inode = getId(names, layers);
    cout << "cur_inode: " << cur_inode << endl;
    int blocks = inodes[cur_inode].i_blocks[0];
    dir_block * p = getAddressByLocation_Folder(blocks);
    for (int i = 0; i < 16; i++)
    {
        if (strcmp(p->dirs[i].name, "") != 0)//非空
        {
            cout << p->dirs[i].name << endl;
        }
    }
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

void printLines(int n)
{
    /*if (n == 0)
        return;
    cout << "└";*/
    int i;
    for (i = 0; i < n; i++)
        cout << "\t";
}

int printTreeView(int id, int tabs)
{
    dir_block * p = getAddressByLocation_Folder(inodes[id].i_blocks[0]);
    int i;
    for (i = 0; i < 16; i++)
    {
        if (strcmp(p->dirs[i].name, ""))
        {
            printLines(tabs);
            cout << p->dirs[i].name << endl;
            int nextid = p->dirs[i].inode_id;
            if (inodes[nextid].i_mode == A_FOLDER && strcmp(p->dirs[i].name, ".") && strcmp(p->dirs[i].name, ".."))
            {
                printTreeView(nextid, tabs + 1);
            }
        }
    }
}

void treeView(string path)
{
    string names[100];
    int layers = parsePath(path, names);
    if (doesExist(names, layers) == layers)//存在
    {
        int id = getId(names, layers);
        printTreeView(id, 0);
    }
    else
    {
        cout << path << ": No such file or directory.\n";
    }
}