/*
 * scpi.h
 *
 *  Created on: 2011-12-7
 *      Author: Administrator
 */

#ifndef BASESCPI_H_
#define BASESCPI_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "command.h"

//检索返回值结构定义
struct resultDef
{
    unsigned long int cmdCode;  //指令
    QString value;              //value
    char auxValue;              //Marker or trace index
    char auxValue2;             //Marker or trace index
};

typedef struct __NODE
{
  char data[128];
  char data_abbreviation[10];
  unsigned long funcode;
  bool isellipsis;
  struct __NODE *leftchild, *rightbrother;

  void init()
  {
    funcode = 0;
    leftchild = NULL;
    rightbrother = NULL;
    isellipsis = false;   //可省略项
    memset(data, 0, 128);
    memset(data_abbreviation, 0, 10);
  }
} CSNode, *pCSTree;

typedef struct _SCPI_keyboard
{
  char data[128];
  bool isellipsis;
  unsigned long funcode;
} SCPI_keyboard;

//解析数组
static struct _SCPI_keyboard parseCmd[MAX_GROUP];

//取出指令简写
static  _SCPI_keyboard abbreviationCmd[MAX_GROUP];

//数值
static _SCPI_keyboardCmd valueCmd;


//SCPI基类
class tBaseScpi : public QObject
{
    Q_OBJECT
  public:
    tBaseScpi();    //构造
    ~tBaseScpi();   //析构

    resultDef GetFunctionCode(QString str);
  protected:
    pCSTree root_tree;
    pCSTree prvNode;

    void initParseCmd(void);
    void CreatTree(pCSTree &tree, int num);
    void UpdataTree(pCSTree &tree, int num);
    CSNode* InsertChild(pCSTree &tree,pCSTree & element);
    char DeleteChild();
    int parseStringCmd(_SCPI_keyboardCmd scpi_key);
    void LowertoUpper(char *element, int n);
    bool compareCmd(pCSTree& tree,pCSTree& element);
    char* strupr(char* element,char *p);
    _SCPI_keyboardCmd StringtoCmd(char* element);
    CSNode* Search(pCSTree &tree,char *element);
    bool compareString(pCSTree& tree, char* element);
  private:
    char AUX_data;
    char AUX_data2;

    void initCommandList(void);                                  //获取指令集合
};

#endif /* SCPI_H_ */
