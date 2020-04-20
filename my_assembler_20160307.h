/*
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/*
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit
{
    /* add your code here */
    char *mnemonic;
    int operand_count; //operand 갯수
    char *format;
    char *opcode;
};

// instruction의 정보를 가진 구조체를 관리하는 테이블 생성
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
char *input_data[MAX_LINES];
static int line_num;

/*
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit
{
    char *label;                //명령어 라인 중 label
    char *operator;             //명령어 라인 중 operator
    char *operand[MAX_OPERAND]; //명령어 라인 중 operand
    char *comment;              //명령어 라인 중 comment
    char nixbpe;
    int location; // 추후 프로젝트에서 사용된다.
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

//주소 관리 배열
struct address
{
    char* operator;
    int location;   //메모리 할당 주소
};
//int address[MAX_LINES];
struct address address[100];
static int address_line;
/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 * 추후 프로젝트에서 사용된다.
 */
struct symbol_unit
{
    char symbol[10];
    int addr;
    int session;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
//symbol 의 갯수를 관리하기 위한 변수
static int symbol_line;

/*
* 리터럴을 관리하는 구조체이다.
* 리터럴 테이블은 리터럴의 이름, 리터럴의 위치로 구성된다.
* 추후 프로젝트에서 사용된다.
*/
struct literal_unit
{
    char literal[10];
    int addr;
    //token_table에 있는 literal주소
    int token_index;
};

typedef struct literal_unit literal;
literal literal_table[MAX_LINES];
//리터럴 문자 token_table 할당하기 위한 변수
static int literal_count;
//literal 의 개수를 관리하기 위한 변수
static int literal_line;
//LTORG에 의해 할당되지 않은 리터럴 인덱스저장 변수
static int literal_index;

static int locctr;
//각 세션의 정보를 저장할 구조체
struct Session_info
{
int session_length;
char session_name[7];

};
struct Session_info session[5];
static int session_count;

//TODO modify values?

//오브젝트 코드 관리할 구조체
struct object_code
{
    int session;
    char object[10];

};
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
//새로 만든 함수
//메모리를 할당해주는 함수.(token 제어 포함)
void memory_allocation(char **parameter, char **inst_token);
//token_table을  초기화 해주는 함수
void clear_token_table(token *token_table);

int search_literal_table(char* str);

char* literal_erase_quote(char* ptr);

int find_addr_symbol_from_table(char* str);

int find_addr_literal_from_table(char* str);

/* 추후 프로젝트에서 사용하게 되는 함수*/
void make_symtab_output(char *file_name);
void make_literaltab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);