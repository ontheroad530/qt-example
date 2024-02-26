#include "frmzipcodes.h"
#include <QtWidgets/QApplication>

//------------------------------------------------------------
// 代码来源：书籍《Qt高级编程》第3章：模型/视图表格模型
// 知识点：
//	1、模型、表格视图、代理 编码范例
//	2、表格内容编辑时显示（代理）ItemDelegate：编辑时显示成 QSPinxBox、QCombbox
//	3、去重显示 UniqueProxyModel
//	4、过滤代理：ProxyModel
// 
//	其它：内容修改后 显示*，提示保存setWindowModified setWindowTitle[*]
//------------------------------------------------------------

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FrmZipCodes w;
    w.show();
    return a.exec();
}
