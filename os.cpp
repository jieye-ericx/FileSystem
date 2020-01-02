////////////////////////////////////////////////////////////////////////////
//
//  ��������ļ�����OSʵ��ο�����2008.CPP
//
////////////////////////////////////////////////////////////////////////////
//
// �����мٶ���
//   FAT[K]�洢FAT����������K��Ϊ5000������FAT[0]��ſ��̿�����
//   Disk[K][R]���ڴ洢��Ӧ���̿������(char��)��ÿ���̿�����R(=64)���ַ���
//   �ļ�Ŀ¼��FCB�У��ļ�����fattrib=1��ʾ��ֻ������=2��ʾ�����ء���=4��ʾ��ϵͳ����
//   �ļ�����fattrib=16��ʾ����Ŀ¼�������ļ��������Կ��Ի�����ϡ�
//   �û����ļ���UOF�У�״̬state=0��ʾ�յǼ�����=1��ʾ����������=2��ʾ���򿪡�״̬��
//   UOF�С��ļ����ԡ���Ϊ���򿪵��ļ������ԣ����ڡ�ֻ�����ļ����򿪺�ֻ�ܶ�������д��
//
//   ��ϵͳ�������������ļ�����Ŀ¼�����ִ�Сд�⣬���ಿ����ĸ�������ִ�Сд��
//
////////////////////////////////////////////////////////////////////////////
//
// ��ģ���ļ�ϵͳ���������²������
// dir [<Ŀ¼��>]������ʾ·����ָ����Ŀ¼���ݣ�
// cd [<Ŀ¼��>]����ָ����ǰĿ¼��·���С�..����ʾ��Ŀ¼��
// md <Ŀ¼��>����������Ŀ¼��
// rd <Ŀ¼��>����ɾ����Ŀ¼��
// create <�ļ���>[ <�ļ�����>]���������ļ���
// open <�ļ���>�������ļ���
// write <�ļ���> [<λ��/app>[ insert]]����д�ļ���
// read <�ļ���> [<λ��m> [<�ֽ���n>]]�������ļ���
// close <�ļ���>�����ر��ļ���
// ren <ԭ�ļ���> <���ļ���>�����ļ�������
// copy <Դ�ļ���> [<Ŀ���ļ���>]���������ļ���
// closeall�����رյ�ǰ�û������д򿪵��ļ�
// del <�ļ���>����ɾ��ָ�����ļ�
// type <�ļ���>������ʾָ���ļ������ݣ�
// undel [<Ŀ¼��>]�����ָ�ָ��Ŀ¼�б�ɾ�����ļ�
// help������ʾ�������ʹ�ø�ʽ��
// attrib <�ļ���> [��<����>]������ʾ[�޸�]�ļ�/Ŀ¼���ԡ�
// rewind <�ļ���>��������дָ���Ƶ��ļ���ͷ(��һ���ֽڴ�)
// fseek <�ļ���> <λ��n>����������дָ�붼�Ƶ�ָ��λ��n����
// block <�ļ���>������ʾ�ļ���Ŀ¼ռ�õ��̿�š�
// uof������ʾ�û���UOF(�ļ��򿪱�)��
// prompt������ʾ������ʾ/����ʾ��ǰĿ¼���л���
// fat������ʾģ����̵Ŀ��п���(FAT����0�ĸ���)��
// check�������˶�FAT���Ӧ�Ŀ��п�����
// exit������������������С�
//
////////////////////////////////////////////////////////////////////////////

#include <iostream> //cout,cin
#include <iomanip>  //setw(),setiosflags()
#include <stdlib.h> //exit(),atoi()
#include <cstdlib>
#include <string.h> //strcpy(),_stricmp()
#include <fstream>  //�ļ�������
#include <cstring>
#include <strings.h>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

//�������ɷ��ų���
#define S 32            //�������ͬʱ��32���ļ�
#define K 5000          //������̹���5000���̿�
#define SIZE 256        //������̵Ĵ�С��64�ֽ�
#define CK 8            //����ֽ��Ķ���
#define INPUT_LEN 128   //���뻺��������
#define COMMAND_LEN 11  //�����ַ�������
#define FILENAME_LEN 11 //�ļ�������
#define PATH_LEN INPUT_LEN - COMMAND_LEN
#define DM 40 //�ָ���ɾ���ļ���ı�����
//����
#define BATCHNUM 1000 //batch�����ļ�������������

struct FCB //�����ļ�Ŀ¼��FCB�Ľṹ(��16���ֽ�)
{
    char FileName[FILENAME_LEN]; //�ļ���(1��10�ֽ�)
    char Fattrib;                //�ļ�����
    short int Addr;              //�ļ��׿��
    short int Fsize;             //�ļ�����
};

struct UOF //�����û����ļ���Ľṹ
{
    char fname[PATH_LEN]; //�ļ�ȫ·����
    char attr;            //�ļ����ԣ�1=ֻ�ɶ���0=�ɶ�д
    short int faddr;      //�ļ����׿��
    short int fsize;      //�ļ���С(�ֽ���)
    FCB *fp;              //���ļ���Ŀ¼��ָ��
    short int state;      //״̬��0=�ձ��1=�½���2=��
    short int readp;      //��ָ�룬ָ��ĳ��Ҫ�����ַ�λ�ã�0=���ļ�
    short int writep;     //д��ָ�룬ָ��ĳ��Ҫд�����ַ�λ��
};

struct CurPath //����洢��ǰĿ¼�����ݽṹ
{
    short int fblock;     //��ǰĿ¼���׿��
    char cpath[PATH_LEN]; //��ǰĿ¼����·���ַ���(ȫ·����)
};

// struct UnDel //�ָ���ɾ���ļ�������ݽṹ
// {
//     char gpath[PATH_LEN];      //��ɾ���ļ���ȫ·����(�����ļ���)
//     char ufname[FILENAME_LEN]; //��ɾ���ļ���
//     short ufaddr;              //��ɾ���ļ������׿��
//     short fb;                  //�洢��ɾ���ļ���ŵĵ�һ�����(����ͷָ��)
//                                //�׿��Ҳ����fb��ָ���̿���
// };
struct UnDel //�ָ���ɾ���ļ������ݽṹ(��128�ֽ�)
{
    char gpath[112];           //��ɾ���ļ���ȫ·����(�����ļ���)
    char ufname[FILENAME_LEN]; //��ɾ���ļ���
    char ufattr;               //��ɾ���ļ�����
    short ufaddr;              //��ɾ���ļ����׿��
    short fb;                  //�洢��ɾ���ļ����ȼ���ŵ�ָ��(�׿��)
};

//���ڻָ���ɾ���ļ����⣬�����Բ���������Windows�Ļ���վ�ķ�������������ڸ�Ŀ¼��
//����һ��������ļ���recycled (������Ϊ��ֻ�������ء�ϵͳ)����FCB��¼�ṹ�еĳ�Ա
//Fsize���������洢�ļ����ȣ��������洢һ���̿�ţ����̿��д洢�ļ����Ⱥ��ļ���ȫ·
//����(�����ļ���)������ġ�ȫ·�����������ļ���ԭλ�ã���ԭ�ļ�ʱ����Ϣ�ǲ��ɻ�ȱ�ġ�
//dir�������recycled�ļ���ʱ������ͨ�ļ������в�ͬ(�����ļ����ȵ���ϢҪ��Fsize��
//�̿���ȡ��������ֱ�ӻ��)��rd����Ӧ�޸ĳɲ���ɾ���ļ���recycled��copy,move,replace
//������Ҳ�ĳɲ��ܶ��ļ���recycled������

//����del����ɾ���ļ�ʱ,�����ļ����й���Ϣ���浽������ļ���recycled�У��༴���ļ����ᡱ
//������վ���ļ�ռ�õĴ��̿ռ䲢���ͷţ��ָ�ʱ�����෴����ջ���վʱ���ͷŴ��̿ռ䡣
//�˷�����ǰ��UnDel�ṹ�ķ����ķѸ���Ĵ��̿ռ�(ɾ�����ļ���ռ�ô��̿ռ�)

int FAT[K];                           //FAT��,�̿���ΪK
int BatchHeader = 0;                  //�����±��ͷָ��
int BatchRail = 0;                    //�����±��βָ��
char BatchComds[BATCHNUM][INPUT_LEN]; //����batch����Ĵ������
//char (*Disk)[SIZE]=new char [K][SIZE];//������̿ռ䣬ÿ���̿�����ΪSIZE���ֽ�
char Disk[K][SIZE];
UOF uof[S];                       //�û����ļ���UOF,���ͬʱ��S���ļ�
char comd[CK][PATH_LEN];          //��������ʱʹ��
char temppath[PATH_LEN];          //��ʱ·��(ȫ·��)
char newestOperateFile[PATH_LEN]; //�����ļ�
CurPath curpath;
UnDel udtab[DM]; //����ɾ���ļ��ָ����˳�ϵͳʱ�ñ�ɴ����ļ�UdTab.dat��
short Udelp = 0; //udtab��ĵ�һ���ձ�����±꣬ϵͳ��ʼ��ʱ��Ϊ0��
                 //��Udelp=DMʱ����ʾ�����������������ı���(������������ǰ��)
short ffbp = 1;
short udtabblock = 4079;
//0���̿��д洢�������ݣ�
//	short ffbp;		//�Ӹ�λ�ÿ�ʼ���ҿ����̿�(����ѭ���״���Ӧ����)
//	short Udelp;	//udtab��ĵ�һ���ձ�����±�
// short udtabblock; //udtab����׿��
int dspath = 1; //dspath=1,��ʾ������ʾ��ǰĿ¼

//����ԭ��˵��
int CreateComd(int);                     //create�������
int OpenComd(int);                       //open�������
int ReadComd(int);                       //read�������
int WriteComd(int);                      //write�������
int CloseComd(int);                      //close�������
void CloseallComd(int);                  //closeaal�������, �رյ�ǰ�û����д򿪵��ļ�
int DelComd(int);                        //del�������
int UndelComd(int);                      //undel����������ָ���ɾ���ļ�
int CopyComd(int);                       //copy�������
int DirComd(int);                        //dir�����������ʾָ�����ļ�Ŀ¼����Ƶ��ʹ��
int CdComd(int);                         //cd�������
int MdComd(int);                         //md�������
int RdComd(int);                         //rd�������
int TypeComd(int);                       //type�������
int RenComd(int);                        //ren�������
int AttribComd(int);                     //attrib�������
void UofComd(void);                      //uof�������
void HelpComd(void);                     //help�������
int FindPath(char *, char, int, FCB *&); //��ָ��Ŀ¼(���׿��)
int FindFCB(char *, int, char, FCB *&);  //��ָ�����ļ���Ŀ¼
int FindBlankFCB(short s, FCB *&fcbp1);  //Ѱ���׿��Ϊs��Ŀ¼�еĿ�Ŀ¼��
int RewindComd(int);                     //rewind�������, ����дָ���Ƶ��ļ���ͷ(��һ���ֽڴ�)
int FseekComd(int);                      //fseek�������, ����дָ���Ƶ��ļ���n���ֽڴ�
int blockf(int);                         //block�������(��ʾ�ļ�ռ�õ��̿��)
int delall(int);                         //delall�������, ɾ��ָ��Ŀ¼�е������ļ�
void save_FAT(void);
void save_Disk(void);
int getblock(void); //���һ���̿�
void FatComd(void);
void CheckComd(void);
int Check_UOF(char *);
void ExitComd(void);
bool IsName(char *);                         //�ж������Ƿ���Ϲ���
void PromptComd(void);                       //prompt�����ʾ���Ƿ���ʾ��ǰĿ¼���л�
void UdTabComd(void);                        //udtab�����ʾudtab������
void releaseblock(short s);                  //�ͷ�s��ʼ���̿���
int buffer_to_file(FCB *fcbp, char *Buffer); //Bufferд���ļ�
int file_to_buffer(FCB *fcbp, char *Buffer); //�ļ����ݶ���Buffer,�����ļ�����
int ParseCommand(char *);                    //������������зֽ������Ͳ�����
void ExecComd(int);                          //ִ������
//��������=========================================================
int fcComd(int);
int replaceComd(int);
int moveComd(int);
int batchComd(int);
//�Ǳ�׼�⺯��ʵ��=============================================================================
char *strlwr(char *src)
{
    while (*src != '\0')
    {
        if (*src > 'A' && *src <= 'Z')
        {
            //*src += 0x20;
            *src += 32;
        }
        src++;
    }
    return src;
}

char *getFileName()
{
    int len = strlen(temppath), x;
    for (x = len - 1; x >= 0; x--)
        if (temppath[x] == '/')
            break;
    return &temppath[x + 1];
}

int fleshBlock(FCB *fcbp)
{
    int t, fileBlock = fcbp->Addr;
    fcbp->FileName[0] = (char)0xe5;
    while (fileBlock > 0) //ȷ�ϸ��Ǻ���մ��̿ռ�
    {
        t = fileBlock;
        fileBlock = FAT[fileBlock];
        FAT[t] = 0;
        FAT[0]++;
    }
    return 1;
}

int showAttribute(FCB *fcbp)
{
    char Attr[5], Attr1[4] = "RHS";
    char or_and[6] = {(char)1, (char)2, (char)4, (char)30, (char)29, (char)27};
    char Attrib = fcbp->Fattrib & (char)7;
    cout << setw(11) << fcbp->FileName;
    if(fcbp->Fattrib>=(char)7){
        cout <<setw(7)<< " <DIR> ";
    }else{
        cout <<setw(7)<< "       ";
    }
    if (Attrib == (char)0)
        strcpy(Attr, "��ͨ");
    else
    {
        int i;
        for (i = 0; i < 3; i++)
        {
            if (Attrib & or_and[i])
                Attr[i] = Attr1[i];
            else
                Attr[i] = ' ';
        }
        Attr[i] = '\0';
    }
    cout << setiosflags(ios::right) << setw(5) << Attr << endl;
    return 1;
}

// #define INIT	//������ʼ�����ǴӴ��̶���
int main()
{

    char cmd[INPUT_LEN]; //�����л�����
    int i, k;
    // ����ϵͳʱ����ǰĿ¼�Ǹ�Ŀ¼
    curpath.fblock = 1;         //��ǰĿ¼(��Ŀ¼)���׿��
    strcpy(curpath.cpath, "/"); //��Ŀ¼��·���ַ���

#ifdef INIT
    int j;
    FCB *fcbp;
    // *********** ��ʼ��FAT��Disk ************
    for (i = 0; i < K; i++)  //��ʼʱ�����̿����
        FAT[i] = 0;          //�����̿���
    FAT[0] = K - 1;          //FAT[0]�б�������̿���
    for (i = 1; i < 30; i++) //�����Ŀ¼�̿���
    {
        FAT[i] = i + 1; //��ʼ����Ŀ¼��FAT��
        FAT[0]--;       //���̿�����1
    }
    FAT[i] = -1; //��Ŀ¼β���
    FAT[0]--;    //���̿�����1
    for (i++; i <= 40; i++)
    {
        FAT[i] = -1; //����Ŀ¼β���
        FAT[0]--;
    }
    for (i = 4979; i < K - 1; i++) //�����Ŀ¼�̿���
    {
        FAT[i] = i + 1; //��ʼ����Ŀ¼��FAT��
        FAT[0]--;       //���̿�����1
    }
    FAT[i] = -1;
    // *********** ��ʼ��Disk ************
    fcbp = (FCB *)Disk[1];
    j = 40 * SIZE / sizeof(FCB);
    for (i = 1; i <= j; i++)
    {
        fcbp->FileName[0] = (char)0xe5; //��ʼĿ¼����Ŀ¼�г�ʼ��Ϊ��Ŀ¼��
        fcbp++;
    }
    //���½�����ʼĿ¼���и�����Ŀ¼
    fcbp = (FCB *)Disk[1];
    strcpy(fcbp->FileName, "bin"); //��Ŀ¼bin
    fcbp->Fattrib = 16;            //��ʾ����Ŀ¼
    fcbp->Addr = 31;               //����Ŀ¼�����̿����31
    fcbp->Fsize = 0;               //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp++;                        //ָ����һ��Ŀ¼��
    strcpy(fcbp->FileName, "usr"); //��Ŀ¼usr
    fcbp->Fattrib = 16;            //��ʾ����Ŀ¼
    fcbp->Addr = 32;               //����Ŀ¼�����̿����32
    fcbp->Fsize = 0;               //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "auto"); //�ļ�unix��Ŀ¼��
    fcbp->Fattrib = 0;              //��ʾ����ͨ�ļ�
    fcbp->Addr = 0;                 //����Ŀ¼�����̿����0����ʾ�ǿ��ļ�
    fcbp->Fsize = 0;                //���ļ��ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "radoapx"); //�ļ�unix��Ŀ¼��
    fcbp->Fattrib = 16;                //��ʾ����ͨ�ļ�
    fcbp->Addr = 40;                   //����Ŀ¼�����̿����0����ʾ�ǿ��ļ�
    fcbp->Fsize = 0;                   //���ļ��ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "dev"); //��Ŀ¼etc
    fcbp->Fattrib = 16;            //��ʾ����Ŀ¼
    fcbp->Addr = 33;               //����Ŀ¼�����̿����33
    fcbp->Fsize = 0;               //Լ����Ŀ¼�ĳ���Ϊ0

    fcbp = (FCB *)Disk[31];
    strcpy(fcbp->FileName, ".."); //bin�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 1;               //��Ŀ¼(�˴��Ǹ�Ŀ¼)�����̿����1
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp = (FCB *)Disk[40];
    strcpy(fcbp->FileName, ".."); //radoapx�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 1;               //��Ŀ¼(�˴��Ǹ�Ŀ¼)�����̿����1
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0

    fcbp = (FCB *)Disk[32];
    strcpy(fcbp->FileName, ".."); //usr�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 1;               //��Ŀ¼(�˴��Ǹ�Ŀ¼)�����̿����1
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "user"); //��Ŀ¼lib
    fcbp->Fattrib = 16;             //��ʾ����Ŀ¼
    fcbp->Addr = 34;                //����Ŀ¼�����̿����34
    fcbp->Fsize = 0;                //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "lib"); //��Ŀ¼user
    fcbp->Fattrib = 16;            //��ʾ����Ŀ¼
    fcbp->Addr = 35;               //����Ŀ¼�����̿����35
    fcbp->Fsize = 0;               //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "bin"); //��Ŀ¼bin
    fcbp->Fattrib = 16;            //��ʾ����Ŀ¼
    fcbp->Addr = 36;               //����Ŀ¼�����̿����36
    fcbp->Fsize = 0;               //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp = (FCB *)Disk[33];
    strcpy(fcbp->FileName, ".."); //etc�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 1;               //��Ŀ¼(�˴��Ǹ�Ŀ¼)�����̿����1
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp = (FCB *)Disk[34];
    strcpy(fcbp->FileName, ".."); //lib�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 32;              //��Ŀ¼(�˴���usrĿ¼)�����̿����32
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "lin"); //��Ŀ¼liu
    fcbp->Fattrib = 16;            //��ʾ����Ŀ¼
    fcbp->Addr = 37;               //����Ŀ¼�����̿����37
    fcbp->Fsize = 0;               //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "sun"); //��Ŀ¼sun
    fcbp->Fattrib = 16;            //��ʾ����Ŀ¼
    fcbp->Addr = 38;               //����Ŀ¼�����̿����38
    fcbp->Fsize = 0;               //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp++;
    strcpy(fcbp->FileName, "ma"); //��Ŀ¼fti
    fcbp->Fattrib = 16;           //��ʾ����Ŀ¼
    fcbp->Addr = 39;              //����Ŀ¼�����̿����39
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp = (FCB *)Disk[35];
    strcpy(fcbp->FileName, ".."); //user�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 32;              //��Ŀ¼(�˴���usrĿ¼)�����̿����32
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp = (FCB *)Disk[36];
    strcpy(fcbp->FileName, ".."); //usr/bin�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 32;              //��Ŀ¼(�˴���usrĿ¼)�����̿����32
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp = (FCB *)Disk[37];
    strcpy(fcbp->FileName, ".."); //usr/lib/liu�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 34;              //��Ŀ¼(�˴���usr/libĿ¼)�����̿����34
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp = (FCB *)Disk[38];
    strcpy(fcbp->FileName, ".."); //usr/lib/sun�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 34;              //��Ŀ¼(�˴���usr/libĿ¼)�����̿����34
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0
    fcbp = (FCB *)Disk[39];
    strcpy(fcbp->FileName, ".."); //usr/lib/fti�ĸ�Ŀ¼��Ӧ��Ŀ¼��
    fcbp->Fattrib = 16;           //��ʾ��Ŀ¼�������ļ�
    fcbp->Addr = 34;              //��Ŀ¼(�˴���usr/libĿ¼)�����̿����34
    fcbp->Fsize = 0;              //Լ����Ŀ¼�ĳ���Ϊ0

    // *********** ��ʼ��UnDel�� ************
    Udelp = 0;

    ffbp = 1; //��FAT��ͷ���ҿ����̿�

#else

    // �����ļ������FAT
    char yn;
    ifstream ffi("FAT.txt", ios::in); //���ļ�FAT.txt
    if (!ffi)
    {
        cout << "Can't open FAT.txt!\n";
        cin >> yn;
        exit(0);
    }
    for (i = 0; i < K; i++) //���ļ�FAT.txt�����ļ������FAT
        if (ffi)
            ffi >> FAT[i];
        else
            break;
    ffi.close();

    //������̿�Disk[ ]��Ϣ
    ffi.open("Disk.dat", ios::binary | ios::in);
    if (!ffi)
    {
        cout << "Can't open Disk.dat!\n";
        cin >> yn;
        exit(0);
    }
    for (i = 0; i < K; i++) //���ļ�Disk.dat�����̿�����
        if (ffi)
            ffi.read((char *)&Disk[i], SIZE);
        else
            break;
    ffi.close();

    // //����ָ�ɾ���ļ���UdTab.dat��Ϣ
    // ffi.open("UdTab.dat", ios::binary | ios::in);
    // if (!ffi)
    // {
    //     cout << "Can't open UdTab.dat!\n";
    //     cin >> yn;
    //     exit(0);
    // }
    // for (i = 0; i < DM; i++) //���ļ�Disk.dat�����̿�����
    //     if (ffi)
    //         ffi.read((char *)&udtab[i], sizeof(udtab[0]));
    //     else
    //         break;
    ffi.close();

    short *pp = (short *)Disk[0];
    ffbp = pp[0];
    Udelp = pp[1];
    udtabblock = pp[2];
    UnDel *ud = (UnDel *)Disk[udtabblock];
    for (i = 0; i < DM; i++)
    {
        udtab[i] = *ud;
        ud++;
    }

#endif
    for (i = 0; i < S; i++) //��ʼ��UOF��state��0���ձ��1���½���2����
        uof[i].state = 0;   //��ʼ��Ϊ�ձ���

    cout << "\n���������������ֲ�������.\n"
         << "Help ���� ���װ�����Ϣ.\n"
         << "exit ���� �˳�������.\n";
    while (1) //ѭ�����ȴ��û��������ֱ�����롰exit������ѭ�����������
    {         //�������������ִ������

        while (1)
        {
            cout << "\nC:"; //��ʾ��ʾ��(��ϵͳ�ܼٶ���C��)
            if (dspath)
                cout << curpath.cpath;
            cout << ">";
            if (BatchHeader == BatchRail)
            {
                cin.getline(cmd, INPUT_LEN); //��������
            }
            else
            {
                strcpy(cmd, BatchComds[BatchRail]);
                BatchRail = (BatchRail + 1) % BATCHNUM;
                cout << cmd << endl;
            }
            if (strlen(cmd) > 0)
                break;
        }
        k = ParseCommand(cmd); //�ֽ���������
                               //comd[0]�������comd[1],comd[2]...�ǲ���
        ExecComd(k);           //ִ������
    }
    return 0;
}

/////////////////////////////////////////////////////////////////

int ParseCommand(char *p) //������������зֽ������Ͳ�����
{
    //CK ����ֽ��Ķ���
    int i, j, k, g = 0, flag = 0;
    for (i = 0; i < CK; i++) //��ʼ��comd[][]
        comd[i][0] = '\0';
    for (k = 0; k < CK; k++)
    { //�ֽ�����������comd[0]�������comd[1],comd[2]...�ǲ���
        for (i = 0; *p != '\0'; i++, p++)
        {
            if (*p != ' ')       //�ո����������֮��ķָ���
                comd[k][i] = *p; //ȡ�����ʶ��
            else
            {
                comd[k][i] = '\0';
                if (strlen(comd[k]) == 0)
                    k--;
                p++;
                break;
            }
        }

        if (*p == '\0') //�������һ���ַ����ֽ����
        {
            comd[k][i] = '\0';
            break;
        }
    }

    for (i = 0; comd[0][i] != '\0'; i++)
    {
        if (comd[0][i] == '/') //����cd/��dir/usr�����
        {
            flag = 1;
            break;
        }
        else if (comd[0][i] == '.' && comd[0][i + 1] == '.')
        {
            flag = 2;
            break;
        }
    }

    if (flag) //comd[0]�д����ַ�'/'
    {
        //TODO:����comd��CK������ô�죿��
        if (k > 0)
            for (j = k; j > 0; j--)
                strcpy(comd[j + 1], comd[j]); //����
        strcpy(comd[1], &comd[0][i]);
        comd[0][i] = '\0';
        k++; //���һ������
    }

    for (int i = 0; i <= k; i++)
    {
        for (int j = 0; comd[i][j] != '\0'; j++)
        {
            if (comd[i][j] == '>' && j != 0)
            {
                for (int h = k; h > i; h--)
                {
                    strcpy(comd[h + 1], comd[h]);
                }
                strcpy(comd[i + 1], &comd[i][j]);
                comd[i][j] = '\0';
                k += 1;
                break;
            }
            if (comd[i][j] == '>' && j == 0)
            {
                if ((comd[i][j + 1] == '>' && comd[i][j + 2] == '\0') || (comd[i][j + 1] == '\0'))
                {
                    break;
                }
                for (int h = k; h > i; h--)
                {
                    strcpy(comd[h + 1], comd[h]);
                }
                if (comd[i][j + 1] == '>')
                {
                    strcpy(comd[i + 1], &comd[i][j + 2]);
                    comd[i][j + 2] = '\0';
                    j = j + 1;
                }
                else
                {
                    strcpy(comd[i + 1], &comd[i][j + 1]);
                    comd[i][j + 1] = '\0';
                }
                k++;
            }
        }
    }
    return k;
}

/////////////////////////////////////////////////////////////////

void ExecComd(int k) //ִ������
{
    int cid; //�����ʶ

    //���������
    char CmdTab[][COMMAND_LEN] = {"create", "open", "write", "read", "close",
                                  "del", "dir", "cd", "md", "rd", "ren", "copy", "type", "help", "attrib",
                                  "uof", "closeall", "block", "rewind", "fseek", "fat", "check", "exit",
                                  "undel", "Prompt", "udtab", "fc", "replace", "move", "batch"};
    int M = sizeof(CmdTab) / COMMAND_LEN;          //ͳ���������
    for (cid = 0; cid < M; cid++)                  //��������м�������
        if (strcasecmp(CmdTab[cid], comd[0]) == 0) //������ִ�Сд
            break;
    //����������У��������ͨ��ȫ�ֱ���comd[][]���ݣ���δ�����ں�����������
    switch (cid)
    {
    case 0:
        CreateComd(k); //create��������ļ�
        break;
    case 1:
        OpenComd(k); //open������ļ�
        break;
    case 2:
        WriteComd(k); //write���kΪ�����еĲ�������(��������)
        break;
    case 3:
        ReadComd(k); //read������ļ�
        break;
    case 4:
        CloseComd(k); //close����ر��ļ�
        break;
    case 5:
        DelComd(k); //del���ɾ���ļ�
        break;
    case 6:
        DirComd(k); //dir����
        break;
    case 7:
        CdComd(k); //cd����
        break;
    case 8:
        MdComd(k); //md����
        break;
    case 9:
        RdComd(k); //rd����
        break;
    case 10:
        RenComd(k); //ren����ļ�����
        break;
    case 11:
        CopyComd(k); //copy��������ļ�
        break;
    case 12:
        TypeComd(k); //type�����ʾ�ļ�����(���)
        break;
    case 13:
        HelpComd(); //help���������Ϣ
        break;
    case 14:
        AttribComd(k); //attrib����޸��ļ�����
        break;
    case 15:
        UofComd(); //uof�����ʾ�û���UOF(�ļ��򿪱�)
        break;
    case 16:
        CloseallComd(1); //closeall����ر������ļ�
        break;
    case 17:
        blockf(k); //block�����ʾ�ļ����̿��
        break;
    case 18:
        RewindComd(k); //rewind�������ָ���Ƶ��ļ���ͷ
        break;
    case 19:
        FseekComd(k); //fseek���������дָ�붼�Ƶ�ָ����¼��
        break;
    case 20:
        FatComd(); //fat����
        break;
    case 21:
        CheckComd(); //check����
        break;
    case 22:
        ExitComd(); //exit����
        break;
    case 23:
        UndelComd(k); //undel����
        break;
    case 24:
        PromptComd(); //prompt����
        break;
    case 25:
        UdTabComd(); //udtab����
        break;
    case 26:
        fcComd(k);
        break;
    case 27:
        replaceComd(k);
        break;
    case 28:
        moveComd(k);
        break;
    case 29:
        batchComd(k);
        break;
    default:
        cout << "\n�����:" << comd[0] << endl;
    }
}

//////////////////////////////////////////////////////////////////////////////////////

void HelpComd() //help���������Ϣ(��ʾ�������ʽ)
{
    cout << "\n* * * * * * * * * ��ϵͳ��Ҫ���ļ���������������� * * * * * * * * * *\n\n";
    cout << "create <�ļ���>[ <�ļ�����>]�����������������������ļ�,�ļ�������r��h��s��\n";
    cout << "open <�ļ���>                           �������ļ����������Ϳ�Ϊr��h��(��)s��\n";
    cout << "write <�ļ���> [<λ��/app>[ insert]]    ������ָ��λ��д�ļ�(�в��빦��)��\n";
    cout << "read <�ļ���> [<λ��m> [<�ֽ���n>]]     �������ļ����ӵ�m�ֽڴ���n���ֽڡ�\n";
    cout << "close <�ļ���>�������������������������������ر��ļ���\n";
    cout << "del <�ļ���>                            ��������(ɾ��)�ļ���\n";
    cout << "dir [<·����>] [|<����>]                ������ʾ��ǰĿ¼��\n";
    cout << "cd [<·����>]                           �����ı䵱ǰĿ¼��\n";
    cout << "md <·����> [<����>]                    ��������ָ��Ŀ¼��\n";
    cout << "rd [<·����>]                           ����ɾ��ָ��Ŀ¼��\n";
    cout << "ren <���ļ���> <���ļ���>               �����ļ�������\n";
    cout << "attrib <�ļ���> [��<����>]              �����޸��ļ�����(r��h��s)��\n";
    cout << "copy <Դ�ļ���> [<Ŀ���ļ���>]          ���������ļ���\n";
    cout << "type <�ļ���>                           ������ʾ�ļ����ݡ�\n";
    cout << "rewind <�ļ���>                         ����������дָ���Ƶ��ļ���һ���ַ�����\n";
    cout << "fseek <�ļ���> <λ��>                   ����������дָ�붼�Ƶ�ָ��λ�á�\n";
    cout << "block <�ļ���>                          ������ʾ�ļ�ռ�õ��̿�š�\n";
    cout << "closeall                                �����رյ�ǰ�򿪵������ļ���\n";
    cout << "uof                                     ������ʾUOF(�û����ļ���)��\n";
    cout << "undel [<·����>]                        �����ָ�ָ��Ŀ¼�б�ɾ�����ļ���\n";
    cout << "exit                                    �����˳�������\n";
    cout << "prompt                                  ������ʾ���Ƿ���ʾ��ǰĿ¼(�л�)��\n";
    cout << "fat                                     ������ʾFAT���п����̿���(0�ĸ���)��\n";
    cout << "check                                   �����˶Ժ���ʾFAT���п����̿�����\n";
}

/////////////////////////////////////////////////////////////////

int GetAttrib(char *str, char &attrib)
{
    int i, len;
    char ar = (char)1, ah = (char)2, as = (char)4;
    // if (str[0] != '|')
    // {
    // 	cout << "\n���������Բ�������\n";
    // 	return -1;
    // }
    len = strlen(str);
    strlwr(str); //ת����Сд��ĸ
    if (str[0] == '|')
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    for (; i < len; i++)
    {
        switch (str[i])
        {
        case 'r':
            attrib = attrib | ar;
            break;
        case 'h':
            attrib = attrib | ah;
            break;
        case 's':
            attrib = attrib | as;
            break;
        default:
            cout << "\n���������Բ�������\n";
            return -1;
        }
    }
    return 1;
}

/////////////////////////////////////////////////////////////////

int DirComd(int k) //dir�����ʾָ��Ŀ¼�����ݣ��ļ�����Ŀ¼���ȣ�
{
    // ������ʽ��dir[ <Ŀ¼��>[ <����>]]
    // ����ܣ���ʾ"Ŀ¼��"ָ����Ŀ¼���ļ����͵�һ����Ŀ¼������ָ
    // ��Ŀ¼�����ڣ������������Ϣ�����������û��ָ��Ŀ¼��������ʾ
    // ��ǰĿ¼�µ���Ӧ���ݡ�����������"����"����������ʾָ��Ŀ¼��"��
    // ����"���Ե�ȫ���ļ����͵�һ����Ŀ¼��������������"����"��������
    // ����ʾָ�����Ե��ļ�����Ŀ¼����h��r��s�����߶��У�����ʾ������
    // �Ի�ֻ�����Ի������������ֻ�����Ե��ļ������Բ�������ʽ��"|<��
    // �Է���>"���������Է�����r��h��s���֣������ִ�Сд�����ֱ��ʾ"ֻ
    // ��"��"����"��"ϵͳ"��������,���ǿ������ʹ���Ҵ����ޡ�����"|rh"
    // ��"|hr"����ʾҪ����ʾͬʱ����"ֻ��"��"����"���Ե��ļ���Ŀ¼������
    // ʾ�ļ���ʱ����ʾ���ļ����ȣ���ʾĿ¼��ʱ��ͬʱ��ʾ"<DIR>"��������

    // ������
    //		dir /usr |h
    // ����������ʾ��Ŀ¼��usr��Ŀ¼��ȫ��"����"���Ե��ļ�������Ŀ¼��
    //		dir ..
    // ����������ʾ��ǰĿ¼�ĸ�Ŀ¼��ȫ��"������"���Ե��ļ�����Ŀ¼��(��
    // ��"ֻ��"���Ե�Ҳ��ʾ����һ�㲻��ʾ"ϵͳ"���Եģ���Ϊ"ϵͳ"���ԵĶ�
    // ��һ��Ҳ��"����"���Ե�)��
    //
    // ѧ���ɿ��ǽ��˺����޸ĳ������е�·��������������ļ����������
    // ���⻹���Կ��Ǻ�ͨ��������⡣

    short i, s, ss, status = 0, pos, newFile;
    short filecount, dircount, fsizecount; //�ļ�����Ŀ¼�����ļ������ۼ�
    char ch, attrib = '\0', attr, cc;
    FCB *fcbp, *p, *target;

    filecount = dircount = fsizecount = 0;
    if (k > 4) //�����ж���1������������(�ϸ��ӵĴ���Ӧ�������ж������)
    {
        cout << "\n������󣺲���̫�ࡣ\n";
        return -1;
    }
    for (pos = 1; pos <= k; pos++)
    {
        if (strcmp(comd[pos], ">") == 0)
        {
            status = 1;
            break;
        }
        else if (strcmp(comd[pos], ">>") == 0)
        {
            status = 2;
            break;
        }
    }
    k = pos - 1;
    if (k < 1) //�����޲�������ʾ��ǰĿ¼
    {
        strcpy(temppath, curpath.cpath);
        s = curpath.fblock; //��ǰĿ¼���׿�ű�����s
    }
    else if (k == 1) //������1������(k=1)
    {
        if (comd[1][0] == '|')
        {
            i = GetAttrib(comd[1], attrib);
            if (i < 0)
                return i;
            strcpy(temppath, curpath.cpath);
            s = curpath.fblock; //��ǰĿ¼���׿�ű�����s
        }
        else
        {
            s = FindPath(comd[1], '\020', 1, fcbp); //��ָ��Ŀ¼(���׿��)
            if (s < 1)
            {
                cout << "\n�����·������" << endl;
                return -1;
            }
        }
    }
    else //������2������(k=2)
    {
        s = FindPath(comd[1], '\020', 1, fcbp); //��ָ��Ŀ¼(���׿��)
        if (s < 1)
        {
            cout << "\n�����·������" << endl;
            return -1;
        }
        i = GetAttrib(comd[2], attrib);
        if (i < 0)
            return i;
    }
    ss = s;
    char buf[1000000], *tempBuf;

    // cout << "\nThe Directory of C:" << temppath << endl
    //      << endl;
    strcpy(buf, "\nThe Directory of C:");
    strcat(buf, temppath);
    strcat(buf, "\n");
    while (s > 0)
    {
        p = (FCB *)Disk[s]; //pָ���Ŀ¼�ĵ�һ���̿�
        for (i = 0; i < SIZE / sizeof(FCB); i++, p++)
        {
            ch = p->FileName[0];  //ȡ�ļ�(Ŀ¼)���ĵ�һ���ַ�
            if (ch == (char)0xe5) //��Ŀ¼��
                continue;
            if (ch == '\0') //����Ŀ¼β��
                break;
            attr = p->Fattrib & '\07'; //�������ļ�����Ŀ¼,ֻ��������
            if (attrib == 0)           //������û��ָ������
            {
                if (attr & '\02') //����ʾ�����ء������ļ�
                    continue;
            }
            else
            {
                cc = attr & attrib;
                if (attrib != cc) //ֻ��ʾָ�����Ե��ļ�
                    continue;
            }
            strcat(buf, p->FileName);
            // cout << setiosflags(ios::left) << setw(20) << p->FileName;
            if (p->Fattrib >= '\20') //����Ŀ¼
            {
                strcat(buf, "      <DIR>\n");
                // cout << "<DIR>\n";
                dircount++;
            }
            else
            {
                strcat(buf, "   ");
                sprintf(tempBuf, "%d", p->Fsize);
                // itoa(p->Fsize, tempBuf, 10);
                strcat(buf, tempBuf);
                strcat(buf, "\n");

                // cout << resetiosflags(ios::left);
                // cout << setiosflags(ios::right) << setw(10) << p->Fsize << endl;
                filecount++;
                fsizecount += p->Fsize;
            }
        }
        if (ch == '\0')
            break;
        s = FAT[s]; //ָ���Ŀ¼����һ���̿�
    }
    // cout << resetiosflags(ios::left) << endl;
    // itoa(filecount, tempBuf, 10);
    sprintf(tempBuf, "%d", filecount);
    strcat(buf, tempBuf);
    strcat(buf, " file(s) ");
    // cout << setiosflags(ios::right) << setw(6) << filecount << " file(s)";
    // itoa(fsizecount, tempBuf, 10);
    sprintf(tempBuf, "%d", fsizecount);
    strcat(buf, tempBuf);
    strcat(buf, " bytes ");
    // cout << setw(8) << fsizecount << " bytes" << endl;
    // itoa(dircount, tempBuf, 10);
    sprintf(tempBuf, "%d", dircount);
    strcat(buf, tempBuf);
    strcat(buf, " dir(s)    ");
    // itoa(SIZE * FAT[0], tempBuf, 10);
    sprintf(tempBuf, "%d", SIZE * FAT[0]);
    strcat(buf, tempBuf);
    strcat(buf, " free");
    strcat(buf, "\000\0\0");
    // cout << setw(6) << dircount << " dir(s) " << setw(8) << SIZE * FAT[0];
    // cout << " free" << endl;
    if (status == 0)
        cout << buf << endl;
    else
    {
        newFile = FindBlankFCB(1, fcbp); //Ѱ���׿��Ϊs��Ŀ¼�еĿ�Ŀ¼��
        if (newFile < 0)
        {
            cout << "���ִ���.";
            return newFile;
        }
        strcpy(fcbp->FileName, "tempFile"); //Ŀ¼���б����ļ���
        fcbp->Fattrib = (char)0;            //�����ļ�����
        fcbp->Addr = 0;                     //���ļ��׿����Ϊ0
        fcbp->Fsize = 0;                    //���ļ�����Ϊ0
        buffer_to_file(fcbp, buf);
        if (status == 1)
        {
            strcpy(comd[1], "/tempFile");
            strcpy(comd[2], comd[pos + 1]);
            // CopyComd(2);
        }
        else
        {
            strcpy(comd[7], comd[pos + 1]);
            strcpy(comd[1], comd[7]);
            strcat(comd[1], "+/tempFile");
            strcpy(comd[2], comd[7]);
        }
        CopyComd(2);
        fleshBlock(fcbp);
    }

    return 1;
}

/////////////////////////////////////////////////////////////////

int CdComd(int k)
{
    // ��ǰĿ¼������Ŀ¼��ת�Ƶ�ָ��Ŀ¼�¡�ָ��Ŀ¼������ʱ������������Ϣ��
    // ����������Ŀ¼��������ʾ��ǰĿ¼·����

    short i, s;
    char attrib = (char)16;
    FCB *fcbp;
    if (k > 1) //�����ж���1������������
    {
        cout << "\n������󣺲���̫�ࡣ\n";
        return -1;
    }
    else if (k < 1) //�����޲�������ʾ��ǰĿ¼
    {
        cout << "\nThe Current Directory is C:" << curpath.cpath << endl;
        return 1;
    }
    else //������һ����������ָ��Ŀ¼��Ϊ��ǰĿ¼
    {
        i = strlen(comd[1]);
        if (i > 1 && comd[1][i - 1] == '/') //·����"/"��β������
        {
            cout << "\n·��������\n";
            return -1;
        }
        s = FindPath(comd[1], attrib, 1, fcbp); //��ָ��Ŀ¼(���׿��)
        if (s < 1)
        {
            cout << "\n·��������" << endl;
            return -1;
        }
        curpath.fblock = s;
        strcpy(curpath.cpath, temppath);
        if (!dspath)
            cout << "\n��ǰĿ¼��Ϊ C:" << curpath.cpath << endl;
        return 1;
    }
}

/////////////////////////////////////////////////////////////////

int M_NewDir(char *Name, FCB *fcb, short fs, char attrib) //��fcbλ�ô���һ����Ŀ¼
{
    //�ɹ���������Ŀ¼���׿��

    short i, b;
    FCB *q;

    b = getblock(); //��Ŀ¼�����һ���̿����ڴ洢Ŀ¼�..��
    if (b < 0)
        return b;
    strcpy(fcb->FileName, Name); //Ŀ¼��
    fcb->Fattrib = attrib;       //Ŀ¼������ΪĿ¼�����ļ�
    fcb->Addr = b;               //����Ŀ¼���׿��
    fcb->Fsize = 0;              //��Ŀ¼�ĳ���Լ��Ϊ0

    q = (FCB *)Disk[b];
    strcpy(q->FileName, ".."); //��Ŀ¼�еĵ�һ��Ŀ¼�����ǡ�..��
    q->Fattrib = (char)16;     //Ŀ¼������ΪĿ¼�����ļ�
    q->Addr = fs;              //��Ŀ¼���׿���Ǹ�Ŀ¼���׿��
    q->Fsize = 0;              //��Ŀ¼�ĳ���Լ��Ϊ0
    q++;
    for (i = 1; i < SIZE / sizeof(FCB); i++, q++)
        q->FileName[0] = '\0'; //�ÿ�Ŀ¼���־*/

    //����ΪԴ���룬����Ϊ�Ż�
    // q = (FCB *)Disk[b];
    // for (i = 0; i < kk; i++, q++)
    // 	q->FileName[0] = '\0'; //�ÿ�Ŀ¼���־*/
    // q = (FCB *)Disk[b];
    // strcpy(q->FileName, ".."); //��Ŀ¼�еĵ�һ��Ŀ¼�����ǡ�..��
    // q->Fattrib = (char)16;		 //Ŀ¼������ΪĿ¼�����ļ�
    // q->Addr = fs;							 //��Ŀ¼���׿���Ǹ�Ŀ¼���׿��
    // q->Fsize = 0;							 //��Ŀ¼�ĳ���Լ��Ϊ0
    return b; //�ɹ�����������
}

/////////////////////////////////////////////////////////////////

int ProcessPath(char *path, char *&Name, int k, int n, char attrib)
{
    // ��path�����һ�����ַ���������������ò���Name���أ�
    // ����path�г���Name�����һ��Ŀ¼�ĵ�ַ(�׿��)��
    // ��Ҫʱ���ú���FindPath()����ͨ��ȫ�ֱ���temppath��
    // ��path(ȥ��Name��)��ȫ·����(����·����)

    short i, len, s;
    FCB *fcbp;

    if (n && k != n) //n=0,��������k����,n>0,����k=n
    {
        cout << "\n���������������\n";
        return -1;
    }
    len = strlen(path);
    for (i = len - 1; i >= 0; i--)
        if (path[i] == '/')
            break;
    Name = &path[i + 1]; //ȡ·�������һ������
    if (i == -1)         //˵��path����һ���ļ���
    {
        s = curpath.fblock;
        strcpy(temppath, curpath.cpath);
    }
    else
    {
        if (i == 0)
        {
            s = 1;
            strcpy(temppath, "/");
        }
        else
        {
            path[i] = '\0';
            s = FindPath(path, attrib, 1, fcbp);
            if (s < 1)
            {
                cout << "\n·��������\n";
                return -3;
            }
        }
    }
    return s;
}

/////////////////////////////////////////////////////////////////

int MdComd(int k) //md�������
{
    // ������ʽ��md <Ŀ¼��>
    // ���ܣ���ָ��·���´���ָ��Ŀ¼����û��ָ��·�������ڵ�ǰĿ¼�´���ָ��Ŀ¼��
    // ��������Ŀ¼����������Ϣ��Ŀ¼���ļ�Ҳ����������

    // ѧ�����Կ��������мӡ����ԡ����������ڴ���ָ�����Ե���Ŀ¼��������ʽ���£�
    //		md <Ŀ¼��>[ <����>]
    // ���԰���R��H��S�Լ����ǵ����(�����ִ�Сд��˳��Ҳ����)�����磺
    //		md user rh
    // �书�����ڵ�ǰĿ¼�д������С�ֻ�����͡����ء����Ե���Ŀ¼user��

    short i, s, isExistBlankFCB, kk;
    char attrib = (char)16, *DirName;
    FCB *p;

    kk = SIZE / sizeof(FCB);

    if (k < 1)
    {
        cout << "\n����������û��Ŀ¼����\n";
        return -1;
    }

    if (k > 2)
    {
        cout << "\n�����������̫�ࡣ\n";
        return -1;
    }
    s = ProcessPath(comd[1], DirName, k, 0, attrib);
    if (s < 0)
        return s; //ʧ�ܣ�����
    if (!IsName(DirName))
    { //�����ֲ����Ϲ���
        cout << "\n�����е���Ŀ¼������\n";
        return -1;
    }
    i = FindFCB(DirName, s, attrib, p);

    if (i > 0)
    {
        cout << "\n����Ŀ¼������\n";
        return -1;
    }
    if (i == -3)
    {
        cout << "\n���󣺴���ͬ���ļ���\n";
        return -1;
    }
    if (k == 2) //������ʽ��md <Ŀ¼��> |<���Է�>
    {
        i = GetAttrib(comd[2], attrib);
        cout << (int)attrib;
        if (i < 0)
            return i;
    }
    isExistBlankFCB = FindBlankFCB(s, p); //�ҿհ�Ŀ¼��
    if (isExistBlankFCB < 0)              //������
        return isExistBlankFCB;
    isExistBlankFCB = M_NewDir(DirName, p, s, attrib); //��p��ָλ�ô���һ����Ŀ¼��
    if (isExistBlankFCB < 0)                           //����ʧ��
    {
        cout << "\n���̿ռ�����������Ŀ¼ʧ�ܡ�\n";
        return -1;
    }
    return 1; //��Ŀ¼�����ɹ�������
}

/////////////////////////////////////////////////////////////////

int RdComd(int k)
{
    // ��ָ��Ŀ¼Ϊ�գ���ɾ��֮�����򣬸���"�ǿ�Ŀ¼����ɾ��"����ʾ��
    // ����ɾ����ǰĿ¼��

    short i, j, count = 0, fs, s0, s;
    char attrib = (char)16, *DirName;
    FCB *p, *fcbp;
    fs = ProcessPath(comd[1], DirName, k, 1, attrib); //����DirName�ĸ�Ŀ¼���׿��
    if (fs < 0)
        return fs;                               //ʧ�ܣ�����
    s0 = s = FindFCB(DirName, fs, attrib, fcbp); //ȡDirName���׿��
    if (s < 1)
    {
        cout << "\nҪɾ����Ŀ¼�����ڡ�\n";
        return -1;
    }
    if (s == curpath.fblock)
    {
        cout << "\n����ɾ����ǰĿ¼��\n";
        return 0;
    }
    while (s > 0) //ѭ�����ң�ֱ��Ŀ¼β��
    {
        p = (FCB *)Disk[s];
        for (i = 0; i < SIZE / sizeof(FCB); i++, p++)
        {
            if (p->FileName[0] != (char)0xe5 && p->FileName[0] != '\0') //�ۼƷǿ�Ŀ¼��
                count++;
        }
        //s0=s;			//������һ���̿��
        s = FAT[s]; //ȡ��һ���̿��
    }
    if (count > 1)
    {
        cout << "\nĿ¼" << DirName << "�ǿգ�����ɾ����\n";
        return -1;
    }
    //s0=fcbp->Addr;		//ȡDirName���׿��
    while (s0 > 0) //�黹Ŀ¼DirName��ռ�Ĵ��̿ռ�
    {
        s = FAT[s0]; //���µ�s0��ĺ������
        FAT[s0] = 0; //���յ�s0��
        FAT[0]++;    //�����̿�����1
        s0 = s;      //������Ÿ���s0
    }
    fcbp->FileName[0] = (char)0xe5; //ɾ��DirName��Ŀ¼��
    if (strcmp(temppath, "/") == 0) //��ɾ������Ŀ¼�ڸ�Ŀ¼
        return 1;
    //��ɾ������Ŀ¼DirName���ڸ�Ŀ¼ʱ�����丸Ŀ¼�����´���
    s0 = s = fs;  //ȡDirName��Ŀ¼���׿��
    while (s > 0) //����DirName�ĸ�Ŀ¼�ռ�(������Ŀ¼����̿�)
    {
        p = (FCB *)Disk[s];
        for (j = i = 0; i < SIZE / sizeof(FCB); i++, p++)
            if (p->FileName[0] != (char)0xe5 && p->FileName[0] != '\0') //�ۼƷǿ�Ŀ¼��
                j++;
        if (j == 0)
        {
            FAT[s0] = FAT[s]; //����ָ��
            FAT[s] = 0;       //����s���̿�
            FAT[0]++;         //�����̿�����1
            s = FAT[s0];
        }
        else
        {
            s0 = s;     //������һ���̿��
            s = FAT[s]; //sָ����һ���̿�
        }
    }
    return 1;
}

/////////////////////////////////////////////////////////////////

int TypeComd(int k) //type�������(��ʾ�ļ�����)
{
    // ��ʾ�ļ����ݣ�type <�ļ���>����ʾָ���ļ������ݡ�
    // ��ָ���ļ������ڣ������������Ϣ��
    // ������ʽ1��type <�ļ���1>  >  <�ļ���2> ok
    // ����ܣ���ԭ��Ӧ����ʾ�ġ��ļ���1��ָ�����ļ����ݣ����浽���ļ���2��ָ�����ļ��С��ļ���2ָ���ļ���ԭ�����ݱ�ɾ�������൱�ڸ����ļ���
    // ������ʽ2��type <�ļ���1>  >>  <�ļ���2>
    // ����ܣ���ԭ��Ӧ����ʾ�ġ��ļ���1��ָ�����ļ����ݣ����浽���ļ���2��ָ�����ļ��С��ļ���2ָ���ļ���ԭ�����ݲ�ɾ���������ݽӵ�ԭ������β�������൱�ںϲ������ļ���
    // type��������������ض����ܺ󣬹���3��������ʽ(������ȱʡ�ļ���1�����ʱ)��

    short i, s, size, jj = 0;
    char attrib = '\0', *FileName;
    char *Buffer;
    char gFileName[PATH_LEN]; //����ļ�ȫ·����
    FCB *fcbpp;
    if (k < 0 || k > 3)
    {
        cout << "\n�����������\n";
        return -1;
    }
    if (k == 0)
    {
        k = 1;
        strcpy(comd[1], newestOperateFile);
    }
    else if (k == 2)
    {
        k = 3;
        strcpy(comd[3], comd[2]);
        strcpy(comd[2], comd[1]);
        strcpy(comd[1], newestOperateFile);
    }
    if (k == 1)
    {
        s = ProcessPath(comd[1], FileName, k, 0, '\020'); //ȡFileName����Ŀ¼���׿��
        if (s < 1)                                        //·������
            return s;                                     //ʧ�ܣ�����
        s = FindFCB(FileName, s, attrib, fcbpp);          //ȡFileName���׿��(���������)
        strcpy(gFileName, temppath);
        i = strlen(temppath);
        if (temppath[i - 1] != '/')
            strcat(gFileName, "/");
        strcat(gFileName, FileName); //�����ļ���ȫ·����
        if (s < 0)
        {
            cout << "\n�ļ�" << gFileName << "�����ڡ�\n";
            return -3;
        }
        if (s == 0)
            cout << "\n�ļ�" << gFileName << "�ǿ��ļ�\n";
        else
        {
            size = fcbpp->Fsize;
            Buffer = new char[size + 1]; //���䶯̬�ڴ�ռ�
            while (s > 0)
            {
                for (i = 0; i < SIZE; i++, jj++)
                {
                    if (jj == size)
                        break;
                    Buffer[jj] = Disk[s][i];
                }
                if (i < SIZE)
                    break;
                s = FAT[s];
            }
            Buffer[jj] = '\0';
            cout << Buffer << endl;
            delete[] Buffer; //�ͷŷ���Ķ�̬�ڴ�ռ�
        }
        return 1;
    }
    else if (k == 3)
    {
        if (strcmp(comd[2], ">") == 0)
        {
            strcpy(comd[2], comd[3]);
            return CopyComd(2);
        }
        else if (strcmp(comd[2], ">>") == 0)
        {
            strcpy(comd[2], comd[3]);
            strcat(comd[3], "+");
            strcpy(comd[7], comd[1]);
            strcpy(comd[1], comd[3]);
            strcat(comd[1], comd[7]);
            return CopyComd(2);
        }
        else
        {
            cout << "��������" << endl;
            return -1;
        }
    }
    else
    {
        cout << "��������" << endl;
        return -1;
    }
}

/////////////////////////////////////////////////////////////////

int blockf(int k) //block�������(��ʾ�ļ���Ŀ¼ռ�õ��̿��)
{
    short s;
    char attrib = '\040'; //32��ʾ����(�ļ�����Ŀ¼)Ŀ¼�����
    FCB *fcbp;

    if (k > 1)
    {
        cout << "\n�����в�����������\n";
        return -1;
    }
    if(k==0){
        strcpy(comd[1], newestOperateFile);
    }
    s = FindPath(comd[1], attrib, 1, fcbp); //��ָ��Ŀ¼(���׿��)
    if (s < 1)
    {
        cout << "\n·��������" << endl;
        return -2;
    }
    cout << "\n"
         << temppath << "ռ�õ��̿��Ϊ��";
    while (s > 0)
    {
        cout << s << "  ";
        s = FAT[s];
    }
    cout << endl;
    return 1;
}

/////////////////////////////////////////////////////////////////

void Put_UOF(char *gFileName, int i, short status, FCB *fcbp)
{
    strcpy(uof[i].fname, gFileName); //�����ļ�ȫ·����
    uof[i].attr = fcbp->Fattrib;     //�����ļ�����
    uof[i].faddr = fcbp->Addr;       //�ļ����׿��(0������ļ�)
    uof[i].fsize = fcbp->Fsize;
    uof[i].fp = fcbp;
    uof[i].state = status; //��״̬
    if (fcbp->Fsize > 0)   //���ļ��ǿ�
        uof[i].readp = 1;  //��ָ��ָ���ļ���ͷ
    else
        uof[i].readp = 0;            //��ָ��ָ���λ��
    uof[i].writep = fcbp->Fsize + 1; //дָ��ָ���ļ�ĩβ
}

/////////////////////////////////////////////////////////////////

int FindBlankFCB(short s, FCB *&fcbp1) //Ѱ���׿��Ϊs��Ŀ¼�еĿ�Ŀ¼��
{
    short i, s0, oris = s;
    while (s > 0) //���׿��Ϊs��Ŀ¼�ҿյǼ�����ֱ��Ŀ¼β��
    {
        fcbp1 = (FCB *)Disk[s];
        for (i = 0; i < SIZE / sizeof(FCB); i++, fcbp1++)
            if (fcbp1->FileName[0] == (char)0xe5 || fcbp1->FileName[0] == '\0')
            {
                fcbp1->Addr = fcbp1->Fsize = 0; //����Ϊ��Ŀ¼��
                return 1;                       //�ҵ���Ŀ¼��ɹ�����
            }
        s0 = s;     //������һ���̿��
        s = FAT[s]; //ȡ��һ���̿��
    }
    if (strcmp(temppath, "/") == 0 && s == 1) //���Ǹ�Ŀ¼
    {
        cout << "\n��Ŀ¼�����������ٴ���Ŀ¼�\n";
        return -1;
    }
    s = getblock(); //ȡһ�����̿�
    if (s < 0)      //�޿����̿�
    {
        cout << "\n���̿ռ�����������Ŀ¼ʧ�ܡ�\n";
        return -1;
    }
    FAT[s0] = s; //����FAT��
    fcbp1 = (FCB *)Disk[s];
    for (i = 0; i < SIZE / sizeof(FCB); i++, fcbp1++)
        fcbp1->FileName[0] = '\0'; //�ÿ�Ŀ¼��־
    fcbp1 = (FCB *)Disk[s];
    fcbp1->Addr = fcbp1->Fsize = 0; //����Ϊ��Ŀ¼��
    return 1;
}

/////////////////////////////////////////////////////////////////

int CreateComd(int k) //create����������������ļ�
{
    // �����ļ���create <�ļ���> [<�ļ�����>]������һ��ָ�����ֵ����ļ���
    // ����Ŀ¼������һĿ¼��������ļ������ݡ����������ļ�����������Ϣ��

    short i, i_uof, s0, s;
    char attrib = (char)0, *FileName;
    char gFileName[PATH_LEN]; //����ļ�ȫ·����
    char ch, *p;
    FCB *fcbp1;
    if (k > 2 || k < 1)
    {
        cout << "\n�����в����������ԡ�\n";
        return -1;
    }
    s = ProcessPath(comd[1], FileName, k, 0, '\020'); //ȡFileName����Ŀ¼���׿��
    if (s < 1)                                        //·������
        return s;                                     //ʧ�ܣ�����
    if (!IsName(FileName))                            //�����ֲ����Ϲ���
    {
        cout << "\n�����е����ļ�������\n";
        return -2;
    }
    s0 = FindFCB(FileName, s, attrib, fcbp1); //ȡFileName���׿��(���������)
    if (s0 >= 0)
    {
        cout << "\n��ͬ���ļ������ܽ�����\n";
        return -2;
    }
    if (s0 == -3)
    {
        cout << "\n��ͬ��Ŀ¼�����ܽ�����\n";
        return -2;
    }
    strcpy(gFileName, temppath);
    i = strlen(temppath);
    if (temppath[i - 1] != '/')
        strcat(gFileName, "/");
    strcat(gFileName, FileName); //�����ļ���ȫ·����
    strcpy(newestOperateFile, gFileName);
    if (k == 2)
    {
        p = comd[2];
        while (*p != '\0') //�����ļ�����
        {
            ch = *p;
            ch = tolower(ch);
            switch (ch)
            {
            case 'r':
                attrib = attrib | (char)1;
                break;
            case 'h':
                attrib = attrib | (char)2;
                break;
            case 's':
                attrib = attrib | (char)4;
                break;
            default:
                cout << "\n������ļ����Դ���\n";
                return -3;
            }
            p++;
        }
    }
    for (i_uof = 0; i_uof < S; i_uof++) //��UOF���ҿձ���
        if (uof[i_uof].state == 0)
            break;
    if (i_uof == S)
    {
        cout << "\nUOF���������ܴ����ļ���\n";
        return -4;
    }
    i = FindBlankFCB(s, fcbp1); //Ѱ���׿��Ϊs��Ŀ¼�еĿ�Ŀ¼��
    if (i < 0)
    {
        cout << "\n�����ļ�ʧ�ܡ�\n";
        return i;
    }
    strcpy(fcbp1->FileName, FileName);   //Ŀ¼���б����ļ���
    fcbp1->Fattrib = attrib;             //�����ļ�����
    fcbp1->Addr = 0;                     //���ļ��׿����Ϊ0
    fcbp1->Fsize = 0;                    //���ļ�����Ϊ0
    Put_UOF(gFileName, i_uof, 1, fcbp1); //����UOF�Ǽ���
    cout << "\n�ļ�" << gFileName << "�����ɹ�\n";
    return 1; //�ļ������ɹ�������
}

/////////////////////////////////////////////////////////////////

int Check_UOF(char *Name) //���UOF������������ָ�����ļ�
{
    int i;
    for (i = 0; i < S; i++) //���û����ļ���UOF
    {
        if (uof[i].state == 0) //�ձ���
            continue;
        if (strcmp(Name, uof[i].fname) == 0) //�ҵ�
            break;
    }
    return i;
}

/////////////////////////////////////////////////////////////////

int OpenComd(int k) //open������������ļ�
{
    // ������ʽ��open <�ļ���>
    // ��ָ���ļ���������δ�򿪣����֮�������û����ļ���UOF���е�
    // �Ǹ��ļ����й���Ϣ����ָ���ļ��Ѿ��򿪣�����ʾ"�ļ��Ѵ�"����Ϣ��
    // ��ָ���ļ������ڣ������������Ϣ��ֻ���ļ��򿪺�ֻ�ܶ�����д��

    short i, s0, s;
    char attrib = '\0', *FileName;
    char gFileName[PATH_LEN]; //����ļ�ȫ·����
    FCB *fcbp;

    s0 = ProcessPath(comd[1], FileName, k, 1, '\20'); //ȡFileName����Ŀ¼���׿��
    if (s0 < 1)                                       //·������
        return s0;                                    //ʧ�ܣ�����
    s = FindFCB(FileName, s0, attrib, fcbp);          //ȡFileName���׿��(���������)
    if (s < 0)
    {
        cout << "\nҪ�򿪵��ļ������ڡ�\n";
        return -2;
    }
    strcpy(gFileName, temppath);
    i = strlen(temppath);
    if (temppath[i - 1] != '/')
        strcat(gFileName, "/");
    strcat(gFileName, FileName); //�����ļ���ȫ·����
    strcpy(newestOperateFile, gFileName);
    i = Check_UOF(gFileName); //��UOF
    if (i < S)                //���ļ�����UOF��
    {
        cout << "\n�ļ�" << gFileName << "ԭ���Ѿ���!\n";
        return -3;
    }
    for (i = 0; i < S; i++) //��UOF���ҿձ���
        if (uof[i].state == 0)
            break;
    if (i == S)
    {
        cout << "\nUOF���������ܴ��ļ���\n";
        return -4;
    }
    Put_UOF(gFileName, i, 2, fcbp);
    cout << "\n�ļ�" << gFileName << "�򿪳ɹ���\n";
    return 1;
}

/////////////////////////////////////////////////////////////////

int getblock() //���һ�������̿飬��fappend()��������
{
    short b;
    if (FAT[0] == 0) //FAT[0]���Ǵ��̿��п���
        return -1;   //��������(���޿����̿�)
    for (b = ffbp; b < K; b++)
        if (!FAT[b])
            break;
    if (b == K)
    {
        for (b = 1; b < ffbp; b++)
            if (!FAT[b])
                break;
    }
    ffbp = b + 1;
    if (ffbp == K)
        ffbp = 1;
    FAT[0]--;    //�̿�����1
    FAT[b] = -1; //���̿��ѷ����־(�˴�����������Ϊ�ļ�β)
    return b;    //����ȡ�õĿ����̿��
}

////////////////////////////////////////////////////////////////

int WriteComd(int k) //write����Ĵ�����
{
    // д�ļ���write <�ļ���> [<λ��>[ insert]]
    // ��"λ��"����������дָ����ָλ��д���ļ����ݣ�
    // �ṩ"λ��"���������ڶ�Ӧλ��д�����ݡ�
    // λ��������n����ָ���ļ��ĵ�n���ֽڴ���ʼд��(λ�ô�1��ʼ���)��
    // "λ��" �������� "append"��ǰ3���ַ���Ч�������ִ�Сд������ʾ���ļ�β��д����Ϣ��
    // ���в���insert������д������ݲ��뵽��Ӧλ�ã���Ӧλ�ÿ�ʼ��ԭ���ݺ��ơ�
    // ���޲��� "insert" ��д������ݴ����ļ�ԭ�ȵ�����(��Ӧλ�õ�����)��
    // д����ϵ����ļ����Ⱥ�дָ��ֵ��
    // ���ļ�δ�򿪻��ļ������ڣ��ֱ����������Ϣ��
    // ���������ṩ������5��������ʽ��
    // write <�ļ���> ������дָ�뵱ǰ��ָλ��д��д�����ݴ���ԭ����(��д��ʽ)
    // write <�ļ���> |pn�������ļ���ͷ��n���ֽڴ�д����д��ʽ
    // write <�ļ���> |ins������дָ����ָλ��д��д�봦��ʼ��ԭ���ݺ���(���뷽ʽ)
    // write <�ļ���> |pn |ins�������ļ���ͷ��n���ֽڴ�д�����뷽ʽ
    // write <�ļ���> |app�������ļ�β��д(��ӷ�ʽ)
    // ������������5�ֲ����ļ�����������ʽ��
    // write������дָ�뵱ǰ��ָλ��д��д�����ݴ���ԭ����(���淽ʽ���д��ʽ)
    // write |pn�������ļ���ͷ��n���ֽڴ�д����д��ʽ
    // write |ins������дָ����ָλ��д��д�봦��ʼ��ԭ���ݺ���(���뷽ʽ)
    // write |pn |ins�������ļ���ͷ��n���ֽڴ�д�����뷽ʽ
    // write |app�������ļ�β��д(��ӷ�ʽ)

    //     ���˲��롢��д��ʽ�⣬��3�Ļ����ϣ������Կ������ӡ�ɾ������ʽ���������ֿ���������������ʽ��
    // write <�ļ���> |del������ָ���ļ�����дָ��λ��ɾ�����ļ�ĩβ
    // write |del�����ԡ���ǰ�����ļ�������дָ��λ��ɾ�����ļ�ĩβ
    // write <�ļ���> |lm |del������ָ���ļ�����дָ��λ�ÿ�ʼ��ɾ��m���ֽ�
    // write |lm |del�����ԡ���ǰ�����ļ�������дָ��λ�ÿ�ʼ��ɾ��m���ֽ�
    // write <�ļ���> |pn |del������ָ���ļ�����ָ��λ��n����ʼɾ�����ļ�ĩβ
    // write |pn |del�����ԡ���ǰ�����ļ�������ָ��λ��n����ʼɾ�����ļ�ĩβ
    // write <�ļ���> |pn |lm |del������ָ���ļ�����ָ��λ��n����ʼɾ��m���ֽ� 1
    // write |pn |lm |del�����ԡ���ǰ�����ļ�������ָ��λ��n����ʼɾ��m���ֽ� 1
    // ��ע��û�����4.16�Ĺ�����Ҳ������write���������ӡ�ɾ�������ܣ���������8���й�writeɾ�����ܵ����ֻ��4�����ļ����������������ʹ�á�

#define BSIZE 40 * SIZE + 1
    short int ii, ii_uof, len0, len, len1, pos = 0, ins = 0;
    short int bn0, bn1, jj, count = 0, isDelete = 0, isAppend = 0;
    int readc = 0;
    char attrib = '\0', Buffer[BSIZE]; //Ϊ����ƣ�����һ�����д��200m�ֽ�
    char *FileName;
    FCB *fcbp;

    if (k < 0 || k > 4)
    {
        cout << "\n�����������\n";
        return -1;
    }
    if (comd[1][0] == '|' || k == 0)
    {
        for (int i = k; i > 0; i--)
        {
            strcpy(comd[i + 1], comd[i]);
        }
        k++;
        strcpy(comd[1], newestOperateFile);
    }
    for (int t = 1; t <= k; t++)
    {
        if (strncasecmp(comd[t], "|delete", strlen(comd[t])) == 0)
        {
            isDelete = 1;
        }
    }

    FindPath(comd[1], attrib, 0, fcbp); //����ȫ·����ȥ����..������temppath��
    strcpy(newestOperateFile, temppath);
    ii_uof = Check_UOF(temppath); //��UOF
    if (ii_uof == S)
    {
        cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ�����д�ļ���\n";
        return -2;
    }
    if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1)
    { //ֻ���ļ����Ǵ���״̬����д
        cout << "\n"
             << temppath << "��ֻ���ļ�������д��\n";
        return -3;
    }

    if (k == 1)
        pos = uof[ii_uof].writep; //��дָ����ָλ�ÿ�ʼд(write <�ļ���>)
    else                          //k=2��3
    {
        // k=2�׶�
        if (comd[2][0] != '|')
        {
            cout << "��������" << endl;
            return -1;
        }
        if (strncasecmp(&comd[2][1], "append", strlen(comd[2]) - 1) == 0)
        {
            isAppend = 1;
            if (isDelete == 1)
            {
                cout << "��������" << endl;
                return -1;
            }
            pos = uof[ii_uof].fsize + 1; //�ļ�β�����ģʽ(write <�ļ���> append)
        }
        else if (strncasecmp(&comd[2][1], "insert", strlen(comd[2]) - 1) == 0)
        {
            if (isDelete == 1 || isAppend == 1)
            {
                cout << "��������" << endl;
                return -1;
            }
            pos = uof[ii_uof].writep; //�ӵ�ǰдָ��λ�ÿ�ʼд
            ins = 1;                  //����ģʽ(write <�ļ���> insert)
        }
        else if (comd[2][1] == 'p')
        {
            if (isAppend == 1)
            {
                cout << "��������" << endl;
                return -1;
            }
            pos = atoi(&comd[2][2]); //��������ָ��λ��д(write <�ļ���> <n>)
            if (pos <= 0 || pos > uof[ii_uof].fsize)
            {
                cout << "�������ṩ�Ķ�λ�ô���\n";
                return -3;
            }
        }
        else if (comd[2][1] == 'l')
        {
            if (isDelete == 0 || isAppend == 1)
            {
                cout << "\n��������\n";
                return -4;
            }
            readc = atoi(&comd[2][2]); //��������ָ��λ��д(write <�ļ���> <n>)
            if (readc < 1)
            {
                cout << "\n�������ṩ�Ķ��ֽ�������\n";
                return -4;
            }
            if (readc > uof[ii_uof].fsize - pos + 1)
                readc = uof[ii_uof].fsize - pos + 1;
        }
        else
        {
            if (strncasecmp(comd[2], "|delete", strlen(comd[2])) == 0)
            {
                isDelete = 1;
            }
            else
            {
                cout << "��������" << endl;
                return -1;
            }
        }

        if (k >= 3)
        {
            if (comd[3][0] != '|')
            {
                cout << "��������" << endl;
                return -1;
            }
            if (strncasecmp(&comd[3][1], "insert", strlen(comd[3]) - 1) == 0 && comd[3][0] == '|')
                ins = 1; //����ģʽ(write <�ļ���> <n> insert)
            else if (comd[3][1] == 'p')
            {
                if (isAppend == 1)
                {
                    cout << "��������" << endl;
                    return -1;
                }
                pos = atoi(&comd[3][2]); //��������ָ��λ��д(write <�ļ���> <n>)
                if (pos <= 0 || pos > uof[ii_uof].fsize)
                {
                    cout << "�������ṩ�Ķ�λ�ô���\n";
                    return -3;
                }
            }
            else if (comd[3][1] == 'l')
            {
                if (isDelete == 0 || isAppend == 1)
                {
                    cout << "\n��������\n";
                    return -4;
                }
                readc = atoi(&comd[3][2]); //��������ָ��λ��д(write <�ļ���> <n>)
                if (readc < 1)
                {
                    cout << "\n�������ṩ�Ķ��ֽ�������\n";
                    return -4;
                }
                if (readc > uof[ii_uof].fsize - pos + 1)
                    readc = uof[ii_uof].fsize - pos + 1;
            }
            else
            {
                if (strncasecmp(comd[3], "|delete", strlen(comd[3])) == 0)
                {
                    isDelete = 1;
                }
                else
                {
                    cout << "��������" << endl;
                    return -1;
                }
            }
        }
    }
    if (pos < 0)
    {
        cout << "\n�������ṩ��д��λ�ô���\n";
        return -1;
    }
    if (pos == 0)
    {
        pos = uof[ii_uof].readp;
    }
    if (pos >= uof[ii_uof].fsize + 1)
    {
        pos = uof[ii_uof].fsize + 1;
        ins = 0; //������������ǲ��뷽ʽ
    }

    if (readc > uof[ii_uof].fsize - pos + 1 || readc == 0)
        readc = uof[ii_uof].fsize - pos + 1;
    pos--; //ʹpos��0��ʼ
    if (isDelete)
    {
        fcbp = uof[ii_uof].fp;
        len0 = uof[ii_uof].fsize; //ȡ�ļ�ԭ���ĳ���ֵ
        if (len0 == 0)            //���ǿ��ļ�
        {
            cout << "���ļ��޷�ɾ�ַ���" << endl;
            return -1;
        }
        // buf = new char[len0+1];
        char buf[len0 + 1];
        memset(buf, 0, sizeof(buf));
        if (buf == 0)
        {
            cout << "\n�����ڴ�ʧ�ܡ�\n";
            return -1;
        }
        file_to_buffer(fcbp, buf); //�ļ�����buf
        int cnt;
        for (cnt = pos + readc; cnt < len0; cnt++)
        {
            buf[cnt - readc] = buf[cnt];
            buf[cnt] = '\0';
        }
        buf[cnt - readc] = '\0';
        buffer_to_file(fcbp, buf);
        // delete[] buf;
        uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
        uof[ii_uof].writep = uof[ii_uof].fsize + 1;
        uof[ii_uof].readp = uof[ii_uof].fsize + 1;
        cout << "\nɾ���ļ�" << uof[ii_uof].fname << "�еĲ����ַ��ɹ�.\n";
        return 1;
    }
    else
    {
        cout << "\n������д���ļ�������(�����������" << sizeof(Buffer) - 1 << "���ֽ�)��\n";
        cin.getline(Buffer, BSIZE);
        len1 = strlen(Buffer);
        if (len1 == 0) //���볤��Ϊ0,���ı��ļ�
            return 0;
        fcbp = uof[ii_uof].fp;
        len0 = uof[ii_uof].fsize; //ȡ�ļ�ԭ���ĳ���ֵ
        if (len0 == 0)            //���ǿ��ļ�
        {
            ii = buffer_to_file(fcbp, Buffer);
            if (ii == 0) //д�ļ�ʧ��
                return ii;
            uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
            uof[ii_uof].faddr = uof[ii_uof].fp->Addr;
            uof[ii_uof].readp = 1;
            uof[ii_uof].writep = uof[ii_uof].fsize + 1;
            return 1;
        }
        //���´����ļ��ǿյ����
        if (ins)
        {
            len = len1 + len0 + 1;
        }
        else
        {
            len = len1 > len0 ? len1 + pos : len0 + 1;
        }
        //����д����ɺ��ļ��ĳ���
        bn0 = len0 / SIZE + (short)(len0 % SIZE > 0);
        //�ļ�ԭ��ռ�õ��̿���
        bn1 = len / SIZE + (short)(len % SIZE > 0);
        //д����ļ���ռ�õ��̿���
        if (FAT[0] < bn1 - bn0)
        {
            cout << "\n���̿ռ䲻��,����д���ļ�.\n";
            return -1;
        }
        char *buf;
        buf = new char[len + 1];
        // char buf[len0 + 1];
        memset(buf, 0, sizeof(buf));
        if (buf == 0)
        {
            cout << "\n�����ڴ�ʧ�ܡ�\n";
            return -1;
        }
        file_to_buffer(fcbp, buf); //�ļ�����buf
        if (ins)                   //���ǲ��뷽ʽ
        {
            for (ii = len0; ii >= pos; ii--)
                buf[ii + len1] = buf[ii]; //����,�ճ������Buffer
            jj = pos;
            ii = 0;
            while (Buffer[ii] != '\0') //Buffer���뵽buf
                buf[jj++] = Buffer[ii++];
        }
        else //���Ǹ�д��ʽ
        {
            // strcpy(&buf[pos], Buffer);�ᵼ��delete����
            int m;
            for (m = 0; m < strlen(Buffer); m++)
            {
                buf[m + pos] = Buffer[m];
                // buf[m] = '\0';
            }
        }
        buffer_to_file(fcbp, buf);
        delete[] buf;
        uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
        uof[ii_uof].writep = uof[ii_uof].fsize + 1;
        uof[ii_uof].readp = uof[ii_uof].fsize + 1;
        cout << "\nд�ļ�" << uof[ii_uof].fname << "�ɹ�.\n";
        return 1;
    }
}

////////////////////////////////////////////////////////////////

int CloseComd(int k) //close����Ĵ��������ر��ļ�
{
    // close <�ļ���>����ָ���ļ��Ѵ򿪣���ر�֮������UOF��ɾ�����ļ�
    // ��Ӧ�ı�����ļ�δ�򿪻��ļ������ڣ��ֱ�����й���Ϣ��

    int i_uof;
    char attrib = '\0';
    FCB *p;
    if (k < 0 || k > 1)
    {
        cout << "\n�����������\n";
        return -1;
    }
    if (k == 0)
        strcpy(comd[1], newestOperateFile);
    FindPath(comd[1], attrib, 0, p); //����ȫ·����ȥ����..������temppath��
    i_uof = Check_UOF(temppath);     //��UOF
    if (i_uof == S)
        cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܹرա�\n";
    else
    {
        uof[i_uof].state = 0;        //��UOF��������ļ��Ǽ���
        p = uof[i_uof].fp;           //ȡ���ļ���Ŀ¼��λ��ָ��
        p->Addr = uof[i_uof].faddr;  //�����ļ����׿��
        p->Fsize = uof[i_uof].fsize; //�����ļ��Ĵ�С
        cout << "\n�ر��ļ�" << temppath << "�ɹ���\n";
    }
    return 1;
}

/////////////////////////////////////////////////////////////////

void CloseallComd(int disp) //closeall����رյ�ǰ�û��������ļ�
{
    int i_uof, j, k;
    FCB *p;
    for (k = i_uof = 0; i_uof < S; i_uof++)
    {
        j = uof[i_uof].state; //UOF��״̬>0Ϊ��Ч�Ǽ���
        if (j > 0)
        {
            k++;                         //�Ѵ��ļ�����
            uof[i_uof].state = 0;        //��UOF��������ļ��Ǽ���
            p = uof[i_uof].fp;           //ȡ���ļ���Ŀ¼��λ��ָ��
            p->Addr = uof[i_uof].faddr;  //�����ļ����׿��
            p->Fsize = uof[i_uof].fsize; //�����ļ��Ĵ�С
            cout << "\n�ļ�" << uof[i_uof].fname << "�ѹر�.\n";
        }
    }
    if (!disp)
        return;
    if (k == 0)
        cout << "\n��û�д��ļ��������ļ��ɹرա�\n\n";
    else
        cout << "\n���ر� " << k << " ���ļ�.\n\n";
}

/////////////////////////////////////////////////////////////////

short int SAVE_bn(short bb)
{
    // ��udtab�д洢��ɾ���ļ��Ŀ��

    short i = 0, b0, b, bs;
    if (bb == 0) //��ɾ���ļ��ǿ��ļ�
        return bb;
    bs = getblock();
    short *pb = (short *)Disk[bs];
    while (bb > 0)
    {
        pb[i] = bb;
        bb = FAT[bb];
        i++;
        if (i == SIZE / 2)
        {
            i = 0;
            b0 = b;
            b = getblock();
            FAT[b0] = b;
            pb = (short *)Disk[b];
        }
    }
    pb[i] = -1;
    return bs;
}

/////////////////////////////////////////////////////////////////

void Del1Ud(short a)
{
    // ��udtab����ɾ��һ���ǰ�ƺ�������

    short i, b, b0;
    b = udtab[a].fb;
    while (b > 0)
    { //���մ洢�ļ���ŵĴ��̿ռ�
        b0 = b;
        b = FAT[b];
        FAT[b0] = 0;
        FAT[0]++;
    }
    for (i = a; i < Udelp - 1; i++) //udtab���б���ǰ��һ��λ��
        udtab[i] = udtab[i + 1];
    Udelp--;
}

/////////////////////////////////////////////////////////////////

int PutUdtab(FCB *fp)
{
    //��udtab�м���һ����

    short bb, bn, n, m, size;
    size = fp->Fsize;
    bn = size / SIZE + (size % SIZE > 0) + 1; //�ļ����̿�Ÿ���(��-1)
    n = SIZE / sizeof(short);                 //ÿ���̿�ɴ洢���̿����
    m = bn / n + (short)(bn % n > 0);         //����m���̿�洢�ļ��Ŀ��
    if (Udelp == DM)
        Del1Ud(0);
    if (m > FAT[0])
    {
        cout << "\n���̿ռ䲻��,���ܱ���ɾ���ָ���Ϣ,���ļ�ɾ���󽫲��ָܻ�.\n";
        return -1;
    }
    strcpy(udtab[Udelp].gpath, temppath);
    strcpy(udtab[Udelp].ufname, fp->FileName);
    bb = udtab[Udelp].ufaddr = fp->Addr;
    udtab[Udelp].fb = SAVE_bn(bb);     //���汻ɾ���ļ����̿��
    udtab[Udelp].ufattr = fp->Fattrib; //���汻ɾ���ļ����̿��

    Udelp++; //����ָ��λ��
    return 1;
}

/////////////////////////////////////////////////////////////////

int DelComd(int k) //del(ɾ���ļ�)�������
{
    // ɾ���ļ���del <�ļ���>��ɾ��ָ�����ļ����������Ŀ¼��ͻ���
    // ����ռ�ô��̿ռ䡣����ֻ���ļ���ɾ��ǰӦѯ���û����õ�ͬ���
    // ����ɾ������ָ���ļ�����ʹ��ʱ����ʾ"�ļ�����ʹ�ã�����ɾ��"
    // ����Ϣ����ָ���ļ�������ʱ����������Ϣ��
    // ɾ���ļ�ʱ�������ļ����й���Ϣ��¼��ɾ���ļ��ָ���Ϣ��udtab�У�
    // �Ա������ָ�ʱʹ�á�

    short i, s0, s, isAll = 0;
    char yn, attr;
    char attrib = '\0', *FileName;
    char gFileName[PATH_LEN]; //����ļ�ȫ·����
    FCB *fcbp, *fcbp1;

    if (strcmp(comd[1], "*") == 0)
    {
        strcpy(comd[1], curpath.cpath);
        isAll = 1;
    }
    if (isAll == 1)
    {
        int block = curpath.fblock;
        while (block > 0)
        {
            fcbp1 = (FCB *)Disk[block];
            for (int i = 0; i < SIZE / sizeof(FCB); i++, fcbp1++)
            {
                if (fcbp1->FileName[0] == (char)0xe5 || strcmp(fcbp1->FileName, "..") == 0||fcbp1->Fattrib>=(char)16)
                {
                    continue;
                }
                // cout << "111:" << fcbp->FileName;
                // s = FindFCB(fcbp1->FileName, block, attrib, fcbp); //ȡFileName���׿��(���������)
                // if (s < 0)
                // {
                //     cout << "\nҪɾ�����ļ������ڡ�\n";
                //     return -2;
                // }
                fcbp = fcbp1;
                FileName = fcbp->FileName;
                strcpy(gFileName, curpath.cpath);
                i = strlen(temppath);
                if (temppath[i - 1] != '/')
                    strcat(gFileName, "/");
                strcat(gFileName, FileName); //�����ļ���ȫ·����
                i = Check_UOF(gFileName);    //��UOF
                if (i < S)                   //���ļ�����UOF��
                {
                    cout << "\n�ļ�" << gFileName << "����ʹ�ã�����ɾ��!\n";
                    return -3;
                }
                attr = fcbp->Fattrib & '\01';
                if (attr == '\01')
                {
                    cout << "\n�ļ�" << gFileName << "��ֻ���ļ�����ȷ��Ҫɾ������(y/n) ";
                    cin >> yn;
                    if (yn != 'Y' && yn != 'y')
                        return 0; //��ɾ��������
                }
                i = PutUdtab(fcbp); //��ɾ���ļ����й���Ϣ���浽udtab����
                if (i < 0)          //����̿ռ䲻�㣬���ܱ��汻ɾ���ļ�����Ϣ
                {
                    cout << "\n���Ƿ���Ҫɾ���ļ� " << gFileName << " ? (y/n) : ";
                    cin >> yn;
                    if (yn == 'N' || yn == 'n')
                        return 0; //��ɾ������
                }
                fleshBlock(fcbp);//ȷ��ɾ��ִ��
            }
            block = FAT[block];
        }
    }
    else
    {
        s0 = ProcessPath(comd[1], FileName, k, 1, '\20'); //ȡFileName����Ŀ¼���׿��
        if (s0 < 1)                                       //·������
            return s0;                                    //ʧ�ܣ�����
        s = FindFCB(FileName, s0, attrib, fcbp);          //ȡFileName���׿��(���������)
        if (s < 0)
        {
            cout << "\nҪɾ�����ļ������ڡ�\n";
            return -2;
        }
        strcpy(gFileName, temppath);
        i = strlen(temppath);
        if (temppath[i - 1] != '/')
            strcat(gFileName, "/");
        strcat(gFileName, FileName); //�����ļ���ȫ·����
        i = Check_UOF(gFileName);    //��UOF
        if (i < S)                   //���ļ�����UOF��
        {
            cout << "\n�ļ�" << gFileName << "����ʹ�ã�����ɾ��!\n";
            return -3;
        }
        attr = fcbp->Fattrib & '\01';
        if (attr == '\01')
        {
            cout << "\n�ļ�" << gFileName << "��ֻ���ļ�����ȷ��Ҫɾ������(y/n) ";
            cin >> yn;
            if (yn != 'Y' && yn != 'y')
                return 0; //��ɾ��������
        }
        i = PutUdtab(fcbp); //��ɾ���ļ����й���Ϣ���浽udtab����
        if (i < 0)          //����̿ռ䲻�㣬���ܱ��汻ɾ���ļ�����Ϣ
        {
            cout << "\n���Ƿ���Ҫɾ���ļ� " << gFileName << " ? (y/n) : ";
            cin >> yn;
            if (yn == 'N' || yn == 'n')
                return 0; //��ɾ������
        }
        fleshBlock(fcbp);
    }
    return 1;
}

/////////////////////////////////////////////////////////////////

int Udfile(FCB *fdp, short s0, char *fn, short &cc)
{
    // ��Ŀ¼���ҵ���ɾ���ļ�(�ļ������ַ�Ϊ'\0xe5')��Ŀ¼�����ô˺���
    // ��������udtab����������ң����ҵ��뱻ɾ���ļ���·����ͬ������(����
    // ������)��ͬ���׿����ͬ�ı���ʱ����ʾ�����ܿ��Իָ���������ѯ����
    // ���õ��϶��𸴺󣬼���ʼ�ָ��������ָ��������ַ���������ͻʱ������
    // ���������ļ���������ָ����������ļ�ԭ��ռ�õ��̿��������ã���ָ�
    // ʧ�ܡ����ۻָ��ɹ���񣬶���ɾ��udtab�ж�Ӧ�ı��

    int i, j;
    char yn[11], Fname[INPUT_LEN];
    short *stp, b, b0, b1, s;
    FCB *fcbp;

    for (i = 0; i < Udelp; i++)
    {
        if (strcmp(udtab[i].gpath, temppath) == 0 && strcmp(&udtab[i].ufname[1], fn) == 0 && udtab[i].ufaddr == fdp->Addr)
        {
            cout << "\n�ļ�" << udtab[i].ufname << "���ܿ��Իָ����Ƿ�ָ�����(y/n) ";
            cin.getline(yn, 10);
            if (yn[0] == 'y' || yn[0] == 'Y')
            {
                if (udtab[i].ufaddr > 0)
                {
                    b = udtab[i].fb;        //ȡ�洢��ɾ�ļ��̿�ŵĵ�һ�����
                    stp = (short *)Disk[b]; //stpָ����̿�
                    b0 = stp[0];            //ȡ��ɾ���ļ��ĵ�һ����ŵ�b0
                    j = 1;
                    while (b0 > 0)
                    {
                        if (FAT[b0] != 0) //����ɾ���ļ����̿��Ѿ�������
                        {
                            cout << "\n�ļ�" << udtab[i].ufname << "�Ѳ��ָܻ���\n";
                            Del1Ud(i); //ɾ��udtab���е�i��(�ñ���������)
                            return -1;
                        }
                        b0 = stp[j++]; //ȡ��ɾ���ļ�����һ����ŵ�b0
                        if (j == SIZE / 2 && b0 != -1)
                        {
                            b = FAT[b];
                            j = 0;
                            stp = (short *)Disk[b];
                        }
                    }
                    b = udtab[i].fb;
                    stp = (short *)Disk[b];
                    b0 = b1 = stp[0];
                    j = 1;
                    while (b1 > 0)
                    {
                        b1 = stp[j];
                        FAT[b0] = b1;
                        FAT[0]--;
                        b0 = b1;
                        j++;
                        if (j == SIZE / 2 && b1 != -1)
                        {
                            b = FAT[b];
                            j = 0;
                            stp = (short *)Disk[b];
                        }
                    }
                }
                s = FindFCB(udtab[i].ufname, s0, '\0', fcbp);
                fdp->FileName[0] = udtab[i].ufname[0]; //�ָ��ļ���
                if (s >= 0)                            //�������ļ�
                {
                    cout << "\n��Ŀ¼���Ѿ�������Ϊ" << udtab[i].ufname << "���ļ���"
                         << "��Ϊ���ָ��ļ�����һ���µ����֣�";
                    while (1)
                    {
                        cin.getline(Fname, INPUT_LEN);
                        if (IsName(Fname)) //����������ַ��Ϲ���
                        {
                            s = FindFCB(Fname, s0, '\0', fcbp); //�����������з�����
                            if (s >= 0)
                                cout << "\n������ļ�������������ͻ��\n�����������ļ�����";
                            else
                                break; //�������ֺϷ����������ļ����ڡ��˳�ѭ��
                        }
                        else //�������ֲ�������������
                            cout << "\n������ļ������Ϸ���\n�����������ļ�����";
                    }
                    strcpy(fdp->FileName, Fname);
                }
                cc++;      //���ָ��ļ�����1
                Del1Ud(i); //ɾ��udtab���е�i��
            }
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////

int UndelComd(int k) //undel����
{
    // ������ʽ��undel [<Ŀ¼��>]
    // ����ܣ��ָ�ָ��Ŀ¼�б�ɾ�����ļ�
    // ����������2��������ʽ��
    //		undel�����ָ���ǰĿ¼�б�ɾ�����ļ�
    //		undel <Ŀ¼��>�����ָ�ָ��Ŀ¼�б�ɾ�����ļ�

    short i, s, s0, cc = 0; //cc�ǻָ��ļ���������
    char *fn;
    FCB *fcbp1;
    if (k > 1)
    {
        cout << "\n������в�����\n";
        return -1;
    }
    if (k < 1) //���������޲���
    {
        strcpy(temppath, curpath.cpath);
        s0 = s = curpath.fblock;
    }
    else
    {
        s0 = s = FindPath(comd[1], '\020', 1, fcbp1);
        if (s < 0)
        {
            cout << "\n������������·������\n";
            return -2;
        }
    }
    while (s > 0) //���׿��Ϊs��Ŀ¼�ұ�ɾ���ļ��ı��ֱ��Ŀ¼β��
    {
        fcbp1 = (FCB *)Disk[s];
        for (i = 0; i < SIZE / sizeof(FCB); i++, fcbp1++)
        {
            if (fcbp1->FileName[0] == (char)0xe5) //�ҵ����ܽ���ɾ���ָ���Ŀ¼��
            {
                fn = &(fcbp1->FileName[1]);
                Udfile(fcbp1, s0, fn, cc);
            }
        }
        s = FAT[s]; //ȡ��һ���̿��
    }
    cout << "\n���ָ��� " << cc << " ����ɾ�����ļ���\n";
    return 1;
}

/////////////////////////////////////////////////////////////////

int ReadComd(int k) //read����Ĵ����������ļ�
{
    // ���ļ���read <�ļ���> [<λ��m>] [<�ֽ���n>]�����Ѵ򿪵��ļ����ļ����ݲ���ʾ������
    // ��λ�á���������Ӷ�ָ����ָλ�ÿ�ʼ��������"λ��"���������ָ��λ�ô���ʼ����λ
    // ��m��ָ���ļ���ͷ��m���ֽڴ�����m��1��ʼ��ţ�������"�ֽ���"���������ָ��λ�ö�
    // ���ļ�ĩβ������"�ֽ���n"���������ָ��λ�ÿ�ʼ��n���ֽڡ�ÿ��һ���ֽڣ���ָ���
    // ��һ���ֽڡ����ļ�δ�򿪻��ļ������ڣ��ֱ����������Ϣ��
    // read���������¼�����ʽ��
    //		(1) read��������ǰ�����ļ����Ӷ�ָ��λ�ÿ�ʼ�����ļ�β��(������ʽ)
    //  (2) read <�ļ���>������ָ���ļ����Ӷ�ָ��λ�ÿ�ʼ�����ļ�β��
    //  (3) read <�ļ���> |pm������ָ���ļ�����ָ��λ��m��ʼ�����ļ�β��
    //  (4) read <�ļ���> |ln������ָ���ļ����Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�
    //  (5) read <�ļ���> |pm |ln������ָ���ļ�����ָ��λ��m��ʼ��n���ֽ�
    //  (6) read |pm |ln��������ǰ�����ļ�����ָ��λ��m��ʼ��n���ֽ�(������ʽ)
    //  (7) read |pm��������ǰ�����ļ�����ָ��λ��m��ʼ�����ļ�β��(������ʽ)
    //  (8) read |ln��������ǰ�����ļ����Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�(������ʽ)
    // ˵�����մ򿪵��ļ������ָ��ָ���ļ���ͷ(����ָ�����1)��Լ�����ļ��Ķ�ָ�����0��

    short i, j, ii, i_uof, pos, offset;
    short b, b0, bnum, count = 0, readc;
    char attrib = '\0';
    char Buffer[SIZE + 1], *FileName;
    FCB *fcbp;

    if (k < 0 || k > 3)
    {
        cout << "\n�����в�������̫���̫�١�\n";
        return -1;
    }

    if (comd[1][0] == '|' || k == 0)
    {
        for (int i = k; i > 0; i--)
        {
            strcpy(comd[i + 1], comd[i]);
        }
        k++;
        strcpy(comd[1], newestOperateFile);
    }
    FindPath(comd[1], attrib, 0, fcbp); //����ȫ·����ȥ����..������temppath��

    strcpy(newestOperateFile, temppath);

    i_uof = Check_UOF(temppath); //��UOF
    if (i_uof == S)
    {
        cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܶ��ļ���\n";
        return -2;
    }
    cout << "readp:" << uof[i_uof].readp << endl;
    cout << "fsize:" << uof[i_uof].fsize << endl;
    cout << "writep:" << uof[i_uof].writep << endl;
    cout << "fname:" << uof[i_uof].fname << endl;
    if (uof[i_uof].readp == 0)
    {
        cout << "\n�ļ�" << temppath << "�ǿ��ļ���\n";
        return 1;
    }
    if (k == 1) //��������k=1��0�����(�޲���m��n)
    {
        pos = uof[i_uof].readp; //�Ӷ�ָ����ָλ�ÿ�ʼ��
        if (pos > uof[i_uof].fsize)
        {
            cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
            return 1;
        }
        readc = uof[i_uof].fsize - pos + 1; //�����ļ�β�������readc���ֽ�
    }
    else if (k == 2) //k=2��k=3�����
    {
        if (comd[2][0] != '|')
        {
            cout << "��������" << endl;
            return -1;
        }
        if (comd[2][1] == 'p') //|p
        {
            pos = atoi(&comd[2][2]);
            cout << "pos:" << pos << endl;
            if (pos > uof[i_uof].fsize)
            {
                cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
                return 1;
            }
            readc = uof[i_uof].fsize - pos + 1; //�����ļ�β�������readc���ֽ�
        }
        else if (comd[2][1] == 'l')
        {                           //|l
            pos = uof[i_uof].readp; //�Ӷ�ָ����ָλ�ÿ�ʼ��
            if (pos > uof[i_uof].fsize)
            {
                cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
                return 1;
            }
            readc = atoi(&comd[2][2]);
            cout << "readc:" << readc << endl;
            if (readc < 1)
            {
                cout << "\n�������ṩ�Ķ��ֽ�������\n";
                return -4;
            }
            if (readc > uof[i_uof].fsize - pos + 1)
                readc = uof[i_uof].fsize - pos + 1;
        }
        else
        {
            cout << "��������" << endl;
            return -1;
        }
    }
    else if (k == 3)
    {
        if (comd[2][0] != '|')
        {
            cout << "��������" << endl;
            return -1;
        }
        if (comd[2][1] == 'p') //|p |l
        {
            if (comd[3][0] != '|' || comd[3][1] != 'l')
            {
                cout << "��������" << endl;
                return -1;
            }
            pos = atoi(&comd[2][2]);
            if (pos > uof[i_uof].fsize)
            {
                cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
                return 1;
            }
            readc = atoi(&comd[3][2]);
            if (readc < 1)
            {
                cout << "\n�������ṩ�Ķ��ֽ�������\n";
                return -4;
            }
            if (readc > uof[i_uof].fsize - pos + 1)
                readc = uof[i_uof].fsize - pos + 1;
        }
        else if (comd[2][1] == 'l')
        { //|l |p
            if (comd[3][0] != '|' || comd[3][1] != 'p')
            {
                cout << "��������" << endl;
                return -1;
            }
            pos = atoi(&comd[3][2]); //�Ӷ�ָ����ָλ�ÿ�ʼ��
            if (pos > uof[i_uof].fsize)
            {
                cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
                return 1;
            }
            readc = atoi(&comd[2][2]);
            if (readc < 1)
            {
                cout << "\n�������ṩ�Ķ��ֽ�������\n";
                return -4;
            }
            if (readc > uof[i_uof].fsize - pos + 1)
                readc = uof[i_uof].fsize - pos + 1;
        }
        else
        {
            cout << "��������" << endl;
            return -1;
        }
    }
    bnum = (pos - 1) / SIZE;   //���ļ��ĵ�bnum���(bnum��0��ʼ���)
    offset = (pos - 1) % SIZE; //�ڵ�bnum���ƫ��λ��offset����ʼ��(offset��0��ʼ)
    b = uof[i_uof].faddr;      //ȡ�ļ��׿��
    for (i = 0; i < bnum; i++) //Ѱ�Ҷ���ĵ�һ���̿��
    {
        b0 = b;
        b = FAT[b];
    }
    ii = offset;
    while (count < readc) //���ļ���Buffer����ʾ֮
    {
        for (i = ii, j = 0; i < SIZE; i++, j++)
        {
            Buffer[j] = Disk[b][i];
            count++;
            if (count == readc)
            {
                j++;
                break;
            }
        }
        Buffer[j] = '\0';
        cout << Buffer;
        ii = 0;
        b = FAT[b]; //׼������һ���̿�
    }
    cout << endl;
    uof[i_uof].readp = pos + readc; //������ָ��
    return 1;
}

/////////////////////////////////////////////////////////////////

int CopyComd(int k) //copy����Ĵ������������ļ�
{
    // �����ļ���copy <Դ�ļ���> [<Ŀ���ļ���>]
    // ����ܣ�ΪĿ���ļ�����Ŀ¼������µ��̿飬����Դ�ļ������ݸ��Ƶ�Ŀ���ļ���
    // ����������һ��������ġ��ļ���������ָ���һ���������ļ���·������
    // ��Ŀ���ļ���Դ�ļ����ڵ�Ŀ¼��ͬ����ֻ�ܽ��и������ƣ���ʱĿ���ļ�������ʡ��
    // ��Ŀ���ļ���Դ�ļ����ڵ�Ŀ¼��ͬ����ȿɸ�������Ҳ��ͬ�����ƣ�ͬ������ʱĿ���ļ�����ʡ��
    // ���磬����
    //		copy mail email
    // 1(1) ����ǰĿ¼�в�����email(Ŀ¼���ļ�)����������ǰĿ¼�е��ļ�mail�����Ƴ�
    //     ��ǰĿ¼�µ��ļ�email;
    // 1(2) ����ǰĿ¼�´���email����email����Ŀ¼�����򽫵�ǰĿ¼�е��ļ�mail�����Ƶ���
    //     ǰĿ¼�е�email��Ŀ¼�ڣ��ļ�����Դ�ļ���ͬ(ͬ������)����ʱ��emailĿ¼���Ѿ�
    //     �����ļ���Ŀ¼mail���������������
    // 1(3) ����ǰĿ¼�ڴ���email�ļ����������������
    // 1(4) ����ǰĿ¼�ڲ�����Դ�ļ�mail(������Ȼ��mail����������Ŀ¼��)����Ҳ����
    // 1����������������Ŀ���ļ�ʱ����Դ�ļ�ͬ�����Ƶ���ǰĿ¼�С����磬��ǰĿ¼Ϊ/usr
    //		copy /box����������Ѹ�Ŀ¼�е��ļ�box���Ƶ���ǰĿ¼/usr�У��ļ�����Ϊbox��

    //��ע����ͬһĿ¼�У���Ŀ¼����������������ļ���������Ŀ¼������
    // ��ִ�����copy boy /usr/testʱ������Ŀ¼/usr���Ѵ����ļ�test��
    // ��ѯ���Ƿ񸲸ǣ���test����Ŀ¼�������ļ�boy���Ƶ���Ŀ¼/usr/test�£�
    // �ļ�����Դ�ļ���ͬ����boy���������/usr/test/boy��Ϊ��Ŀ¼������ʾ������Ϣ
    // ��ִֹͣ��copy���

    // ѧ�����ɿ���ʹ��ͨ����Ķ��ļ�ͬ�����Ƶ����(Ŀ���ļ���Դ�ļ�����Ŀ¼���벻ͬ)��

    short int i, size, dirStack, s02, resStack, s2, s22, b, b0, bnum, isMerge = 0, dirStack3, resStack3, isSameFile = 0;
    char attrib = '\0', *FileName1, *FileName2, *FileName3;
    char gFileName[PATH_LEN], *buf0, *buf1, *buf2; //����ļ�ȫ·����
    FCB *fcbp, *fcbp1 = new FCB, *fcbp2, *fcbp3 = new FCB, *fcbp4 = new FCB;
    if (k < 1 || k > 2)
    {
        cout << "\n�����в���̫���̫�١�\n";
        return -1;
    }
    for (int j = 1; j <= k; j++)
    {
        if (strcmp(comd[j], "*") == 0)
        {
            int block = curpath.fblock;
            while (block > 0)
            {
                fcbp1 = (FCB *)Disk[block];
                for (int i = 0; i < SIZE / sizeof(FCB); i++, fcbp1++)
                {
                    if (strcmp(fcbp1->FileName,"")== 0||fcbp1->FileName[0] == (char)0xe5 || strcmp(fcbp1->FileName, "..") == 0 || fcbp1->Fattrib >= (char)16)
                    {
                        continue;
                    }
                    char comd1[INPUT_LEN];
                    strcpy(comd1, "copy ");
                    strcat(comd1, fcbp1->FileName);
                    strcat(comd1, " ");
                    strcat(comd1, comd[j + 1]);
                    strcpy(BatchComds[BatchHeader], comd1);
                    BatchHeader = (BatchHeader + 1) % BATCHNUM;
                    if (BatchRail == BatchHeader)
                    {
                        cout << "batch�������������������������������󻺳������Ѿ�����Ľ������ơ�" << endl;
                        BatchHeader--;
                        return -1;
                    }
                }
                block = FAT[block];
            }
            return 1;
        }
    }
    for (int i = 0; i < strlen(comd[1]); i++)
    {
        if (comd[1][i] == '+')
        {
            comd[1][i] = '\0';
            strcpy(comd[3], &comd[1][i + 1]);
            isMerge = 1;
            k = 3;
        }
    }
    //�����һ���ļ�==================================================
    dirStack = ProcessPath(comd[1], FileName1, k, 0, '\20');
    //ȡFileName����Ŀ¼���׿��
    if (dirStack < 1)    //·������
        return dirStack; //ʧ�ܣ�����
    resStack = FindFCB(FileName1, dirStack, attrib, fcbp);
    //ȡFileName(Դ�ļ�)���׿��(���������)
    if (resStack < 0)
    {
        cout << "\nҪ���Ƶ��ļ������ڡ�\n";
        return -1;
    }
    *fcbp1 = *fcbp; //����Դ�ļ�Ŀ¼��ָ��ֵ
    strcpy(gFileName, temppath);
    i = strlen(temppath);
    if (temppath[i - 1] != '/')
        strcat(gFileName, "/");
    strcat(gFileName, FileName1); //�����ļ���ȫ·����
    i = Check_UOF(gFileName);     //��UOF
    if (i < S)                    //���ļ�����UOF��
    {
        cout << "\n�ļ�" << gFileName << "�Ѿ��򿪣����ܸ���!\n";
        return -2;
    }
    //�����һ���ļ�����==================================================
    //����������ļ�������Ǻϲ����ƣ�����������ļ�ָ�ڶ��������ļ�========
    if (isMerge == 1)
    {
        dirStack3 = ProcessPath(comd[3], FileName3, k, 0, '\20');
        //ȡFileName����Ŀ¼���׿��
        if (dirStack3 < 1)    //·������
            return dirStack3; //ʧ�ܣ�����
        resStack3 = FindFCB(FileName3, dirStack3, attrib, fcbp);
        //ȡFileName(Դ�ļ�)���׿��(���������)
        if (resStack3 < 0)
        {
            cout << "\nҪ���Ƶ��ļ�" << FileName3 << "�����ڡ�\n";
            return -1;
        }
        *fcbp3 = *fcbp; //����Դ�ļ�Ŀ¼��ָ��ֵ
        strcpy(gFileName, temppath);
        i = strlen(temppath);
        if (temppath[i - 1] != '/')
            strcat(gFileName, "/");
        strcat(gFileName, FileName1); //�����ļ���ȫ·����
        i = Check_UOF(gFileName);     //��UOF
        if (i < S)                    //���ļ�����UOF��
        {
            cout << "\n�ļ�" << gFileName << "�Ѿ��򿪣����ܸ���!\n";
            return -2;
        }
    }
    //����������ļ�����������Ǻϲ����ƣ�����������ļ�ָ�ڶ��������ļ�=========
    //����Ǻϲ��������ļ���3ȱʡ������ļ�1���ļ������ƽ�comd[2]
    if (isMerge == 1 && comd[2][0] == '\0')
    {
        strcpy(comd[2], FileName1);
    }

    if (k == 1) //��������Ŀ���ļ�,ͬ�����Ƶ���ǰĿ¼
    {
        s02 = curpath.fblock; //ȡ��ǰĿ¼���׿��
        FileName2 = FileName1;
    }
    else //k==2||k==3 1.�������ṩĿ���ļ������ 2.��Ϊ�ϲ�����
    {
        s02 = ProcessPath(comd[2], FileName2, k, 0, '\20'); //ȡFileName2����Ŀ¼���׿��
        if (s02 < 1)                                        //Ŀ��·������
            return s02;
    }

    if (!IsName(FileName2) && strcmp(temppath, "/") != 0 && strcmp(FileName2, "..") != 0)
    {
        //�����ֲ����Ϲ����Ҳ��Ǹ�Ŀ¼
        cout << "\n�����е�Ŀ���ļ�������\n";
        return -2;
    }
    else if (strcmp(temppath, "/") == 0 && strcmp(FileName2, "") == 0)
    { //FileNameΪ�գ���s02�Ǹ�Ŀ¼,��Ҫ�Ǹ�Ŀ¼û��fcb
        s2 = s02;
        strcpy(fcbp->FileName, "/");
        fcbp->Addr = 1;
        fcbp->Fattrib = (char)16;
    }
    else
    {
        s2 = FindFCB(FileName2, s02, '\040', fcbp);
    }
    //ȡFileName2(Ŀ���ļ�)���׿��,���������ļ�����Ŀ¼
    if (s2 >= 0 && fcbp->Fattrib <= '\07')
    { //���ļ���ѯ�ʸ���
        s22 = s02;
        char b;
        // if (s02 == dirStack) //Դ�ļ���Ŀ���ļ�ͬĿ¼
        // {
        //     cout << "ͬĿ¼�޷����ơ�" << endl;
        //     return -1;
        // }
        cout << "�Ƿ񸲸ǣ�y/n" << endl;
        while (1)
        {
            cin >> b;
            if (b != 'y' && b != 'Y' && b != 'n' && b != 'N')
                cout << "������������������:" << endl;
            else
            {
                break;
            }
        }
        if (b == 'y' || b == 'Y')
        {
            if (fcbp->Addr != fcbp1->Addr)
            {
                fleshBlock(fcbp);
            }
            else
            {
                buf2 = new char[fcbp->Fsize + 1];
                file_to_buffer(fcbp, buf2);
                isSameFile = 1;
                fleshBlock(fcbp);
            }
        }
        else
        {
            cout << "���Ƴ�ͻ������ȡ��" << endl;
            return -1;
        }
    }
    else if (s2 >= 0 && fcbp->Fattrib >= (char)16)
    { //FileName2������Ŀ¼�������������Ŀ¼�ڲ��Ƿ���ͬ���ļ���Ŀ¼
        s22 = s2;
        if (s2 != dirStack) //Դ�ļ���Ŀ���ļ���ͬĿ¼
        {
            b = FindFCB(FileName1, s2, (char)32, fcbp); //���FileName2Ŀ¼����û���ļ�FileNa
            if (b >= 0 && fcbp->Fattrib <= (char)7)
            {
                cout << "��ͬ���ļ����Ƿ񸲸ǣ�y/n" << endl;
                char bb;
                while (1)
                {
                    cin >> bb;
                    if (bb != 'y' && bb != 'Y' && bb != 'n' && bb != 'N')
                        cout << "������������������:" << endl;
                    else
                    {
                        break;
                    }
                }
                if (bb == 'y' || bb == 'Y')
                {
                    if (fcbp->Addr != fcbp1->Addr)
                    {
                        fleshBlock(fcbp);
                    }
                    else
                    {
                        buf2 = new char[fcbp->Fsize + 1];
                        file_to_buffer(fcbp, buf2);
                        isSameFile = 1;
                        fleshBlock(fcbp);
                    }
                }
                else
                {
                    cout << "���Ƴ�ͻ������ȡ��" << endl;
                    return -1;
                }
            }
            else if (b >= 0 && fcbp->Fattrib >= (char)16)
            {
                cout << "����ͬ��Ŀ¼���޷����ơ�" << endl;
                return -1;
            }
        }
        else
        {
            cout << "\n����ͬĿ¼ͬ�����ơ�\n";
            return -5;
        }
        //��ΪFileName2�Ǹ�Ŀ¼��������Ŀ¼����ͬ��Ŀ¼
        // ����ͬ���ļ�����ͬ���ļ���ѡ�񸲸ǣ���ͬ������
        FileName2 = FileName1;
    }
    else if (s2 < 0)
    { //FileName2�в����ڣ���s02Ϊ�׿�ŵ�Ŀ¼�ڸ���Ŀ���ļ�
        s22 = s02;
    }

    //���ϼ�������ʼ����
    i = FindBlankFCB(s22, fcbp2);
    if (i < 0)
    {
        cout << "\n�����ļ�ʧ�ܡ�\n";
        return i;
    }
    *fcbp2 = *fcbp1;
    //Դ�ļ���Ŀ¼��Ƹ�Ŀ���ļ���a��fcb��c
    //дĿ���ļ���cд����
    strcpy(fcbp2->FileName, FileName2);
    fcbp2->Addr = 0;
    if (isMerge == 1)
    { //�ϲ�����
        if (isSameFile == 0)
        {
            buf0 = new char[fcbp1->Fsize + fcbp3->Fsize + 1];
            file_to_buffer(fcbp1, buf0);
        }
        else
        {
            buf0 = new char[strlen(buf2) + fcbp3->Fsize + 1];
            strcpy(buf0, buf2);
        }
        buf1 = new char[fcbp3->Fsize + 1];
        file_to_buffer(fcbp3, buf1);
        strcat(buf0, buf1);
        buffer_to_file(fcbp2, buf0);
        delete[] buf1, buf0;
    }
    else
    {                                                  //��ͨ����
        size = fcbp1->Fsize;                           //Դ�ļ��ĳ���
        bnum = size / SIZE + (short)(size % SIZE > 0); //����Դ�ļ���ռ�̿���
        if (FAT[0] < bnum)
        {
            cout << "\n���̿ռ����������ܸ����ļ���\n";
            return -6;
        }
        b0 = 0;
        while (resStack > 0) //��ʼ�����ļ�����
        {
            b = getblock();
            if (b0 == 0)
                fcbp2->Addr = b; //Ŀ���ļ����׿��
            else
                FAT[b0] = b;
            memcpy(Disk[b], Disk[resStack], SIZE); //�����̿�
            resStack = FAT[resStack];              //׼��������һ���̿�
            b0 = b;
        }
        // if (isSameFile == 1)
        //     fleshBlock(fcbp4);
    }
    delete fcbp1, fcbp3, fcbp4;
    return 1; //�ļ����Ƴɹ�������
}

/////////////////////////////////////////////////////////////////

int FseekComd(int k) //fseek����Ĵ�����
{
    // ������ʽ��fseek <�ļ���> <n>
    // ���ܣ�������дָ���Ƶ�ָ��λ��n��

    int i_uof, n;
    char attrib = '\0', *FileName;
    FCB *fcbp;

    if (k < 1 || k > 2)
    {
        cout << "\n���������������\n";
        return -1;
    }
    if (k == 1)
    {
        strcpy(comd[2], comd[1]);
        strcpy(comd[1], newestOperateFile);
    }
    n = atoi(comd[2]);
    FindPath(comd[1], attrib, 0, fcbp); //����ȫ·����ȥ����..������temppath��
    strcpy(newestOperateFile, temppath);

    i_uof = Check_UOF(temppath); //��UOF
    if (i_uof == S)
    {
        cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܲ�����\n";
        return -2; //����ʧ�ܷ���
    }
    if (uof[i_uof].fsize == 0) //���ļ�
    {
        cout << "\n"
             << temppath << "�ǿ��ļ������ܽ��д˲�����\n";
        return -3;
    }
    if (n <= 0 || n > uof[i_uof].fsize + 1)
    {
        cout << "\nλ�ò������󡣸ò���������1��" << uof[i_uof].fsize + 1 << "֮�䡣\n";
        return -4;
    }
    uof[i_uof].readp = n;  //��ָ���趨Ϊn
    uof[i_uof].writep = n; //дָ���趨Ϊn
    return 1;              //�޸ĳɹ�������
}

/////////////////////////////////////////////////////////////////

int RenComd(int k) //ren����Ĵ��������ļ�����
{
    // ������ʽ��ren <ԭ�ļ���> <���ļ���>
    // ��ԭ�ļ������ڣ�����������Ϣ��
    // ��ԭ�ļ����ڣ�������ʹ�ã�Ҳ���ܸ�����ͬ����ʾ������Ϣ��
    // Ӧ������ļ����Ƿ������������

    short i, s0, s;
    char attrib = '\0', *FileName;
    char gFileName[PATH_LEN]; //����ļ�ȫ·����
    FCB *fp, *fcbp;
    s0 = ProcessPath(comd[1], FileName, k, 2, '\20'); //ȡFileName����Ŀ¼���׿��
    if (s0 < 1)                                       //·������
        return s0;                                    //ʧ�ܣ�����
    s = FindFCB(FileName, s0, attrib, fcbp);          //ȡFileName���׿��(���������)
    if (s < 0)
    {
        cout << "\nҪ�������ļ������ڡ�\n";
        return -2;
    }
    strcpy(gFileName, temppath);
    i = strlen(temppath);
    if (temppath[i - 1] != '/')
        strcat(gFileName, "/");
    strcat(gFileName, FileName); //�����ļ���ȫ·����
    i = Check_UOF(gFileName);    //��UOF
    if (i < S)                   //���ļ�����UOF��
    {
        cout << "\n�ļ�" << gFileName << "�Ѿ��򿪣����ܸ���!\n";
        return -3;
    }
    if (IsName(comd[2]))
    {
        fp = fcbp;                              //����ָ��Ҫ�����ļ�Ŀ¼���ָ��
        s = FindFCB(comd[2], s0, attrib, fcbp); //�����ļ����Ƿ�����
        if (s < 0)                              //������
        {
            strcpy(fp->FileName, comd[2]);
            return 1; //��ȷ����
        }
        cout << "\n���������ļ���ͬ�����ļ���\n";
        return -5;
    }
    cout << "\n�������ṩ�����ļ�������\n";
    return -4;
}

/////////////////////////////////////////////////////////////////

int AttribComd(int k) //attrib����Ĵ��������޸��ļ���Ŀ¼����
{
    // ��ʾ�޸��ļ����ԣ�attrib <�ļ���> [��<����>]������������"�ļ�����"������
    // ����ʾָ���ļ������ԣ�����������"�ļ�����"���������޸�ָ���ļ������ԡ�"��
    // ������"����ʽ�С�+r��+h��+s���͡�-r��-h��-s��������ʽ��ǰ��Ϊ����ָ���ļ�
    // Ϊ"ֻ��"��"����"��"ϵͳ"���ԣ�����Ϊȥ��ָ���ļ���"ֻ��"��"����"��"ϵͳ"
    // ���ԡ������Կ����ʹ����˳���ޡ����磺
    //		attrib user/boy +r +h
    // �书�������õ�ǰĿ¼��user��Ŀ¼�е��ļ�boyΪֻ���������ļ�������
    //		attrib /usr/user/box -h -r -s
    // ��������Ĺ�����ȡ���ļ�/usr/user/box��"����"��"ֻ��"��"ϵͳ"���ԡ�
    // ��������ָ�����ļ��Ѵ򿪻򲻴��ڣ�����������Ϣ��
    // ���������ṩ�Ĳ�������Ҳ��ʾ������Ϣ��

    short i, j, i_uof, s, isAll = 0;
    char Attrib, attrib = '\40';
    char Attr[5], Attr1[4] = "RHS";
    char attr[6][3] = {"+r", "+h", "+s", "-r", "-h", "-s"};
    char or_and[6] = {(char)1, (char)2, (char)4, (char)30, (char)29, (char)27};
    FCB *fcbp;

    if (k < 1)
    {
        cout << "��������\n";
        return -1;
    }
    if (strcmp(comd[1], "*") == 0)
    {
        strcpy(comd[1], curpath.cpath);
        isAll = 1;
    }
    cout << comd[1] << endl;
    s = FindPath(comd[1], attrib, 1, fcbp); //Ѱ��ָ�����ļ���Ŀ¼���������׿��
    cout << s << endl;
    if (s < 0)
    {
        cout << '\n'
             << temppath << "�ļ���Ŀ¼�����ڡ�\n";
        return -2;
    }
    if (k == 1) //��ʾ�ļ�/Ŀ¼������
    {
        if (isAll == 0)
        {
            showAttribute(fcbp);
        }
        else
        {
            int block = s;
            while (block > 0)
            {
                fcbp = (FCB *)Disk[block];
                for (int i = 0; i < SIZE / sizeof(FCB); i++, fcbp++)
                {
                    if (fcbp->FileName[0] == (char)0xe5||strcmp(fcbp->FileName,"..")==0)
                    {
                        continue;
                    }
                    // cout << "111:" << fcbp->FileName;
                    showAttribute(fcbp);
                }
                block = FAT[block];
            }
        }
        return 1;
    }
    if (fcbp->Fattrib <= '\07') //�����ļ���Ҫ�����Ƿ��ѱ���
    {
        i_uof = Check_UOF(temppath); //��UOF
        if (i_uof < S)
        {
            cout << "\n�ļ�" << temppath << "�����ţ������޸����ԡ�\n";
            return -3;
        }
    }
    for (i = 2; i <= k; i++) //�������Բ���
    {
        for (j = 0; j < 6; j++)
            if (strcasecmp(comd[i], attr[j]) == 0)
            {
                if (j < 3)
                    fcbp->Fattrib = fcbp->Fattrib | or_and[j];
                else
                    fcbp->Fattrib = fcbp->Fattrib & or_and[j];
                break;
            }
        if (j == 6)
        {
            cout << "\n�����е����Բ�������\n";
            return -4;
        }
    }
    return 1; //�޸�������ɣ�����
}

/////////////////////////////////////////////////////////////////

int RewindComd(int k) //rewind����Ĵ�����������дָ���Ƶ��ļ���ͷ
{
    // ������ʽ��rewind <�ļ���>
    // ��ָ���ļ�������ͬʱ���ļ���Ϊ��ǰ�����ļ�

    int i_uof;
    char attrib = '\0';
    FCB *fcbp;

    if (k < 0 || k > 1)
    {
        cout << "\n���������������\n";
        return -1;
    }
    if (k == 0)
    {
        strcpy(comd[1], newestOperateFile);
    }
    FindPath(comd[1], attrib, 0, fcbp); //����ȫ·����ȥ����..������temppath��
    strcpy(newestOperateFile, temppath);
    i_uof = Check_UOF(temppath); //��UOF
    if (i_uof == S)
    {
        cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܲ�����\n";
        return -1; //����ʧ�ܷ���
    }
    if (uof[i_uof].faddr > 0) //���ǿ��ļ�
        uof[i_uof].readp = 1; //��ָ���趨Ϊ0
    else
        uof[i_uof].readp = 0; //�ǿ��ļ��Ķ�ָ���趨Ϊ1
    uof[i_uof].writep = 1;    //�ļ���дָ���趨Ϊ1
    cout << "\n�ļ�" << temppath << "��ָ���趨Ϊ" << uof[i_uof].readp << ",дָ���趨Ϊ1\n";
    return 1; // �޸ĳɹ�������
}

/////////////////////////////////////////////////////////////////

void UofComd() //uof�����ʾ��ǰ�û������ļ���
{
    //��ʾ�û��Ѵ��ļ���UOF������

    int i, k;
    char ch;
    for (k = i = 0; i < S; i++)
        k += uof[i].state;
    if (k > 0)
    {
        cout << "\n���ļ���UOF����������:\n\n"
             << "�ļ���                       �ļ�����  "
             << "�׿��  �ļ�����  ״̬  ��ָ��  дָ��\n";
        for (i = 0; i < S; i++)
        {
            if (uof[i].state == 0)
                continue; //��Ŀ¼��
            cout.setf(ios::left);
            cout << setw(32) << uof[i].fname; //��ʾ�ļ���
            ch = uof[i].attr;
            switch (ch)
            {
            case '\0':
                cout << "��ͨ    ";
                break;
            case '\01':
                cout << "R       ";
                break;
            case '\02':
                cout << "H       ";
                break;
            case '\03':
                cout << "RH      ";
                break;
            case '\04':
                cout << "S       ";
                break;
            case '\05':
                cout << "RS      ";
                break;
            case '\06':
                cout << "HS      ";
                break;
            case '\07':
                cout << "RHS     ";
                break;
            default:
                cout << "����    ";
            }
            cout << setw(8) << uof[i].faddr; //�׿��
            cout << setw(8) << uof[i].fsize; //�ļ���С
            k = uof[i].state;
            if (k == 1)
                cout << " ����   "; //״̬Ϊ��������
            else
                cout << " ��   "; //״̬Ϊ���򿪡�
            cout << setw(8) << uof[i].readp;
            cout << uof[i].writep << endl; //��ָ��
        }
    }
    else
        cout << "Ŀǰ���޴򿪵��ļ���\n";
}
/*

  */
/////////////////////////////////////////////////////////////////

void save_FAT() //�����ļ������FAT�������ļ�FAT.txt
{
    int i;
    ofstream ffo;
    ffo.open("FAT.txt");
    for (i = 0; i < K; i++)
        ffo << FAT[i] << ' ';
    ffo.close();
}

/////////////////////////////////////////////////////////////////

void save_Disk() //�����̿��е��ļ�����
{
    int i;
    short *p = (short *)Disk[0];
    p[0] = ffbp;
    p[1] = Udelp;
    p[2] = udtabblock;
    UnDel *ud = (UnDel *)Disk[udtabblock];
    for (i = 0; i < DM; i++)
    {
        strcpy(ud->gpath, udtab[i].gpath);
        ud->ufaddr = udtab[i].ufaddr;
        ud->ufattr = udtab[i].ufattr;
        ud->fb = udtab[i].fb;
        strcpy(ud->ufname, udtab[i].ufname);
        ud++;
    }
    ofstream ffo("Disk.dat", ios::binary);
    for (i = 0; i < K; i++)
        ffo.write((char *)&Disk[i], SIZE);
    ffo.close();
}

/////////////////////////////////////////////////////////////////

void save_UdTab() //���汻ɾ���ļ���Ϣ��
{
    int i;
    // ofstream ffo("UdTab.dat", ios::binary);
    // for (i = 0; i < DM; i++)
    //     ffo.write((char *)&udtab[i], sizeof(udtab[0]));
    // ffo.close();
}

/////////////////////////////////////////////////////////////////

int FindFCB(char *Name, int s, char attrib, FCB *&fcbp)
{
    // �ӵ�s�鿪ʼ����������ΪName�ҷ�������attrib��Ŀ¼��
    // ��������Nameû���ҵ����ظ������ҵ����طǸ���(��Ŀ¼ʱ���غ���)
    // ������ȷ����ʱ�����ò���ָ�����fcbpָ��NameĿ¼�

    int i;
    char ch, Attrib;
    while (s > 0)
    {
        fcbp = (FCB *)Disk[s];
        for (i = 0; i < SIZE / sizeof(FCB); i++, fcbp++) //ÿ���̿�4��Ŀ¼��
        {
            ch = fcbp->FileName[0];
            if (ch == (char)0xe5)
                continue;
            if (ch == '\0')
            {
                return -1; //·������(����Ŀ¼β����δ�ҵ�)
            }
            if (strcmp(Name, fcbp->FileName) == 0) //�����ҵ�
            {
                if (attrib == (char)32) //attribΪ32ʱ���ļ�����Ŀ¼����
                    return fcbp->Addr;
                //����ҵ���fcb�ļ������Ƿ���ϴ����attribҪ��
                Attrib = fcbp->Fattrib;                     //�õ��ҵ����ļ�������
                if (attrib == (char)16 && Attrib >= attrib) //��Ŀ¼����
                    return fcbp->Addr;
                if (attrib == (char)0 && Attrib <= (char)7) //�ļ�����(�ҵ����ļ�)
                    return fcbp->Addr;
                return -3; //���ַ��ϵ����Բ�����Ȼû���ҵ�
            }
        }
        s = FAT[s]; //ȡ��һ���̿��
    }
    return -2;
}

/////////////////////////////////////////////////////////////////

int FindPath(char *pp, char attrib, int ffcb, FCB *&fcbp)
{
    // ���������и�����·����ȷ��·������ȷ�ԣ�������·�������һ��
    // ����(Ŀ¼��)�����Ŀ¼�ĵ�ַ(�׿��)����·�����д���ȥ��·
    // ���еġ�..����������һ��ȫ·��������temppath�У�����������ffcb
    // ����ʱ��ͨ������FindFCB( )������ʹ�������ɹ�����ʱ,FCB���͵�
    // ���ò���ָ�����fcbpָ��·�����һ��Ŀ¼��Ŀ¼�

    short i, j, len, s = 0;
    char paths[60][FILENAME_LEN]; //�ֽ�·����(·������಻����60������)
    char *q, Name[PATH_LEN];

    strcpy(temppath, "/");
    if (strcmp(pp, "/") == 0) //�Ǹ�Ŀ¼
        return 1;             //���ظ�Ŀ¼���׿��

    if (*pp == '/') //����·�����Ӹ�Ŀ¼��ʼ
    {
        s = 1; //��Ŀ¼���׿��
        pp++;
    }
    else
    {
        s = curpath.fblock; //���·�����ӵ�ǰĿ¼��ʼ
        strcpy(temppath, curpath.cpath);
    }

    j = 0;
    while (*pp != '\0') //�������е�·���ֽ�
    {
        for (i = 0; i < PATH_LEN; i++, pp++)
        {
            if (*pp != '/' && *pp != '\0')
                Name[i] = *pp;
            else
            {
                if (i > 0)
                {
                    Name[i] = '\0';
                    if (i > FILENAME_LEN - 1) //���ֹ������ȡǰFILENAME_LEN-1���ַ�
                        Name[FILENAME_LEN - 1] = '\0';
                    strcpy(paths[j], Name);
                    j++;
                }
                else
                    return -1; //·������

                if (*pp == '/')
                    pp++;
                break; //�Ѵ����ַ���β��
            }
        }
    }
    for (i = 0; i < j; i++)
    {
        if (strcmp(paths[i], "..") == 0)
        {
            if (strcmp(temppath, "/") == 0)
                return -1; //·������(��Ŀ¼�޸�Ŀ¼)
            len = strlen(temppath);
            q = &temppath[len - 1];
            while (*q != '/')
                q--;
            *q = '\0';
            if (*temppath == '\0')
            {
                *temppath = '/';
                temppath[1] = '\0';
            }
        }
        else
        {
            if (strcmp(temppath, "/") != 0)
                strcat(temppath, "/");
            strcat(temppath, paths[i]);
        }
        if (ffcb)
        {
            s = FindFCB(paths[i], s, attrib, fcbp);
            if (s < 0)
                return s;
        }
    }
    return s;
}

/////////////////////////////////////////////////////////////////

void FatComd() //������"fat"
{
    cout << "\n��ǰ����ʣ����п���Ϊ" << FAT[0] << endl;
}

/////////////////////////////////////////////////////////////////

void CheckComd() //check����
{
    cout << "\n��ǰ���̿����ǣ�" << FAT[0] << endl;
    int j = 0;
    for (int i = 2; i < K; i++)
    {
        if (FAT[i] == 0)
            j++;
    }
    FAT[0] = j;
    cout << "���¼��󣬴��̵Ŀ��п��ǣ�" << FAT[0] << endl;
    cout << "\nffbp=" << ffbp << endl;
    cout << "Udelp=" << Udelp << endl;
}

/////////////////////////////////////////////////////////////////

void ExitComd() //exit�����
{
    char yn;
    CloseallComd(0); //�ر����д򿪵��ļ��Է����ݶ�ʧ
    cout << "\n�˳�ʱFAT��Disk��Udtab�Ƿ�Ҫ���̣�(y/n) ";
    cin >> yn;
    if (yn == 'Y' || yn == 'y')
    {
        save_FAT();   //FAT�����
        save_Disk();  //���̿��д洢������
        save_UdTab(); //���汻ɾ���ļ���Ϣ��
    }
    //delete [] Disk;
    exit(0);
}

/////////////////////////////////////////////////////////////////

bool isunname(char ch)
{
    char cc[] = "\"*+,/:;<=>?[\\]| ";
    for (int i = 0; i < 16; i++)
        if (ch == cc[i])
            return true;
    return false;
}

/////////////////////////////////////////////////////////////////

bool IsName(char *Name)
{
    // �ж������Ƿ�������¹���
    // (1) ���ֳ��Ȳ��ܳ���FILENAME_LEN-1���ֽڣ���10���ֽڡ�
    //     ������������ֳ���10���ַ�����ֻ��ǰ10���ַ���Ч��
    // (2) ����һ������ĸ�����ִ�Сд�������֡��»��ߵ���ɣ����������Ǻ��֣�
    // (3) ���ֲ��ܰ�������16���ַ�֮һ��
    //		" * + , / : ; < = > ? [ \ ] | space(�ո�)
    // (4) ��������������ַ���.�����������������ֵĵ�һ���ַ����ʡ�.����
    //    ��.abc������..���͡�..abc���ȶ��ǲ��Ϸ������֡�

    int i, len, Len = FILENAME_LEN - 1;
    bool yn = true;
    char ch;
    len = strlen(Name);
    if (len == 0)
        return false;
    if (Name[0] == '.') //���ֵ�һ���ַ��������ַ�'.'
        return false;
    if (len > Len) //�����ֹ�������ȥ�����β��
    {
        Name[Len] = '\0';
        len = Len;
    }
    for (i = 0; i < len; i++)
    {
        ch = Name[i];
        if (isunname(ch)) //�������к��в��Ϸ�����
        {
            yn = false;
            break;
        }
    }
    if (!yn)
        cout << "\n�����в��ܰ����ַ�'" << ch << "'��\n";
    return yn;
}

/////////////////////////////////////////////////////////////////

void PromptComd(void) //prompt����
{
    dspath = !dspath;
}

/////////////////////////////////////////////////////////////////

void UdTabComd(void) //udtab����
{
    //��ʾɾ���ļ��ָ���udtab������

    cout << "\n�ָ���ɾ���ļ���Ϣ��(UdTab)�������£�\n\n";
    cout << "�ļ�·����                      "
         << "�ļ���        "
         << "�׿��      "
         << "�洢���" << endl;
    for (int i = 0; i < Udelp; i++)
        cout << setiosflags(ios::left) << setw(32) << udtab[i].gpath
             << setw(15) << udtab[i].ufname << setw(12) << udtab[i].ufaddr
             << setw(8) << udtab[i].fb << endl;
}

/////////////////////////////////////////////////////////////////

int file_to_buffer(FCB *fcbp, char *Buffer) //�ļ����ݶ���Buffer,�����ļ�����
{
    //�ļ����ݶ���Buffer,�����ļ�����

    short s, len, i, j = 0;

    len = fcbp->Fsize; //ȡ�ļ�����
    s = fcbp->Addr;    //ȡ�ļ��׿��
    while (s > 0)
    {
        for (i = 0; i < SIZE; i++, j++)
        {
            if (j >= len) //�Ѷ�����ļ�
                break;
            Buffer[j] = Disk[s][i];
        }
        s = FAT[s]; //ȡ��һ���̿�
    }
    Buffer[j] = '\0';
    return len; //�����ļ�����
}

/////////////////////////////////////////////////////////////////

int buffer_to_file(FCB *fcbp, char *Buffer) //Bufferд���ļ�
{
    //�ɹ�д���ļ�������1��д�ļ�ʧ�ܣ�����0

    short bn1, bn2, i, j, s, s0, len, size, count = 0;

    len = strlen(Buffer); //ȡ�ַ���Buffer����
    s0 = s = fcbp->Addr;  //ȡ�ļ��׿��
    if (len == 0)
    {
        fcbp->Addr = fcbp->Fsize = 0; //�ļ���Ϊ���ļ�
        releaseblock(s);              //�ͷ��ļ�ռ�õĴ��̿ռ�
        return 1;
    }
    size = fcbp->Fsize;                           //ȡ�ļ�����
    bn1 = len / SIZE + (short)(len % SIZE > 0);   //Buffer������ռ�õ��̿���
    bn2 = size / SIZE + (short)(size % SIZE > 0); //�ļ�ԭ������ռ�õ��̿���
    if (FAT[0] < bn1 - bn2)
    {
        cout << "\n���̿ռ䲻�㣬���ܽ���Ϣд���ļ���\n";
        return 0;
    }
    if (s == 0) //���ǿ��ļ�
    {
        s0 = s = getblock(); //Ϊ������׸��̿�
        fcbp->Addr = s0;     //�����׿��
    }
    j = 0;
    while (j < len) //Bufferд��FilName2
    {
        if (s < 0)
        {
            s = getblock();
            FAT[s0] = s;
        }
        for (i = 0; i < SIZE; i++, j++)
        {
            if (j == len)
                break;
            if (Buffer[j] == '\\' && Buffer[j + 1] == 'n')
            {
                Disk[s][i] = '\n';
                j++;
                count++;
            }
            else
                Disk[s][i] = Buffer[j];
        }
        s0 = s;
        s = FAT[s];
    }
    if (s > 0)
    {
        FAT[s0] = -1;    //Ŀ���ļ������̿���
        releaseblock(s); //��FileName2�����̿�δʹ�ã�Ӧ�ͷ�����
    }
    fcbp->Fsize = len - count; //�ı��ļ��ĳ���
    cout << fcbp->Fsize << endl;
    return 1;
}

/////////////////////////////////////////////////////////////////

void releaseblock(short s) //���մ��̿ռ�
{                          //�ͷ�s��ʼ���̿���
    short s0;
    while (s > 0) //ѭ��������ֱ���̿���β��
    {
        s0 = s;      //s0���µ�ǰ���
        s = FAT[s];  //sָ����һ���̿�
        FAT[s0] = 0; //�ͷ��̿�s0
        FAT[0]++;    //�����̿�����1
    }
}

//��������/////////////////////////////////////////////////////////////////
int fcComd(int k)
{
    if (k != 2)
    {
        cout << "\n�����в���̫���̫�١�\n";
        return -1;
    }
    char attrib = '\0', *FileName1, *FileName2, gFileName[PATH_LEN]; //����ļ�ȫ·����
    FCB *fcbp, *fcbp1, *fcbp2;
    int firstFileSize, firstFileBlocks, secondFileSize, secondFileBlocks;
    short int i, firstDirBlock, secondDirBlock, firstFileBlock, secondFileBlock;
    firstDirBlock = ProcessPath(comd[1], FileName1, k, 0, '\20');
    //ȡFileName����Ŀ¼���׿��
    if (firstDirBlock < 1)
        return firstDirBlock; //·������ʧ�ܷ���
    firstFileBlock = FindFCB(FileName1, firstDirBlock, attrib, fcbp);
    if (firstFileBlock < 0)
    {
        cout << "\n��һ���ļ������ڡ�\n";
        return -1;
    }
    fcbp1 = fcbp; //����Դ�ļ�Ŀ¼��ָ��ֵ

    strcpy(gFileName, temppath);
    i = strlen(temppath);
    if (temppath[i - 1] != '/')
        strcat(gFileName, "/");
    strcat(gFileName, FileName1); //�����ļ���ȫ·����
    i = Check_UOF(gFileName);     //��UOF
    if (i < S)                    //���ļ�����UOF��
    {
        cout << "\n�ļ�" << gFileName << "�Ѿ��򿪣����ܱȽ�!\n";
        return -2;
    }

    secondDirBlock = ProcessPath(comd[2], FileName2, k, 0, '\20');
    //ȡFileName����Ŀ¼���׿��
    if (secondDirBlock < 1)
        return secondDirBlock; //·������ʧ�ܷ���
    secondFileBlock = FindFCB(FileName2, secondDirBlock, attrib, fcbp);
    if (secondFileBlock < 0)
    {
        cout << "\n�ڶ����ļ������ڡ�\n";
        return -1;
    }
    fcbp2 = fcbp; //����Դ�ļ�Ŀ¼��ָ��ֵ

    strcpy(gFileName, temppath);
    i = strlen(temppath);
    if (temppath[i - 1] != '/')
        strcat(gFileName, "/");
    strcat(gFileName, FileName2); //�����ļ���ȫ·����
    i = Check_UOF(gFileName);     //��UOF
    if (i < S)                    //���ļ�����UOF��
    {
        cout << "\n�ļ�" << gFileName << "�Ѿ��򿪣����ܱȽ�!\n";
        return -2;
    }

    firstFileSize = fcbp1->Fsize;
    firstFileBlocks = firstFileSize / SIZE + (short)(firstFileSize % SIZE > 0);
    secondFileSize = fcbp2->Fsize;
    secondFileBlocks = secondFileSize / SIZE + (short)(secondFileSize % SIZE > 0);
    int m = 0;
    while (firstFileBlock > 0)
    {
        if (strcmp(Disk[firstFileBlock], Disk[secondFileBlock]) != 0)
        {
            for (int i = 0; i < SIZE; i++)
            {
                if (Disk[firstFileBlock][i] != Disk[secondFileBlock][i])
                {
                    cout << "�ļ����ݲ�ͬ��\n";
                    cout << "λ�ã�" << m * SIZE + i << "\n";
                    cout << "�ļ�" << FileName1 << ":'" << Disk[firstFileBlock][i] << "'\n";
                    cout << "�ļ�" << FileName2 << ":'" << Disk[secondFileBlock][i] << "'\n";
                    return 0;
                }
            }
        }
        m++;
        firstFileBlock = FAT[firstFileBlock];
        secondFileBlock = FAT[secondFileBlock];
    }
    cout << "�ļ�������ͬ\n";
    return 1;
}

int replaceComd(int k)
{
    short int i, size, s0, firstDirBlock, secondDirBlock, secondFileBlock, firstFileBlock, s2, s22, b, b0, bnum;
    char yn, attr, attrib = '\0', attrib0 = '\07', *FileName1, *FileName2;
    char gFileName[PATH_LEN], gFileName0[PATH_LEN]; //����ļ�ȫ·����
    FCB *fcbp, *fcbp1, *fcbp2, *fcbp3;

    if (k < 1 || k > 2)
    {
        cout << "\n�����в���̫���̫�١�\n";
        return -1;
    }
    //ȡFileName1����Ŀ¼���׿��
    firstDirBlock = ProcessPath(comd[1], FileName1, k, 0, '\20');
    if (firstDirBlock < 1)    //·������
        return firstDirBlock; //ʧ�ܣ�����
    //ȡFileName1(Դ�ļ�)���׿��(���������)
    firstFileBlock = FindFCB(FileName1, firstDirBlock, attrib, fcbp);
    if (firstFileBlock < 0)
    {
        cout << "\nָ���ļ������ڡ�\n";
        return -1;
    }
    fcbp1 = fcbp; //����Դ�ļ�Ŀ¼��ָ��ֵ
    strcpy(gFileName, temppath);
    i = strlen(temppath);
    if (temppath[i - 1] != '/')
        strcat(gFileName, "/");
    strcat(gFileName, FileName1); //�����ļ���ȫ·����
    i = Check_UOF(gFileName);     //��UOF
    if (i < S)                    //���ļ�����UOF��
    {
        cout << "\n�ļ�" << gFileName << "�Ѿ��򿪣����ܽ���ȡ��!\n";
        return -2;
    }

    if (k == 1) //��������Ŀ���ļ�,��ȡ����ǰĿ¼ͬ���ļ�
    {
        secondDirBlock = curpath.fblock; //ȡ��ǰĿ¼���׿��
    }
    else //k=2(�������ṩĿ���ļ�)�����
    {
        //ȡFileName2����Ŀ¼���׿��
        secondDirBlock = ProcessPath(comd[2], FileName2, k, 0, (char)16);
        if (secondDirBlock < 0)
        {
            cout << "\nĿ¼��ָ��·�������ΪĿ¼��\n";
            return -1;
        }
        if(strcmp(FileName2,"")!=0){//˵�����һ����/�����滹�и�����
            secondDirBlock = FindFCB(FileName2, secondDirBlock, (char)16, fcbp);
            if(secondDirBlock<0){
                cout << "���һ��Ŀ¼�������ΪĿ¼" << endl;
                return -1;
            }
        }
    }

    //ȡFileName2(Ŀ���ļ�)���׿��(���������)
    secondFileBlock = FindFCB(FileName1, secondDirBlock, attrib, fcbp);
    if (secondFileBlock < 0)
    {
        cout << "\n��ȡ���ļ������ڡ�\n";
        return -1; //���ļ�����UOF��
    }

    fcbp3 = fcbp; //����Դ�ļ�Ŀ¼��ָ��ֵ
    strcpy(gFileName0, temppath);
    i = strlen(temppath);
    if (temppath[i - 1] != '/')
        strcat(gFileName0, "/");
    strcat(gFileName0, FileName1); //�����ļ���ȫ·����
    i = Check_UOF(gFileName0);     //��UOF
    if (i < S)                     //���ļ�����UOF��
    {
        cout << "\n�ļ�" << gFileName0 << "�Ѿ��򿪣����ܽ���ȡ��!\n";
        return -3;
    }

    if (fcbp->Fattrib > (char)7) //����ͬ��Ŀ��Ŀ¼
    {
        cout << "\nĿ��Ŀ¼�����ͬ��Ŀ¼��\n";
        return -2;
    }
    else
    {
        if (secondDirBlock == firstDirBlock)
        {
            cout << "\nָ���ļ��ͱ�ȡ���ļ���ͬһ���ļ��������Լ�ȡ���Լ���\n";
            return -3;
        }
        else
        {
            attr = fcbp->Fattrib & '\01';
            if (attr == '\01')
            {
                cout << "\nĿ���ļ�" << gFileName0 << "��ֻ���ļ�����ȷ��Ҫȡ������(y/n) ";
                cin >> yn;
                if (yn == 'N' && yn == 'n')
                    return -4; //��ɾ��������
            }
            if (attr == (char)2 || attr == (char)4)
            {
                cout << "�������غ�ϵͳ���Ե��ļ����ܱ�ȡ��";
                return -5;
            }

            fleshBlock(fcbp);

            i = FindBlankFCB(secondDirBlock, fcbp2);
            if (i < 0)
            {
                cout << "\nȡ���ļ�ʧ�ܡ�\n";
                return i;
            }
            size = fcbp1->Fsize;                           //Դ�ļ��ĳ���
            bnum = size / SIZE + (short)(size % SIZE > 0); //����Դ�ļ���ռ�̿���
            if (FAT[0] < bnum)
            {
                cout << "\n���̿ռ䲻�㣬����ȡ���ļ���\n";
                return -6;
            }
            *fcbp2 = *fcbp1;                    //Դ�ļ���Ŀ¼��Ƹ�Ŀ���ļ�
            strcpy(fcbp2->FileName, FileName1); //дĿ���ļ���
            b0 = 0;
            while (firstFileBlock > 0) //��ʼ�����ļ�����
            {
                b = getblock();
                if (b0 == 0)
                    fcbp2->Addr = b; //Ŀ���ļ����׿��
                else
                    FAT[b0] = b;
                memcpy(Disk[b], Disk[firstFileBlock], SIZE); //�����̿�
                firstFileBlock = FAT[firstFileBlock];        //׼��������һ���̿�
                b0 = b;
            }
            return 1;
        }
    }
}

int moveComd(int k)
{
    if (k != 2)
    {
        cout << "\n�����в���̫���̫�١�\n";
        return -1;
    }
    char attrib = (char)32, *FileName1, *FileName2, gFileName[PATH_LEN]; //����ļ�ȫ·����
    FCB *fcbp, *fcbp1, *fcbp2;
    int firstFileSize, firstFileBlocks, secondFileSize, secondFileBlocks;
    short int i, firstDirBlock, secondDirBlock, firstFileBlock, secondFileBlock, blankFCB;

    firstDirBlock = ProcessPath(comd[1], FileName1, k, 0, '\20');
    //ȡFileName1����Ŀ¼���׿��
    if (firstDirBlock < 1)
        return firstDirBlock; //·������ʧ�ܷ���
    firstFileBlock = FindFCB(FileName1, firstDirBlock, attrib, fcbp);
    if (firstFileBlock < 0)
    {
        cout << "\n��һ���ļ���Ŀ¼�����ڡ�\n";
        return -1;
    }
    fcbp1 = fcbp; //����Դ�ļ�Ŀ¼��ָ��ֵ

    strcpy(gFileName, temppath);
    i = strlen(gFileName);
    if (gFileName[i - 1] != '/')
        strcat(gFileName, "/");
    strcat(gFileName, FileName1); //�����ļ���ȫ·����
    i = Check_UOF(gFileName);     //��UOF
    if (i < S)                    //���ļ�����UOF��
    {
        cout << "\n�ļ�" << gFileName << "�Ѿ��򿪣�����Ǩ��!\n";
        return -2;
    }

    secondDirBlock = ProcessPath(comd[2], FileName2, k, 0, (char)16);
    //ȡFileName2����Ŀ¼���׿��
    if (secondDirBlock < 1)
        return secondDirBlock; //·������ʧ�ܷ���
    secondFileBlock = FindFCB(FileName2, secondDirBlock, attrib, fcbp);
    fcbp2 = fcbp;            //����Ŀ��Ŀ¼��ָ��ֵ
    if (secondFileBlock < 0) //�ڶ���Ŀ¼������
    {
        if (fcbp1->Fattrib <= (char)7)
        { //��һ���������ļ����ܸ���
            cout << "��һ���������ļ����ڶ����������ݲ����ڲ��ܸ���������" << endl;
            return -1;
        }
        if (firstDirBlock == secondDirBlock)
        {
            //ͬĿ¼�£�����Ը���
            strcpy(fcbp1->FileName, FileName2);
            cout << "�����ɹ�";
            return 0;
        }
        else
        {
            cout << "��������ͬĿ¼������" << endl;
            return -1;
        }
    }
    else //�ڶ���Ŀ¼����
    {
        //��һ���������ļ�
        if (fcbp1->Fattrib <= (char)7)
        {
            if (firstDirBlock == secondFileBlock)
            {
                cout << "�޷�Ǩ�ƣ�ͬĿ¼" << endl;
                return -1;
            }

            int isExistFile1 = FindFCB(FileName1, secondFileBlock, attrib, fcbp);
            //����Ƿ���ͬ���ļ�
            if (fcbp->Fattrib > (char)7 && isExistFile1 >= 0)
            {
                cout << "��ͬ��Ŀ¼���޷�Ǩ��" << endl;
                return -1;
            }
            if (isExistFile1 >= 0) //��ͬ���ļ�
            {
                char b;
                cout << "��ͬ���ļ����Ƿ񸲸ǣ�y/n" << endl;
                while (1)
                {
                    cin >> b;
                    if (b != 'y' && b != 'Y' && b != 'n' && b != 'N')
                        cout << "������������������:" << endl;
                    else
                    {

                        break;
                    }
                }
                if (b == 'y' || b == 'Y')
                {
                    int t;
                    fcbp->FileName[0] = (char)0xe5;
                    while (isExistFile1 > 0) //ȷ�ϸ��Ǻ���մ��̿ռ�
                    {
                        t = isExistFile1;
                        isExistFile1 = FAT[isExistFile1];
                        FAT[t] = 0;
                        FAT[0]++;
                    }
                }
                else
                {
                    cout << "���Ƴ�ͻ��Ǩ��ȡ��" << endl;
                    return -1;
                }
            }

            blankFCB = FindBlankFCB(secondFileBlock, fcbp);
            if (blankFCB < 0)
            {
                cout << "\nǨ���ļ�ʧ�ܡ�\n";
                return blankFCB;
            }
            *fcbp = *fcbp1;                  //����һ���ļ���fcb���ƽ��µ�Ŀ¼��
            fcbp1->FileName[0] = (char)0xe5; //ɾ��1�ļ���fcb
            cout << "Ǩ�Ƴɹ�";
            return 1;
        }
        else
        {
            //��һ��������Ŀ¼
            // ��ָ��Ŀ¼���ڣ��򽫡��ļ�����ָ����Ŀ¼ת�Ƶ���Ŀ¼�У�
            // ����ָ��Ŀ¼�д����롰�ļ�����ָ����Ŀ¼ͬ������Ŀ¼���򱨴�
            if (firstDirBlock == secondFileBlock)
            {
                cout << "ͬĿ¼�޷�Ǩ�ƣ�����" << endl;
                return -1;
            }
            int isExistDir = FindFCB(FileName1, secondFileBlock, attrib, fcbp);
            //����Ƿ���ͬ���ļ�
            if (fcbp->Fattrib > (char)7 && isExistDir >= 0)
            {
                cout << "��ͬ��Ŀ¼���޷�Ǩ��1" << endl;
                return -1;
            }
            if (isExistDir >= 0)
            {
                char b;
                cout << "��ͬ���ļ����Ƿ񸲸ǣ�y/n" << endl;
                while (1)
                {
                    cin >> b;
                    if (b != 'y' && b != 'Y' && b != 'n' && b != 'N')
                        cout << "������������������:" << endl;
                    else
                    {

                        break;
                    }
                }
                if (b == 'y' || b == 'Y')
                {
                    int t;
                    fcbp->FileName[0] = (char)0xe5;
                    while (isExistDir > 0) //ȷ�ϸ��Ǻ���մ��̿ռ�
                    {
                        t = isExistDir;
                        isExistDir = FAT[isExistDir];
                        FAT[t] = 0;
                        FAT[0]++;
                    }
                }
                else
                {
                    cout << "���Ƴ�ͻ��Ǩ��ȡ��" << endl;
                    return -1;
                }
            }
            blankFCB = FindBlankFCB(secondFileBlock, fcbp);
            if (blankFCB < 0)
            {
                cout << "\nǨ���ļ�ʧ�ܡ�\n";
                return blankFCB;
            }
            *fcbp = *fcbp1;                  //����һ��Ŀ¼��fcb���ƽ��µ�Ŀ¼��
            fcbp1->FileName[0] = (char)0xe5; //ɾ��1�ļ���fcb
            cout << "Ǩ�Ƴɹ�";
            return 1;
        }
    }
}

int batchComd(int k)
{
    if (k < 1 || k > 2)
    {
        cout << "\n�����в���̫���̫�١�\n";
        return -1;
    }
    if (k == 2 && comd[2][0] == 's' && comd[2][1] == '\0')
    {
        cout << "��ģ����̶���" << comd[1] << endl;
        char *FileName;
        FCB *fcbp;
        short int dirBlock, fileBlock;
        dirBlock = ProcessPath(comd[1], FileName, k, 0, '\20');
        if (dirBlock < 1)    //·������
            return dirBlock; //ʧ�ܣ�����
        //ȡFileName1(Դ�ļ�)���׿��(���������)
        fileBlock = FindFCB(FileName, dirBlock, 0, fcbp);
        if (fileBlock < 0)
        {
            cout << "\nָ���ļ������ڡ�\n";
            return -1;
        }
        int fileSize = fcbp->Fsize, m = 0, n = 0;
        char comds[INPUT_LEN];
        int i = 0, j = 0;
        while (n < fileSize)
        {
            for (; i < SIZE; i++)
            {
                if (Disk[fileBlock][i] == '\n')
                {
                    n++;
                    i++;
                    break;
                }
                else
                {
                    comds[j++] = Disk[fileBlock][i];
                    n++;
                }
                if (n == fileSize)
                {
                    j++;
                    break;
                }
            }
            if (i == SIZE || i == SIZE - 1)
            {
                fileBlock = FAT[fileBlock]; //׼������һ���̿�
                i = 0;
            }
            if (i <= SIZE - 1)
            {
                comds[j] = '\0';
                strcpy(BatchComds[BatchHeader], comds);
                BatchHeader = (BatchHeader + 1) % BATCHNUM;
                if (BatchRail == BatchHeader)
                {
                    cout << "batch�������������������������������󻺳������Ѿ�����Ľ���ִ�С�" << endl;
                    BatchHeader--;
                    return -1;
                }
                j = 0;
                memset(comds, 0, sizeof(comds));
            }
        }
        return 1;
    }
    else
    {
        cout << "����ʵ���̶���" << comd[1] << endl;
        ifstream ff(comd[1], ios::in); //���ļ�FAT.txt
        if (!ff)
        {
            cout << "Can't open " << comd[1] << "\n";
            return -1;
        }
        while (ff.getline(BatchComds[BatchHeader], INPUT_LEN))
        {
            BatchHeader = (BatchHeader + 1) % BATCHNUM;
            if (BatchRail == BatchHeader)
            {
                cout << "batch�������������������������������󻺳������Ѿ�����Ľ���ִ�С�" << endl;
                BatchHeader--;
                return -1;
            }
        }
        ff.close();
        return 1;
    }
}