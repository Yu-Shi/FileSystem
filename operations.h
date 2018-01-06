//
// Created by Shi on 2017/12/8.
//

#ifndef FILESYSTEM_OPERATIONS_H
#define FILESYSTEM_OPERATIONS_H

#endif //FILESYSTEM_OPERATIONS_H

#include "structs.h"

//将一行命令打碎，存放在commands中。
//例如：parseCmd("ls home") = 2


void setColor(int color)
{

#ifdef WIN32//if windows

    switch(color)
    {
        case COLOR_ERR:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_RED);
            break;
        case COLOR_FOLDER:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 31);
            break;
        case COLOR_ORIGIN:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|
                                                                    FOREGROUND_GREEN|FOREGROUND_BLUE);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            break;
    }

#endif

}

int parseCmd(string cmd, string *commands) {
    int a = 0, j = 0;
    for (int i = 0; i < cmd.size() + 1; i++) {
        if (i == cmd.size()) {
            commands[j] = cmd.substr(a, i - a);
            break;
        }
        if (cmd[i] == ' ') {
            commands[j] = cmd.substr(a, i - a);
            a = i + 1;
            j++;
        }
    }
    return j + 1;
}


int getPathType(string path) {
    if (path[0] == '/' && !path.empty())
        return ABSOLUTE_PATH;
    return RELATIVE_PATH;
}

string convertToAbsolutePath(string path) {
    if (path[path.size() - 1] != '/')
        path += '/';
    return location + path;
}

File_Block *getAddressByLocation_File(int location) {
    auto *p = (File_Block *) emptyspaces;
    return p + location;
}

dir_block *getAddressByLocation_Folder(int location) {
    auto *p = (dir_block *) emptyspaces;
    return p + location;
}

//将路径打碎成各级文件夹的名字，存放在names中。
//例如：parsePath("/home/foldera/folderb/file") = 4
int parsePath(string path, string *names) {
    if (path.empty()) {
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
    for (i = 1; i < s; i++) {
        if (path[i] == '/') {
            names[j] = path.substr(static_cast<unsigned long>(last_slash + 1),
                                   static_cast<unsigned long>(i - last_slash - 1));
            //cout << names[j] << endl;
            last_slash = i;
            j++;
        }
    }
    return j;
}

//检查文件/文件夹是否存在，返回存在的最深层
int doesExist(string *names, int num_of_layers, bool isRelative) {
    int i;
    int current = isRelative ? current_node : 0;//当前所在索引结点
    for (i = 0; i < num_of_layers; i++) {
        int mode = inodes[current].i_mode;
        if (mode == A_FOLDER)//文件夹
        {
            int blocks = inodes[current].i_blocks[0];
            dir_block *p = getAddressByLocation_Folder(blocks);
            bool flag = false;
            for (auto &dir : p->dirs) {
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
            return -1;
        }
    }
    return num_of_layers;
}

//将内存中的内容写入文件
void writeMemoryToFile() {
    ofstream fout(FILEPATH, ios::binary);
    fout.write((char *) super_block.inode_bitmap, sizeof(super_block.inode_bitmap));
    fout.write((char *) super_block.block_bitmap, sizeof(super_block.block_bitmap));
    /*for (int i = 0; i < 4096; i++)
     {
     fout.write((char *)&inodes[i].i_id, 4);
     fout.write((char *)&inodes[i].i_mode, 4);
     fout.write((char *)&inodes[i].i_file_size, 4);
     fout.write((char *)inodes[i].i_blocks, sizeof(inodes[i].i_blocks));
     fout.write((char *)inodes[i].i_place_holder, 16);
     }*/
    fout.write((char *) inodes, sizeof(inodes));
    fout.write(emptyspaces, sizeof(emptyspaces));
    fout.close();
}

//初始化（“格式化”）硬盘，并写入文件
void initialize() {
    super_block.inode_bitmap[0] = true;
    super_block.block_bitmap[0] = true;
    inodes[0].i_mode = A_FOLDER;//0号索引节点指示根目录
    for (int i = 0; i < 4096; i++) {
        inodes[i].i_id = i;
        inodes[i].i_file_size = 0;
        inodes[i].i_blocks[0] = 0;
    }
    auto *p = (dir_block *) emptyspaces;
    p->dirs[0].inode_id = 0;//inode_id指示当前目录
    super_block.inode_bitmap[0] = true;
    strcpy(p->dirs[0].name, ".");//指当前目录
    location = "";
}

int getParentById(int id) {
    int block = inodes[id].i_blocks[0];
    dir_block *p = getAddressByLocation_Folder(block);
    return p->dirs[1].inode_id;
}

//pwd
void getLocation() {
    int cur = current_node;
    stack<string> names;
    while (cur) {
        int t = cur;//this
        cur = getParentById(cur);//Parent's inode_id
        dir_block *p = getAddressByLocation_Folder(cur);//Parent folder's address
        for (auto &dir : p->dirs) {
            if (dir.inode_id == t) {
                names.push(dir.name);
                break;
            }
        }
    }
    cout << "/";
    while (!names.empty()) {
        cout << names.top() << "/";
        names.pop();
    }
    cout << endl;
}

//查找特定文件的结点编号（inode_id）。在检查文件夹存在时调用。
int getId(string *names, int layers, bool isRelative) {
    int current = isRelative ? current_node : 0;
    //cout << "current: " << current << endl;
    for (int i = 0; i < layers; i++) {
        int blocks = inodes[current].i_blocks[0];
        //cout << "blocks" << blocks << endl;
        dir_block *p = getAddressByLocation_Folder(blocks);
        for (auto &dir : p->dirs) {
            char t[252];
            strcpy(t, names[i].c_str());//将打碎的路径的第i项转化为c风格字符串
            if (strcmp(t, dir.name) == 0)//存在
            {
                //cout << "hellohello" << i << endl;
                current = dir.inode_id;
                break;
            }
        }
    }
    return current;
}

//cd $path
void setLocation(string path) {
    string names[100];
    int layers = parsePath(path, names);
    if (doesExist(names, layers, getPathType(path) == RELATIVE_PATH) == layers)//存在
    {
        int id = getId(names, layers, getPathType(path) == RELATIVE_PATH);
        if (inodes[id].i_mode == A_FILE) {
            setColor(COLOR_ERR);
            //设置红色
            cout << path << " is not a directory.\n";
            setColor(COLOR_ORIGIN);
            return;
        }
        current_node = id;
        if (getPathType(path) == RELATIVE_PATH) {
            path = convertToAbsolutePath(path);
        }
        location = path;
    }
    else {
        setColor(COLOR_ERR);
        cout << path << ": No such file or directory.\n";
        setColor(COLOR_ORIGIN);
    }
}

//（不公开调用！）根据索引节点序号创建名为name的子文件夹。返回子文件夹的id。
int makeDirectoryById(int id, string name) {
    dir_block *p = getAddressByLocation_Folder(inodes[id].i_blocks[0]);
    int i;
    for (i = 0; i < 16; i++) {
        if (strcmp(p->dirs[i].name, "") == 0)//找到空的目录项
        {
            char t[252];
            strcpy(t, name.c_str());
            strcpy(p->dirs[i].name, t);//名字
            break;
        }
    }
    int j;
    for (j = 0; j < 4096; j++) {
        if (!super_block.inode_bitmap[j])//inode_bitmap中j号为空
        {
            p->dirs[i].inode_id = j;
            inodes[j].i_mode = A_FOLDER;//inode的j号标记为folder
            super_block.inode_bitmap[j] = true;
            break;
        }
    }
    int k;
    for (k = 0; k < 4096; k++) {
        if (!super_block.block_bitmap[k]) {
            inodes[j].i_blocks[0] = k;
            super_block.block_bitmap[k] = true;
            break;
        }
    }
    dir_block *new_folder = getAddressByLocation_Folder(k);
    strcpy(new_folder->dirs[0].name, ".");
    strcpy(new_folder->dirs[1].name, "..");
    new_folder->dirs[0].inode_id = j;
    new_folder->dirs[1].inode_id = id;
    return j;
}

//mkdir $path
void newItem(string path) {
    string names[100];
    int layers = parsePath(path, names);
    //cout << names[0] << endl;
    int exist_layers;
    exist_layers = doesExist(names, layers, getPathType(path) == RELATIVE_PATH);
    //cout << "exist_layers = " << exist_layers << endl;
    if (exist_layers == layers)//执行mkdir却发现已经存在
    {
        setColor(COLOR_ERR);
        cout << path << " already exists.\n";
        setColor(COLOR_ORIGIN);
        return;
    }
    int id = getId(names, exist_layers, getPathType(path) == RELATIVE_PATH);
    //cout << names[0] << endl;
    //cout << "id = " << id << endl;
    for (int i = exist_layers; i < layers; i++) {
        //cout << "names[" << i << "] = " << names[i] << endl;
        id = makeDirectoryById(id, names[i]);
        //cout << "id = " << id << endl;
    }
}

//ls $path
void getChildItem(string path) {
    string cur_path = location;
    int cur_inode;
    string names[100];
    int layers = parsePath(path, names);
    if (doesExist(names, layers, getPathType(path) == RELATIVE_PATH) != layers) {
        cout << path << ": No such file or directory.\n";
        return;
    }
    cur_inode = getId(names, layers, getPathType(path) == RELATIVE_PATH);
    //cout << "cur_inode: " << cur_inode << endl;
    int blocks = inodes[cur_inode].i_blocks[0];
    dir_block *p = getAddressByLocation_Folder(blocks);
    for (int i = 0; i < 16; i++) {
        if (strcmp(p->dirs[i].name, "") != 0)//非空
        {
            if (inodes[p->dirs[i].inode_id].i_mode == A_FOLDER)
            {
                setColor(COLOR_FOLDER);
            }
            cout << p->dirs[i].name << "\t" << inodes[p->dirs[i].inode_id].i_file_size << "B" << endl;
            setColor(COLOR_ORIGIN);
        }
    }
}

//根据索引节点序号创建文件，返回该文件id
int makeFileById(int id, string name, string str) {
    dir_block *p = getAddressByLocation_Folder(inodes[id].i_blocks[0]);
    int i;
    for (i = 0; i < 16; i++) {
        if (strcmp(p->dirs[i].name, "") == 0) {//找到空的目录项
            char t[252];
            strcpy(t, name.c_str());
            strcpy(p->dirs[i].name, t);//名字
            break;
        }
    }
    int j;
    for (j = 0; j < 4096; j++) {
        if (!super_block.inode_bitmap[j]) {
            p->dirs[i].inode_id = j;
            inodes[j].i_mode = A_FILE;
            super_block.inode_bitmap[j] = true;
            break;
        }
    }
    int k;
    for (k = 0; k < 4096; k++) {
        if (!super_block.block_bitmap[k]) {
            inodes[j].i_blocks[0] = k;
            super_block.block_bitmap[k] = true;
            break;
        }
    }
    File_Block *q = getAddressByLocation_File(k);
    for (int m = 0; m < str.size(); m++) {
        q->data[m] = str[m];
    }
    inodes[j].i_file_size = str.size();//文件大小
    int cur = id;
    //cout << "j=" << j << endl;
    while (cur)
    {
        inodes[cur].i_file_size += str.size();
        cur = getParentById(cur);
        //cout << "cur: " << cur << endl;
    }
    inodes[0].i_file_size += str.size();
    return j;
}

//rmdir的递归函数
void delDirOrFile(int id, int parentid) {
    if (inodes[id].i_mode == A_FILE) {
        int size = inodes[id].i_file_size;
        inodes[parentid].i_file_size -= size;
        while(parentid != 0){
            parentid = getParentById(parentid);
            inodes[parentid].i_file_size -= size;
        }
        File_Block *p = getAddressByLocation_File(inodes[id].i_blocks[0]);
        inode *q = &inodes[id];
        super_block.inode_bitmap[id] = 0;
        super_block.block_bitmap[inodes[id].i_blocks[0]] = 0;
        memset(p, '\0', sizeof(File_Block));
        memset(q, '\0', sizeof(inode));
    }
    else {
        dir_block *p = getAddressByLocation_Folder(inodes[id].i_blocks[0]);
        for (int i = 2; i < 16; i++) {
            if (strcmp(p->dirs[i].name, "") != 0) {
                delDirOrFile(p->dirs[i].inode_id, id);
            }
        }
        inode *q = &inodes[id];
        super_block.inode_bitmap[id] = 0;
        super_block.block_bitmap[inodes[id].i_blocks[0]] = 0;
        memset(p, '\0', sizeof(dir_block));
        memset(q, '\0', sizeof(inode));
    }
}

///rmdir $path
void removeDirectory(string path) {
    string names[100];
    int layers = parsePath(path, names);
    int legal_layers = doesExist(names, layers, getPathType(path) == RELATIVE_PATH);
    if (legal_layers == layers) {
        int id = getId(names, layers, getPathType(path) == RELATIVE_PATH);
        int parentid = getId(names, layers - 1, getPathType(path) == RELATIVE_PATH);
        delDirOrFile(id, parentid);
        dir_block *parent = getAddressByLocation_Folder(inodes[parentid].i_blocks[0]);
        for (int i = 0; i < 16; i++) {
            if (strcmp(parent->dirs[i].name, names[layers - 1].c_str()) == 0) {
                dir_entry *temp = &parent->dirs[i];
                memset(temp, '\0', sizeof(dir_entry));
                break;
            }
        }
    }
    else {
        setColor(COLOR_ERR);
        cout << "Path is illegal" << endl;
        setColor(COLOR_ORIGIN);
    }
}

//echo $str $path
void writeOutput(string str, string path) {
    string names[100];
    int layers = parsePath(path, names);
    int legal_layers = doesExist(names, layers, getPathType(path) == RELATIVE_PATH);
    if (str[0] != '\"' || str[str.size() - 1] != '\"') {
        setColor(COLOR_ERR);
        cout << "File content is illegal." << endl;
        setColor(COLOR_ORIGIN);
    }
    else if (legal_layers == layers) {
        int id = getId(names, layers, getPathType(path) == RELATIVE_PATH);//找到该路径在inode中的i.id
        if (inodes[id].i_mode == A_FOLDER)
        {
            setColor(COLOR_ERR);
            cout << path << " is not a file.\n";
            setColor(COLOR_ORIGIN);
            return;
        }
        int parent_id = getId(names, layers - 1, getPathType(path) == RELATIVE_PATH);//parent id
        File_Block *p = getAddressByLocation_File(inodes[id].i_blocks[0]);
        memset(p, '\0', sizeof(File_Block));
        for (int i = 1; i < str.size() - 1; i++) {
            p->data[i - 1] = str[i];
        }
        int origin_size = inodes[id].i_file_size;
        inodes[id].i_file_size = (int) (str.size()) - 2;
        int dsize = origin_size - inodes[id].i_file_size;
        int cur = parent_id;
        while (cur)
        {
            inodes[cur].i_file_size -= dsize;
            cur = getParentById(cur);
        }
        inodes[0].i_file_size -= dsize;
    }
    else if (legal_layers + 1 == layers) {
        int id = getId(names, layers - 1, getPathType(path) == RELATIVE_PATH);//找到要创建文件所在目录的inode中的i.id
        string realstr = str.substr(1, str.size() - 2);
        makeFileById(id, names[layers - 1], realstr);
    }
    else {
        setColor(COLOR_ERR);
        cout << "Invalid path." << endl;
        setColor(COLOR_ORIGIN);
    }
}

//cat $path
void getContent(string path) {
    string names[100];
    int layers = parsePath(path, names);
    int legal_layers = doesExist(names, layers, getPathType(path) == RELATIVE_PATH);
    if (legal_layers == layers) {
        int id = getId(names, layers, getPathType(path) == RELATIVE_PATH);
        if (inodes[id].i_mode == A_FOLDER) {
            setColor(COLOR_ERR);
            cout << path << " is not a file." << endl;
            setColor(COLOR_ORIGIN);
        }
        else {
            File_Block *p = getAddressByLocation_File(inodes[id].i_blocks[0]);
            for (int i = 0; i < inodes[id].i_file_size; i++) {
                cout << p->data[i];
            }
            cout << endl;
        }
    }
    else {
        setColor(COLOR_ERR);
        cout << path << ": No such file or directory.\n";
        setColor(COLOR_ORIGIN);
    }
}

//rm $path
void removeFile(string path) {
    string names[100];
    int layers = parsePath(path, names);
    int legal_layers = doesExist(names, layers, getPathType(path) == RELATIVE_PATH);
    if (legal_layers == layers) {
        int id = getId(names, layers, getPathType(path) == RELATIVE_PATH);//找到该路径在inode中的i.id
        int size = inodes[id].i_file_size;
        File_Block *p = getAddressByLocation_File(inodes[id].i_blocks[0]);
        inode *q = &inodes[id];
        super_block.inode_bitmap[id] = 0;
        super_block.block_bitmap[inodes[id].i_blocks[0]] = 0;
        memset(p, '\0', sizeof(File_Block));
        memset(q, '\0', sizeof(inode));
        int parentid = getId(names, layers - 1, getPathType(path) == RELATIVE_PATH);//找到母文件夹
        dir_block *parent = getAddressByLocation_Folder(inodes[parentid].i_blocks[0]);
        for (int i = 0; i < 16; i++) {
            if (strcmp(parent->dirs[i].name, names[layers - 1].c_str()) == 0) {
                dir_entry *temp = &parent->dirs[i];
                memset(temp, '\0', sizeof(dir_entry));
                break;
            }
        }
        inodes[parentid].i_file_size -= size;
        while(parentid != 0){
            parentid = getParentById(parentid);
            inodes[parentid].i_file_size -= size;
        }
    }
    else {
        setColor(COLOR_ERR);
        cout << path << ": No such file or directory.\n";
        setColor(COLOR_ORIGIN);
    }
}

void printLines(int n) {
    /*if (n == 0)
     return;
     cout << "└";*/
    int i;
    for (i = 0; i < n; i++)
        cout << "\t";
}

void printTreeView(int id, int tabs) {
    dir_block *p = getAddressByLocation_Folder(inodes[id].i_blocks[0]);
    int i;
    for (i = 0; i < 16; i++) {
        if (strcmp(p->dirs[i].name, "")) {
            printLines(tabs);
            cout << p->dirs[i].name << endl;
            int nextid = p->dirs[i].inode_id;
            if (inodes[nextid].i_mode == A_FOLDER && strcmp(p->dirs[i].name, ".") && strcmp(p->dirs[i].name, "..")) {
                printTreeView(nextid, tabs + 1);
            }
        }
    }
}

void treeView(string path) {
    string names[100];
    int layers = parsePath(path, names);
    if (doesExist(names, layers, getPathType(path) == RELATIVE_PATH) == layers)//存在
    {
        int id = getId(names, layers, getPathType(path) == RELATIVE_PATH);
        printTreeView(id, 0);
    }
    else {
        setColor(COLOR_ERR);
        cout << path << ": No such file or directory.\n";
        setColor(COLOR_ORIGIN);
    }
}

//cpy $path1 $path2
void copyFile(string filepath, string dirpath) {
    string names1[100];
    int layers1 = parsePath(filepath, names1);
    int legal_layers1 = doesExist(names1, layers1, getPathType(filepath) == RELATIVE_PATH);
    if (layers1 == legal_layers1) {
        int id1 = getId(names1, layers1, getPathType(filepath) == RELATIVE_PATH);
        if (inodes[id1].i_mode == A_FOLDER) {
            setColor(COLOR_ERR);
            cout << filepath << " is not a file." << endl;
            setColor(COLOR_ORIGIN);
        }
        else {
            string names2[100];
            int layers2 = parsePath(dirpath, names2);
            int legal_layers2 = doesExist(names2, layers2, getPathType(dirpath) == RELATIVE_PATH);
            if (layers2 == legal_layers2) {//dirpath合法
                int id2 = getId(names2, layers2, getPathType(dirpath) == RELATIVE_PATH);
                int id3 = getId(names1, layers1 - 1, getPathType(filepath) == RELATIVE_PATH);
                if (inodes[id2].i_mode == A_FILE) {
                    setColor(COLOR_ERR);
                    cout << dirpath << " is not a folder.\n";
                    setColor(COLOR_ORIGIN);
                }
                else if (id2 == id3) {//若是需拷贝文件所在的目录
                    setColor(COLOR_ERR);
                    cout << "Invalid path." << endl;
                    setColor(COLOR_ORIGIN);
                }
                else {//彻底合法了。。。
                    string filename, filecontent;
                    File_Block *p = getAddressByLocation_File(inodes[id1].i_blocks[0]);
                    dir_block *q = getAddressByLocation_Folder(inodes[id3].i_blocks[0]);
                    filecontent = p->data;
                    for (int i = 2; i < 16; i++) {
                        if (strcmp(q->dirs[i].name, names1[layers1 - 1].c_str()) == 0) {
                            filename = q->dirs[i].name;
                            break;
                        }
                    }
                    makeFileById(id2, filename, filecontent);
                }
            }
            else {
                setColor(COLOR_ERR);
                cout << "Invalid path." << endl;
                setColor(COLOR_ORIGIN);
            }
        }
    }
    else {
        setColor(COLOR_ERR);
        cout << "Invalid path." << endl;

    }
}
