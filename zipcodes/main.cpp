#include "frmzipcodes.h"
#include <QtWidgets/QApplication>

//------------------------------------------------------------
// ������Դ���鼮��Qt�߼���̡���3�£�ģ��/��ͼ���ģ��
// ֪ʶ�㣺
//	1��ģ�͡������ͼ������ ���뷶��
//	2��������ݱ༭ʱ��ʾ������ItemDelegate���༭ʱ��ʾ�� QSPinxBox��QCombbox
//	3��ȥ����ʾ UniqueProxyModel
//	4�����˴���ProxyModel
// 
//	�����������޸ĺ� ��ʾ*����ʾ����setWindowModified setWindowTitle[*]
//------------------------------------------------------------

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FrmZipCodes w;
    w.show();
    return a.exec();
}
