/*
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/*
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit
{
    /* add your code here */
    char *mnemonic;
    int operand_count; //operand ����
    char *format;
    char *opcode;
};

// instruction�� ������ ���� ����ü�� �����ϴ� ���̺� ����
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */
char *input_data[MAX_LINES];
static int line_num;

/*
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 * nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
 */
struct token_unit
{
    char *label;                //��ɾ� ���� �� label
    char *operator;             //��ɾ� ���� �� operator
    char *operand[MAX_OPERAND]; //��ɾ� ���� �� operand
    char *comment;              //��ɾ� ���� �� comment
    // char nixbpe;
    int location; // ���� ������Ʈ���� ���ȴ�.
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * ���� ������Ʈ���� ���ȴ�.
 */
struct symbol_unit
{
    char symbol[10];
    int addr;
    int session;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
//symbol �� ������ �����ϱ� ���� ����
static int symbol_line;

/*
* ���ͷ��� �����ϴ� ����ü�̴�.
* ���ͷ� ���̺��� ���ͷ��� �̸�, ���ͷ��� ��ġ�� �����ȴ�.
* ���� ������Ʈ���� ���ȴ�.
*/
struct literal_unit
{
    char literal[10];
    int addr;
    //token_table�� �ִ� literal�ּ�
    int token_index;
};

typedef struct literal_unit literal;
literal literal_table[MAX_LINES];
//���ͷ� ���� token_table �Ҵ��ϱ� ���� ����
static int literal_count;
//literal �� ������ �����ϱ� ���� ����
static int literal_line;
//LTORG�� ���� �Ҵ���� ���� ���ͷ� �ε������� ����
static int literal_index;

static int locctr;
//���� ���̸� ������ �迭
int session_length[4];
static int session_count;
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_opcode_output(char *file_name);
//���� ���� �Լ�
//�޸𸮸� �Ҵ����ִ� �Լ�.(token ���� ����)
void memory_allocation(char **parameter, char **inst_token);
//token_table��  �ʱ�ȭ ���ִ� �Լ�
void clear_token_table(token *token_table);

int search_literal_table(char* str);

char* literal_erase_quote(char* ptr);

int find_addr_symbol_from_table(char* str);

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
void make_symtab_output(char *file_name);
void make_literaltab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);